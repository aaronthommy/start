#pragma once
#include "../entities/Coin.h"
#include <vector>
#include <memory>

class CoinManager {
public:
    void init();
    void loadLevel(const std::vector<Vector2>& coinPositions);
    void update(float deltaTime, Rectangle playerRect, int& playerCoins);
    void draw() const;
    void clear();
    
    int getCollectedCount() const { return collectedThisLevel; }
    int getTotalCount() const { return coins.size(); }

private:
    std::vector<std::unique_ptr<Coin>> coins;
    Texture2D coinTexture;
    Sound collectSound;
    int collectedThisLevel = 0;
};