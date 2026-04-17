#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <windows.h>
#include "nlohmann/json.hpp"

class Config {
private:
    nlohmann::json data;
    std::string configPath;

    std::string GetConfigDir() {
        std::string appdata = std::getenv("APPDATA");
        return appdata + "\\ScreenOff";
    }

    std::string GetConfigFile() {
        return GetConfigDir() + "\\config.json";
    }

public:
    Config() {
        configPath = GetConfigFile();
        Load();
    }

    void Load() {
        // Create directory if it doesn't exist
        std::filesystem::create_directories(GetConfigDir());

        // Load existing config or create default
        bool loaded = false;
        std::ifstream file(configPath);

        if (file.is_open()) {
            try {
                file >> data;
                loaded = true;
                file.close();
            }
            catch (const nlohmann::json::parse_error& e) {
                file.close();
                std::filesystem::rename(configPath, configPath + ".corrupt"); //corrupt backup
            }
        }
        if (!loaded) {
            // Default values
            data = nlohmann::json();
            data["restoreOnWake"] = true;
            data["newDisplayTimeout"] = 1;
            data["sleepAction"] = 0;
            data["mode"] = 0;
            data["resetTimer"] = true;
            data["sleepTimerMinutes"] = 0;
            Save();
        }
    }

    void Save() {
        std::ofstream file(configPath);
        if (file.is_open()) {
            file << data.dump(4); // Pretty print with 4 spaces
            file.close();
        }
    }

    // Getters/Setters
    bool GetRestoreOnWake() { return data.value("restoreOnWake", true); }
    void SetRestoreOnWake(bool tf) { data["restoreOnWake"] = tf; Save(); }

    int GetNewDisplayTimeout() { return data.value("newDisplayTimeout", 1); }
    void SetNewDisplayTimeout(int minutes) { data["newDisplayTimeout"] = minutes; Save(); }

    int GetSleepAction() { return data.value("sleepAction", 0); }
    void SetSleepAction(int action) { data["sleepAction"] = action; Save(); }
    /*
    int GetMode() { return data.value("mode", 0); }
    void SetMode(int mode) { data["mode"] = mode; Save(); }

    bool GetResetTimer() { return data.value("resetTimer", true); }
    void SetResetTimer(bool tf) { data["resetTimer"] = tf; Save(); }

    int GetSleepTimerMinutes() { return data.value("sleepTimerMinutes", 0); }
    void SetRestoreTimer(int minutes) { data["SleepTimerMinutes"] = minutes; Save(); }
    */
};