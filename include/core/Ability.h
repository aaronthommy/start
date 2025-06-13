#pragma once
#include <string>

enum class AbilityType { Passive, Active };

struct Ability {
    std::string id;           // "double_jump", "fireball" …
    AbilityType type;
    float cooldown = 0.f;     // nur Active
    // später: KeyBind, Partikeleffekt, etc.
};
