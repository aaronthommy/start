// src/core/Projectile.cpp
#include "core/Projectile.h"
#include "raylib.h"
#include "raymath.h" 


const float PROJECTILE_SPEED = 800.0f;

Projectile::Projectile(Vector2 startPos, Vector2 direction) {
    position = startPos;
    velocity = Vector2Scale(Vector2Normalize(direction), PROJECTILE_SPEED);
}

void Projectile::update(float delta) {
    if (!active) return;
    lifetime -= delta;
    if (lifetime <= 0) {
        active = false;
    }
    position.x += velocity.x * delta;
    position.y += velocity.y * delta;
}

void Projectile::draw() const {
    if (active) {
        DrawCircleV(position, radius, GOLD);
    }
}