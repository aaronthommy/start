#include "screens/GameScreen.h"
#include "config.h"
#include "core/Projectile.h"
#include "core/Enemy.h" // <-- NEU
#include "nlohmann/json.hpp"
#include <fstream>
#include <cmath>

using json = nlohmann::json;

// Die Helferfunktion "DrawTextureTiled" bleibt unverändert...
void DrawTextureTiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint)
{
    if ((texture.id <= 0) || (scale <= 0.0f)) return;
    if ((source.width == 0) || (source.height == 0)) return;
    int tileWidth = (int)(source.width * scale);
    int tileHeight = (int)(source.height * scale);
    float startX = dest.x;
    float startY = dest.y;
    for (int y = 0; y < (int)dest.height; y += tileHeight)
    {
        for (int x = 0; x < (int)dest.width; x += tileWidth)
        {
            Rectangle currentSource = source;
            if (x + tileWidth > (int)dest.width) {
                currentSource.width = ((float)dest.width - x) / scale;
            }
            if (y + tileHeight > (int)dest.height) {
                currentSource.height = ((float)dest.height - y) / scale;
            }
            DrawTexturePro(texture, 
                           currentSource,
                           {startX + x, startY + y, currentSource.width * scale, currentSource.height * scale},
                           origin, rotation, tint);
        }
    }
}


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
    // Stelle sicher, dass alles vom vorigen Level entladen ist.
    unload(); 
    
    levelMgr = levelManager;
    currentLevel = levelIndex;

    // --- NEU: Lade die Gegner-Definitionen einmalig ---
    if (enemyDefinitions.empty()) {
        std::ifstream enemyFile("data/enemies/definitions.json");
        if (enemyFile.is_open()) {
            json enemyData = json::parse(enemyFile);
            for (auto& [key, value] : enemyData.items()) {
                enemyDefinitions[key] = value;
            }
        } else {
            TraceLog(LOG_ERROR, "Konnte Gegner-Definitionen nicht laden: data/enemies/definitions.json");
        }
    }

    const LevelInfo& info = levelMgr->get(levelIndex);
    std::ifstream f(info.jsonPath);
    if (!f.is_open()) {
        TraceLog(LOG_ERROR, "Konnte Level-JSON nicht öffnen: %s", info.jsonPath.c_str());
        return;
    }
    json data = json::parse(f);

    // Lade Hintergründe...
    for (const auto& layerData : data["background_layers"]) {
        backgroundLayers.push_back({
            LoadTexture(std::string(layerData["path"]).c_str()),
            layerData["scroll_speed"]
        });
    }

    // Lade Spieler & registriere ihn
    player.load();
    player.setPosition({ data["player_start"]["x"], data["player_start"]["y"] });
    combatSystem.registerPlayer(&player);
    
    // Lade Projektil-Texturen
    Projectile::loadTexture();
    
    // Setze Kamera-Ziel
    camera.target = player.getPosition();
    
    // Lade Plattformen...
    platforms.clear();
    for (const auto& pData : data["platforms"]) {
        Platform p;
        p.bounds = { pData["x"], pData["y"], pData["width"], pData["height"] };
        if (pData.contains("texture_id")) {
            p.textureId = pData["texture_id"];
            if (platformTextures.find(p.textureId) == platformTextures.end()) {
                std::string texturePath = "assets/graphics/textures/" + p.textureId + ".png";
                platformTextures[p.textureId] = LoadTexture(texturePath.c_str());
            }
        }
        platforms.push_back(p);
    }
    
    // --- NEU: Lade die Gegner basierend auf der Level-Datei ---
    if (data.contains("enemies")) {
        for (const auto& enemyData : data["enemies"]) {
            std::string type = enemyData["type"];
            if (enemyDefinitions.count(type)) { // Prüfe, ob wir diesen Gegnertyp kennen
                Vector2 pos = { enemyData["x"], enemyData["y"] };
                // Erstelle einen neuen Gegner und füge ihn dem Vektor hinzu
                enemies.emplace_back(enemyDefinitions[type], pos);
            }
        }
    }
    
    // --- NEU: Registriere alle neuen Gegner beim CombatSystem ---
    for (auto& enemy : enemies) {
        combatSystem.registerEnemy(&enemy);
    }
}

void GameScreen::unload()
{
    player.unload();
    Projectile::unloadTexture();

    for (auto& layer : backgroundLayers) UnloadTexture(layer.texture);
    backgroundLayers.clear();

    for (auto const& [key, val] : platformTextures) UnloadTexture(val);
    platformTextures.clear();

    // NEU: Räume die Gegner-Liste auf und setze das CombatSystem zurück
    enemies.clear(); 
    combatSystem.clearAll();
}

void GameScreen::update()
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (onFinish) onFinish();
        return;
    }
    
    // Erstelle eine temporäre Liste von Rectangles für die Kollisionserkennung
    std::vector<Rectangle> platformBounds;
    for(const auto& p : platforms) {
        platformBounds.push_back(p.bounds);
    }
    
    player.update(GetFrameTime(), platformBounds);
    
    // --- NEU: Aktualisiere jeden Gegner ---
    for (auto& enemy : enemies) {
        enemy.update(GetFrameTime(), platformBounds);
    }
    
    camera.target = player.getPosition();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mouseTarget = GetScreenToWorld2D(GetMousePosition(), camera);
        player.usePrimaryAbility(combatSystem, mouseTarget);
    }
    combatSystem.update(GetFrameTime(), platformBounds);
}


void GameScreen::draw() const
{
    ClearBackground(BLACK); 

    // Zeichne Parallax-Hintergründe...
    for (const auto& layer : backgroundLayers) {
        float scale = (float)VIRTUAL_SCREEN_HEIGHT / (float)layer.texture.height;
        float scaledWidth = (float)layer.texture.width * scale;
        float scrollOffset = fmodf(camera.target.x * layer.scrollSpeed, scaledWidth);
        Rectangle source = { 0.0f, 0.0f, (float)layer.texture.width, (float)layer.texture.height };
        Rectangle dest1 = { -scrollOffset, 0, scaledWidth, (float)VIRTUAL_SCREEN_HEIGHT };
        Rectangle dest2 = { scaledWidth - scrollOffset, 0, scaledWidth, (float)VIRTUAL_SCREEN_HEIGHT };
        DrawTexturePro(layer.texture, source, dest1, {0, 0}, 0.0f, WHITE);
        DrawTexturePro(layer.texture, source, dest2, {0, 0}, 0.0f, WHITE);
    }

    BeginMode2D(camera);

        // Zeichne Plattformen...
        for (const auto& platform : platforms) {
            if (!platform.textureId.empty() && platformTextures.count(platform.textureId)) {
                const Texture2D& tex = platformTextures.at(platform.textureId);
                DrawTextureTiled(tex, {0, 0, (float)tex.width, (float)tex.height}, platform.bounds, {0, 0}, 0.0f, 1.0f, WHITE);
            } else {
                DrawRectangleRec(platform.bounds, DARKGRAY);
            }
        }

        // --- NEU: Zeichne jeden Gegner ---
        for (const auto& enemy : enemies) {
            enemy.draw();
        }

        player.draw();
        combatSystem.draw(); 

    EndMode2D();

    if (levelMgr && currentLevel != -1) {
        const LevelInfo& info = levelMgr->get(currentLevel);
        DrawText(TextFormat("Du spielst: %s", info.name.c_str()), 20, 20, 30, GOLD);
    }
}