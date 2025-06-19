#pragma once
#include "raylib.h"

class Projectile {
public:
    Projectile(Vector2 startPos, Vector2 direction);
    void update(float delta);
    void draw() const;

    // Statische Funktionen, die einmalig vom GameScreen aufgerufen werden
    static void loadTexture();
    static void unloadTexture();

    bool active = true;
    Vector2 position;
    Vector2 velocity;
    Vector2 size = {32, 32}; 

private:
    static Texture2D spriteSheet; 
    int frameCount = 4;
    int currentFrame = 0;
    float frameTimer = 0.0f;
    float frameSpeed = 1.0f / 15.0f;
    float lifetime = 2.0f;
};