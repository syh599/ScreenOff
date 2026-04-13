#include "silentcmd.h"
#include <windows.h>
#include <vector>
#include <string>

int SilentSystem(const std::string& command)
{
    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    // Add cmd.exe /c to run the command
    std::string fullCmd = "cmd.exe /c " + command;

    std::vector<char> cmdLine(fullCmd.begin(), fullCmd.end());
    cmdLine.push_back('\0');

    BOOL success = CreateProcessA(NULL, cmdLine.data(), NULL, NULL, FALSE,
        CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    if (!success) return -1;

    // Wait for process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get exit code
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (int)exitCode;
}