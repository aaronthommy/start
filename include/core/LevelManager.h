#pragma once
#include <string>
#include <vector>

struct LevelInfo {
    int id = 0;
    std::string name;
    std::string file;
    std::string backgroundPath; // <-- Hinzugefügt
};

class LevelManager {
public:
    void init();
    const LevelInfo& get(int index) const;
    int count() const;
private:
    std::vector<LevelInfo> m_levels;
};