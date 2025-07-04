#ifndef ENEMY_H
#define ENEMY_H

#include "Character.h"
#include <string>
#include <vector>

class Enemy : public Character {
public:
    // Constructor mit String-Namen (nutzt neuen Character-Konstruktor)
    Enemy(const std::string& name, float x, float y, float width, float height,
          float speed, float health, float attackDamage,
          const std::string& texturePath);

    // Update method - Override der Character::update Methode
    void update(float deltaTime, const std::vector<Rectangle>& platforms) override;

    // Draw method - Override der Character::draw Methode  
    void draw() const override;

    // Handle collision (z.B. mit Player-Projektilen)
    void takeDamage(float damage) override;

    // Virtueller Destruktor
    virtual ~Enemy() = default;

private:
    std::string enemyType;  // "frog", "duck", etc.
    
    // Einfache KI-Eigenschaften
    float moveDirection = 1.0f;  // 1 = rechts, -1 = links
    float patrolDistance = 200.0f;  // Wie weit patrouilliert der Enemy
    float startX;  // Startposition für Patrouille
};

#endif // ENEMY_H