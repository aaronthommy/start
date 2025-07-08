// include/core/Player.h

#pragma once
#include "raylib.h"
#include <vector>
#include <memory>
#include "core/Character.h"
#include "core/abilities/IAbility.h"

class Player : public Character
{
public:
    enum class AnimState
    {
        IDLE,
        RUNNING,
        JUMPING
    };

    Player();
    void load();
    void unload();
    void applyKnockback(Vector2 enemyPosition);

    void takeDamage(float damage) override;
    void update(float delta, const std::vector<Rectangle> &platforms);
    void draw() const;
    void reset();
    void setPosition(Vector2 newPosition);
    void usePrimaryAbility(CombatSystem &combatSystem, Vector2 target);
    Vector2 getPosition() const;

    Rectangle getCollisionBounds() const {
        return { 
            pos.x +16,        
            pos.y + 20, 
            size.x - 46,       // 58 Pixel breit
            size.y -30       
        };
    }

private:
    Texture2D spriteIdle;
    Texture2D spriteRun;
    Texture2D spriteJump;

    float knockbackTime = 0.0f;
    const float KNOCKBACK_DURATION = 0.3f;

    float damageBlinkTime = 0.0f;

    AnimState currentAnimState;

    bool canJump;

    int frameCount;   // Wie viele Frames hat die Animation? (z.B. 20)
    int currentFrame; // Welcher Frame wird gerade angezeigt?
    float frameTimer; // Ein Zähler für die Zeit
    float frameSpeed; // Wie schnell soll die Animation laufen? (z.B. 12 FPS)
    bool facingRight;

    // Physik-Konstanten
    static constexpr float GRAVITY = 1200.0f;
    static constexpr float MOVE_SPEED = 300.0f;
    static constexpr float JUMP_SPEED = -600.0f;

    float primaryAbilityCooldownTimer = 0.0f;

    std::unique_ptr<IAbility> primaryAbility;
};