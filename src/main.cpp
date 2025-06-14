// src/main.cpp

#include "raylib.h"
#include "core/GameState.h"
#include "screens/LevelSelectScreen.h"
#include "core/LevelManager.h"
#include "screens/MenuScreen.h"
#include "screens/GameScreen.h"
#include <cmath>
#include "SpriteUtils.h"

// NEU: Definiere die "ideale" Auflösung für dein Spiel.
// Alle Positionen und Größen beziehen sich ab jetzt hierauf.
const int VIRTUAL_SCREEN_WIDTH = 1920;
const int VIRTUAL_SCREEN_HEIGHT = 1080;

int main()
{
    // --- Fenster initialisieren (wie bisher) ---
    SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
    int w = GetMonitorWidth(GetCurrentMonitor());
    int h = GetMonitorHeight(GetCurrentMonitor());
    InitWindow(w, h, "Jump-Run");
    InitAudioDevice();
    SetExitKey(0);
    SetTargetFPS(120);

    // NEU: Erstelle die virtuelle Leinwand (RenderTexture)
    RenderTexture2D target = LoadRenderTexture(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
    // Sorge dafür, dass die Textur schön gefiltert wird, wenn sie skaliert wird
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // --- Assets und Spielobjekte laden (wie bisher) ---
    Music backgroundMusic = LoadMusicStream("assets/sfx/intro-game-sound.wav");
    PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, 0.7f);

    GameState state = GameState::INTRO_MENU;

    // --- Menüs und Screens einrichten (wie bisher) ---
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
    mainMenu.load("", {{"Spiel starten", [&]{ state = GameState::PLAY; }}, {"Level auswählen", [&]{ state = GameState::LEVEL_SELECT; }}, {"Shop", [&]{ state = GameState::SHOP; }}, {"Zurück", [&]{ state = GameState::INTRO_MENU; }}});
    
    LevelSelectScreen levelSelect;
    int currentLevel = 0;
    levelSelect.setOnBack([&]{ state = GameState::MAIN_MENU; });
    levelSelect.setOnStart([&](int idx){ currentLevel = idx; state = GameState::PLAY; });
    levelSelect.load(&levelManager);

    GameScreen gameScreen;
    gameScreen.setOnFinish([&]{ state = GameState::MAIN_MENU; gameScreen.unload(); });
    // Wichtig: Lade das Level einmal initial, falls der Spieler direkt "Spiel starten" wählt
    gameScreen.load(&levelManager, currentLevel); 

    // --- Haupt-Schleife ---
    while (!WindowShouldClose())
    {
        UpdateMusicStream(backgroundMusic);
        
        // --- Update-Logik (wie bisher) ---
        // Hier bleibt alles gleich, die Logik ist von der Auflösung unabhängig.
        switch (state)
        {
            case GameState::INTRO_MENU:   introMenu.update(); break;
            case GameState::MAIN_MENU:    mainMenu.update(); break;
            case GameState::LEVEL_SELECT: levelSelect.update(); break;
            case GameState::PLAY:         gameScreen.update(); break;
            case GameState::SHOP: if (IsKeyPressed(KEY_BACKSPACE)) state = GameState::MAIN_MENU; break;
            default: break;
        }

        // --- ZEICHNEN - Teil 1: Alles auf die virtuelle Leinwand zeichnen ---
        BeginTextureMode(target);
            ClearBackground(DARKGRAY); // Hintergrund der Leinwand säubern

            // Dein gesamter Zeichen-Code kommt hier rein
            switch (state)
            {
                case GameState::INTRO_MENU:   introMenu.draw(); break;
                case GameState::MAIN_MENU:    mainMenu.draw(); break;
                case GameState::LEVEL_SELECT: levelSelect.draw(); break;
                case GameState::PLAY:         gameScreen.draw(); break;
                case GameState::SHOP:         DrawText("Shop (stub)", 400, 400, 40, RAYWHITE); break; // Feste Koordinaten!
                default: break;
            }
        EndTextureMode();

        // --- ZEICHNEN - Teil 2: Die fertige Leinwand auf den Bildschirm zeichnen ---
        BeginDrawing();
            ClearBackground(BLACK); // Echte Ränder werden schwarz

            // NEU: Berechne die Skalierung, um das Seitenverhältnis beizubehalten
            float scale = fmin((float)GetScreenWidth() / VIRTUAL_SCREEN_WIDTH, (float)GetScreenHeight() / VIRTUAL_SCREEN_HEIGHT);
            
            // NEU: Berechne Position, um die Leinwand zu zentrieren
            float destX = (GetScreenWidth() - ((float)VIRTUAL_SCREEN_WIDTH * scale)) * 0.5f;
            float destY = (GetScreenHeight() - ((float)VIRTUAL_SCREEN_HEIGHT * scale)) * 0.5f;

            // NEU: Definiere die Quell- und Ziel-Rechtecke für das Zeichnen
            Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }; // Y-Achse umdrehen!
            Rectangle destRec = { destX, destY, (float)VIRTUAL_SCREEN_WIDTH * scale, (float)VIRTUAL_SCREEN_HEIGHT * scale };

            // NEU: Zeichne die fertige Leinwand auf den Bildschirm
            DrawTexturePro(target.texture, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE);

        EndDrawing();
    }

    // --- Aufräumen ---
    UnloadRenderTexture(target); // Wichtig: Die Leinwand am Ende entladen
    introMenu.unload();
    mainMenu.unload();
    levelSelect.unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}