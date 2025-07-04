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
        for (const auto& platformRect : platforms) {
            if (CheckCollisionRecs(projBounds, platformRect)) {
                proj.active = false;
                break; 
            }
        }
    }

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