#include "raylib.h"
#pragma once



class GoalFlag {
public:
    GoalFlag(Vector2 position);
    ~GoalFlag();

    
    void update(float deltaTime);
    void draw() const;
    Rectangle getBounds() const;
    bool isPlayerTouching(const Rectangle& playerBounds) const;
    
private:
    Vector2 pos;
    Vector2 size = {80, 120};
    Texture2D flagTexture;
    float animationTime = 0.0f;
    bool collected = false;
};