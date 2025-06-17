// src/core/CombatSystem.cpp

#include "core/CombatSystem.h"
#include "core/Projectile.h"
#include "core/Character.h"
#include "ProjectileAbility.h"
#include "raylib.h" // Für Vector2
#include <algorithm> // Für std::remove_if

void CombatSystem::spawnProjectile(Vector2 startPos, Vector2 direction)
{
    // Erstellt ein neues Projektil und fügt es zur Liste hinzu
    projectiles.emplace_back(startPos, direction);
}

void CombatSystem::update(float delta)
{
    // Aktualisiere alle Projektile
    for (auto& proj : projectiles) {
        proj.update(delta);
    }

    // Entferne alle inaktiven Projektile (deren Lebenszeit abgelaufen ist)
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(), 
            [](const Projectile& proj) {
                return !proj.active;
            }),
        projectiles.end()
    );

    // HIER kommt später die Kollisionsabfrage hin:
    // - Projektil trifft Gegner?
    // - Projektil trifft Wand?
}

void CombatSystem::draw() const
{
    // Zeichne alle aktiven Projektile
    for (const auto& proj : projectiles) {
        proj.draw();
    }
}