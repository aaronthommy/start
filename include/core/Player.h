// include/core/Player.h

#pragma once
#include "raylib.h"
#include <vector>

class Player {
public:
    Player();
    void load();
    void unload();
    void update(float delta, const std::vector<Rectangle>& platforms); 
    void draw() const;
    void reset();
    Rectangle getBounds() const;
    Vector2 getPosition() const;

private:
    Texture2D sprite;
    Vector2 position;
    Vector2 velocity;
    bool canJump;

    // Physik-Konstanten
    static constexpr float GRAVITY = 1200.0f;
    static constexpr float MOVE_SPEED = 300.0f;
    static constexpr float JUMP_SPEED = -600.0f;
};