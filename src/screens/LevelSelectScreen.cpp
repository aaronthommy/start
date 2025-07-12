#include "screens/LevelSelectScreen.h"
#include "config.h"
#include <cmath>

LevelSelectScreen::LevelSelectScreen() {
    // Constructor
}

LevelSelectScreen::~LevelSelectScreen() {
    unload();
}

void LevelSelectScreen::load(LevelManager* manager) {
    levelMgr = manager;
    
    // Load Assets
    backgroundTexture = LoadTexture("assets/ui/level_select_bg.png");
    cardFrameTexture = LoadTexture("assets/ui/level_card_frame.png");
    lockedIcon = LoadTexture("assets/ui/lock_icon.png");
    completedIcon = LoadTexture("assets/ui/star_icon.png");
    
    titleFont = LoadFont("assets/fonts/Silkscreen-Bold.ttf");
    cardFont = LoadFont("assets/fonts/Silkscreen-Regular.ttf");
    
    hoverSound = LoadSound("assets/sfx/menu_hover.wav");
    selectSound = LoadSound("assets/sfx/menu_select.wav");
    
    setupLevelCards();
}

void LevelSelectScreen::setupLevelCards() {
    levelCards.clear();
    
    for (int i = 0; i < levelMgr->count(); ++i) {
        const LevelInfo& info = levelMgr->get(i);
        
        LevelCard card;
        card.levelIndex = i;
        card.name = info.name;
        card.description = "Jump and run adventure!";
        
        // Calculate grid position
        int row = i / cardsPerRow;
        int col = i % cardsPerRow;
        
        card.bounds = {
            gridStart.x + col * (cardSize.x + cardSpacing.x),
            gridStart.y + row * (cardSize.y + cardSpacing.y),
            cardSize.x,
            cardSize.y
        };
        
        // Determine status (for now, first level available, others locked)
        if (i == 0) {
            card.status = LevelStatus::AVAILABLE;
        } else {
            card.status = LevelStatus::LOCKED; // TODO: Load from save game
        }
        
        // Load thumbnail (fallback to default if not found)
        const char* thumbnailPath = TextFormat("assets/ui/level_%d_thumb.png", i + 1);
        card.thumbnail = LoadTexture(thumbnailPath);
        
        // If thumbnail failed to load, texture.id will be 0
        
        levelCards.push_back(card);
    }
}

void LevelSelectScreen::update() {
    float deltaTime = GetFrameTime();
    pulseTime += deltaTime;
    
    Vector2 mousePos = GetMousePosition();
    bool anyHovered = false;
    
    // Update card hover states and handle input
    for (size_t i = 0; i < levelCards.size(); ++i) {
        LevelCard& card = levelCards[i];
        bool wasHovered = card.isHovered;
        card.isHovered = CheckCollisionPointRec(mousePos, card.bounds);
        
        if (card.isHovered) {
            anyHovered = true;
            selectedCard = (int)i;
            
            // Play hover sound when starting to hover
            if (!wasHovered) {
                PlaySound(hoverSound);
            }
            
            // Handle click
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (card.status != LevelStatus::LOCKED) {
                    PlaySound(selectSound);
                    if (onStart) {
                        onStart(card.levelIndex);
                    }
                }
            }
        }
    }
    
    if (!anyHovered) {
        selectedCard = -1;
    }
    
    // Keyboard navigation
    if (IsKeyPressed(KEY_LEFT) && selectedCard > 0) {
        selectedCard--;
        PlaySound(hoverSound);
    }
    if (IsKeyPressed(KEY_RIGHT) && selectedCard < (int)levelCards.size() - 1) {
        selectedCard++;
        PlaySound(hoverSound);
    }
    if (IsKeyPressed(KEY_UP) && selectedCard >= cardsPerRow) {
        selectedCard -= cardsPerRow;
        PlaySound(hoverSound);
    }
    if (IsKeyPressed(KEY_DOWN) && selectedCard + cardsPerRow < (int)levelCards.size()) {
        selectedCard += cardsPerRow;
        PlaySound(hoverSound);
    }
    
    // Update hover states for keyboard selection
    for (size_t i = 0; i < levelCards.size(); ++i) {
        levelCards[i].isHovered = ((int)i == selectedCard);
    }
    
    // Handle selection with Enter/Space
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (selectedCard >= 0 && selectedCard < (int)levelCards.size()) {
            const LevelCard& card = levelCards[selectedCard];
            if (card.status != LevelStatus::LOCKED) {
                PlaySound(selectSound);
                if (onStart) {
                    onStart(card.levelIndex);
                }
            }
        }
    }
    
    // Back button
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        if (onBack) {
            onBack();
        }
    }
    
    updateCardAnimations(deltaTime);
}

void LevelSelectScreen::updateCardAnimations(float deltaTime) {
    for (LevelCard& card : levelCards) {
        if (card.isHovered) {
            // Smooth scale up
            card.hoverScale += deltaTime * 5.0f;
            if (card.hoverScale > 1.1f) card.hoverScale = 1.1f;
            
            // Subtle rotation
            card.hoverRotation = sinf(pulseTime * 8.0f) * 2.0f;
        } else {
            // Scale back down
            card.hoverScale -= deltaTime * 8.0f;
            if (card.hoverScale < 1.0f) card.hoverScale = 1.0f;
            
            card.hoverRotation = 0.0f;
        }
    }
}

void LevelSelectScreen::draw() const {
    drawBackground();
    
    // Draw title
    const char* title = "SELECT LEVEL";
    Vector2 titleSize = MeasureTextEx(titleFont, title, 48, 0);
    Vector2 titlePos = {
        (VIRTUAL_SCREEN_WIDTH - titleSize.x) / 2,
        80
    };
    
    // Title glow effect
    DrawTextEx(titleFont, title, {titlePos.x + 2, titlePos.y + 2}, 48, 0, {0, 0, 0, 100});
    DrawTextEx(titleFont, title, titlePos, 48, 0, WHITE);
    
    // Draw level cards
    for (const LevelCard& card : levelCards) {
        drawLevelCard(card);
    }
    
    // Draw instructions
    const char* instructions = "ARROW KEYS: Navigate    ENTER: Select    ESC: Back";
    Vector2 instrSize = MeasureTextEx(cardFont, instructions, 16, 0);
    Vector2 instrPos = {
        (VIRTUAL_SCREEN_WIDTH - instrSize.x) / 2,
        VIRTUAL_SCREEN_HEIGHT - 40
    };
    DrawTextEx(cardFont, instructions, instrPos, 16, 0, {200, 200, 200, 255});
}

void LevelSelectScreen::drawBackground() const {
    if (backgroundTexture.id > 0) {
        // Tiled background
        float scale = (float)VIRTUAL_SCREEN_HEIGHT / backgroundTexture.height;
        for (int x = 0; x < VIRTUAL_SCREEN_WIDTH; x += (int)(backgroundTexture.width * scale)) {
            DrawTextureEx(backgroundTexture, {(float)x, 0}, 0, scale, WHITE);
        }
    } else {
        // Gradient fallback
        DrawRectangleGradientV(0, 0, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, 
                              {20, 30, 60, 255}, {60, 80, 120, 255});
    }
}

void LevelSelectScreen::drawLevelCard(const LevelCard& card) const {
    // Calculate draw position with hover animation
    Vector2 center = {
        card.bounds.x + card.bounds.width / 2,
        card.bounds.y + card.bounds.height / 2
    };
    
    Rectangle drawBounds = {
        center.x - (card.bounds.width * card.hoverScale) / 2,
        center.y - (card.bounds.height * card.hoverScale) / 2,
        card.bounds.width * card.hoverScale,
        card.bounds.height * card.hoverScale
    };
    
    // Card shadow
    Rectangle shadowBounds = {
        drawBounds.x + 4, drawBounds.y + 4,
        drawBounds.width, drawBounds.height
    };
    DrawRectangle((int)shadowBounds.x, (int)shadowBounds.y, 
                  (int)shadowBounds.width, (int)shadowBounds.height, 
                  (Color){0, 0, 0, 100});
    
    // Card background
    Color cardColor = WHITE;
    if (card.status == LevelStatus::LOCKED) {
        cardColor = (Color){100, 100, 100, 255};
    } else if (card.status == LevelStatus::COMPLETED) {
        cardColor = (Color){255, 255, 180, 255};
    }
    
    if (card.isHovered) {
        cardColor = WHITE;
    }
    
    DrawRectangle((int)drawBounds.x, (int)drawBounds.y, 
                  (int)drawBounds.width, (int)drawBounds.height, cardColor);
    
    // Card border
    Color borderColor = card.isHovered ? (Color){255, 200, 0, 255} : (Color){200, 200, 200, 255};
    DrawRectangleLines((int)drawBounds.x, (int)drawBounds.y, 
                       (int)drawBounds.width, (int)drawBounds.height, borderColor);
    
    // Thumbnail area
    Rectangle thumbArea = {
        drawBounds.x + 10,
        drawBounds.y + 10,
        drawBounds.width - 20,
        drawBounds.height - 60
    };
    
    if (card.thumbnail.id > 0) {
        DrawTexturePro(card.thumbnail, 
                      (Rectangle){0, 0, (float)card.thumbnail.width, (float)card.thumbnail.height},
                      thumbArea, (Vector2){0, 0}, card.hoverRotation, WHITE);
    } else {
        // Placeholder gradient
        Color color1 = (Color){100, 150, 255, 255};
        Color color2 = (Color){200, 100, 255, 255};
        if (card.status == LevelStatus::LOCKED) {
            color1 = (Color){80, 80, 80, 255};
            color2 = (Color){120, 120, 120, 255};
        }
        DrawRectangleGradientV((int)thumbArea.x, (int)thumbArea.y, 
                              (int)thumbArea.width, (int)thumbArea.height, 
                              color1, color2);
    }
    
    drawStatusIcon(card);
    drawCardInfo(card);
}

void LevelSelectScreen::drawStatusIcon(const LevelCard& card) const {
    Vector2 iconPos = {
        card.bounds.x + card.bounds.width - 30,
        card.bounds.y + 10
    };
    
    if (card.status == LevelStatus::LOCKED && lockedIcon.id > 0) {
        DrawTextureEx(lockedIcon, iconPos, 0, 0.5f, WHITE);
    } else if (card.status == LevelStatus::COMPLETED && completedIcon.id > 0) {
        // Pulsing golden star
        float pulse = 0.8f + 0.2f * sinf(pulseTime * 6.0f);
        DrawTextureEx(completedIcon, iconPos, 0, 0.5f * pulse, {255, 215, 0, 255});
    }
}

void LevelSelectScreen::drawCardInfo(const LevelCard& card) const {
    // Level name
    Vector2 namePos = {
        card.bounds.x + 10,
        card.bounds.y + card.bounds.height - 45
    };
    
    Color textColor = card.status == LevelStatus::LOCKED ? 
                     Color{120, 120, 120, 255} : Color{50, 50, 50, 255};
    
    DrawTextEx(cardFont, card.name.c_str(), namePos, 18, 0, textColor);
    
    // Level number
    const char* levelNum = TextFormat("Level %d", card.levelIndex + 1);
    Vector2 numPos = {
        card.bounds.x + 10,
        card.bounds.y + card.bounds.height - 25
    };
    DrawTextEx(cardFont, levelNum, numPos, 14, 0, textColor);
    
    // Best time (if completed)
    if (card.status == LevelStatus::COMPLETED && card.bestTime > 0) {
        const char* timeText = TextFormat("Best: %ds", (int)card.bestTime);
        Vector2 timeSize = MeasureTextEx(cardFont, timeText, 12, 0);
        Vector2 timePos = {
            card.bounds.x + card.bounds.width - timeSize.x - 10,
            card.bounds.y + card.bounds.height - 25
        };
        DrawTextEx(cardFont, timeText, timePos, 12, 0, {0, 150, 0, 255});
    }
}

void LevelSelectScreen::unload() {
    UnloadTexture(backgroundTexture);
    UnloadTexture(cardFrameTexture);
    UnloadTexture(lockedIcon);
    UnloadTexture(completedIcon);
    UnloadFont(titleFont);
    UnloadFont(cardFont);
    UnloadSound(hoverSound);
    UnloadSound(selectSound);
    
    // Unload level thumbnails
    for (LevelCard& card : levelCards) {
        if (card.thumbnail.id > 0) {
            UnloadTexture(card.thumbnail);
        }
    }
    levelCards.clear();
}