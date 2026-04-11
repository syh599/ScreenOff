#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <powrprof.h>
#include <iostream>
#include <shellapi.h>

#include <vector>
#include <array>
#include <string>
#include <regex>
#include <cstdio>

#pragma comment(lib, "powrprof.lib")

#define APP_ICON 101
#define ACTIVE_ICON 102

static const GUID GUID_DISPLAY_SUBGROUP = 
    { 0x7516b95f, 0xf776, 0x4464, 0x8c, 0x53, 0x06, 0x16, 0x7f, 0x40, 0xcc, 0x99 };

static const GUID GUID_DISPLAY_TIMEOUT = 
    { 0x3c0bc021, 0xc8a8, 0x4e07, 0xa9, 0x73, 0x6b, 0x14, 0xcb, 0xcb, 0x2b, 0x7e };

NOTIFYICONDATAA nid = {};
GUID* pActiveScheme = NULL;
DWORD originalTimeout = 0;
bool hasSavedTimeout = false;
int isActive = 0;
HPOWERNOTIFY hPowerNotify = NULL; 
UINT uTaskbarRestart = 0;

std::vector<std::string> activeOverrides;

bool SaveOriginalTimeout()
{
    // active power plan
    if (PowerGetActiveScheme(NULL, &pActiveScheme) != ERROR_SUCCESS)
        return 0;  // Failed
    
    // read current display timeout value
    if (PowerReadACValueIndex(NULL, pActiveScheme, &GUID_DISPLAY_SUBGROUP, 
                              &GUID_DISPLAY_TIMEOUT, &originalTimeout) != ERROR_SUCCESS)
        return 0;

    hasSavedTimeout = true;
    return 1;
}

bool SetNewTimeout()
{
    DWORD newTimeout = 1; // in seconds
    
    if (PowerWriteACValueIndex(NULL, pActiveScheme, &GUID_DISPLAY_SUBGROUP,
                               &GUID_DISPLAY_TIMEOUT, newTimeout) != ERROR_SUCCESS)
        return 0;

    PowerSetActiveScheme(NULL, pActiveScheme);

    
    return 1;
}

bool RestoreOriginalTimeout()
{  
    if (!hasSavedTimeout) return 0;

    if (PowerWriteACValueIndex(NULL, pActiveScheme, &GUID_DISPLAY_SUBGROUP,
                               &GUID_DISPLAY_TIMEOUT, originalTimeout) != ERROR_SUCCESS)
        return 0;
    
    PowerSetActiveScheme(NULL, pActiveScheme);

    return 1;
}

bool IsRunningAsAdmin()
{
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;

    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup);

    CheckTokenMembership(NULL, adminGroup, &isAdmin);
    FreeSid(adminGroup);

    return isAdmin == TRUE;
}

void RelaunchAsAdmin() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    ShellExecuteA(NULL, "runas", path, NULL, NULL, SW_SHOW);
}

std::string GetWakeLockProcesses()
{
    std::array<char, 128> buffer;
    std::string result;

    //run powercfg /requests and capture output
    FILE* pipe = _popen("powercfg /requests 2>nul", "r");
    if (!pipe) return "";

    while (fgets(buffer.data(), 128, pipe) != NULL) {
        result += buffer.data();
    }
    _pclose(pipe);

    return result;
}

std::vector<std::string> FindProcessesWithWakeLock(const std::string& output)
{
    std::vector<std::string> processes;
    std::regex processRegex(R"(\[PROCESS\].*\\([^\\]+\.exe))");

    auto begin = std::sregex_iterator(output.begin(), output.end(), processRegex);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        std::string proc = (*it)[1].str();
        // lowercase for comparison
        for (char& c : proc) c = tolower(c);
        if (std::find(processes.begin(), processes.end(), proc) == processes.end()) {
            processes.push_back(proc);
        }
    }

    return processes;
}

void RunPowerCfg(const char* args)
{
    ShellExecuteA(NULL, "runas", "C:\\Windows\\System32\\powercfg.exe", args, NULL, SW_HIDE);
}

bool OverrideWakeLocks()
{   
    bool isAdmin = IsRunningAsAdmin();
    if (!isAdmin) return 0;

    std::string output = GetWakeLockProcesses();
    auto processes = FindProcessesWithWakeLock(output);

    for (const auto& proc : processes) {
        std::string cmd = "/requestsoverride PROCESS \"" + proc + "\" DISPLAY";
        RunPowerCfg(cmd.c_str());
        activeOverrides.push_back(proc);
    }
    return 1;
}

bool RevertOverrides()
{   
    bool isAdmin = IsRunningAsAdmin();
    if (!isAdmin) return 0;

    for (const auto& proc : activeOverrides) {
        std::string cmd = "/requestsoverride PROCESS \"" + proc + "\"";
        RunPowerCfg(cmd.c_str());
    }
    activeOverrides.clear();
    return 1;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // When window is first created, add the tray icon
    if (msg == WM_CREATE)
    {
        nid.cbSize = sizeof(NOTIFYICONDATAA);
        nid.hWnd = hwnd;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
        nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101)); //icon
        strcpy_s(nid.szTip, "Double click!");  // tooltip
        nid.uCallbackMessage = WM_USER + 1;
        uTaskbarRestart = RegisterWindowMessageA("TaskbarCreated");

        Shell_NotifyIconA(NIM_ADD, &nid);  //add to taskbar

        hPowerNotify = RegisterPowerSettingNotification(hwnd, &GUID_MONITOR_POWER_ON, DEVICE_NOTIFY_WINDOW_HANDLE);
    }
    
    // When window is destroyed, remove the tray icon
    if (msg == WM_DESTROY){   
        if (hPowerNotify) {
            UnregisterPowerSettingNotification(hPowerNotify);
        }
        if (isActive) RestoreOriginalTimeout();
        Shell_NotifyIconA(NIM_DELETE, &nid);
        PostQuitMessage(0);
    }
    
    if (msg == WM_USER + 1){ // tray icon clicks
        if (lParam== WM_LBUTTONDBLCLK){  // Left double-click
            if (!isActive) {
                if (SaveOriginalTimeout() && SetNewTimeout()) {
                    isActive = 1;
                    OverrideWakeLocks();

                    // Refresh icon
                    nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ACTIVE_ICON));
                    Shell_NotifyIconA(NIM_MODIFY, &nid);
                    //MessageBoxA(hwnd, "Applied.", "Hey!", MB_OK);
                } else {
                    MessageBoxA(hwnd, "Failed to activate.", "Error", MB_OK);
                }
            } /*else {
                RestoreOriginalTimeout();
                isActive = 0;
                MessageBoxA(hwnd, "Original display settings restored.", "Monitor Woke", MB_OK);
            } */
        }
        else if (lParam == WM_RBUTTONUP){  // Right-click
            HMENU hMenu = CreatePopupMenu();
            AppendMenuA(hMenu, MF_STRING, 1, "Exit");
            
            // Show the menu at mouse position
            POINT pt;
            GetCursorPos(&pt);

            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
            PostMessage(hwnd, WM_NULL, 0, 0);
        }
        return 0;
    }

    if (msg == WM_COMMAND){
        int command = LOWORD(wParam);
        if (command == 1) {
            DestroyWindow(hwnd);
        }
        return 0;
    }

    /*if (isActive && (msg == WM_KEYDOWN || msg == WM_MOUSEMOVE))
    {
        RestoreOriginalTimeout();
        isActive = 0;
        MessageBoxA(hwnd, "Original display settings restored.", "Monitor Woke", MB_OK);
    }*/

    if (msg == WM_POWERBROADCAST && wParam == PBT_POWERSETTINGCHANGE)
    {
        POWERBROADCAST_SETTING* pbs = (POWERBROADCAST_SETTING*)lParam;
        if (pbs->PowerSetting == GUID_MONITOR_POWER_ON){
            DWORD monitorState = *(DWORD*)pbs->Data;
            if (monitorState == 1 && isActive)  // Monitor turned ON
            {
                RestoreOriginalTimeout();
                isActive = 0;
                RevertOverrides();
                nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
                Shell_NotifyIconA(NIM_MODIFY, &nid);
            }
        }
        return 0;
    }

    if (msg == uTaskbarRestart) {
        Shell_NotifyIconA(NIM_ADD, &nid);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    if (!IsRunningAsAdmin()) {
        int result = MessageBoxA(NULL, "ScreenOff needs to run as Admin to work during video playback.\nWould you like to relaunch as Administrator?", "Warning", MB_YESNOCANCEL | MB_ICONWARNING);
        if (result == IDYES) {
            RelaunchAsAdmin();
            return 0;
        }
        else if (result == IDCANCEL) {
            return 0;
        }
        //else if (result == IDNO) {}
    }
    // Step 1: Register a "window type"
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;      // "Use my function above"
    wc.hInstance = hInstance;          // "This program"
    wc.lpszClassName = "MyClass";  // "Name of this window type"
    RegisterClassA(&wc);
    
    // Step 2: Create the actual window (hidden)
    CreateWindowExA(0, "MyClass", "", 0, 0,0,0,0, NULL, NULL, hInstance, NULL);
    
    // Step 3: Message loop (waits for messages)
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        DispatchMessage(&msg);
    }
    
    return 0;
}
