#include "autostart.h"
#include "silentcmd.h"

#include <windows.h>
#include <cstdio>
#include <string>
#include <vector>

void AddToStartup()
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    char cmd[1024];
    sprintf_s(cmd,
        "schtasks /create /tn \"ScreenOff\" /tr \"\\\"%s\\\"\" /sc ONLOGON /delay 0000:01 /f /rl HIGHEST",
        exePath);

    ShellExecuteA(NULL, "runas", "cmd.exe",
        ("/c " + std::string(cmd)).c_str(),
        NULL, SW_HIDE);
}

void RemoveFromStartup()
{
    ShellExecuteA(NULL, "runas", "cmd.exe",
        "/c schtasks /delete /tn \"ScreenOff\" /f",
        NULL, SW_HIDE);
}

bool IsInStartup()
{
    return SilentSystem("schtasks /query /tn \"ScreenOff\" > nul 2>&1") == 0;
}