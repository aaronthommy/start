#pragma once
#include "core/Projectile.h"
#include <vector>
#include <memory>
class Character;

class CombatSystem {
public:
    void registerPlayer(Character* p)   { player = p; }
    void registerEnemy (Character* e)   { enemies.push_back(e); }
    void update(float dt);              // prüft Kollisionen & Schaden
    void draw() const;
    void spawnProjectile(Vector2 startPos, Vector2 direction);

private:
    Character* player = nullptr;
    std::vector<Character*> enemies;
    std::vector<Projectile> projectiles; 
};
