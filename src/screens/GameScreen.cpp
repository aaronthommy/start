// src/screens/GameScreen.cpp

#include "screens/GameScreen.h"
#include "config.h"
#include "core/Projectile.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <cmath>
#include "entities/Coin.h"
#include "core/CoinManager.h"

using json = nlohmann::json;

// Die Methoden GameScreen(), ~GameScreen(), load(), unload() und update()
// bleiben exakt so, wie sie in der letzten Antwort waren.
// Ich füge sie hier der Vollständigkeit halber nochmal ein.

GameScreen::GameScreen()
{
    camera = {0};
    camera.offset = {(float)VIRTUAL_SCREEN_WIDTH / 2.0f, (float)VIRTUAL_SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

GameScreen::~GameScreen()
{
    unload();
}

void GameScreen::load(LevelManager *levelManager, int levelIndex, SaveManager *sm, Stats *ps)
{
    levelMgr = levelManager;
    currentLevel = levelIndex;

    saveManager = sm;
    playerStats = ps;

    const LevelInfo &info = levelMgr->get(levelIndex);
    std::ifstream f(info.jsonPath);
    if (!f.is_open())
    {
        TraceLog(LOG_ERROR, "Konnte Level-JSON nicht öffnen: %s", info.jsonPath.c_str());
        return;
    }
    json data = json::parse(f);

    backgroundLayers.clear();
    for (const auto &layerData : data["background_layers"])
    {
        backgroundLayers.push_back({LoadTexture(std::string(layerData["path"]).c_str()),
                                    layerData["scroll_speed"]});
    }

    player.load();
    coinManager.init();
    enemyManager.init();
    Projectile::loadTexture();

    coinIcon = LoadTexture("assets/ui/coin.png");

    player.setPosition({data["player_start"]["x"], data["player_start"]["y"]});
    camera.target = player.getPosition();

    combatSystem.registerPlayer(&player);

    platforms.clear();
    for (const auto &pData : data["platforms"])
    {
        platforms.push_back({pData["x"], pData["y"], pData["width"], pData["height"]});
    }

    std::vector<Vector2> coinPositions;
    std::vector<EnemySpawn> enemySpawns;

    // Beispiel für Level 0 - du musst das für jedes Level anpassen
    if (levelIndex == 0)
    {
        // Münzen
        coinPositions.push_back({200, 400});
        coinPositions.push_back({300, 400});
        coinPositions.push_back({400, 350});
        coinPositions.push_back({500, 300});

        // Gegner
        enemySpawns.push_back({Enemy::Type::WALKER, {600, 400}, {800, 400}});
        enemySpawns.push_back({Enemy::Type::SHOOTER, {900, 350}, {0, 0}});
    }
    else if (levelIndex == 1)
    {
        // Level 1 Münzen und Gegner
        coinPositions.push_back({150, 450});
        coinPositions.push_back({250, 400});
        // ... mehr Positionen

        enemySpawns.push_back({Enemy::Type::JUMPER, {400, 400}, {0, 0}});
        enemySpawns.push_back({Enemy::Type::CHASER, {700, 350}, {0, 0}});
    }
    // ... weitere Level

    coinManager.loadLevel(coinPositions);
    enemyManager.loadLevel(enemySpawns);
}

void GameScreen::unload()
{
    player.unload();
    Projectile::unloadTexture();
    UnloadTexture(coinIcon);
    coinManager.clear();
    enemyManager.clear();
    for (auto &layer : backgroundLayers)
    {
        UnloadTexture(layer.texture);
    }
    backgroundLayers.clear();
}

void GameScreen::update()
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (onFinish)
            onFinish();
        return;
    }
    player.update(GetFrameTime(), platforms);
    Rectangle playerRect = player.getHitbox();
    camera.target = player.getPosition();
    coinManager.update(GetFrameTime(), playerRect, playerStats->coins);

    // Gegner updaten (du brauchst getTileCollisions() in deinem Level-System)
    enemyManager.update(GetFrameTime(), player.getPosition(), nullptr); // oder level->getTileCollisions()

    // Kollisionen prüfen
    if (enemyManager.checkPlayerCollision(playerRect, playerStats->currentHP))
    {
        // player.takeDamage(); // Du musst takeDamage() in Player implementieren
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouseTarget = GetScreenToWorld2D(GetMousePosition(), camera);
        player.usePrimaryAbility(combatSystem, mouseTarget);
    }
    combatSystem.update(GetFrameTime());

    if (coinManager.getCollectedCount() >= coinManager.getTotalCount())
    {
        // Bonus für alle Münzen
        playerStats->coins += 50;
    }

    // Speichern nach wichtigen Ereignissen
    if (coinManager.getCollectedCount() > 0)
    {
        saveManager->data().coins = playerStats->coins;
        saveManager->save();
    }
}

// HIER IST DIE NEUE, KORRIGIERTE ZEICHENMETHODE
void GameScreen::draw() const
{
    ClearBackground(BLACK); // Füllt den echten Bildschirm mit Schwarz

    // --- PARALLAX-EBENEN ZEICHNEN ---
    for (const auto &layer : backgroundLayers)
    {
        // Berechne die Skalierung, um die Höhe des Bildschirms zu füllen
        float scale = (float)VIRTUAL_SCREEN_HEIGHT / (float)layer.texture.height;
        float scaledWidth = (float)layer.texture.width * scale;

        // Berechne den Offset für nahtloses Scrollen
        float scrollOffset = fmodf(camera.target.x * layer.scrollSpeed, scaledWidth);

        // Definiere Quell- und Ziel-Rechtecke für DrawTexturePro
        Rectangle source = {0.0f, 0.0f, (float)layer.texture.width, (float)layer.texture.height};

        // Zeichne die Textur zweimal nebeneinander, skaliert auf volle Höhe
        Rectangle dest1 = {-scrollOffset, 0, scaledWidth, (float)VIRTUAL_SCREEN_HEIGHT};
        Rectangle dest2 = {scaledWidth - scrollOffset, 0, scaledWidth, (float)VIRTUAL_SCREEN_HEIGHT};

        DrawTexturePro(layer.texture, source, dest1, {0, 0}, 0.0f, WHITE);
        DrawTexturePro(layer.texture, source, dest2, {0, 0}, 0.0f, WHITE);
    }

    // --- HAUPT-SPIELWELT ZEICHNEN ---
    BeginMode2D(camera);

    for (const auto &platform : platforms)
    {
        DrawRectangleRec(platform, DARKGRAY);
    }

    player.draw();
    combatSystem.draw();
    coinManager.draw();
    enemyManager.draw();

    EndMode2D();

    DrawTexture(coinIcon, 10, 10, WHITE);
    DrawText(TextFormat("%d", playerStats->coins), 50, 15, 30, GOLD);

    // Leben anzeigen
    for (int i = 0; i < playerStats->currentHP; i++)
    {
        DrawRectangle(10 + i * 35, 50, 30, 30, RED);
    }

    // --- UI ZEICHNEN ---
    if (levelMgr && currentLevel != -1)
    {
        const LevelInfo &info = levelMgr->get(currentLevel);
        DrawText(TextFormat("Du spielst: %s", info.name.c_str()), 20, 20, 30, GOLD);
    }
}