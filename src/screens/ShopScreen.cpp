// src/screens/ShopScreen.cpp
#include "screens/ShopScreen.h"
#include "config.h"
#include <algorithm>
#include <cmath>

void ShopScreen::load(SaveManager* sm, Stats* ps) {
    saveManager = sm;
    playerStats = ps;
    
    font = LoadFont("assets/fonts/Silkscreen-Bold.ttf");
    coinIcon = LoadTexture("assets/ui/coin.png");
    purchaseSound = LoadSound("assets/sfx/purchase.wav");
    errorSound = LoadSound("assets/sfx/error.wav");
    
    // Initialize shop items
    // UPGRADES
    upgrades.push_back({
        "double_jump",
        "Doppelsprung",
        "Ermöglicht einen zweiten Sprung in der Luft",
        500,
        [this]() { 
            saveManager->data().unlockedAbilities.push_back("double_jump");
            saveManager->save();
        },
        [this]() { 
            auto& abilities = saveManager->data().unlockedAbilities;
            return std::find(abilities.begin(), abilities.end(), "double_jump") != abilities.end();
        }
    });
    
    upgrades.push_back({
        "health_upgrade_1",
        "Extra Leben",
        "Erhöht maximale Leben um 1",
        300,
        [this]() { 
            saveManager->data().maxLives++;
            playerStats->maxHP++;
            saveManager->save();
        },
        [this]() { 
            return saveManager->data().maxLives > 3;
        }
    });
    
    upgrades.push_back({
        "speed_boost",
        "Geschwindigkeitsschub",
        "Erhöht Bewegungsgeschwindigkeit um 20%",
        400,
        [this]() { 
            playerStats->speed *= 1.2f;
            saveManager->data().unlockedAbilities.push_back("speed_boost");
            saveManager->save();
        },
        [this]() { 
            auto& abilities = saveManager->data().unlockedAbilities;
            return std::find(abilities.begin(), abilities.end(), "speed_boost") != abilities.end();
        }
    });
    
    // CHARACTERS
    characters.push_back({
        "knight",
        "Ritter",
        "Schwerer Nahkämpfer mit hoher Verteidigung",
        1000,
        [this]() { 
            saveManager->data().ownedCosmetics.push_back("knight");
            saveManager->save();
        },
        [this]() { 
            auto& cosmetics = saveManager->data().ownedCosmetics;
            return std::find(cosmetics.begin(), cosmetics.end(), "knight") != cosmetics.end();
        }
    });
    
    characters.push_back({
        "mage",
        "Magier",
        "Beherrscht mächtige Fernkampfzauber",
        1200,
        [this]() { 
            saveManager->data().ownedCosmetics.push_back("mage");
            saveManager->save();
        },
        [this]() { 
            auto& cosmetics = saveManager->data().ownedCosmetics;
            return std::find(cosmetics.begin(), cosmetics.end(), "mage") != cosmetics.end();
        }
    });
    
    // ABILITIES
    abilities.push_back({
        "dash",
        "Dash",
        "Schnelle Vorwärtsbewegung mit kurzer Unverwundbarkeit",
        800,
        [this]() { 
            saveManager->data().unlockedAbilities.push_back("dash");
            saveManager->save();
        },
        [this]() { 
            auto& abs = saveManager->data().unlockedAbilities;
            return std::find(abs.begin(), abs.end(), "dash") != abs.end();
        }
    });
    
    abilities.push_back({
        "shield",
        "Schild",
        "Blockt den nächsten Treffer",
        600,
        [this]() { 
            saveManager->data().unlockedAbilities.push_back("shield");
            saveManager->save();
        },
        [this]() { 
            auto& abs = saveManager->data().unlockedAbilities;
            return std::find(abs.begin(), abs.end(), "shield") != abs.end();
        }
    });
    
    // Load icons for items (placeholder - you'll need actual sprites)
    Texture2D placeholderIcon = LoadTexture("assets/ui/item_placeholder.png");
    for (auto& item : upgrades) item.icon = placeholderIcon;
    for (auto& item : characters) item.icon = placeholderIcon;
    for (auto& item : abilities) item.icon = placeholderIcon;
}

void ShopScreen::unload() {
    UnloadFont(font);
    UnloadTexture(coinIcon);
    UnloadSound(purchaseSound);
    UnloadSound(errorSound);
    
    // Unload all item icons
    for (auto& item : upgrades) UnloadTexture(item.icon);
    for (auto& item : characters) UnloadTexture(item.icon);
    for (auto& item : abilities) UnloadTexture(item.icon);
}

void ShopScreen::update() {
    auto& items = getCurrentCategoryItems();
    
    // Category switching
    if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_LEFT)) {
        int cat = static_cast<int>(currentCategory);
        cat = (cat - 1 + 4) % 4;
        currentCategory = static_cast<ShopCategory>(cat);
        selectedIndex = 0;
    }
    if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_RIGHT)) {
        int cat = static_cast<int>(currentCategory);
        cat = (cat + 1) % 4;
        currentCategory = static_cast<ShopCategory>(cat);
        selectedIndex = 0;
    }
    
    // Item selection
    if (!items.empty()) {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            selectedIndex = (selectedIndex - 1 + items.size()) % items.size();
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            selectedIndex = (selectedIndex + 1) % items.size();
        }
        
        // Purchase
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            purchaseCurrentItem();
        }
    }
    
    // Back button
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        if (onBack) onBack();
    }
    
    // Update animations
    coinAnimTime += GetFrameTime();
    if (showInsufficientFunds) {
        errorMessageTimer -= GetFrameTime();
        if (errorMessageTimer <= 0) {
            showInsufficientFunds = false;
        }
    }
}

void ShopScreen::draw() const {
    ClearBackground(Color{20, 20, 30, 255});
    
    // Title
    const char* title = "SHOP";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, (VIRTUAL_SCREEN_WIDTH - titleWidth) / 2, 40, 60, WHITE);
    
    // Draw category tabs
    drawCategoryTabs();
    
    // Draw coin display
    drawCoinDisplay();
    
    // Draw items
    const auto& items = getCurrentCategoryItems();
    float startY = 200;
    float itemHeight = 80;
    
    for (size_t i = 0; i < items.size(); i++) {
        drawShopItem(items[i], i, startY + i * itemHeight);
    }
    
    // Draw insufficient funds message
    if (showInsufficientFunds) {
        const char* msg = "Nicht genug Münzen!";
        int msgWidth = MeasureText(msg, 30);
        float alpha = errorMessageTimer > 0.5f ? 1.0f : errorMessageTimer * 2.0f;
        DrawText(msg, (VIRTUAL_SCREEN_WIDTH - msgWidth) / 2, VIRTUAL_SCREEN_HEIGHT - 100, 
                 30, Fade(RED, alpha));
    }
    
    // Instructions
    DrawText("Q/E: Kategorie wechseln | W/S: Auswählen | Enter: Kaufen | ESC: Zurück", 
             50, VIRTUAL_SCREEN_HEIGHT - 40, 20, GRAY);
}

std::vector<ShopScreen::ShopItem>& ShopScreen::getCurrentCategoryItems() {
    switch (currentCategory) {
        case ShopCategory::UPGRADES: return upgrades;
        case ShopCategory::CHARACTERS: return characters;
        case ShopCategory::ABILITIES: return abilities;
        case ShopCategory::COSMETICS: return cosmetics;
    }
    return upgrades;
}

const std::vector<ShopScreen::ShopItem>& ShopScreen::getCurrentCategoryItems() const {
    switch (currentCategory) {
        case ShopCategory::UPGRADES: return upgrades;
        case ShopCategory::CHARACTERS: return characters;
        case ShopCategory::ABILITIES: return abilities;
        case ShopCategory::COSMETICS: return cosmetics;
    }
    return upgrades;
}

void ShopScreen::purchaseCurrentItem() {
    auto& items = getCurrentCategoryItems();
    if (selectedIndex >= items.size()) return;
    
    auto& item = items[selectedIndex];
    
    // Check if already owned
    if (item.isOwned()) {
        PlaySound(errorSound);
        return;
    }
    
    // Check if enough coins
    if (saveManager->data().coins >= item.price) {
        saveManager->data().coins -= item.price;
        item.onPurchase();
        PlaySound(purchaseSound);
        saveManager->save();
    } else {
        PlaySound(errorSound);
        showInsufficientFunds = true;
        errorMessageTimer = 2.0f;
    }
}

void ShopScreen::drawShopItem(const ShopItem& item, int index, float y) const {
    bool isSelected = (index == selectedIndex);
    bool isOwned = item.isOwned();
    
    // Background
    Color bgColor = isSelected ? Color{60, 60, 80, 255} : Color{40, 40, 60, 255};
    if (isOwned) bgColor = Color{40, 60, 40, 255};
    
    DrawRectangle(50, y, VIRTUAL_SCREEN_WIDTH - 100, 70, bgColor);
    if (isSelected) {
        DrawRectangleLines(50, y, VIRTUAL_SCREEN_WIDTH - 100, 70, WHITE);
    }
    
    // Icon (placeholder rectangle for now)
    DrawRectangle(60, y + 10, 50, 50, GRAY);
    
    // Name
    DrawTextEx(font, item.name.c_str(), {130, y + 10}, 24, 0, WHITE);
    
    // Description
    DrawTextEx(font, item.description.c_str(), {130, y + 35}, 16, 0, LIGHTGRAY);
    
    // Price or owned status
    if (isOwned) {
        DrawText("GEKAUFT", VIRTUAL_SCREEN_WIDTH - 180, y + 25, 20, GREEN);
    } else {
        // Coin icon
        DrawTexture(coinIcon, VIRTUAL_SCREEN_WIDTH - 220, y + 20, WHITE);
        DrawText(TextFormat("%d", item.price), VIRTUAL_SCREEN_WIDTH - 180, y + 25, 20, GOLD);
    }
}

void ShopScreen::drawCategoryTabs() const {
    const char* categories[] = {"Upgrades", "Charaktere", "Fähigkeiten", "Kosmetik"};
    float tabWidth = (VIRTUAL_SCREEN_WIDTH - 100) / 4.0f;
    
    for (int i = 0; i < 4; i++) {
        float x = 50 + i * tabWidth;
        bool isActive = (i == static_cast<int>(currentCategory));
        
        Color tabColor = isActive ? Color{80, 80, 100, 255} : Color{40, 40, 60, 255};
        DrawRectangle(x, 120, tabWidth - 5, 40, tabColor);
        
        if (isActive) {
            DrawRectangleLines(x, 120, tabWidth - 5, 40, WHITE);
        }
        
        int textWidth = MeasureText(categories[i], 20);
        DrawText(categories[i], x + (tabWidth - textWidth) / 2, 130, 20, WHITE);
    }
}

void ShopScreen::drawCoinDisplay() const {
    // Animated coin
    float scale = 1.0f + 0.1f * sin(coinAnimTime * 3.0f);
    
    DrawTextureEx(coinIcon, {VIRTUAL_SCREEN_WIDTH - 150, 50}, 0, scale, WHITE);
    DrawText(TextFormat("%d", saveManager->data().coins), 
             VIRTUAL_SCREEN_WIDTH - 100, 55, 30, GOLD);
}