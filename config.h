#pragma once
#include <string>
#include <filesystem>
#include <fstream>
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
        std::ifstream file(configPath);
        if (file.is_open()) {
            file >> data;
            file.close();
        }
        else {
            // Default values
            data["restoreOnWake"] = true;
            data["newDisplayTimeout"] = 1;
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
    bool GetRestoreOnWake() { return data.value("restoreOnWake", 0); }
    void SetRestoreOnWake(bool tf) { data["restoreOnWake"] = tf; Save(); }

    int GetNewDisplayTimeout() { return data.value("newDisplayTimeout", 30); }
    void SetNewDisplayTimeout(int minutes) { data["newDisplayTimeout"] = minutes; Save(); }
};