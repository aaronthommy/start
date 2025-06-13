// src/screens/GameScreen.cpp

#include "screens/GameScreen.h"

GameScreen::GameScreen()
{
    // Konstruktor
}

void GameScreen::load(LevelManager *levelManager, int levelIndex)
{
    levelMgr = levelManager;
    currentLevel = levelIndex;

    const LevelInfo &info = levelMgr->get(currentLevel);

    // Lade spezifische Level-Assets
    // Fürs Erste nehmen wir den generischen Hintergrund
    background = LoadTexture("assets/ui/background.png");

    player.load();

    // Hier würdest du später das Level aus info.filePath laden
}

void GameScreen::unload()
{
    UnloadTexture(background);
    player.unload();
}

void GameScreen::update()
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (onFinish)
        {
            onFinish();
        }
        return; // Verhindert Spieler-Update nach Verlassen des Screens
    }

    player.update(GetFrameTime()); // <-- HIER: Spieler-Logik aufrufen
}

void GameScreen::draw() const
{
    // Hintergrund zeichnen
    DrawTexturePro(background,
                   {0, 0, (float)background.width, (float)background.height},
                   {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                   {0, 0}, 0, WHITE);

    player.draw();

    // Aktuellen Levelnamen anzeigen
    if (levelMgr && currentLevel != -1)
    {
        const LevelInfo &info = levelMgr->get(currentLevel);
        DrawText(TextFormat("Du spielst: %s", info.name.c_str()),
                 100, 100, 40, GOLD);
    }

    DrawText("Drücke ESC, um zum Menü zurückzukehren.", 100, 150, 20, RAYWHITE);
}