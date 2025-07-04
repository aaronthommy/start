// src/core/LevelManager.cpp

#include "core/LevelManager.h"
#include "nlohmann/json.hpp" // Wir brauchen die JSON-Bibliothek auch hier
#include <filesystem>        // Um Ordner zu durchsuchen
#include <fstream>           // Um Dateien zu lesen
#include <iostream>          // Für Debug-Ausgaben

// für den schnellen Zugriff
using json = nlohmann::json;
namespace fs = std::filesystem;

void LevelManager::init()
{
    levels.clear();
    std::string path = "data/levels"; // Der Ordner, in dem deine Level-JSONs liegen

    try
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                // Öffne die gefundene JSON-Datei
                std::ifstream f(entry.path());
                if (f.is_open())
                {
                    json data = json::parse(f);
                    
                    // Erstelle einen neuen LevelInfo-Eintrag
                    LevelInfo info;
                    info.jsonPath = entry.path().string(); // Speichere den kompletten Pfad
                    
                    // Lese den Namen direkt aus der JSON-Datei
                    if (data.contains("level_name")) {
                        info.name = data["level_name"];
                    } else {
                        info.name = "Unbenanntes Level";
                    }
                    
                    levels.push_back(info);
                    std::cout << "Level gefunden: " << info.name << " at " << info.jsonPath << std::endl;
                }
            }
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Fehler beim Zugriff auf den Level-Ordner: " << e.what() << std::endl;
    }
}

size_t LevelManager::count() const
{
    return levels.size();
}

const LevelInfo& LevelManager::get(int index) const
{
    // Hier könnte man noch eine Überprüfung einbauen, ob der Index gültig ist
    return levels[index];
}