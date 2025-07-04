#include "core/Character.h"
#include "raylib.h"

// Bestehender Konstruktor für Player
Character::Character(const Texture2D& tex, Vector2 spawn)
    : texture(tex), pos(spawn), size({100, 100})
{
    hp = base.maxHP;
}

// Neuer Konstruktor für Enemy
Character::Character(const std::string& name, float x, float y, float width, float height,
                     float speed, float health, float attackDamage,
                     const std::string& texturePath)
    : pos({x, y}), size({width, height})
{
    // Lade die Textur wenn ein Pfad angegeben wurde
    if (!texturePath.empty()) {
        texture = LoadTexture(texturePath.c_str());
    } else {
        texture = {0}; // Leere Textur
    }
    
    // Setze Stats
    base.maxHP = health;
    base.speed = speed;
    base.attack = attackDamage;
    hp = health;
    
    vel = {0, 0};
}

// Update-Methode mit platforms
void Character::update(float deltaTime, const std::vector<Rectangle>& platforms) {
    // Basis-Implementierung - kann von Player/Enemy überschrieben werden
    // Grundlegende Physik könnte hier stehen
}

void Character::draw() const {
    // Basis-Implementierung
}

void Character::takeDamage(float damage) {
    if (invTime <= 0.0f) {
        hp -= damage;
        if (hp < 0) hp = 0;
        invTime = 0.5f; // 0.5 Sekunden Unverwundbarkeit
    }
}

void Character::attack(Character& target) {
    target.takeDamage(base.attack);
}