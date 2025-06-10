#include "raylib.h"
#include "core/GameState.h"
#include "screens/MenuScreen.h"
#include "SpriteUtils.h"

Texture2D loadSprite(const char *path, int frames);

int main()
{

    SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
    int w = GetMonitorWidth(GetCurrentMonitor());
    int h = GetMonitorHeight(GetCurrentMonitor());
    InitWindow(w, h, "Jump-Run");
    InitAudioDevice();

    Texture2D bg = LoadTexture("assets/ui/background.png");
    Texture2D comet = loadSprite("assets/sprites/fx/comet-left-100x100.png", 16);
    Texture2D duck = loadSprite("assets/sprites/enemies/duck-100x100.png", 20);
    Texture2D hero = loadSprite("assets/sprites/player/player.png", 20);
    float cometX = GetScreenWidth() + 64;

    SetTargetFPS(120);

    GameState state = GameState::INTRO_MENU;

    // ---------- Intro-Menu ---------- //
    MenuScreen introMenu;
    introMenu.load("assets/ui/title.png", {{"Start", [&]
                                            { state = GameState::MAIN_MENU; }},
                                           {"Optionen", [&] { /* später */ }}});

    MenuScreen::Style s;
    s.vGap = 60;
    s.origin = {0, 0}; // exakt zentriert
    introMenu.setStyle(s);

    // ---------- Haupt-Menü ---------- //
    MenuScreen mainMenu;
    mainMenu.setStyle(s);
    mainMenu.load("assets/ui/title.png", {{"Spiel starten", [&]
                                           { state = GameState::PLAY; }},
                                          {"Level auswählen", [&]
                                           { state = GameState::LEVEL_SELECT; }},
                                          {"Shop", [&]
                                           { state = GameState::SHOP; }},
                                          {"Zurück", [&]
                                           { state = GameState::INTRO_MENU; }}});

    while (!WindowShouldClose() && state != GameState::EXIT)
    {
        cometX -= 120 * GetFrameTime();
        if (cometX < -64)
            cometX = GetScreenWidth() + rand() % 200;

        BeginDrawing();
        ClearBackground(DARKGRAY);

        switch (state)
        {
        case GameState::INTRO_MENU:
            introMenu.update();
            introMenu.draw();
            break;

        case GameState::MAIN_MENU:
            mainMenu.update();
            mainMenu.draw();
            break;

        case GameState::LEVEL_SELECT:
            DrawText("Level-Select (stub)", 100, 100, 20, RAYWHITE);
            if (IsKeyPressed(KEY_BACKSPACE))
                state = GameState::MAIN_MENU;
            break;

        case GameState::SHOP:
            DrawText("Shop (stub)", 100, 100, 20, RAYWHITE);
            if (IsKeyPressed(KEY_BACKSPACE))
                state = GameState::MAIN_MENU;
            break;

        case GameState::PLAY:
            DrawText("Gameplay (stub)", 100, 100, 20, RAYWHITE);
            if (IsKeyPressed(KEY_ESCAPE))
                state = GameState::MAIN_MENU;
            break;

        default:
            break;
        }
        DrawTexturePro(bg,
                       {0, 0, (float)bg.width, -(float)bg.height},
                       {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                       {0, 0}, 0, WHITE);
        DrawTextureEx(comet, {cometX, 120}, -45, 6, WHITE); // diagonaler Flug
        DrawTextureEx(
            duck,
            {48.0f,
             static_cast<float>(GetScreenHeight()) - duck.height * 6.0f},
            0.0f, 6.0f, WHITE);
        DrawTextureEx(
            hero,
            {static_cast<float>(GetScreenWidth()) - hero.width * 6.0f - 48.0f,
             static_cast<float>(GetScreenHeight()) - hero.height * 6.0f},
            0.0f, 6.0f, WHITE);
        EndDrawing();
    }

    introMenu.unload();
    mainMenu.unload();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
