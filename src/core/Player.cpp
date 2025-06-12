// src/core/Player.cpp

#include "core/Player.h"
#include "SpriteUtils.h" // Für loadSprite

Player::Player() {
    position = { 100, 100 };
    velocity = { 0, 0 };
    canJump = false;
}

void Player::load() {
    // Wir nehmen den Sprite, der schon in main geladen wurde
    sprite = loadSprite("assets/sprites/player/player.png", 20);
}

void Player::unload() {
    UnloadTexture(sprite);
}

void Player::update(float delta) {
    // 1. Spielereingabe (links/rechts)
    velocity.x = 0;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        velocity.x = -MOVE_SPEED;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        velocity.x = MOVE_SPEED;
    }

    // 2. Springen
    if (IsKeyPressed(KEY_SPACE) && canJump) {
        velocity.y = JUMP_SPEED;
        canJump = false;
    }

    // 3. Physik anwenden
    // Gravitation
    velocity.y += GRAVITY * delta;
    // Position aktualisieren
    position.x += velocity.x * delta;
    position.y += velocity.y * delta;

    // 4. Provisorischer Boden (damit der Spieler nicht durchfällt)
    // Wir nehmen eine feste Y-Position als Boden
    float groundLevel = GetScreenHeight() - 150.0f; 
    if (position.y >= groundLevel) {
        position.y = groundLevel;
        velocity.y = 0;
        canJump = true;
    }
}

void Player::draw() const {
    // Zeichne den Spieler. Flip-Logik für Blickrichtung kommt später.
    Rectangle src = {0, 0, 100, 100}; // Erster Frame des Sprites
    Rectangle dest = getBounds();
    DrawTexturePro(sprite, src, dest, {0, 0}, 0, WHITE);
}

Rectangle Player::getBounds() const {
    return { position.x, position.y, 100, 100 }; // Größe an Sprite anpassen
}