#include "core/Player.h"
#include "SpriteUtils.h"
#include "config.h"       // <-- WICHTIG: Einbinden, um die virtuellen Dimensionen zu kennen

Player::Player() {
    reset();
}

void Player::reset() {
    position = { 100, 100 }; // Startposition
    velocity = { 0, 0 };    // Keine Anfangsgeschwindigkeit
    canJump = false;        // Kann am Anfang nicht springen (erst bei Bodenkontakt)
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
    velocity.y += GRAVITY * delta;
    position.x += velocity.x * delta;
    position.y += velocity.y * delta;

    // 4. Provisorischer Boden
    // KORREKTUR: Benutze VIRTUAL_SCREEN_HEIGHT statt GetScreenHeight()
    float groundLevel = VIRTUAL_SCREEN_HEIGHT - 150.0f; 
    if (position.y >= groundLevel) {
        position.y = groundLevel;
        velocity.y = 0;
        canJump = true;
    }
}

void Player::draw() const {
    Rectangle src = {0, 0, 100, 100};
    Rectangle dest = getBounds();
    DrawTexturePro(sprite, src, dest, {0, 0}, 0, WHITE);
}

Rectangle Player::getBounds() const {
    return { position.x, position.y, 100, 100 };
}