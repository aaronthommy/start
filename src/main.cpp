// src/main.cpp

#include "raylib.h"
#include "core/GameState.h"
#include "screens/LevelSelectScreen.h"
#include "core/LevelManager.h"
#include "screens/MenuScreen.h"
#include "screens/GameScreen.h"
#include "config.h"
#include "SpriteUtils.h"
#include <cmath>

int main()
{
    // --- Fenster & Audio Initialisierung (DEINE VERSION) ---
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, "Jump-Run");
    ToggleFullscreen(); // Auf echten Vollbildmodus umschalten
    
    // HINWEIS: InitWindow wird von raylib nur einmal effektiv ausgeführt. 
    // Der zweite Aufruf in deinem Originalcode war wahrscheinlich überflüssig, 
    // da ToggleFullscreen() das Fenster bereits an den Monitor anpasst. 
    // Wir lassen es bei dieser sauberen Sequenz.
    
    InitAudioDevice();
    SetExitKey(0);
    SetTargetFPS(120);

    // Virtuelle Leinwand für Letterboxing erstellen
    RenderTexture2D target = LoadRenderTexture(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    Music backgroundMusic = LoadMusicStream("assets/sfx/intro-game-sound.wav");
    PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, 0.7f);

    GameState state = GameState::INTRO_MENU;

    // --- Manager und Screens einrichten (REPARIERTE VERSION) ---
    LevelManager levelManager;
    levelManager.init();

    MenuScreen introMenu;
    introMenu.load("assets/ui/title.png", {{"Start", [&]{ state = GameState::MAIN_MENU; }}, {"Optionen", [&] {}}});

    MenuScreen::Style s;
    s.vGap = 70;
    s.origin = {0, -50};
    s.topMargin = 150.0f;
    s.normal = {200, 200, 200, 255};
    s.hover = {255, 220, 100, 255};
    s.active = {255, 150, 50, 255};
    introMenu.setStyle(s);

    MenuScreen mainMenu;
    mainMenu.setStyle(s);
    mainMenu.load("", {{"Spiel starten", [&]{ state = GameState::PLAY; }},
                       {"Level auswählen", [&]{ state = GameState::LEVEL_SELECT; }},
                       {"Shop", [&]{ state = GameState::SHOP; }},
                       {"Zurück", [&]{ state = GameState::INTRO_MENU; }}});

    GameScreen gameScreen;
    LevelSelectScreen levelSelect;
    int currentLevel = 0;

    levelSelect.setOnBack([&]{ state = GameState::MAIN_MENU; });
    
    levelSelect.setOnStart([&](int idx) {
        currentLevel = idx;
        gameScreen.load(&levelManager, currentLevel);
        state = GameState::PLAY;
    });
    levelSelect.load(&levelManager);

    gameScreen.setOnFinish([&] {
        gameScreen.unload();
        state = GameState::MAIN_MENU;
    });

    // --- Haupt-Schleife ---
    while (!WindowShouldClose())
    {
        UpdateMusicStream(backgroundMusic);

        // --- Update-Logik (REPARIERTE VERSION) ---
        switch (state)
        {
        case GameState::INTRO_MENU:
            introMenu.update();
            break;
        case GameState::MAIN_MENU:
            mainMenu.update();
            if (state == GameState::PLAY) {
                gameScreen.load(&levelManager, currentLevel); 
            }
            break;
        case GameState::LEVEL_SELECT:
            levelSelect.update();
            break;
        case GameState::PLAY:
            gameScreen.update();
            break;
        case GameState::SHOP:
            if (IsKeyPressed(KEY_BACKSPACE))
                state = GameState::MAIN_MENU;
            break;
        default:
            break;
        }

        // --- ZEICHNEN - Teil 1: Auf virtuelle Leinwand ---
        BeginTextureMode(target);
            ClearBackground(DARKGRAY);
            switch (state)
            {
            case GameState::INTRO_MENU:   introMenu.draw(); break;
            case GameState::MAIN_MENU:    mainMenu.draw(); break;
            case GameState::LEVEL_SELECT: levelSelect.draw(); break;
            case GameState::PLAY:         gameScreen.draw(); break;
            case GameState::SHOP:         DrawText("Shop (stub)", 400, 400, 40, RAYWHITE); break;
            default: break;
            }
        EndTextureMode();

        // --- ZEICHNEN - Teil 2: Leinwand auf Bildschirm (DEINE VERSION) ---
        BeginDrawing();
            ClearBackground(BLACK);
            float scale = fmin((float)GetScreenWidth() / VIRTUAL_SCREEN_WIDTH, (float)GetScreenHeight() / VIRTUAL_SCREEN_HEIGHT);
            float destX = (GetScreenWidth() - ((float)VIRTUAL_SCREEN_WIDTH * scale)) * 0.5f;
            float destY = (GetScreenHeight() - ((float)VIRTUAL_SCREEN_HEIGHT * scale)) * 0.5f;
            Rectangle sourceRec = {0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height};
            Rectangle destRec = {destX, destY, (float)VIRTUAL_SCREEN_WIDTH * scale, (float)VIRTUAL_SCREEN_HEIGHT * scale};
            DrawTexturePro(target.texture, sourceRec, destRec, {0, 0}, 0.0f, WHITE);
        EndDrawing();
    }

    // --- Aufräumen ---
    UnloadRenderTexture(target);
    UnloadMusicStream(backgroundMusic);
    introMenu.unload();
    mainMenu.unload();
    levelSelect.unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}