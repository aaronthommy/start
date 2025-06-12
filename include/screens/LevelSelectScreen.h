#pragma once
#include "screens/MenuScreen.h"
#include "core/LevelManager.h"
#include <functional>

class LevelSelectScreen {
public:
    void load(LevelManager* manager);
    void unload();
    void update();
    void draw() const;

    void setOnBack(std::function<void()> cb) { onBack = std::move(cb); }
    void setOnStart(std::function<void(int)> cb) { onStart = std::move(cb); }

private:
    MenuScreen menu;
    LevelManager* levelMgr = nullptr;
    std::function<void()> onBack;
    std::function<void(int)> onStart;
};