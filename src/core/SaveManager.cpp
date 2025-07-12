// src/core/SaveManager.cpp
#include "core/SaveManager.h"
#include "nlohmann/json.hpp"  // <-- NEU
#include <filesystem>
#include <fstream>            // <-- NEU
#include <cstdlib>

using json = nlohmann::json;

namespace fs = std::filesystem;


SaveData& SaveManager::data() {
    return m_data;
}


// --------------------------------------------------
//   Hilfsfunktion: Speicherort
// --------------------------------------------------
std::string SaveManager::path() const
{
#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    fs::path dir = appData ? fs::path(appData) / "JumpRun" : fs::temp_directory_path();
#else   // macOS & Linux → $HOME/.local/share
    const char* home = std::getenv("HOME");
    fs::path dir = home ? fs::path(home) / ".local/share/jump-run"
                        : fs::temp_directory_path();
#endif
    fs::create_directories(dir);       // existiert sonst nicht
    return (dir / "save.json").string();
}

bool SaveData::isLevelCompleted(int levelIndex) const {
    return std::find(completedLevels.begin(), completedLevels.end(), levelIndex) != completedLevels.end();
}

bool SaveData::isLevelUnlocked(int levelIndex) const {
    if (levelIndex == 0) return true;  // Erstes Level immer freigeschaltet
    return isLevelCompleted(levelIndex - 1);  // Vorheriges Level muss abgeschlossen sein
}

void SaveData::completeLevel(int levelIndex, float time) {
    // Level als abgeschlossen markieren
    if (!isLevelCompleted(levelIndex)) {
        completedLevels.push_back(levelIndex);
    }
    
    // Beste Zeit updaten
    if (levelBestTimes.find(levelIndex) == levelBestTimes.end() || time < levelBestTimes[levelIndex]) {
        levelBestTimes[levelIndex] = time;
    }
    
    // Sterne berechnen (einfaches System basierend auf Zeit)
    int stars = 3;  // Standard: 3 Sterne
    if (time > 60.0f) stars = 2;   // Über 1 Minute: 2 Sterne  
    if (time > 120.0f) stars = 1;  // Über 2 Minuten: 1 Stern
    levelStars[levelIndex] = stars;
}

bool SaveManager::load() {
    std::string savePath = path();
    std::ifstream file(savePath);
    
    if (!file.is_open()) {
        // Datei existiert nicht - das ist okay, verwende Standard-Werte
        m_data = SaveData{};  // Reset auf Standard
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Lade Daten aus JSON
        if (j.contains("coins")) m_data.coins = j["coins"];
        if (j.contains("lastLevel")) m_data.lastLevel = j["lastLevel"];
        if (j.contains("maxLives")) m_data.maxLives = j["maxLives"];
        
        // Lade Level-Progress
        if (j.contains("completedLevels")) {
            m_data.completedLevels = j["completedLevels"].get<std::vector<int>>();
        }
        
        if (j.contains("levelBestTimes")) {
            for (auto& [key, value] : j["levelBestTimes"].items()) {
                m_data.levelBestTimes[std::stoi(key)] = value;
            }
        }
        
        if (j.contains("levelStars")) {
            for (auto& [key, value] : j["levelStars"].items()) {
                m_data.levelStars[std::stoi(key)] = value;
            }
        }
        
        // Lade Cosmetics & Abilities
        if (j.contains("ownedCosmetics")) {
            m_data.ownedCosmetics = j["ownedCosmetics"].get<std::vector<std::string>>();
        }
        if (j.contains("unlockedAbilities")) {
            m_data.unlockedAbilities = j["unlockedAbilities"].get<std::vector<std::string>>();
        }
        
        return true;
    }
    catch (const std::exception& e) {
        // JSON parsing fehler - verwende Standard-Werte
        m_data = SaveData{};
        return false;
    }
}

void SaveManager::save() const {
    std::string savePath = path();
    
    json j;
    j["coins"] = m_data.coins;
    j["lastLevel"] = m_data.lastLevel;
    j["maxLives"] = m_data.maxLives;
    
    // Speichere Level-Progress
    j["completedLevels"] = m_data.completedLevels;
    
    // Speichere Level-Times (konvertiere int keys zu strings für JSON)
    json timesJson = json::object();
    for (const auto& [levelId, time] : m_data.levelBestTimes) {
        timesJson[std::to_string(levelId)] = time;
    }
    j["levelBestTimes"] = timesJson;
    
    // Speichere Level-Stars
    json starsJson = json::object();
    for (const auto& [levelId, stars] : m_data.levelStars) {
        starsJson[std::to_string(levelId)] = stars;
    }
    j["levelStars"] = starsJson;
    
    // Speichere Cosmetics & Abilities
    j["ownedCosmetics"] = m_data.ownedCosmetics;
    j["unlockedAbilities"] = m_data.unlockedAbilities;
    
    // Schreibe in Datei
    std::ofstream file(savePath);
    if (file.is_open()) {
        file << j.dump(4);  // Pretty-print with 4 spaces indent
        file.close();
    }
}