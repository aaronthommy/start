// src/core/GoalFlag.cpp
#include "core/GoalFlag.h"
#include "raylib.h"
#include <cmath>

GoalFlag::GoalFlag(Vector2 position) : pos(position) {
    // Lade Flaggen-Textur (optional)
    flagTexture = LoadTexture("assets/sprites/goal_flag.png");
    
    // Falls Textur nicht geladen werden kann, verwenden wir einen Fallback
    if (flagTexture.id == 0) {
        // Wird als farbiges Rechteck gezeichnet
    }
}

GoalFlag::~GoalFlag() {
    if (flagTexture.id > 0) {
        UnloadTexture(flagTexture);
    }
}

void GoalFlag::update(float deltaTime) {
    animationTime += deltaTime;
}

void GoalFlag::draw() const {
    if (flagTexture.id > 0) {
        // Zeichne Flaggen-Textur mit Animation
        float wave = sinf(animationTime * 4.0f) * 5.0f;
        DrawTextureEx(flagTexture, {pos.x + wave, pos.y}, 0, 1.0f, WHITE);
    } else {
        // Fallback: Grünes Rechteck als Flagge
        Color flagColor = collected ? GOLD : LIME;
        DrawRectangle((int)pos.x, (int)pos.y, (int)size.x, (int)size.y, flagColor);
        DrawRectangleLines((int)pos.x, (int)pos.y, (int)size.x, (int)size.y, BLACK);
        
        // Zeichne Flaggen-Mast
        DrawRectangle((int)pos.x - 5, (int)pos.y, 10, (int)size.y + 20, BROWN);
    }
    
    // Debug: Zeichne Kollisions-Bounds
    Rectangle bounds = getBounds();
    DrawRectangleLines((int)bounds.x, (int)bounds.y, (int)bounds.width, (int)bounds.height, RED);
}

Rectangle GoalFlag::getBounds() const {
    return {pos.x, pos.y, size.x, size.y};
}

bool GoalFlag::isPlayerTouching(const Rectangle& playerBounds) const {
    return CheckCollisionRecs(playerBounds, getBounds());
}