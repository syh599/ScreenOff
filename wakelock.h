#pragma once
#include <windows.h>

#include <vector>
#include <string>

void RunPowerCfg(const char* args);
std::string GetWakeLockProcesses();
std::vector<std::string> FindProcessesWithWakeLock(const std::string& output);
bool OverrideWakeLocks(std::vector<std::string>& activeOverrides);
bool RevertOverrides(std::vector<std::string>& activeOverrides);

