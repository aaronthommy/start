#pragma once
#include "raylib.h"
#include <functional>

class Enemy {
public:
    enum class Type {
        WALKER,      // Patrouilliert horizontal
        JUMPER,      // Springt auf der Stelle
        SHOOTER,     // Schießt Projektile
        CHASER       // Verfolgt den Spieler
    };
    
    enum class State {
        IDLE,
        PATROL,
        ATTACK,
        HURT,
        DEAD
    };

    Vector2 position;
    Vector2 velocity = {0, 0};
    Rectangle bounds;
    
    Type type;
    State state = State::PATROL;
    
    // Stats
    int health = 3;
    int damage = 1;
    float speed = 100.0f;
    float detectionRange = 200.0f;
    float attackRange = 50.0f;
    float attackCooldown = 1.0f;
    
    // Patrol
    Vector2 patrolStart;
    Vector2 patrolEnd;
    bool movingRight = true;
    
    // Timers
    float stateTimer = 0.0f;
    float attackTimer = 0.0f;
    float animTimer = 0.0f;
    
    // Callbacks
    std::function<void(Vector2, Vector2)> onShoot;
    
    Enemy(Type t, Vector2 pos);
    
    void update(float deltaTime, Vector2 playerPos, bool* levelCollisions);
    void takeDamage(int dmg);
    void draw(Texture2D spriteSheet) const;
    Rectangle getHitbox() const;
    
private:
    void updateWalker(float deltaTime, Vector2 playerPos);
    void updateJumper(float deltaTime, Vector2 playerPos);
    void updateShooter(float deltaTime, Vector2 playerPos);
    void updateChaser(float deltaTime, Vector2 playerPos, bool* levelCollisions);
    
    bool canSeePlayer(Vector2 playerPos) const;
    void applyGravity(float deltaTime);
    void checkPlatformCollisions(bool* levelCollisions);
};