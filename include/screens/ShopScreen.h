// include/screens/ShopScreen.h
#pragma once
#include "raylib.h"
#include "../core/SaveManager.h"
#include "../core/Stats.h"
#include <vector>
#include <functional>
#include <string>

class ShopScreen {
public:
    struct ShopItem {
        std::string id;
        std::string name;
        std::string description;
        int price;
        std::function<void()> onPurchase;
        std::function<bool()> isOwned;
        Texture2D icon;
    };

    enum class ShopCategory {
        UPGRADES,
        CHARACTERS,
        ABILITIES,
        COSMETICS
    };

    void load(SaveManager* saveManager, Stats* playerStats);
    void unload();
    void update();
    void draw() const;
    
    void setOnBack(std::function<void()> callback) { onBack = callback; }

private:
    SaveManager* saveManager;
    Stats* playerStats;
    
    std::vector<ShopItem> upgrades;
    std::vector<ShopItem> characters;
    std::vector<ShopItem> abilities;
    std::vector<ShopItem> cosmetics;
    
    ShopCategory currentCategory = ShopCategory::UPGRADES;
    int selectedIndex = 0;
    
    // UI Resources
    Font font;
    Texture2D background;
    Texture2D coinIcon;
    Sound purchaseSound;
    Sound errorSound;
    
    // Callbacks
    std::function<void()> onBack;
    
    // Helper functions
    std::vector<ShopItem>& getCurrentCategoryItems();
    const std::vector<ShopItem>& getCurrentCategoryItems() const;
    void purchaseCurrentItem();
    void drawShopItem(const ShopItem& item, int index, float y) const;
    void drawCategoryTabs() const;
    void drawCoinDisplay() const;
    
    // Animation
    float coinAnimTime = 0.0f;
    bool showInsufficientFunds = false;
    float errorMessageTimer = 0.0f;
};