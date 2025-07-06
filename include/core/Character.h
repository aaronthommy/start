#pragma once
#include "raylib.h"
#include "Stats.h"
#include "AbilityManager.h"
#include <string>
#include <vector>

class Character {
public:
    // Bestehender Konstruktor für Player
    Character(const Texture2D& tex, Vector2 spawn);
    
    // Neuer Konstruktor für Enemy
    Character(const std::string& name, float x, float y, float width, float height,
              float speed, float health, float attackDamage,
              const std::string& texturePath);

    // Update mit platforms für Player und Enemy
    virtual void update(float deltaTime, const std::vector<Rectangle>& platforms);
    virtual void draw() const;

    // Kampf
    virtual void takeDamage(float damage);
    void attack(Character& target);
    bool isDead() const { return hp <= 0; }

    // Zugriff
    const Stats& stats() const { return base; }
    Stats&       stats()       { return base; }
    AbilityManager& abilities() { return abilityMgr; }
    Vector2 position() const   { return pos; }
    Vector2 size;

    float getCurrentHP() const { return hp; }
    float getMaxHP() const { return base.maxHP; }

    Vector2 getCenter() const {
        return { pos.x + size.x / 2.0f, pos.y + size.y / 2.0f };
    }
    
    Rectangle getBounds() const {
        return { pos.x, pos.y, size.x, size.y };
    }

    // Virtueller Destruktor für Polymorphismus
    virtual ~Character() = default;

protected:
    Texture2D texture;
    Vector2   pos;
    Vector2   vel;
    Stats     base;
    float     hp;  // Geändert zu float für Enemy-Kompatibilität
    AbilityManager abilityMgr;

    float invTime = 0.f;               // Unverwundbarkeit nach Hit
};