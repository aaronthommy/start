#include "core/CombatSystem.h"
#include "core/Projectile.h"
#include "core/Character.h"
#include "core/abilities/ProjectileAbility.h" //<- Pfad ggf. anpassen, falls in "core/abilities"
#include "raylib.h"
#include <algorithm>

// NEU: Leert alle Listen, wenn ein Level verlassen wird.
void CombatSystem::clearAll()
{
    player = nullptr;
    enemies.clear();
    projectiles.clear();
}

void CombatSystem::spawnProjectile(Vector2 startPos, Vector2 direction)
{
    projectiles.emplace_back(startPos, direction);
}

void CombatSystem::update(float dt, const std::vector<Rectangle>& platforms)
{
    // --- Projektile aktualisieren und auf Kollision mit Plattformen prüfen ---
    for (auto& proj : projectiles) {
        proj.update(dt);

        if (!proj.active) {
            continue;
        }

        Rectangle projBounds = proj.getBounds();
        for (const auto& platformRect : platforms) {
            if (CheckCollisionRecs(projBounds, platformRect)) {
                proj.active = false; // Deaktiviere Projektil bei Treffer
                break; 
            }
        }
    }

    // Entferne alle inaktiven Projektile
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(), 
            [](const Projectile& proj) {
                return !proj.active;
            }),
        projectiles.end()
    );

    // TODO: Später hier Kollision zwischen Projektilen und Gegnern einfügen
}

void CombatSystem::draw() const
{
    for (const auto& proj : projectiles) {
        proj.draw();
    }
}