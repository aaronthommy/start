// include/core/Player.h

#pragma once
#include "raylib.h"
#include <vector>
#include <memory>  
#include "core/Character.h"
#include "core/abilities/IAbility.h"

class Player : public Character{
public:
    Player();
    void load();
    void unload();
    void update(float delta, const std::vector<Rectangle>& platforms); 
    void draw() const;
    void reset();
    void setPosition(Vector2 newPosition);
    void usePrimaryAbility(CombatSystem& combatSystem, Vector2 target);
    Vector2 getPosition() const;

private:
    Texture2D sprite;
    bool canJump;

    int frameCount;      // Wie viele Frames hat die Animation? (z.B. 20)
    int currentFrame;    // Welcher Frame wird gerade angezeigt?
    float frameTimer;    // Ein Zähler für die Zeit
    float frameSpeed;    // Wie schnell soll die Animation laufen? (z.B. 12 FPS)
    bool facingRight;

    // Physik-Konstanten
    static constexpr float GRAVITY = 1200.0f;
    static constexpr float MOVE_SPEED = 300.0f;
    static constexpr float JUMP_SPEED = -600.0f;

    std::unique_ptr<IAbility> primaryAbility;
};