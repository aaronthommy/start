#pragma once
#include "raylib.h"
#include "core/LevelManager.h"
#include "core/Player.h"
#include "core/Enemy.h" // NEU: Enemy include
#include <functional>
#include "core/CombatSystem.h"
#include "GoalFlag.h"
#include <vector>
#include <string>
#include <map>

// Eine kleine Struktur, die eine Ebene unseres Parallax-Hintergrunds beschreibt
struct ParallaxLayer
{
    Texture2D texture;
    float scrollSpeed;
};

struct Platform
{
    Rectangle bounds;
    std::string textureId; // Z.B. "grass", "stone", ...
};

struct HeartDrop
{
    Vector2 position;
    float lifetime = 10.0f; // 10 Sekunden bevor es verschwindet
    bool collected = false;
};

class GameScreen
{
public:
    GameScreen();
    ~GameScreen(); // Wichtig, um Texturen sauber zu entladen

    void load(LevelManager *levelManager, int levelIndex);
    void unload();
    void update();
    void draw() const;

    void setOnFinish(std::function<void()> cb) { onFinish = std::move(cb); }

private:
    void drawHearts() const;
    void applyScreenShake(float intensity, float duration);
    void restartLevel();

    float levelDeathHeight = 1500.0f;
    float enemyCollisionCooldown = 0.0f;

    GoalFlag *levelGoal = nullptr;
    bool levelCompleted = false;
    float completionTimer = 0.0f;

    bool playerDead = false;
    float deathTimer = 0.0f;
    const float DEATH_DELAY = 1.5f;

    float screenShakeTime = 0.0f;
    float screenShakeIntensity = 0.0f;
    Vector2 screenShakeOffset = {0, 0};

    std::vector<HeartDrop> heartDrops;

    LevelManager *levelMgr = nullptr;
    int currentLevel = -1;
    Player player;
    Camera2D camera;
    CombatSystem combatSystem;

    std::vector<ParallaxLayer> backgroundLayers;
    std::vector<Platform> platforms;
    std::vector<Enemy> enemies; // NEU: Enemy-Vektor

    std::map<std::string, Texture2D> platformTextures;
    std::function<void()> onFinish;

    Texture2D heartTexture;
    Texture2D halfHeartTexture;
    Texture2D emptyHeartTexture;
};