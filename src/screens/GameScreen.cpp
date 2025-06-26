#include "screens/GameScreen.h"
#include "config.h"
#include "core/Projectile.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <cmath>

using json = nlohmann::json;

// Helferfunktion:
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

    backgroundLayers.clear();
    for (const auto &layerData : data["background_layers"])
    {
        backgroundLayers.push_back({LoadTexture(std::string(layerData["path"]).c_str()),
                                    layerData["scroll_speed"]});
    }

    player.load();
    Projectile::loadTexture();

    heartTexture = LoadTexture("assets/ui/hearts/heart100x100.png");
    halfHeartTexture = LoadTexture("assets/ui/hearts/half-heart100x100.png");
    emptyHeartTexture = LoadTexture("assets/ui/hearts/empty-heart100x100.png");

    player.setPosition({data["player_start"]["x"], data["player_start"]["y"]});
    camera.target = player.getPosition();

    combatSystem.registerPlayer(&player);

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
}

void GameScreen::unload()
{
    player.unload();
    Projectile::unloadTexture();
    for (auto &layer : backgroundLayers)
    {
        UnloadTexture(layer.texture);
    }
    backgroundLayers.clear();

    // Entlade alle Plattform-Texturen
    for (auto const &[key, val] : platformTextures)
    {
        UnloadTexture(val);
    }
    platformTextures.clear();

    UnloadTexture(heartTexture);
    UnloadTexture(halfHeartTexture);
    UnloadTexture(emptyHeartTexture);
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

    player.update(GetFrameTime(), platformBounds);
    camera.target = player.getPosition();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouseTarget = GetScreenToWorld2D(GetMousePosition(), camera);
        player.usePrimaryAbility(combatSystem, mouseTarget);
    }
    combatSystem.update(GetFrameTime(), platformBounds);
}

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

void GameScreen::draw() const
{
    ClearBackground(BLACK);

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

    player.draw();
    combatSystem.draw();
    EndMode2D();

    drawHearts();
}
