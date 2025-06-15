// include/screens/GameScreen.h

#pragma once

#include "raylib.h"
#include "core/LevelManager.h"
#include <functional>
#include "core/Player.h" 

class GameScreen {
public:
    GameScreen();
    void load(LevelManager* levelManager, int levelIndex);
    void unload();
    void update();
    void draw() const;

    // Callback, um zum Menü zurückzukehren
    void setOnFinish(std::function<void()> cb) { onFinish = std::move(cb); }

private:
    LevelManager* levelMgr = nullptr;
    int currentLevel = -1;
    Texture2D backgroundSky;   
    Texture2D backgroundHills;
    Camera2D camera;
    std::vector<Rectangle> platforms;
    std::function<void()> onFinish;
    Player player; 
};