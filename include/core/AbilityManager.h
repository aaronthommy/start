#pragma once
#include <unordered_map>
#include "Ability.h"

class AbilityManager {
public:
    void unlock(const std::string& id);
    bool isUnlocked(const std::string& id) const;

    void trigger(const std::string& id);   // setzt Cooldown
    bool ready  (const std::string& id) const;

    void update(float dt);                 // Cooldown-Countdown
private:
    struct Slot { Ability ability; float timer = 0.f; };
    std::unordered_map<std::string, Slot> pool;
};
