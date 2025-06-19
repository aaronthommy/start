// src/core/Character.cpp

#include "core/Character.h"

// Hier ist die Implementierung für den Konstruktor, den der Player jetzt aufruft
Character::Character(const Texture2D& tex, Vector2 spawn)
    : texture(tex), size({100, 100}), pos(spawn) // Korrigierte Reihenfolge
{
    // Hier könnten weitere Initialisierungen für Character stehen
    // z.B. hp = base.maxHp;
}

// Implementiere die anderen Character-Funktionen hier, falls nötig
void Character::update(float dt) {
    // Leere Implementierung, kann von Player/Enemy überschrieben werden
}

void Character::draw() const {
    // Leere Implementierung
}

void Character::takeDamage(int dmg) {
    // Leere Implementierung
}

void Character::attack(Character& target) {
    // Leere Implementierung
}