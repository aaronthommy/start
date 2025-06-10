#pragma once
#include "raylib.h"
#include <iostream>

// Lädt einen Sprite-Streifen OHNE die Breite zu verändern
inline Texture2D loadSprite(const char* path, int frames)
{
    Texture2D tex = LoadTexture(path);
    if (tex.id == 0) {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    // NICHT tex.width ändern! Das verfälscht die Texture
    return tex;
}

// Hilfsfunktion für animierte Sprites
inline Rectangle getSpriteFrame(const Texture2D& tex, int frame, int totalFrames)
{
    float frameWidth = (float)tex.width / totalFrames;
    return Rectangle{
        frameWidth * frame, 0,
        frameWidth, (float)tex.height
    };
}