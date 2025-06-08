#include "menu.h"
#include <array>

static constexpr std::array<const char*, 3> ITEMS = { "Start", "Optionen", "Beenden" };

void Menu::load() {
    title     = LoadTexture("assets/ui/title.png");
    font = LoadFont("assets/fonts/first_font.ttf");
    sfxSelect = LoadSound("assets/sfx/select_menu.wav");
}

void Menu::unload() {
    UnloadTexture(title);
    UnloadFont(font);
    UnloadSound(sfxSelect);
}

MenuResult Menu::update() {
    // --- Tastatur / Gamepad ---
    if (IsKeyPressed(KEY_UP))   { selected = (selected + ITEMS.size()-1) % ITEMS.size(); PlaySound(sfxSelect); }
    if (IsKeyPressed(KEY_DOWN)) { selected = (selected + 1) % ITEMS.size(); PlaySound(sfxSelect); }
    if (IsKeyPressed(KEY_ENTER)) {
        if      (selected == 0) return MenuResult::START_GAME;
        else if (selected == 2) return MenuResult::EXIT;
    }

    // --- Maus ---
    Vector2 m = GetMousePosition();
    usingMouse = IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    for (size_t i = 0; i < ITEMS.size(); ++i) {
        Rectangle r{ 400, 300 + (float)i*60, 200, 48 };
        if (CheckCollisionPointRec(m, r)) {
            selected = (int)i;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(sfxSelect);
                if      (i == 0) return MenuResult::START_GAME;
                else if (i == 2) return MenuResult::EXIT;
            }
        }
    }
    return MenuResult::NONE;
}

void Menu::draw() const {
    DrawTexture(title, 260, 80, WHITE);
    for (size_t i = 0; i < ITEMS.size(); ++i) {
        Color c = (i == selected) ? YELLOW : RAYWHITE;
        DrawTextEx(font, ITEMS[i], { 420, 310 + (float)i*60 }, 32, 0, c);
    }
}
