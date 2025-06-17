#include "core/EnemyManager.h"
#include "config.h"

void EnemyManager::init() {
    // Load sprite sheets for each enemy type
    enemySpriteSheets[0] = LoadTexture("assets/enemies/walker.png");
    enemySpriteSheets[1] = LoadTexture("assets/enemies/jumper.png");
    enemySpriteSheets[2] = LoadTexture("assets/enemies/shooter.png");
    enemySpriteSheets[3] = LoadTexture("assets/enemies/chaser.png");
    
    hurtSound = LoadSound("assets/sfx/enemy_hurt.wav");
    deathSound = LoadSound("assets/sfx/enemy_death.wav");
    shootSound = LoadSound("assets/sfx/enemy_shoot.wav");
}

void EnemyManager::loadLevel(const std::vector<EnemySpawn>& spawns) {
    clear();
    
    for (const auto& spawn : spawns) {
        auto enemy = std::make_unique<Enemy>(spawn.type, spawn.position);
        
        if (spawn.type == Enemy::Type::WALKER) {
            enemy->patrolEnd = spawn.patrolEnd;
        }
        
        if (spawn.type == Enemy::Type::SHOOTER) {
            enemy->onShoot = [this](Vector2 pos, Vector2 vel) {
                handleEnemyShoot(pos, vel);
            };
        }
        
        enemies.push_back(std::move(enemy));
    }
}

void EnemyManager::update(float deltaTime, Vector2 playerPos, bool* levelCollisions) {
    // Update enemies
    for (auto it = enemies.begin(); it != enemies.end();) {
        (*it)->update(deltaTime, playerPos, levelCollisions);
        
        if ((*it)->state == Enemy::State::DEAD && (*it)->stateTimer > 1.0f) {
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }
    
    // Update projectiles
    for (auto it = enemyProjectiles.begin(); it != enemyProjectiles.end();) {
        it->first.x += it->second.x * deltaTime;
        it->first.y += it->second.y * deltaTime;
        
        // Remove if out of bounds
        if (it->first.x < 0 || it->first.x > VIRTUAL_SCREEN_WIDTH ||
            it->first.y < 0 || it->first.y > VIRTUAL_SCREEN_HEIGHT) {
            it = enemyProjectiles.erase(it);
        } else {
            ++it;
        }
    }
}

void EnemyManager::draw() const {
    for (const auto& enemy : enemies) {
        int spriteIndex = static_cast<int>(enemy->type);
        enemy->draw(enemySpriteSheets[spriteIndex]);
    }
    
    // Draw projectiles
    for (const auto& proj : enemyProjectiles) {
        DrawCircleV(proj.first, 4, RED);
    }
}

void EnemyManager::clear() {
    enemies.clear();
    enemyProjectiles.clear();
}

bool EnemyManager::checkPlayerCollision(Rectangle playerRect, int& playerHealth) {
    for (const auto& enemy : enemies) {
        if (enemy->state != Enemy::State::DEAD) {
            if (CheckCollisionRecs(playerRect, enemy->getHitbox())) {
                playerHealth -= enemy->damage;
                return true;
            }
        }
    }
    
    // Check projectile collisions
    for (auto it = enemyProjectiles.begin(); it != enemyProjectiles.end();) {
        Rectangle projRect = {it->first.x - 4, it->first.y - 4, 8, 8};
        if (CheckCollisionRecs(playerRect, projRect)) {
            playerHealth--;
            it = enemyProjectiles.erase(it);
            return true;
        } else {
            ++it;
        }
    }
    
    return false;
}

void EnemyManager::checkProjectileCollisions(Rectangle projectileRect, int damage) {
    for (auto& enemy : enemies) {
        if (enemy->state != Enemy::State::DEAD) {
            if (CheckCollisionRecs(projectileRect, enemy->getHitbox())) {
                enemy->takeDamage(damage);
                if (enemy->state == Enemy::State::DEAD) {
                    PlaySound(deathSound);
                } else {
                    PlaySound(hurtSound);
                }
            }
        }
    }
}

std::vector<Rectangle> EnemyManager::getEnemyProjectiles() const {
    std::vector<Rectangle> rects;
    for (const auto& proj : enemyProjectiles) {
        rects.push_back({proj.first.x - 4, proj.first.y - 4, 8, 8});
    }
    return rects;
}

void EnemyManager::handleEnemyShoot(Vector2 pos, Vector2 vel) {
    enemyProjectiles.push_back({pos, vel});
    PlaySound(shootSound);
}