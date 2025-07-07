#include "Enemy.h"
#include "raylib.h"
#include <vector>
#include <cmath>

Enemy::Enemy(const std::string& name, float x, float y, float width, float height,
             float speed, float health, float attackDamage,
             const std::string& texturePath)
    : Character(name, x, y, width, height, speed, health, attackDamage, texturePath),
      enemyType(name), startX(x)
{
    // Enemy-spezifische Initialisierung
    TraceLog(LOG_INFO, "Enemy '%s' erstellt bei Position (%.1f, %.1f)", 
             name.c_str(), x, y);
}

void Enemy::update(float deltaTime, const std::vector<Rectangle>& platforms) 
{
    // Einfache KI: Patrouille hin und her
    float currentSpeed = base.speed * moveDirection;
    pos.x += currentSpeed * deltaTime;
    
    // Prüfe Patrouille-Grenzen
    float distanceFromStart = pos.x - startX;
    if (std::abs(distanceFromStart) > patrolDistance) {
        moveDirection *= -1.0f;  // Richtung umkehren
    }
    
    // Einfache Gravitation
    const float GRAVITY = 1200.0f;
    vel.y += GRAVITY * deltaTime;
    pos.y += vel.y * deltaTime;
    
    // Kollision mit Plattformen
    Rectangle enemyBounds = getBounds();
    for (const auto& platform : platforms) {
        if (CheckCollisionRecs(enemyBounds, platform)) {
            // Sehr einfache Kollisionsauflösung - Enemy landet auf Plattform
            if (vel.y > 0) {  // Fällt nach unten
                pos.y = platform.y - size.y;
                vel.y = 0;
            }
        }
    }
    
    // Reduziere Unverwundbarkeitszeit
    if (invTime > 0.0f) {
        invTime -= deltaTime;
    }
    
    // Basisklassen-Update aufrufen (falls weitere Verarbeitung nötig)
    // Character::update(deltaTime, platforms); // Optional
}

void Enemy::draw() const 
{
    // Zeichne Enemy-Sprite oder Fallback-Rechteck
    if (texture.id != 0) {
        Color tint = WHITE;
        
        // Blinken bei Schaden
        if (invTime > 0.0f) {
            // Blinke rot bei Schaden
            tint = {255, 100, 100, 255};
        }
        
        DrawTextureRec(texture, 
                      {0, 0, (float)texture.width, (float)texture.height}, 
                      pos, tint);
    } else {
        // Fallback: Rotes Rechteck
        Color enemyColor = RED;
        if (invTime > 0.0f) {
            enemyColor = {150, 50, 50, 255};  // Dunkleres Rot bei Schaden
        }
        DrawRectangle(pos.x, pos.y, size.x, size.y, enemyColor);
    }
    
    // Gesundheitsbalken zeichnen
    if (hp < base.maxHP) {  // Nur zeigen wenn beschädigt
        float healthBarWidth = size.x;
        float healthBarHeight = 5;
        float healthPercentage = hp / base.maxHP;
        
        // Hintergrund (rot)
        DrawRectangle(pos.x, pos.y - 10, healthBarWidth, healthBarHeight, RED);
        
        // Vordergrund (grün) 
        DrawRectangle(pos.x, pos.y - 10, 
                     healthPercentage * healthBarWidth, healthBarHeight, GREEN);
    }
    
  
    DrawText(TextFormat("%s\nHP: %.0f/%.0f", enemyType.c_str(), hp, base.maxHP), 
             pos.x, pos.y - 30, 10, WHITE);

    Rectangle bounds = getBounds();
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GREEN);

}

void Enemy::takeDamage(float damage) 
{
    // Rufe Basisklassen-Methode auf
    Character::takeDamage(damage);
    
    // Enemy-spezifische Reaktion auf Schaden
    if (hp <= 0) {
        TraceLog(LOG_INFO, "Enemy '%s' wurde besiegt!", enemyType.c_str());
        // Hier könntest du z.B. Partikel-Effekte, Sounds, oder Drops hinzufügen
    } else {
        // Kleine Rückstoß-Bewegung bei Schaden
        vel.y = -100.0f;  // Kurzer "Hüpfer" nach oben
    }
}