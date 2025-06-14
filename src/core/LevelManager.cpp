#include "core/LevelManager.h"

void LevelManager::init()
{
    m_levels.clear();
    for (int i = 1; i <= 6; ++i) {
        LevelInfo info;
        info.id = i;
        info.name = "Level " + std::to_string(i);
        info.file = "data/levels/level" + std::to_string(i) + ".json";
        
        // Hinzugefügt: Weise jedem Level einen Hintergrund zu.
        // Du kannst hier für jedes Level einen anderen Pfad verwenden,
        // z.B. "assets/ui/background-level1.png" etc.
        info.backgroundPath = "assets/ui/background.png"; 
        
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