#pragma once
#include "raylib.h"

// Lädt einen Sprite-Streifen und liefert die Textur (Breite sofort pro Frame skaliert)
inline Texture2D loadSprite(const char* path, int frames)
{
    Texture2D tex = LoadTexture(path);
    tex.width /= frames;     // jeder Frame = Breite / frames
    return tex;
}