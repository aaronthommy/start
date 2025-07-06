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
    camera.offset = {(float)VIRTUAL_SCREEN_WIDTH / 2, (float)VIRTUAL_SCREEN_HEIGHT / 2};
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
    player.reset();
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
    if (data.contains("enemies"))
    {
        for (const auto &eData : data["enemies"])
        {
            // Standard-Werte falls nicht angegeben - explizite Konvertierung zu float
            float width = eData.contains("width") ? (float)eData["width"] : 100.0f;
            float height = eData.contains("height") ? (float)eData["height"] : 100.0f;
            float speed = eData.contains("speed") ? (float)eData["speed"] : 50.0f;
            float health = eData.contains("health") ? (float)eData["health"] : 50.0f;
            float attackDamage = eData.contains("attackDamage") ? (float)eData["attackDamage"] : 10.0f;

            std::string texturePath = "";
            if (eData.contains("texturePath"))
            {
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


void GameScreen::applyScreenShake(float intensity, float duration) {
    screenShakeIntensity = intensity;
    screenShakeTime = duration;
}

void GameScreen::update()
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (onFinish)
            onFinish();
        return;
    }

    float deltaTime = GetFrameTime();

     // === NEU: PLAYER DEATH CHECK ===
    if (!playerDead && player.getCurrentHP() <= 0.0f) {
        // Player ist gestorben
        playerDead = true;
        deathTimer = DEATH_DELAY;
        
        // Dramatic Screen Shake beim Tod
        applyScreenShake(8.0f, 1.0f);
        
        TraceLog(LOG_INFO, "Player gestorben! Level wird in %.1f Sekunden neu gestartet.", DEATH_DELAY);
    }
    
    // === DEATH TIMER & RESTART ===
    if (playerDead) {
        deathTimer -= deltaTime;
        
        if (deathTimer <= 0.0f) {
            // Level neu starten
            restartLevel();
            return;  // Wichtig: Verhindere weitere Updates in diesem Frame
        }
        
        // Während Death Timer läuft, normale Updates pausieren
        return;
    }

    // === NORMALE GAME UPDATES (nur wenn Player lebt) ===
    // Screen Shake Update
    if (screenShakeTime > 0.0f) {
        screenShakeTime -= deltaTime;
        
        float intensity = screenShakeIntensity * (screenShakeTime / 0.3f);
        screenShakeOffset.x = (float)(rand() % 20 - 10) * intensity * 0.1f;
        screenShakeOffset.y = (float)(rand() % 20 - 10) * intensity * 0.1f;
        
        if (screenShakeTime <= 0.0f) {
            screenShakeOffset = {0, 0};
        }
    }
    // Erstelle eine temporäre Liste von Rectangles für die Kollisionserkennung
    std::vector<Rectangle> platformBounds;
    for (const auto &p : platforms)
    {
        platformBounds.push_back(p.bounds);
    }

    // Update Player
    player.update(deltaTime, platformBounds);
    camera.target = player.getPosition();

    // Update Enemies
    for (auto &enemy : enemies)
    {
        enemy.update(deltaTime, platformBounds);
    }

    // === MODIFIZIERTE ENEMY-PLAYER KOLLISION MIT SCREEN SHAKE ===
    if (enemyCollisionCooldown > 0.0f)
    {
        enemyCollisionCooldown -= deltaTime;
    }

    if (enemyCollisionCooldown <= 0.0f)
    {
        Rectangle playerBounds = player.getBounds();

        for (const auto &enemy : enemies)
        {
            if (!enemy.isDead())
            {
                Rectangle enemyBounds = enemy.getBounds();

                if (CheckCollisionRecs(playerBounds, enemyBounds))
                {
                    player.takeDamage(0.5f);
                    player.applyKnockback(enemy.getCenter());
                    enemyCollisionCooldown = 1.0f;
                    
                    // NEU: Screen Shake bei Player-Schaden!
                    applyScreenShake(1.0f, 0.9f);  // Intensity 3.0, Duration 0.3s

                    TraceLog(LOG_INFO, "Player von Enemy berührt! Schaden: 0.5, Player HP: %d",
                             player.getCurrentHP());

                    break;
                }
            }
        }
    }

    // === NEU: HEART-DROP UPDATE ===
    // Update Heart-Drops
    for (auto& drop : heartDrops) {
        drop.lifetime -= deltaTime;
        
        // Kollision mit Player prüfen
        if (!drop.collected) {
            Rectangle dropBounds = {drop.position.x, drop.position.y, 25, 25};
            Rectangle playerBounds = player.getBounds();
            
            if (CheckCollisionRecs(dropBounds, playerBounds)) {
                // Player sammelt Herz
                float currentHP = player.getCurrentHP();
                float maxHP = player.getMaxHP();
                
                if (currentHP < maxHP) {  // Nur heilen wenn nicht voll
                    player.takeDamage(-0.5f);  // Negative damage = Heilung
                    drop.collected = true;
                    
                    TraceLog(LOG_INFO, "Heart-Drop gesammelt! Player HP: %d", 
                             player.getCurrentHP());
                }
            }
        }
    }
    
    // Entferne abgelaufene oder gesammelte Heart-Drops
    heartDrops.erase(
        std::remove_if(heartDrops.begin(), heartDrops.end(),
            [](const HeartDrop& drop) {
                return drop.collected || drop.lifetime <= 0.0f;
            }),
        heartDrops.end()
    );

    // Handle Player Abilities
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouseTarget = GetScreenToWorld2D(GetMousePosition(), camera);
        player.usePrimaryAbility(combatSystem, mouseTarget);
    }

    // Update Combat System
    combatSystem.update(deltaTime, platformBounds);

    // === MODIFIZIERTE ENEMY-CLEANUP MIT HEART-DROPS ===
    size_t enemiesBeforeCleanup = enemies.size();
    
    // Prüfe welche Enemies gerade gestorben sind für Heart-Drops
    for (const auto& enemy : enemies) {
        if (enemy.isDead()) {
            // 30% Chance für Heart-Drop
            if (rand() % 100 < 30) {
                HeartDrop drop;
                drop.position = enemy.getCenter();
                drop.position.x -= 12.5f; // Zentriere das 25x25 Herz
                drop.position.y -= 12.5f;
                heartDrops.push_back(drop);
                
                TraceLog(LOG_INFO, "Heart-Drop gespawnt bei (%.1f, %.1f)", 
                         drop.position.x, drop.position.y);
            }
        }
    }
    
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
                       [](const Enemy &enemy)
                       {
                           return enemy.isDead();
                       }),
        enemies.end());

    if (enemies.size() != enemiesBeforeCleanup)
    {
        combatSystem.clearEnemies();
        for (auto &enemy : enemies)
        {
            combatSystem.registerEnemy(&enemy);
        }

        TraceLog(LOG_INFO, "Tote Enemies entfernt. Verbleibende Enemies: %zu", enemies.size());
    }
}

void GameScreen::draw() const
{
    ClearBackground(BLACK);

    // Zeichne Background-Layers (Parallax) - UNVERÄNDERT
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

    // === NEU: KAMERA MIT SCREEN SHAKE ===
    Camera2D shakeCamera = camera;
    shakeCamera.offset.x += screenShakeOffset.x;
    shakeCamera.offset.y += screenShakeOffset.y;
    
    BeginMode2D(shakeCamera);  // Verwende shakeCamera statt camera

    // Zeichne Platforms - UNVERÄNDERT
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

    // Zeichne Player - UNVERÄNDERT
    player.draw();
    
    // Zeichne Enemies - UNVERÄNDERT
    for (const auto &enemy : enemies)
    {
        enemy.draw();
    }
    
    // === NEU: ZEICHNE HEART-DROPS ===
    for (const auto& drop : heartDrops) {
        if (!drop.collected) {
            // Pulsing-Effekt
            float pulse = sinf(GetTime() * 8.0f) * 0.1f + 0.9f;
            float size = 25.0f * pulse;
            
            Rectangle destRect = {
                drop.position.x + (25.0f - size) / 2.0f,
                drop.position.y + (25.0f - size) / 2.0f,
                size, size
            };
            
            DrawTextureEx(halfHeartTexture, 
                         {destRect.x, destRect.y}, 
                         0.0f, 
                         size / 100.0f,  // Scale factor (da Textur 100x100 ist)
                         WHITE);
                         
            // DEBUG: Zeichne auch einen kleinen Kreis zur Visualisierung
            DrawCircle(drop.position.x + 12.5f, drop.position.y + 12.5f, 3, YELLOW);
        }
    }
    
    // Zeichne Combat System (Projektile, etc.) - UNVERÄNDERT
    combatSystem.draw();

    EndMode2D();

    // UI Elements - UNVERÄNDERT
    drawHearts();

    if (playerDead) {
        // Dunkler Overlay
        DrawRectangle(0, 0, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, 
                     {0, 0, 0, 150});  // Halbtransparentes Schwarz
        
        // Death Message
        const char* deathText = "You Died!";
        int textWidth = MeasureText(deathText, 60);
        int x = (VIRTUAL_SCREEN_WIDTH - textWidth) / 2;
        int y = VIRTUAL_SCREEN_HEIGHT / 2 - 60;
        
        DrawText(deathText, x, y, 60, RED);
        
        // Restart Timer
        const char* restartText = TextFormat("Restarting in %.1f...", deathTimer);
        int restartWidth = MeasureText(restartText, 30);
        int restartX = (VIRTUAL_SCREEN_WIDTH - restartWidth) / 2;
        int restartY = y + 80;
        
        DrawText(restartText, restartX, restartY, 30, WHITE);
    }
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

void GameScreen::drawHearts() const
{
    float maxHp = player.getMaxHP();  // Verwende float statt int
    float currentHp = player.getCurrentHP();  // Verwende float statt int
    
    float heartSize = 30.0f;
    float padding = 10.0f;

    // Zeichne Herzen basierend auf halben Herzen (0.5 Schritte)
    int fullHearts = (int)maxHp;  // Anzahl der vollen Herzen zum Zeichnen
    
    for (int i = 0; i < fullHearts; ++i)
    {
        float x = 20.0f + i * (heartSize + padding);
        float y = 20.0f;
        
        float heartValue = (float)i + 1.0f;  // Das i-te Herz repräsentiert HP von i+1
        
        if (currentHp >= heartValue) {
            // Volles Herz
            DrawTextureEx(heartTexture, {x, y}, 0.0f, heartSize / 100.0f, WHITE);
        } else if (currentHp >= heartValue - 0.5f) {
            // Halbes Herz
            DrawTextureEx(halfHeartTexture, {x, y}, 0.0f, heartSize / 100.0f, WHITE);
        } else {
            // Leeres Herz
            DrawTextureEx(emptyHeartTexture, {x, y}, 0.0f, heartSize / 100.0f, WHITE);
        }
    }
    
    // DEBUG: Zeige exakte HP-Werte
    DrawText(TextFormat("HP: %.1f/%.1f", currentHp, maxHp), 20, 60, 20, WHITE);
}

void GameScreen::restartLevel() {
    TraceLog(LOG_INFO, "Level wird neu gestartet...");
    
    // Reset Player
    player.reset();
    player.setPosition({100, 100});  // Oder aus level JSON laden
    
    // Reset Game State
    playerDead = false;
    deathTimer = 0.0f;
    enemyCollisionCooldown = 0.0f;
    
    // Reset Screen Shake
    screenShakeTime = 0.0f;
    screenShakeIntensity = 0.0f;
    screenShakeOffset = {0, 0};
    
    // Clear Heart-Drops
    heartDrops.clear();
    
    // Reset Camera
    camera.target = player.getPosition();
    
    // Enemies müssen wir neu laden - am einfachsten das ganze Level neu laden
    if (levelMgr && currentLevel >= 0) {
        load(levelMgr, currentLevel);
    }
}