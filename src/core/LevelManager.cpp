#include "core/LevelManager.h"

void LevelManager::init()
{
    m_levels.clear();
    for (int i = 1; i <= 10; ++i) {
        LevelInfo info;
        info.id = i;
        info.name = "Level " + std::to_string(i);
        info.file = "data/levels/level" + std::to_string(i) + ".json";
        m_levels.push_back(info);
    }
}

const LevelInfo& LevelManager::get(int index) const
{
    return m_levels.at(index);
}

int LevelManager::count() const
{
    return static_cast<int>(m_levels.size());
}