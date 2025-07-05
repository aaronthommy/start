// src/core/CombatSystem.cpp

#include "core/CombatSystem.h"
#include "core/Projectile.h"
#include "core/Character.h"
#include "ProjectileAbility.h"
#include "raylib.h" // Für Vector2
#include <algorithm> // Für std::remove_if

void CombatSystem::spawnProjectile(Vector2 startPos, Vector2 direction)
{
    projectiles.emplace_back(startPos, direction);
}

void CombatSystem::update(float dt, const std::vector<Rectangle>& platforms)
{
    for (auto& proj : projectiles) {
        proj.update(dt);

        if (!proj.active) {
            continue;
        }

        Rectangle projBounds = proj.getBounds(); // Hol die Bounding Box des Projektils
        
        // Kollision mit Plattformen (bestehende Logik)
        for (const auto& platformRect : platforms) {
            if (CheckCollisionRecs(projBounds, platformRect)) {
                proj.active = false;
                break; 
            }
        }
        
        // NEU: Kollision mit Enemies
        if (proj.active) { // Nur prüfen wenn Projektil noch aktiv
            for (auto* enemy : enemies) {
                if (enemy && !enemy->isDead()) { // Nur lebende Enemies prüfen
                    Rectangle enemyBounds = enemy->getBounds();
                    
                    if (CheckCollisionRecs(projBounds, enemyBounds)) {
                        // Enemy erleidet Schaden
                        float projectileDamage = 25.0f; // Schaden pro Projektil
                        enemy->takeDamage(projectileDamage);
                        
                        // Projektil wird zerstört
                        proj.active = false;
                        
                        // Log für Debug
                        TraceLog(LOG_INFO, "Projektil trifft Enemy! Schaden: %.1f, Enemy HP: %d", 
                                projectileDamage, enemy->getCurrentHP());
                        
                        break; // Ein Projektil kann nur einen Enemy treffen
                    }
                }
            }
        }
    }

    // Entferne inaktive Projektile
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(), 
            [](const Projectile& proj) {
                return !proj.active;
            }),
        projectiles.end()
    );
}

void CombatSystem::draw() const
{
    // Zeichne alle aktiven Projektile
    for (const auto& proj : projectiles) {
        proj.draw();
    }
}