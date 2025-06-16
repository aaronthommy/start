// include/core/LevelManager.h

#pragma once

#include <string>
#include <vector>

// Die Struktur, die alle Informationen über ein einzelnes Level enthält.
// Wir fügen hier den Pfad zur JSON-Datei hinzu.
struct LevelInfo
{
    std::string name;       // Name des Levels, z.B. "Grassy Plains"
    std::string jsonPath;   // Pfad zur Level-Datei, z.B. "data/levels/level-1.json"
};

class LevelManager
{
public:
    void init(); // Findet alle Level-Dateien
    
    size_t count() const;
    const LevelInfo& get(int index) const;

private:
    std::vector<LevelInfo> levels;
};