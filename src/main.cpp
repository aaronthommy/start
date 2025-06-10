#include "raylib.h"
#include "core/GameState.h"
#include "screens/MenuScreen.h"
#include "SpriteUtils.h"

int main()
{

    SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
    int w = GetMonitorWidth(GetCurrentMonitor());
    int h = GetMonitorHeight(GetCurrentMonitor());
    InitWindow(w, h, "Jump-Run");
    InitAudioDevice();

    Texture2D bg = LoadTexture("assets/ui/background.png");
    Texture2D comet = loadSprite("assets/sprites/fx/comet-right-100x100.png", 16);
    Texture2D duck = loadSprite("assets/sprites/enemies/duck-100x100.png", 20);
    Texture2D hero = loadSprite("assets/sprites/player/player.png", 20);
    float cometX = GetScreenWidth() + 64;

    SetTargetFPS(120);

    int cometFrame = 0;
    int duckFrame = 0;
    int heroFrame = 0;
    float animTime = 0.0f;

    GameState state = GameState::INTRO_MENU;

    // ---------- Intro-Menu ---------- //
    MenuScreen introMenu;
    introMenu.load("assets/ui/title.png", {{"Start", [&]
                                            { state = GameState::MAIN_MENU; }},
                                           {"Optionen", [&] { /* später */ }}});

    MenuScreen::Style s;
    s.vGap = 70;                     // Mehr Platz zwischen Einträgen
    s.origin = {0, -50};             // Leicht nach oben verschieben
    s.normal = {200, 200, 200, 255}; // Hellgrau statt weiß
    s.hover = {255, 220, 100, 255};  // Warmes Gelb
    s.active = {255, 150, 50, 255};  // Orange
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
        animTime += GetFrameTime();
        if (animTime >= 0.1f)
        { // 10 FPS für Animationen
            animTime = 0.0f;
            cometFrame = (cometFrame + 1) % 16;
            duckFrame = (duckFrame + 1) % 20;
            heroFrame = (heroFrame + 1) % 20;
        }
        ClearBackground(DARKGRAY);
        DrawTexturePro(bg,
                       {0, 0, (float)bg.width, (float)bg.height},
                       {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                       {0, 0}, 0, WHITE);

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

        // Animationen
        Rectangle cometSrc = {(float)(cometFrame * 100), 0, 100, 100};
        DrawTexturePro(comet, cometSrc,
                       {cometX, 120, 300, 300}, // 3x Skalierung
                       {150, 150}, -45, WHITE);

        // Duck animiert
        Rectangle duckSrc = {(float)(duckFrame * 100), 0, 100, 100};
        DrawTexturePro(duck, duckSrc,
                       {48, (float)(GetScreenHeight() - 300), 300, 300},
                       {0, 0}, 0, WHITE);

        // Hero animiert
        Rectangle heroSrc = {(float)(heroFrame * 100), 0, 100, 100};
        DrawTexturePro(hero, heroSrc,
                       {(float)(GetScreenWidth() - 348), (float)(GetScreenHeight() - 300), 300, 300},
                       {0, 0}, 0, WHITE);

        EndDrawing();
    }

    introMenu.unload();
    mainMenu.unload();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
