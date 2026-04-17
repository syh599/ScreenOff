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

    // Escape backslashes for XML
    std::string path(exePath);
    std::string escaped;
    for (char c : path) {
        if (c == '&') escaped += "&amp;";
        else escaped += c;
    }

    // Build task XML:
    //   - No StartBoundary (activate unchecked)
    //   - DisallowStartIfOnBatteries = false
    //   - StopIfGoingOnBatteries    = false
    //   - ExecutionTimeLimit = PT0S  (no "stop if runs longer than")
    std::string xml = R"(<?xml version="1.0" encoding="UTF-16"?>
<Task version="1.2" xmlns="http://schemas.microsoft.com/windows/2004/02/mit/task">
  <Triggers>
    <LogonTrigger>
      <Enabled>true</Enabled>
      <Delay>PT1S</Delay>
    </LogonTrigger>
  </Triggers>
  <Principals>
    <Principal id="Author">
      <RunLevel>HighestAvailable</RunLevel>
    </Principal>
  </Principals>
  <Settings>
    <MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy>
    <DisallowStartIfOnBatteries>false</DisallowStartIfOnBatteries>
    <StopIfGoingOnBatteries>false</StopIfGoingOnBatteries>
    <ExecutionTimeLimit>PT0S</ExecutionTimeLimit>
    <IdleSettings>
        <StopOnIdleEnd>false</StopOnIdleEnd>
    </IdleSettings>
    <Enabled>true</Enabled>
  </Settings>
  <Actions>
    <Exec>
      <Command>)" + escaped + R"(</Command>
    </Exec>
  </Actions>
</Task>)";

    // Write XML to a temp file
    char tempPath[MAX_PATH], xmlFile[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    GetTempFileNameA(tempPath, "tsk", 0, xmlFile);

    // Write as UTF-16 LE (required by schtasks /xml)
    FILE* f = nullptr;
    fopen_s(&f, xmlFile, "wb");
    if (f) {
        // UTF-16 LE BOM
        unsigned char bom[] = { 0xFF, 0xFE };
        fwrite(bom, 1, 2, f);
        std::wstring wxml(xml.begin(), xml.end());
        fwrite(wxml.c_str(), sizeof(wchar_t), wxml.size(), f);
        fclose(f);
    }

    std::string cmd = "schtasks /create /tn \"ScreenOff\" /xml \"";
    cmd += xmlFile;
    cmd += "\" /f";

    ShellExecuteA(NULL, "runas", "cmd.exe",
        ("/c " + cmd).c_str(),
        NULL, SW_HIDE);

    // Clean up temp file after a short delay (task is registered by then)
    Sleep(2000);
    DeleteFileA(xmlFile);
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