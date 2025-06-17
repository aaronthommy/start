#pragma once
#include "raylib.h"
#include <vector>

class Coin {
public:
    Vector2 position;
    float radius = 16.0f;
    bool collected = false;
    
    // Animation
    float animTime = 0.0f;
    float collectAnimTime = 0.0f;
    
    Coin(float x, float y) : position{x, y} {}
    
    void update(float deltaTime);
    void draw(Texture2D coinSprite) const;
    bool checkCollision(Rectangle playerRect) const;
    void collect();
};
