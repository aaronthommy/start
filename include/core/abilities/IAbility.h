// include/core/abilities/IAbility.h
#pragma once
#include "raylib.h"

// Forward declaration, um zirkuläre Abhängigkeiten zu vermeiden
class CombatSystem; 
class Character;

class IAbility {
public:
    virtual ~IAbility() = default; // Wichtig für Interfaces!

    // Die zentrale Methode. Jeder, der diesen Vertrag erfüllt, muss sie implementieren.
    virtual void execute(CombatSystem& combatSystem, Character& caster, Vector2 target) = 0;
    
    // Jede Fähigkeit hat eine Abklingzeit
    float cooldown = 0.5f; 
};