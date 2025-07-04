// src/core/abilities/ProjectileAbility.cpp
#include "core/abilities/ProjectileAbility.h"
#include "core/CombatSystem.h" // Muss CombatSystem kennen, um Projektile zu spawnen
#include "core/Character.h"   // Muss den Caster kennen für die Startposition
#include "raylib.h" // Für Vector2
#include "raymath.h" // Für Vector2-Operationen

void ProjectileAbility::execute(CombatSystem& combatSystem, Character& caster, Vector2 target) {
    // Hole die Startposition aus dem Character
    Vector2 startPos = caster.getCenter(); // Annahme: Character hat eine getCenter() Methode
    
    // Berechne die Richtung zum Ziel
    Vector2 direction = Vector2Subtract(target, startPos);
    
    // Sage dem CombatSystem, es soll ein neues Projektil erstellen
    combatSystem.spawnProjectile(startPos, direction);
}