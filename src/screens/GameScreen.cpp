// src/screens/GameScreen.cpp

#include "screens/GameScreen.h"
#include "config.h"
#include "nlohmann/json.hpp" // JSON-Bibliothek
#include <fstream>          // Zum Lesen von Dateien
#include <cmath>            // Für fmodf (seamless scrolling)

using json = nlohmann::json;

GameScreen::GameScreen()
{
    // Kamera initialisieren, sodass sie auf den Spieler zentriert ist
    camera = { 0 };
    camera.offset = {(float)VIRTUAL_SCREEN_WIDTH / 2.0f, (float)VIRTUAL_SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

GameScreen::~GameScreen()
{
    // Stellt sicher, dass auch bei Zerstörung des Objekts alles entladen wird
    unload();
}

void GameScreen::load(LevelManager* levelManager, int levelIndex)
{
    levelMgr = levelManager;
    currentLevel = levelIndex;

    // Angenommen, dein LevelManager gibt dir eine LevelInfo mit dem jsonPath
    const LevelInfo& info = levelMgr->get(levelIndex);
    std::ifstream f(info.jsonPath); 
    if (!f.is_open()) {
        TraceLog(LOG_ERROR, "Konnte Level-JSON nicht öffnen: %s", info.jsonPath.c_str());
        return;
    }
    json data = json::parse(f);

    // --- Level aus den JSON-Daten aufbauen ---

    // 1. Lade alle Hintergrund-Ebenen aus dem JSON-Array
    backgroundLayers.clear();
    for (const auto& layerData : data["background_layers"]) {
        std::string path = layerData["path"];
        ParallaxLayer layer = {
            LoadTexture(path.c_str()),
            layerData["scroll_speed"]
        };
        backgroundLayers.push_back(layer);
    }

    // 2. Lade den Spieler und setze ihn an die Startposition
    player.load();
    Vector2 startPos = { data["player_start"]["x"], data["player_start"]["y"] };
    player.setPosition(startPos);
    camera.target = startPos; // Kamera initial auf Spieler ausrichten

    // 3. Lade alle Plattformen aus dem JSON-Array
    platforms.clear();
    for (const auto& platformData : data["platforms"]) {
        platforms.push_back({
            platformData["x"],
            platformData["y"],
            platformData["width"],
            platformData["height"]
        });
    }
}

void GameScreen::unload()
{
    player.unload();
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

    // Übergib die Level-Plattformen an den Spieler für die Kollisionsabfrage
    player.update(GetFrameTime(), platforms);

    // Die Kamera folgt immer der aktuellen Spielerposition
    camera.target = player.getPosition();
}

void GameScreen::draw() const
{
    // Parallax-Hintergründe als Erstes zeichnen
    for (const auto& layer : backgroundLayers)
    {
        // Diese Logik erzeugt einen nahtlosen, unendlich scrollenden Hintergrund
        float scrollOffset = fmodf(camera.target.x * layer.scrollSpeed, (float)layer.texture.width);
        DrawTexture(layer.texture, -scrollOffset, 0, WHITE);
        DrawTexture(layer.texture, (float)layer.texture.width - scrollOffset, 0, WHITE);
    }

    // Starte den 2D-Kameramodus. Alles was jetzt gezeichnet wird, bewegt sich mit der Welt.
    BeginMode2D(camera);

        for (const auto& platform : platforms) {
            DrawRectangleRec(platform, DARKGRAY);
        }

        player.draw();

    EndMode2D();
    // Ende des Kameramodus

    // Zeichne statische UI-Elemente (z.B. Score, Leben), die sich nicht bewegen sollen
    if (levelMgr && currentLevel != -1) {
        const LevelInfo &info = levelMgr->get(currentLevel);
        DrawText(TextFormat("Du spielst: %s", info.name.c_str()), 20, 20, 30, GOLD);
    }
    DrawText("Drücke ESC zum Verlassen", 20, 60, 20, RAYWHITE);
}