// include/screens/GameScreen.h

#pragma once

#include "raylib.h"
#include "core/LevelManager.h"
#include <functional>

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
    Texture2D background;
    std::function<void()> onFinish;
    // Hier kommen später Spieler, Gegner etc. hinzu
};