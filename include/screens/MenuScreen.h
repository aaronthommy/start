#pragma once
#include "raylib.h"
#include <functional>
#include <string>
#include <vector>

class MenuScreen
{
public:
    struct Item
    {
        std::string text;
        std::function<void()> onSelect; // Callback bei ENTER / Klick
    };

    struct Style
    {
        int vGap = 52;             // vertikaler Abstand
        Vector2 origin = {0, 260}; // Start-Y wird dynamisch zentriert
        Color normal = RAYWHITE;
        Color hover = {0xFF, 0xCE, 0x65, 0xFF};  // warmes Gelb
        Color active = {0xFF, 0x9D, 0x1F, 0xFF}; // Orange
    };
    void setStyle(const Style &s) { style = s; }
    static Style defaultStyle; 
    void load(const char *titlePath, const std::vector<Item> &items);
    void unload();
    void update();
    void draw() const;

private:
    Texture2D title{};
    Font font{};
    Sound sfxGame{}, sfxSelect{};
    std::vector<Item> entries;
    int selected = 0;
    Style style;
};
