#pragma once
#include "raylib.h"
enum class MenuResult { NONE, START_GAME, EXIT };

struct Menu {
    // Ressourcen
    Texture2D title{};
    Font      font{};
    Sound     sfxSelect{};
    // Laufzeit-Daten
    int   selected = 0;                 // Index des aktiven Buttons
    bool  usingMouse = false;

    void load();
    void unload();
    MenuResult update();                // Eingabe + Logik
    void draw() const;                  // Rendering
};