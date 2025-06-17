// include/core/Stats.h
#pragma once

struct Stats {
    // Leben
    int maxHP     = 3;
    int currentHP = 3;
    
    // Kampf
    int attack    = 1;
    int defense   = 0;
    
    // Währung
    int coins     = 0;
    
    // Bewegung
    float speed   = 220.0f;
    float jump    = 350.0f;
    
    // Utility Funktionen
    void heal(int amount) {
        currentHP += amount;
        if (currentHP > maxHP) currentHP = maxHP;
    }
    
    void takeDamage(int amount) {
        currentHP -= (amount - defense);
        if (currentHP < 0) currentHP = 0;
    }
    
    bool isDead() const {
        return currentHP <= 0;
    }
    
    void reset() {
        currentHP = maxHP;
    }
};