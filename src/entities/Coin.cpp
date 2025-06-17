#include "entities/Coin.h"
#include <cmath>

void Coin::update(float deltaTime) {
    animTime += deltaTime;
    
    if (collected) {
        collectAnimTime += deltaTime;
    }
}

void Coin::draw(Texture2D coinSprite) const {
    if (collected && collectAnimTime > 0.5f) return; // Hide after collect animation
    
    float yOffset = sin(animTime * 3.0f) * 5.0f;
    float scale = 1.0f;
    float alpha = 1.0f;
    
    if (collected) {
        // Collection animation
        scale = 1.0f + collectAnimTime * 2.0f;
        alpha = 1.0f - (collectAnimTime * 2.0f);
        yOffset -= collectAnimTime * 50.0f;
    }
    
    Color tint = Fade(WHITE, alpha);
    
    // Draw coin sprite
    Rectangle sourceRec = {0, 0, (float)coinSprite.width, (float)coinSprite.height};
    Rectangle destRec = {
        position.x - radius * scale,
        position.y - radius * scale + yOffset,
        radius * 2 * scale,
        radius * 2 * scale
    };
    
    DrawTexturePro(coinSprite, sourceRec, destRec, {0, 0}, 0, tint);
}

bool Coin::checkCollision(Rectangle playerRect) const {
    if (collected) return false;
    
    // Simple circle-rectangle collision
    float closestX = fmax(playerRect.x, fmin(position.x, playerRect.x + playerRect.width));
    float closestY = fmax(playerRect.y, fmin(position.y, playerRect.y + playerRect.height));
    
    float distanceX = position.x - closestX;
    float distanceY = position.y - closestY;
    
    return (distanceX * distanceX + distanceY * distanceY) < (radius * radius);
}

void Coin::collect() {
    if (!collected) {
        collected = true;
        collectAnimTime = 0.0f;
    }
}