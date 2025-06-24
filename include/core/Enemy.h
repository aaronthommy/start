#pragma once

#include "core/Character.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

class Enemy : public Character {
public:
    // Der Konstruktor nimmt die JSON-Definition und eine Startposition entgegen.
    Enemy(const nlohmann::json& definition, Vector2 startPos);

    // Die Update-Funktion wird jeden Frame aufgerufen.
    // WICHTIG: Die `update`-Methode in der Basisklasse "Character" muss als "virtual" markiert sein!
    void update(float dt, const std::vector<Rectangle>& platforms);
    
private:
    // Simple Patrouillen-KI
    void patrol(float dt, const std::vector<Rectangle>& platforms);

    float patrolSpeed;
    bool hasGravity;
};
