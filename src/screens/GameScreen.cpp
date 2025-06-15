// src/screens/GameScreen.cpp

#include "screens/GameScreen.h"
#include "config.h" // <-- WICHTIG: Einbinden

GameScreen::GameScreen()
{
    camera = { 0 };
    // Der Offset sorgt dafür, dass die Kamera den Spieler in der Mitte des Bildschirms hält
    camera.offset = { (float)VIRTUAL_SCREEN_WIDTH / 2.0f, (float)VIRTUAL_SCREEN_HEIGHT / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void GameScreen::load(LevelManager *levelManager, int levelIndex)
{
    levelMgr = levelManager;
    currentLevel = levelIndex;

    const LevelInfo &info = levelMgr->get(currentLevel);

    // KORREKTUR: Lade den Hintergrund aus den Level-Daten
    background = LoadTexture(info.backgroundPath.c_str());

    player.reset(); // Spieler auf Startposition zurücksetzen
    player.load();
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
        return;
    }

    player.update(GetFrameTime());

    camera.target = player.getPosition();
}

void GameScreen::draw() const
{

    BeginMode2D(camera);
    // KORREKTUR: Zeichne den Hintergrund auf die virtuelle Leinwand
    DrawTexturePro(background,
                   {0, 0, (float)background.width, (float)background.height},
                   {0, 0, (float)VIRTUAL_SCREEN_WIDTH, (float)VIRTUAL_SCREEN_HEIGHT},
                   {0, 0}, 0, WHITE);

    player.draw();

    EndMode2D();

    if (levelMgr && currentLevel != -1)
    {
        const LevelInfo &info = levelMgr->get(currentLevel);
        DrawText(TextFormat("Du spielst: %s", info.name.c_str()),
                 100, 100, 40, GOLD);
    }

    DrawText("Drücke ESC, um zum Menü zurückzukehren.", 100, 150, 20, RAYWHITE);
}