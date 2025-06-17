#include "core/CoinManager.h"

void CoinManager::init() {
    coinTexture = LoadTexture("assets/items/coin.png");
    collectSound = LoadSound("assets/sfx/coin_collect.wav");
}

void CoinManager::loadLevel(const std::vector<Vector2>& coinPositions) {
    clear();
    
    for (const auto& pos : coinPositions) {
        coins.push_back(std::make_unique<Coin>(pos.x, pos.y));
    }
}

void CoinManager::update(float deltaTime, Rectangle playerRect, int& playerCoins) {
    for (auto& coin : coins) {
        coin->update(deltaTime);
        
        if (!coin->collected && coin->checkCollision(playerRect)) {
            coin->collect();
            playerCoins++;
            collectedThisLevel++;
            PlaySound(collectSound);
        }
    }
}

void CoinManager::draw() const {
    for (const auto& coin : coins) {
        coin->draw(coinTexture);
    }
}

void CoinManager::clear() {
    coins.clear();
    collectedThisLevel = 0;