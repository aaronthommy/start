#pragma once
#include <vector>
class Character;

class CombatSystem {
public:
    void registerPlayer(Character* p)   { player = p; }
    void registerEnemy (Character* e)   { enemies.push_back(e); }

    void update(float dt);              // prüft Kollisionen & Schaden

private:
    Character* player = nullptr;
    std::vector<Character*> enemies;
};
