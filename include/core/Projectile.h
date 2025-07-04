// include/core/Projectile.h
#pragma once
#include "raylib.h"

class Projectile {
public:
    Projectile(Vector2 startPos, Vector2 direction);
    void update(float delta);
    void draw() const;

    // Statische Funktionen, die einmalig vom GameScreen aufgerufen werden

    Rectangle getBounds() const;
    
    static void loadTexture();
    static void unloadTexture();

    bool active = true;
    Vector2 position;
    Vector2 velocity;
    Vector2 size = {32, 32}; 

private:
    // Die Textur ist 'static', d.h. alle Projektile teilen sich dieselbe.
    static Texture2D spriteSheet; 

    // KORREKTUR: Das Orb-Sprite-Sheet hat 4 Frames.
    int frameCount = 4;
    int currentFrame = 0;

    // KORREKTUR: Der Timer muss bei 0 starten.
    float frameTimer = 0.0f; 
    
    float frameSpeed = 1.0f / 15.0f; // 15 FPS
    float lifetime = 2.0f;
};