#pragma once
#include "../entities/Enemy.h"
#include <vector>
#include <memory>

struct EnemySpawn {
    Enemy::Type type;
    Vector2 position;
    Vector2 patrolEnd;  // For walker enemies
};

class EnemyManager {
public:
    void init();
    void loadLevel(const std::vector<EnemySpawn>& spawns);
    void update(float deltaTime, Vector2 playerPos, bool* levelCollisions);
    void draw() const;
    void clear();
    
    // Collision checking
    bool checkPlayerCollision(Rectangle playerRect, int& playerHealth);
    void checkProjectileCollisions(Rectangle projectileRect, int damage);
    
    // Get enemy projectiles for collision checking
    std::vector<Rectangle> getEnemyProjectiles() const;

private:
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::pair<Vector2, Vector2>> enemyProjectiles; // pos, velocity
    
    Texture2D enemySpriteSheets[4]; // One for each enemy type
    Sound hurtSound;
    Sound deathSound;
    Sound shootSound;
    
    void handleEnemyShoot(Vector2 pos, Vector2 dir);
};
