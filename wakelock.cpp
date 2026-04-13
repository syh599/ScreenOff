#include "wakelock.h"
#include "isAdmin.h"

#include <array>
#include <regex>
#include <cstdio>

void RunPowerCfg(const char* args)
{
    ShellExecuteA(NULL, "runas", "C:\\Windows\\System32\\powercfg.exe", args, NULL, SW_HIDE);
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

bool OverrideWakeLocks(std::vector<std::string>& activeOverrides)
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

bool RevertOverrides(std::vector<std::string>& activeOverrides)
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