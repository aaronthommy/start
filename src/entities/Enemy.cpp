#include "entities/Enemy.h"
#include "raylib.h"
#include "raymath.h"
#include <cmath>

Enemy::Enemy(Type t, Vector2 pos) : type(t), position(pos) {
    patrolStart = pos;
    patrolEnd = {pos.x + 200, pos.y}; // Default patrol range
    
    switch (type) {
        case Type::WALKER:
            bounds = {0, 0, 48, 48};
            speed = 80.0f;
            health = 2;
            break;
        case Type::JUMPER:
            bounds = {0, 0, 40, 56};
            speed = 0.0f;
            health = 3;
            break;
        case Type::SHOOTER:
            bounds = {0, 0, 44, 52};
            speed = 0.0f;
            health = 2;
            attackRange = 300.0f;
            break;
        case Type::CHASER:
            bounds = {0, 0, 48, 48};
            speed = 120.0f;
            health = 4;
            detectionRange = 300.0f;
            break;
    }
}

void Enemy::update(float deltaTime, Vector2 playerPos, bool* levelCollisions) {
    if (state == State::DEAD) {
        stateTimer += deltaTime;
        if (stateTimer > 1.0f) {
            // Enemy should be removed by manager
        }
        return;
    }
    
    animTimer += deltaTime;
    attackTimer -= deltaTime;
    
    switch (type) {
        case Type::WALKER:
            updateWalker(deltaTime, playerPos);
            break;
        case Type::JUMPER:
            updateJumper(deltaTime, playerPos);
            break;
        case Type::SHOOTER:
            updateShooter(deltaTime, playerPos);
            break;
        case Type::CHASER:
            updateChaser(deltaTime, playerPos, levelCollisions);
            break;
    }
    
    // Apply physics
    applyGravity(deltaTime);
    checkPlatformCollisions(levelCollisions);
    
    // Update position
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
    
    // Handle hurt state
    if (state == State::HURT) {
        stateTimer -= deltaTime;
        if (stateTimer <= 0) {
            state = State::PATROL;
        }
    }
}

void Enemy::updateWalker(float deltaTime, Vector2 playerPos) {
    if (state == State::HURT) return;
    
    // Patrol between points
    if (movingRight) {
        velocity.x = speed;
        if (position.x >= patrolEnd.x) {
            movingRight = false;
        }
    } else {
        velocity.x = -speed;
        if (position.x <= patrolStart.x) {
            movingRight = true;
        }
    }
    
    // Attack if player is close
    float dist = Vector2Distance(position, playerPos);
    if (dist < attackRange && attackTimer <= 0) {
        state = State::ATTACK;
        velocity.x = 0;
        attackTimer = attackCooldown;
        // Deal damage through collision
    }
}

void Enemy::updateJumper(float deltaTime, Vector2 playerPos) {
    if (state == State::HURT) return;
    
    // Jump periodically
    stateTimer += deltaTime;
    if (stateTimer > 2.0f && velocity.y == 0) {
        velocity.y = -300.0f; // Jump force
        stateTimer = 0.0f;
    }
    
    // Face player
    if (canSeePlayer(playerPos)) {
        velocity.x = 0; // Stay in place
    }
}

void Enemy::updateShooter(float deltaTime, Vector2 playerPos) {
    if (state == State::HURT) return;
    
    if (canSeePlayer(playerPos)) {
        state = State::ATTACK;
        
        if (attackTimer <= 0) {
            // Calculate shot direction
            Vector2 dir = Vector2Normalize(Vector2Subtract(playerPos, position));
            if (onShoot) {
                onShoot(position, Vector2Scale(dir, 300.0f)); // Projectile speed
            }
            attackTimer = attackCooldown;
        }
    } else {
        state = State::IDLE;
    }
}

void Enemy::updateChaser(float deltaTime, Vector2 playerPos, bool* levelCollisions) {
    if (state == State::HURT) return;
    
    float dist = Vector2Distance(position, playerPos);
    
    if (dist < detectionRange) {
        state = State::ATTACK;
        
        // Move towards player
        if (playerPos.x > position.x + 10) {
            velocity.x = speed;
        } else if (playerPos.x < position.x - 10) {
            velocity.x = -speed;
        } else {
            velocity.x = 0;
        }
        
        // Jump if player is above and on ground
        if (playerPos.y < position.y - 50 && velocity.y == 0) {
            velocity.y = -350.0f;
        }
    } else {
        state = State::PATROL;
        velocity.x = 0;
    }
}

bool Enemy::canSeePlayer(Vector2 playerPos) const {
    float dist = Vector2Distance(position, playerPos);
    return dist < detectionRange;
}

void Enemy::applyGravity(float deltaTime) {
    velocity.y += 980.0f * deltaTime; // Gravity
    if (velocity.y > 600.0f) velocity.y = 600.0f; // Terminal velocity
}

void Enemy::checkPlatformCollisions(bool* levelCollisions) {
    // Simplified collision - should integrate with your tile system
    Rectangle hitbox = getHitbox();
    
    // Check ground collision
    int tileX = hitbox.x / 32;
    int tileY = (hitbox.y + hitbox.height) / 32;
    
    if (tileY < 20 && levelCollisions[tileY * 40 + tileX]) {
        position.y = tileY * 32 - bounds.height;
        velocity.y = 0;
    }
}

void Enemy::takeDamage(int dmg) {
    if (state == State::DEAD) return;
    
    health -= dmg;
    if (health <= 0) {
        state = State::DEAD;
        stateTimer = 0.0f;
        velocity = {0, 0};
    } else {
        state = State::HURT;
        stateTimer = 0.3f;
        
        // Knockback
        velocity.x = (velocity.x > 0) ? -100 : 100;
    }
}

void Enemy::draw(Texture2D spriteSheet) const {
    Color tint = WHITE;
    if (state == State::HURT) {
        tint = RED;
    } else if (state == State::DEAD) {
        tint = Fade(WHITE, 1.0f - stateTimer);
    }
    
    // Calculate sprite frame based on animation
    int frame = 0;
    if (state == State::PATROL || velocity.x != 0) {
        frame = ((int)(animTimer * 8) % 4); // Walk animation
    } else if (state == State::ATTACK) {
        frame = 4; // Attack frame
    } else if (state == State::DEAD) {
        frame = 5; // Death frame
    }
    
    Rectangle sourceRec = {
        frame * bounds.width,
        0,
        bounds.width * (movingRight ? 1 : -1),
        bounds.height
    };
    
    DrawTextureRec(spriteSheet, sourceRec, position, tint);
}

Rectangle Enemy::getHitbox() const {
    return {position.x, position.y, bounds.width, bounds.height};
}