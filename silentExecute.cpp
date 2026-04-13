#include "silentExecute.h"
#include <windows.h>

#include <vector>
#include <cstdio>

bool SilentExecute(const std::string& command, const std::string& arguments) {
    std::string cmdLine;
    if (arguments.empty()) {
        cmdLine = command;
    }
    else {
        cmdLine = command + " " + arguments;
    }

    // Create a modifiable buffer for the command line
    char* cmdBuffer = new char[cmdLine.size() + 1];
    strcpy_s(cmdBuffer, cmdLine.size() + 1, cmdLine.c_str());

    // Setup process startup info
    STARTUPINFOA si = { 0 };
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // This hides the window

    PROCESS_INFORMATION pi = { 0 };

    // Create the process
    BOOL result = CreateProcessA(
        NULL,           // No application name - use command line
        cmdBuffer,      // Command line (modifiable)
        NULL,           // Process security attributes
        NULL,           // Thread security attributes
        FALSE,          // Handle inheritance
        CREATE_NO_WINDOW, // Creation flag to prevent window creation
        NULL,           // Environment
        NULL,           // Current directory
        &si,            // Startup info
        &pi             // Process info
    );

    // Wait for the process to complete (optional)
    if (result) {
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Get exit code (optional)
        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        // Clean up
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        delete[] cmdBuffer;
        return true;
    }

    delete[] cmdBuffer;
    return false;
}