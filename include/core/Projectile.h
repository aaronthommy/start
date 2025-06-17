// include/core/Projectile.h
#pragma once
#include "raylib.h"

class Projectile {
public:
    Projectile(Vector2 startPos, Vector2 direction);
    void update(float delta);
    void draw() const;

    bool active = true;
    Vector2 position;
    Vector2 velocity;
    float radius = 5.0f;
    float lifetime = 2.0f; // in Sekunden
};