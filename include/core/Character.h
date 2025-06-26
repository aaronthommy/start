#pragma once
#include "raylib.h"
#include "Stats.h"
#include "AbilityManager.h"

class Character {
public:
    Character(const Texture2D& tex, Vector2 spawn);

    void update(float dt);
    void draw() const;

    // Kampf
    void takeDamage(int dmg);
    void attack(Character& target);
    bool isDead() const { return hp <= 0; }

    // Zugriff
    const Stats& stats() const { return base; }
    Stats&       stats()       { return base; }
    AbilityManager& abilities() { return abilityMgr; }
    Vector2 position() const   { return pos; }
    Vector2 size;

    int getCurrentHP() const { return hp; }
    int getMaxHP() const { return base.maxHP; }

    Vector2 getCenter() const {
        return { pos.x + size.x / 2.0f, pos.y + size.y / 2.0f };;
    }
    
    Rectangle getBounds() const {
        return { pos.x, pos.y, size.x, size.y };
    }

protected:
    Texture2D texture;
    Vector2   pos;
    Vector2   vel;
    Stats     base;
    int       hp;
    AbilityManager abilityMgr;

    float invTime = 0.f;               // Unverwundbarkeit nach Hit
};
