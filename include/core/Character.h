#pragma once

#include "raylib.h"
#include "core/Stats.h"
#include "core/abilities/IAbility.h"
#include <memory>

class CombatSystem;

class Character {
public:
    // Der Destruktor sollte bei Basisklassen immer virtual sein!
    virtual ~Character() = default;

    Character(Texture2D spr, Vector2 startPos);

    void draw() const;

    // WICHTIG: Diese Methode muss virtual sein, damit die Enemy-Klasse sie überschreiben kann.
    virtual void update(float dt, const std::vector<Rectangle>& platforms);

    void usePrimaryAbility(CombatSystem& combatSystem, Vector2 target);

    // Getter-Methoden, damit andere Teile des Codes sicher auf die Position zugreifen können.
    Vector2 getPosition() const { return pos; }
    Vector2 getCenter() const { return { pos.x + bounds.width / 2, pos.y + bounds.height / 2 }; }
    Rectangle getBounds() const { return bounds; }
    void setPosition(Vector2 newPos) { pos = newPos; bounds.x = newPos.x; bounds.y = newPos.y; }
    void load();
    void unload();


// HIER IST DIE WICHTIGSTE ÄNDERUNG: private -> protected
protected: 
    Vector2 pos;
    Vector2 vel;
    Rectangle bounds;
    Stats stats;

    Texture2D sprite;
    int currentFrame = 0;
    int frameCount = 1;
    float frameTimer = 0.0f;
    float frameSpeed = 1.0f / 12.0f; // 12 FPS

    bool facingRight = true;
    bool canJump = false;

    // Fähigkeiten
    std::unique_ptr<IAbility> primaryAbility;
    float primaryAbilityCooldownTimer = 0.0f;
};
