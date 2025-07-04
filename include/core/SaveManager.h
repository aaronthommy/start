#pragma once
#include <string>
#include <vector>

struct SaveData {
    int coins = 0;
    int lastLevel = 0;
    int maxLives  = 3;

    std::vector<std::string> ownedCosmetics;
    std::vector<std::string> unlockedAbilities;
};


class SaveManager {
public:
    bool load();
    void save() const;
    SaveData& data();
private:
    SaveData m_data;
    std::string path() const;   // Plattformabhängig
};
