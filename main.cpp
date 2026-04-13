#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <powrprof.h>
#include <iostream>

#include <vector>
#include <array>
#include <string>
#include <regex>
#include <cstdio>

#include "resource.h"

#include "isAdmin.h"
#include "timeout.h"
#include "wakelock.h"
#include "autostart.h"
#include "silentcmd.h"
#include "sleeptimer.h"

#pragma comment(lib, "powrprof.lib")

NOTIFYICONDATAA nid = {};

GUID* pActiveScheme = NULL;
DWORD originalTimeoutAC = 0;
DWORD originalTimeoutDC = 0;
bool hasSavedTimeout = false;

bool isActive = 0;
HPOWERNOTIFY hPowerNotify = NULL;

bool sleepTimerActive = false;
int sleepAfterMinutes = 0;
time_t sleepTargetTime = 0;

UINT uTaskbarRestart = 0;

std::vector<std::string> activeOverrides;

void PressMediaPlayPause() {
    INPUT inputs[2] = { 0 };

    // Key down event
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_MEDIA_PLAY_PAUSE;

    // Key up event
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_MEDIA_PLAY_PAUSE;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
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
        if (sleepTimerActive) KillTimer(hwnd, 1);
        if (isActive) RestoreOriginalTimeout(pActiveScheme, originalTimeoutAC, originalTimeoutDC, hasSavedTimeout);
        Shell_NotifyIconA(NIM_DELETE, &nid);
        PostQuitMessage(0);
    }
    
    if (msg == WM_USER + 1){ // tray icon clicks
        if (lParam== WM_LBUTTONDBLCLK){  // Left double-click
            if (!isActive) {
                if (SaveOriginalTimeout(&pActiveScheme, &originalTimeoutAC, &originalTimeoutDC, &hasSavedTimeout) 
                    && 
                    SetNewTimeout(pActiveScheme)) 
                {
                    isActive = 1;
                    OverrideWakeLocks(activeOverrides);
                    StartSleepTimer(sleepAfterMinutes, sleepTimerActive, sleepTargetTime, hwnd);

                    // Refresh icon
                    nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ACTIVE_ICON));
                    Shell_NotifyIconA(NIM_MODIFY, &nid);
                    //MessageBoxA(hwnd, "Applied.", "Hey!", MB_OK);
                } else {
                    MessageBoxA(hwnd, "Failed to activate.", "Error", MB_OK);
                }
            } else {
                RestoreOriginalTimeout(pActiveScheme, originalTimeoutAC, originalTimeoutDC, hasSavedTimeout);
                isActive = 0;
                RevertOverrides(activeOverrides);

                sleepTimerActive = false;
                sleepAfterMinutes = 0;
                KillTimer(hwnd, 1);
                nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
                Shell_NotifyIconA(NIM_MODIFY, &nid);
                //MessageBoxA(hwnd, "Original display settings restored.", "Monitor Woke", MB_OK);
            } 
        }
        else if (lParam == WM_RBUTTONUP){  // Right-click
            HMENU hMenu = CreatePopupMenu();
            HMENU hSleepMenu = CreatePopupMenu();

            bool isCustomTime = ((sleepAfterMinutes > 60) || (sleepAfterMinutes <= 60 && sleepAfterMinutes % 15 != 0));
            std::string customText = "Custom: " +
                (isCustomTime ? std::to_string(sleepAfterMinutes) + " mins" : "");

            AppendMenuA(hSleepMenu, MF_STRING | (sleepAfterMinutes == 15 ? MF_CHECKED : MF_UNCHECKED), 10, "15 minutes");
            AppendMenuA(hSleepMenu, MF_STRING | (sleepAfterMinutes == 30 ? MF_CHECKED : MF_UNCHECKED), 11, "30 minutes");
            AppendMenuA(hSleepMenu, MF_STRING | (sleepAfterMinutes == 45 ? MF_CHECKED : MF_UNCHECKED), 12, "45 minutes");
            AppendMenuA(hSleepMenu, MF_STRING | (sleepAfterMinutes == 60 ? MF_CHECKED : MF_UNCHECKED), 13, "60 minutes");
            AppendMenuA(hSleepMenu, MF_STRING | ( isCustomTime ? MF_CHECKED : MF_UNCHECKED), 14, customText.c_str());
            AppendMenuA(hSleepMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuA(hSleepMenu, MF_STRING, 15, "Cancel");
            
            AppendMenuA(hMenu, MF_STRING, 98, "Test PAUSE command");
            AppendMenuA(hMenu, MF_POPUP | (sleepAfterMinutes > 0 ? MF_CHECKED : MF_UNCHECKED), (UINT_PTR)hSleepMenu, "Sleep Timer");
            AppendMenuA(hMenu, MF_STRING | (IsInStartup() ? MF_CHECKED : MF_UNCHECKED), 2, "Run on startup");
            AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuA(hMenu, MF_STRING, 99, "Exit");
            
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
        if (command == 99) {
            DestroyWindow(hwnd);
        }
        else if (command == 2) {
            if (IsInStartup()) RemoveFromStartup();
            else AddToStartup();
        }
        else if (command == 10) sleepAfterMinutes = 15;
        else if (command == 11) sleepAfterMinutes = 30;
        else if (command == 12) sleepAfterMinutes = 45;
        else if (command == 13) sleepAfterMinutes = 60;
        else if (command == 14) sleepAfterMinutes = GetCustomTime();
        else if (command == 15) sleepAfterMinutes = 0;
        else if (command == 98) PressMediaPlayPause(); //SetSuspendState(FALSE, FALSE, FALSE);
            
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
                RestoreOriginalTimeout(pActiveScheme, originalTimeoutAC, originalTimeoutDC, hasSavedTimeout);
                isActive = 0;
                RevertOverrides(activeOverrides);
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

    if (msg == WM_TIMER && wParam == 1)
    {
        if (sleepTimerActive && time(NULL) >= sleepTargetTime)
        {
            sleepTimerActive = false;
            sleepAfterMinutes = 0; //option to skip???? TODO!!!!!!
            KillTimer(hwnd, 1);
            PressMediaPlayPause();
            //SetSuspendState(FALSE, TRUE, FALSE);
        }
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    //admin check
    if (!IsRunningAsAdmin()) {
        /*int result = MessageBoxA(NULL, "ScreenOff needs to run as Admin to work during video playback.\nWould you like to relaunch as Administrator?", "Warning", MB_OKCANCEL | MB_ICONWARNING);
        if (result == IDOK) {
            RelaunchAsAdmin();
        }*/
        RelaunchAsAdmin();
        return 0;
    }

    //single instance
    HWND hExisting = FindWindowA("ScreenOffWindow", NULL);

    if (hExisting) { //close existing instance
        PostMessage(hExisting, WM_CLOSE, 0, 0);
        Sleep(500);
    }

    // Register a window type
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;      // Use function above
    wc.hInstance = hInstance;
    wc.lpszClassName = "ScreenOffWindow";  // Name of this window type
    RegisterClassA(&wc);
    
    // Create hidden window
    CreateWindowExA(0, "ScreenOffWindow", "", 0, 0,0,0,0, NULL, NULL, hInstance, NULL);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        DispatchMessage(&msg);
    }
    
    return 0;
}
