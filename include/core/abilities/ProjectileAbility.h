// include/core/abilities/ProjectileAbility.h
#pragma once
#include "core/abilities/IAbility.h"

class ProjectileAbility : public IAbility {
public:
    void execute(CombatSystem& combatSystem, Character& caster, Vector2 target) override;
};