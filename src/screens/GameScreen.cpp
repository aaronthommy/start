// src/screens/GameScreen.cpp

#include "screens/GameScreen.h"
#include "config.h" // <-- WICHTIG: Einbinden

GameScreen::GameScreen()
{
    camera = {0};
    // Der Offset sorgt dafür, dass die Kamera den Spieler in der Mitte des Bildschirms hält
    camera.offset = {(float)VIRTUAL_SCREEN_WIDTH / 2.0f, (float)VIRTUAL_SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void GameScreen::load(LevelManager *levelManager, int levelIndex)
{
    levelMgr = levelManager;
    currentLevel = levelIndex;

    const LevelInfo &info = levelMgr->get(currentLevel);

    backgroundSky = LoadTexture("assets/graphics/backgrounds/sky.png");
    backgroundHills = LoadTexture("assets/graphics/backgrounds/low_mountains.png");

    player.reset(); // Spieler auf Startposition zurücksetzen
    player.load();

    platforms.clear();                        // Alte Plattformen löschen
    platforms.push_back({0, 800, 2000, 200}); // Langer Boden
    platforms.push_back({400, 650, 250, 50}); // Eine schwebende Plattform
    platforms.push_back({800, 550, 250, 50}); // Noch eine...
    platforms.push_back({1200, 450, 150, 50});
}

void GameScreen::unload()
{
    UnloadTexture(backgroundSky);
    UnloadTexture(backgroundHills);
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

    player.update(GetFrameTime(), platforms);

    camera.target = player.getPosition();
}

void GameScreen::draw() const
{

    BeginMode2D(camera);

    for (const auto &platform : platforms)
    {
        DrawRectangleRec(platform, GRAY);
    }

    Rectangle skySource = {camera.target.x * 0.1f, 0, (float)VIRTUAL_SCREEN_WIDTH, (float)backgroundSky.height};
    Rectangle skyDest = {camera.target.x - VIRTUAL_SCREEN_WIDTH / 2.0f, 0, (float)VIRTUAL_SCREEN_WIDTH, (float)VIRTUAL_SCREEN_HEIGHT};
    DrawTexturePro(backgroundSky, skySource, skyDest, {0, 0}, 0, WHITE);

    Rectangle hillsSource = {camera.target.x * 0.5f, 0, (float)VIRTUAL_SCREEN_WIDTH, (float)backgroundHills.height};
    Rectangle hillsDest = {camera.target.x - VIRTUAL_SCREEN_WIDTH / 2.0f, 0, (float)VIRTUAL_SCREEN_WIDTH, (float)VIRTUAL_SCREEN_HEIGHT};
    DrawTexturePro(backgroundHills, hillsSource, hillsDest, {0, 0}, 0, WHITE);

    for (const auto &platform : platforms)
    {
        DrawRectangleRec(platform, DARKGRAY); // Geändert zu DARKGRAY für besseren Kontrast
    }

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