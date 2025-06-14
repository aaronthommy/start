// src/main.cpp

#include "raylib.h"
#include <cmath>
#include "config.h"             // Für VIRTUAL_SCREEN_WIDTH/HEIGHT
#include "core/GameState.h"
#include "screens/MenuScreen.h"
#include "screens/GameScreen.h"
#include "core/LevelManager.h"
#include "screens/LevelSelectScreen.h"
#include "SpriteUtils.h"

int main(void)
{
    // =========================================================================
    // 1. FENSTER INITIALISIEREN (Sauber und stabil)
    // =========================================================================
    // Wir starten in einem Fenster, das wir später auf Vollbild schalten können.
    // Das ist die stabilste Methode.
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, "Jump-Run");
    SetWindowMinSize(VIRTUAL_SCREEN_WIDTH / 2, VIRTUAL_SCREEN_HEIGHT / 2);
    SetExitKey(KEY_NULL);
    SetTargetFPS(120);

    // Erstelle die virtuelle Leinwand (RenderTexture)
    RenderTexture2D target = LoadRenderTexture(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    InitAudioDevice();

    // =========================================================================
    // 2. ASSETS UND SCREENS LADEN
    // =========================================================================
    Music backgroundMusic = LoadMusicStream("assets/sfx/intro-game-sound.wav");
    PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, 0.7f);

    GameState state = GameState::INTRO_MENU;
    LevelManager levelManager;
    levelManager.init();

    MenuScreen introMenu;
    MenuScreen::Style s;
    s.vGap = 70;
    s.origin = {0, -50};
    s.topMargin = 150.0f;
    s.normal = {200, 200, 200, 255};
    s.hover = {255, 220, 100, 255};
    s.active = {255, 150, 50, 255};
    introMenu.setStyle(s);
    introMenu.load("assets/ui/title.png", {{"Start", [&]{ state = GameState::MAIN_MENU; }}, {"Optionen", [&] {}}});

    MenuScreen mainMenu;
    mainMenu.setStyle(s);
    mainMenu.load("", {{"Spiel starten", [&]{ state = GameState::PLAY; }}, {"Level auswählen", [&]{ state = GameState::LEVEL_SELECT; }}, {"Shop", [&]{ state = GameState::SHOP; }}, {"Zurück", [&]{ state = GameState::INTRO_MENU; }}});
    
    LevelSelectScreen levelSelect;
    int currentLevel = 0;
    levelSelect.setOnBack([&]{ state = GameState::MAIN_MENU; });
    levelSelect.setOnStart([&](int idx){ currentLevel = idx; state = GameState::PLAY; });
    levelSelect.load(&levelManager);

    GameScreen gameScreen;
    gameScreen.setOnFinish([&]{ state = GameState::MAIN_MENU; gameScreen.unload(); });
    gameScreen.load(&levelManager, currentLevel);

    // =========================================================================
    // 3. HAUPT-SCHLEIFE
    // =========================================================================
    while (!WindowShouldClose())
    {
        // --- UPDATE ---
        // (Update-Logik bleibt unverändert)
        UpdateMusicStream(backgroundMusic);
        switch (state) {
            case GameState::INTRO_MENU:   introMenu.update(); break;
            case GameState::MAIN_MENU:    mainMenu.update(); break;
            case GameState::LEVEL_SELECT: levelSelect.update(); break;
            case GameState::PLAY:         gameScreen.update(); break;
            case GameState::SHOP:         if (IsKeyPressed(KEY_BACKSPACE)) state = GameState::MAIN_MENU; break;
            default: break;
        }

        // --- ZEICHNEN AUF VIRTUELLE LEINWAND ---
        BeginTextureMode(target);
            ClearBackground(DARKGRAY); // Hintergrund der virtuellen Leinwand
            // (Hier kommt dein gesamter Spiel-Zeichencode rein)
             switch (state) {
                case GameState::INTRO_MENU:   introMenu.draw(); break;
                case GameState::MAIN_MENU:    mainMenu.draw(); break;
                case GameState::LEVEL_SELECT: levelSelect.draw(); break;
                case GameState::PLAY:         gameScreen.draw(); break;
                case GameState::SHOP:         DrawText("Shop (stub)", 400, 400, 40, RAYWHITE); break;
                default: break;
            }
        EndTextureMode();

        // --- ZEICHNEN DER LEINWAND AUF DEN BILDSCHIRM ---
        BeginDrawing();
            ClearBackground(BLACK); // Farbe für die Ränder (Letterbox/Pillarbox)

            // "Fit inside"-Skalierung, die das Seitenverhältnis beibehält
            float scale = fmin((float)GetScreenWidth() / VIRTUAL_SCREEN_WIDTH, (float)GetScreenHeight() / VIRTUAL_SCREEN_HEIGHT);
            float destX = (GetScreenWidth() - (VIRTUAL_SCREEN_WIDTH * scale)) * 0.5f;
            float destY = (GetScreenHeight() - (VIRTUAL_SCREEN_HEIGHT * scale)) * 0.5f;
            Rectangle source = { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height };
            Rectangle dest = { destX, destY, VIRTUAL_SCREEN_WIDTH * scale, VIRTUAL_SCREEN_HEIGHT * scale };
            
            DrawTexturePro(target.texture, source, dest, { 0, 0 }, 0.0f, WHITE);

            // Optional: Visuelles Debugging, um die Skalierung zu sehen
            // DrawRectangleLinesEx(dest, 2.0, RED);
            // DrawFPS(10, 10);

        EndDrawing();
    }

    // =========================================================================
    // 4. AUFRÄUMEN
    // =========================================================================
    UnloadRenderTexture(target);
    UnloadMusicStream(backgroundMusic);
    introMenu.unload();
    mainMenu.unload();
    levelSelect.unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}