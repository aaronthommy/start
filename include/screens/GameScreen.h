// include/screens/GameScreen.h

#pragma once

#include "raylib.h"
#include "core/LevelManager.h"
#include "core/Player.h"
#include <functional>
#include "core/CombatSystem.h"
#include <vector>
#include "core/CoinManager.h"
#include "core/EnemyManager.h"
#include "core/SaveManager.h"
#include "core/Stats.h"

// Eine kleine Struktur, die eine Ebene unseres Parallax-Hintergrunds beschreibt
struct ParallaxLayer {
    Texture2D texture;
    float scrollSpeed;
};

class GameScreen {
public:
    GameScreen();
    ~GameScreen(); // Wichtig, um Texturen sauber zu entladen

    void load(LevelManager* levelManager, int levelIndex, SaveManager* sm, Stats* ps);
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
    CoinManager coinManager;
    EnemyManager enemyManager;
    SaveManager* saveManager = nullptr;
    Stats* playerStats = nullptr;
    
    std::vector<ParallaxLayer> backgroundLayers;
    std::vector<Rectangle> platforms;
    Texture2D coinIcon;
    std::function<void()> onFinish;
};