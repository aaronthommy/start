#pragma once

#include "raylib.h"
#include "core/LevelManager.h"
#include "core/Player.h"
#include "core/CombatSystem.h"
#include "core/Enemy.h" // <-- NEU
#include "nlohmann/json.hpp" // <-- NEU

#include <functional>
#include <vector>
#include <string>
#include <map> 

struct ParallaxLayer {
    Texture2D texture;
    float scrollSpeed;
};

struct Platform {
    Rectangle bounds;
    std::string textureId;
};

class GameScreen {
public:
    GameScreen();
    ~GameScreen();

    void load(LevelManager* levelManager, int levelIndex);
    void unload();
    void update();
    void draw() const;

    void setOnFinish(std::function<void()> cb) { onFinish = std::move(cb); }

private:
    LevelManager* levelMgr = nullptr;
    int currentLevel = -1;
    Player player;
    Camera2D camera;
    CombatSystem combatSystem;
    
    std::vector<ParallaxLayer> backgroundLayers;
    std::vector<Platform> platforms;
    
    // NEU: Ein Vektor, der alle Gegner-Objekte im Level besitzt.
    std::vector<Enemy> enemies; 
    
    // NEU: Eine Map, die die JSON-Definitionen für jeden Gegnertyp speichert.
    std::map<std::string, nlohmann::json> enemyDefinitions;

    std::map<std::string, Texture2D> platformTextures; 
    std::function<void()> onFinish;
};
