// src/screens/GameScreen.cpp

#include "screens/GameScreen.h"
#include "config.h"
#include "core/Projectile.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <cmath>

using json = nlohmann::json;

// Die Methoden GameScreen(), ~GameScreen(), load(), unload() und update() 
// bleiben exakt so, wie sie in der letzten Antwort waren.
// Ich füge sie hier der Vollständigkeit halber nochmal ein.

GameScreen::GameScreen()
{
    camera = { 0 };
    camera.offset = {(float)VIRTUAL_SCREEN_WIDTH / 2.0f, (float)VIRTUAL_SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

GameScreen::~GameScreen()
{
    unload();
}

void GameScreen::load(LevelManager* levelManager, int levelIndex)
{
    levelMgr = levelManager;
    currentLevel = levelIndex;

    const LevelInfo& info = levelMgr->get(levelIndex);
    std::ifstream f(info.jsonPath);
    if (!f.is_open()) {
        TraceLog(LOG_ERROR, "Konnte Level-JSON nicht öffnen: %s", info.jsonPath.c_str());
        return;
    }
    json data = json::parse(f);

    backgroundLayers.clear();
    for (const auto& layerData : data["background_layers"]) {
        backgroundLayers.push_back({
            LoadTexture(std::string(layerData["path"]).c_str()),
            layerData["scroll_speed"]
        });
    }

    player.load();
    Projectile::loadTexture();
    
    player.setPosition({ data["player_start"]["x"], data["player_start"]["y"] });
    camera.target = player.getPosition();
    
    combatSystem.registerPlayer(&player);

    platforms.clear();
    for (const auto& pData : data["platforms"]) {
        platforms.push_back({ pData["x"], pData["y"], pData["width"], pData["height"] });
    }
}

void GameScreen::unload()
{
    player.unload();
    Projectile::unloadTexture();
    for (auto& layer : backgroundLayers) {
        UnloadTexture(layer.texture);
    }
    backgroundLayers.clear();
}

void GameScreen::update()
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (onFinish) onFinish();
        return;
    }
    player.update(GetFrameTime(), platforms);
    camera.target = player.getPosition();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mouseTarget = GetScreenToWorld2D(GetMousePosition(), camera);
        player.usePrimaryAbility(combatSystem, mouseTarget);
    }
    combatSystem.update(GetFrameTime());
}


// HIER IST DIE NEUE, KORRIGIERTE ZEICHENMETHODE
void GameScreen::draw() const
{
    ClearBackground(BLACK); // Füllt den echten Bildschirm mit Schwarz

    // --- PARALLAX-EBENEN ZEICHNEN ---
    for (const auto& layer : backgroundLayers) {
        // Berechne die Skalierung, um die Höhe des Bildschirms zu füllen
        float scale = (float)VIRTUAL_SCREEN_HEIGHT / (float)layer.texture.height;
        float scaledWidth = (float)layer.texture.width * scale;

        // Berechne den Offset für nahtloses Scrollen
        float scrollOffset = fmodf(camera.target.x * layer.scrollSpeed, scaledWidth);

        // Definiere Quell- und Ziel-Rechtecke für DrawTexturePro
        Rectangle source = { 0.0f, 0.0f, (float)layer.texture.width, (float)layer.texture.height };
        
        // Zeichne die Textur zweimal nebeneinander, skaliert auf volle Höhe
        Rectangle dest1 = { -scrollOffset, 0, scaledWidth, (float)VIRTUAL_SCREEN_HEIGHT };
        Rectangle dest2 = { scaledWidth - scrollOffset, 0, scaledWidth, (float)VIRTUAL_SCREEN_HEIGHT };

        DrawTexturePro(layer.texture, source, dest1, {0, 0}, 0.0f, WHITE);
        DrawTexturePro(layer.texture, source, dest2, {0, 0}, 0.0f, WHITE);
    }


    // --- HAUPT-SPIELWELT ZEICHNEN ---
    BeginMode2D(camera);

        for (const auto& platform : platforms) {
            DrawRectangleRec(platform, DARKGRAY);
        }

        player.draw();
        combatSystem.draw(); 

    EndMode2D();

    // --- UI ZEICHNEN ---
    if (levelMgr && currentLevel != -1) {
        const LevelInfo& info = levelMgr->get(currentLevel);
        DrawText(TextFormat("Du spielst: %s", info.name.c_str()), 20, 20, 30, GOLD);
    }
}