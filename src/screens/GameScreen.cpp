#include "config.h"
#include "screens/GameScreen.h"
#include "core/Projectile.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <cmath>

using json = nlohmann::json;

// Helferfunktion: DrawTextureTiled
void DrawTextureTiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint)
{
    if ((texture.id <= 0) || (scale <= 0.0f))
        return; // Nichts tun bei ungültiger Textur oder Skalierung
    if ((source.width == 0) || (source.height == 0))
        return;

    int tileWidth = (int)(source.width * scale);
    int tileHeight = (int)(source.height * scale);

    // Startkoordinaten für das Zeichnen
    float startX = dest.x;
    float startY = dest.y;

    // Schleife, um den Zielbereich (dest) Kachel für Kachel zu füllen
    for (int y = 0; y < (int)dest.height; y += tileHeight)
    {
        for (int x = 0; x < (int)dest.width; x += tileWidth)
        {
            // Bestimme den Quell-Ausschnitt der Textur für die aktuelle Kachel
            Rectangle currentSource = source;

            // Prüfe, ob die Kachel am rechten Rand abgeschnitten werden muss
            if (x + tileWidth > (int)dest.width)
            {
                currentSource.width = ((float)dest.width - x) / scale;
            }

            // Prüfe, ob die Kachel am unteren Rand abgeschnitten werden muss
            if (y + tileHeight > (int)dest.height)
            {
                currentSource.height = ((float)dest.height - y) / scale;
            }

            // Zeichne die aktuelle Kachel mit DrawTexturePro
            DrawTexturePro(texture,
                           currentSource,
                           {startX + x, startY + y, currentSource.width * scale, currentSource.height * scale},
                           origin, rotation, tint);
        }
    }
}

GameScreen::GameScreen()
{
    camera = {0};
    camera.offset = {(float)VIRTUAL_SCREEN_WIDTH / 2, (float)VIRTUAL_SCREEN_HEIGHT / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

GameScreen::~GameScreen()
{
    unload();
}

void GameScreen::load(LevelManager *levelManager, int levelIndex)
{
    levelMgr = levelManager;
    currentLevel = levelIndex;

    const LevelInfo &info = levelMgr->get(levelIndex);
    std::ifstream f(info.jsonPath);
    if (!f.is_open())
    {
        TraceLog(LOG_ERROR, "Konnte Level-JSON nicht öffnen: %s", info.jsonPath.c_str());
        return;
    }
    json data = json::parse(f);

    // Lade Background-Layers
    backgroundLayers.clear();
    for (const auto &layerData : data["background_layers"])
    {
        backgroundLayers.push_back({LoadTexture(std::string(layerData["path"]).c_str()),
                                    layerData["scroll_speed"]});
    }

    // Lade Player
    player.load();
    Projectile::loadTexture();

    heartTexture = LoadTexture("assets/ui/hearts/heart100x100.png");
    halfHeartTexture = LoadTexture("assets/ui/hearts/half-heart100x100.png");
    emptyHeartTexture = LoadTexture("assets/ui/hearts/empty-heart100x100.png");

    player.setPosition({data["player_start"]["x"], data["player_start"]["y"]});
    camera.target = player.getPosition();

    combatSystem.registerPlayer(&player);

    // Lade Platforms
    platforms.clear();
    for (const auto &pData : data["platforms"])
    {
        Platform p;
        p.bounds = {pData["x"], pData["y"], pData["width"], pData["height"]};

        if (pData.contains("texture_id"))
        {
            p.textureId = pData["texture_id"];
            // Lade die Textur, wenn sie noch nicht geladen wurde
            if (platformTextures.find(p.textureId) == platformTextures.end())
            {
                std::string texturePath = "assets/graphics/textures/" + p.textureId + ".png";
                platformTextures[p.textureId] = LoadTexture(texturePath.c_str());
            }
        }
        platforms.push_back(p);
    }

    // NEU: Lade Enemies
    enemies.clear();
    if (data.contains("enemies")) {
        for (const auto &eData : data["enemies"])
        {
            // Standard-Werte falls nicht angegeben - explizite Konvertierung zu float
            float width = eData.contains("width") ? (float)eData["width"] : 100.0f;
            float height = eData.contains("height") ? (float)eData["height"] : 100.0f;
            float speed = eData.contains("speed") ? (float)eData["speed"] : 50.0f;
            float health = eData.contains("health") ? (float)eData["health"] : 50.0f;
            float attackDamage = eData.contains("attackDamage") ? (float)eData["attackDamage"] : 10.0f;
            
            std::string texturePath = "";
            if (eData.contains("texturePath")) {
                texturePath = eData["texturePath"];
            }
            
            std::string enemyType = eData.contains("type") ? eData["type"] : "default";
            
            // Erstelle Enemy mit allen Parametern
            Enemy enemy(enemyType, 
                       eData["x"], eData["y"], 
                       width, height,
                       speed, health, attackDamage,
                       texturePath);
            
            enemies.push_back(enemy);
            
            // Registriere Enemy beim CombatSystem
            combatSystem.registerEnemy(&enemies.back());
            
            TraceLog(LOG_INFO, "Enemy geladen: %s at (%.1f, %.1f)", 
                     enemyType.c_str(), (float)eData["x"], (float)eData["y"]);
        }
    }
}

void GameScreen::update()
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (onFinish)
            onFinish();
        return;
    }

    // Erstelle eine temporäre Liste von Rectangles für die Kollisionserkennung
    std::vector<Rectangle> platformBounds;
    for (const auto &p : platforms)
    {
        platformBounds.push_back(p.bounds);
    }

    // Update Player
    player.update(GetFrameTime(), platformBounds);
    camera.target = player.getPosition();

    // Update Enemies
    for (auto &enemy : enemies)
    {
        enemy.update(GetFrameTime(), platformBounds);
    }

    // Handle Player Abilities
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouseTarget = GetScreenToWorld2D(GetMousePosition(), camera);
        player.usePrimaryAbility(combatSystem, mouseTarget);
    }
    
    // Update Combat System
    combatSystem.update(GetFrameTime(), platformBounds);
    
    // NEU: Entferne tote Enemies
    size_t enemiesBeforeCleanup = enemies.size();
    
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(), 
            [](const Enemy& enemy) {
                return enemy.isDead();
            }),
        enemies.end()
    );
    
    // Falls Enemies entfernt wurden, aktualisiere auch das CombatSystem
    if (enemies.size() != enemiesBeforeCleanup) {
        // Aktualisiere die Enemy-Registrierung im CombatSystem
        combatSystem.clearEnemies(); // Diese Methode müssen wir noch hinzufügen
        for (auto& enemy : enemies) {
            combatSystem.registerEnemy(&enemy);
        }
        
        TraceLog(LOG_INFO, "Tote Enemies entfernt. Verbleibende Enemies: %zu", enemies.size());
    }
}

void GameScreen::draw() const
{
    ClearBackground(BLACK);

    // Zeichne Background-Layers (Parallax)
    for (const auto &layer : backgroundLayers)
    {
        float scale = (float)VIRTUAL_SCREEN_HEIGHT / (float)layer.texture.height;
        float scaledWidth = (float)layer.texture.width * scale;

        float scrollOffset = fmodf(camera.target.x * layer.scrollSpeed, scaledWidth);

        Rectangle source = {0.0f, 0.0f, (float)layer.texture.width, (float)layer.texture.height};

        Rectangle dest1 = {-scrollOffset, 0, scaledWidth, (float)VIRTUAL_SCREEN_HEIGHT};
        Rectangle dest2 = {scaledWidth - scrollOffset, 0, scaledWidth, (float)VIRTUAL_SCREEN_HEIGHT};

        DrawTexturePro(layer.texture, source, dest1, {0, 0}, 0.0f, WHITE);
        DrawTexturePro(layer.texture, source, dest2, {0, 0}, 0.0f, WHITE);
    }

    BeginMode2D(camera);

    // Zeichne Platforms
    for (const auto &platform : platforms)
    {
        if (!platform.textureId.empty() && platformTextures.count(platform.textureId))
        {
            const Texture2D &tex = platformTextures.at(platform.textureId);
            DrawTextureTiled(tex,
                             {0, 0, (float)tex.width, (float)tex.height},
                             platform.bounds,
                             {0, 0}, 0.0f, 1.0f, WHITE);
        }
        else
        {
            DrawRectangleRec(platform.bounds, DARKGRAY);
        }
    }

    // Zeichne Player
    player.draw();
    
    // Zeichne Enemies
    for (const auto &enemy : enemies)
    {
        enemy.draw();
    }
    
    // Zeichne Combat System (Projektile, etc.)
    combatSystem.draw();

    EndMode2D();

    // UI Elements
    drawHearts();
}

void GameScreen::unload()
{
    player.unload();
    Projectile::unloadTexture();
    
    // Entlade Background-Texturen
    for (auto &layer : backgroundLayers)
    {
        UnloadTexture(layer.texture);
    }
    backgroundLayers.clear();

    // Entlade Platform-Texturen
    for (auto const &[key, val] : platformTextures)
    {
        UnloadTexture(val);
    }
    platformTextures.clear();

    // Entlade Enemy-Texturen (werden automatisch im Enemy-Destruktor entladen)
    enemies.clear();

    // Entlade UI-Texturen
    UnloadTexture(heartTexture);
    UnloadTexture(halfHeartTexture);
    UnloadTexture(emptyHeartTexture);
}

// Bestehende drawHearts() Methode bleibt gleich
void GameScreen::drawHearts() const
{
    int maxHp = player.getMaxHP();
    int currentHp = player.getCurrentHP();
    float heartSize = 30.0f;
    float padding = 10.0f;

    for (int i = 0; i < maxHp; ++i)
    {
        float x = 20.0f + i * (heartSize + padding);
        float y = 20.0f;

        if (i < currentHp)
        {
            DrawTextureEx(heartTexture, {x, y}, 0.0f, heartSize / 100.0f, WHITE);
        }
        else
        {
            DrawTextureEx(emptyHeartTexture, {x, y}, 0.0f, heartSize / 100.0f, WHITE);
        }
    }
}