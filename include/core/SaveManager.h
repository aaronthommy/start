#pragma once
#include <string>
#include <vector>
#include <map>        // <-- FEHLT!
#include <algorithm> 

struct SaveData {
    int coins = 0;
    int lastLevel = 0;
    int maxLives  = 3;

    std::vector<int> completedLevels;        // Liste abgeschlossener Level-IDs
    std::map<int, float> levelBestTimes;     // Level-ID → beste Zeit
    std::map<int, int> levelStars;      

    std::vector<std::string> ownedCosmetics;
    std::vector<std::string> unlockedAbilities;

    bool isLevelCompleted(int levelIndex) const;
    bool isLevelUnlocked(int levelIndex) const;
    void completeLevel(int levelIndex, float time);
};


class SaveManager {
public:
    bool load();
    void save() const;
    SaveData& data();
    void reset();
private:
    SaveData m_data;
    std::string path() const;   // Plattformabhängig
};
