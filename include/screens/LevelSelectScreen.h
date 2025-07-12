#pragma once
#include "raylib.h"
#include "core/LevelManager.h"
#include <functional>
#include <vector>

enum class LevelStatus {
    LOCKED,
    AVAILABLE, 
    COMPLETED
};

struct LevelCard {
    Rectangle bounds;
    LevelStatus status;
    Texture2D thumbnail;
    std::string name;
    std::string description;
    float bestTime = 0.0f;
    int levelIndex;
    
    // Animation state
    float hoverScale = 1.0f;
    float hoverRotation = 0.0f;
    bool isHovered = false;
};

class LevelSelectScreen {
public:
    LevelSelectScreen();
    ~LevelSelectScreen();
    
    void load(LevelManager* manager);
    void unload();
    void update();
    void draw() const;
    void refresh();

    void setOnBack(std::function<void()> cb) { onBack = std::move(cb); }
    void setOnStart(std::function<void(int)> cb) { onStart = std::move(cb); }

private:
    void setupLevelCards();
    void updateCardAnimations(float deltaTime);
    void drawBackground() const;
    void drawLevelCard(const LevelCard& card) const;
    void drawStatusIcon(const LevelCard& card) const;
    void drawCardInfo(const LevelCard& card) const;
    
    LevelManager* levelMgr = nullptr;
    std::vector<LevelCard> levelCards;
    
    // Assets
    Texture2D backgroundTexture;
    Texture2D cardFrameTexture;
    Texture2D lockedIcon;
    Texture2D completedIcon;
    Font titleFont;
    Font cardFont;
    
    // Audio
    Sound hoverSound;
    Sound selectSound;
    
    // Layout
    Vector2 cardSize = {240, 180};
    Vector2 cardSpacing = {60, 50}; 
    Vector2 gridStart = {120, 200};
    int cardsPerRow = 4;
    
    // Animation
    int selectedCard = -1;
    float pulseTime = 0.0f;
    
    std::function<void()> onBack;
    std::function<void(int)> onStart;
};