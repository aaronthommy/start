#include "core/Projectile.h"
#include "raymath.h"

// Initialisiere die statische Textur außerhalb der Klasse
Texture2D Projectile::spriteSheet = { 0 };

const float PROJECTILE_SPEED = 800.0f;
const float PROJECTILE_GRAVITY = 980.0f;

void Projectile::loadTexture() {
    spriteSheet = LoadTexture("assets/sprites/fx/projectile_fireball.png");
}

void Projectile::unloadTexture() {
    UnloadTexture(spriteSheet);
}

Projectile::Projectile(Vector2 startPos, Vector2 direction) {
    position = startPos;
    velocity = Vector2Scale(Vector2Normalize(direction), PROJECTILE_SPEED);
}

void Projectile::update(float delta) {
    if (!active) return;
    lifetime -= delta;
    if (lifetime <= 0) {
        active = false;
        return;
    }
    velocity.y += PROJECTILE_GRAVITY * delta;
    position.x += velocity.x * delta;
    position.y += velocity.y * delta;

    frameTimer += delta;
    if (frameTimer >= frameSpeed) {
        frameTimer = 0.0f;
        currentFrame = (currentFrame + 1) % frameCount;
    }
}

void Projectile::draw() const {
    if (active) {
        float frameWidth = (float)spriteSheet.width / frameCount;
        Rectangle sourceRec = { currentFrame * frameWidth, 0, frameWidth, (float)spriteSheet.height };
        Rectangle destRec = { position.x - size.x / 2, position.y - size.y / 2, size.x, size.y };
        DrawTexturePro(spriteSheet, sourceRec, destRec, {0, 0}, 0, WHITE);
    }
}