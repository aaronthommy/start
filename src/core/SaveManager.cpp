#include "core/SaveManager.h"
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

std::string SaveManager::path() const {
    // Plattformabhängiger Pfad
    #ifdef _WIN32
        return "save.json";
    #else
        return "save.json"; // Für Linux/Mac anpassen
    #endif
}

bool SaveManager::load() {
    try {
        std::ifstream file(path());
        if (!file.is_open()) return false;
        
        json j;
        file >> j;
        
        m_data.coins = j.value("coins", 0);
        m_data.lastLevel = j.value("lastLevel", 0);
        m_data.maxLives = j.value("maxLives", 3);
        
        if (j.contains("abilities")) {
            m_data.unlockedAbilities = j["abilities"].get<std::vector<std::string>>();
        }
        
        if (j.contains("cosmetics")) {
            m_data.ownedCosmetics = j["cosmetics"].get<std::vector<std::string>>();
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

void SaveManager::save() const {
    json j;
    j["coins"] = m_data.coins;
    j["lastLevel"] = m_data.lastLevel;
    j["maxLives"] = m_data.maxLives;
    j["abilities"] = m_data.unlockedAbilities;
    j["cosmetics"] = m_data.ownedCosmetics;
    
    std::ofstream file(path());
    file << j.dump(4);
}

SaveData& SaveManager::data() {
    return m_data;
}