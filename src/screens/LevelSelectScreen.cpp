#include "screens/LevelSelectScreen.h"

void LevelSelectScreen::load(LevelManager* manager)
{
    levelMgr = manager;
    std::vector<MenuScreen::Item> items;
    for (int i = 0; i < manager->count(); ++i) {
        const LevelInfo& info = manager->get(i);
        items.push_back({info.name, [this, i]() {
            if (onStart)
                onStart(i);
        }});
    }
    items.push_back({"Zur\xC3\xBCck", [this]() {
        if (onBack)
            onBack();
    }});
    menu.load("assets/ui/title.png", items);
}

void LevelSelectScreen::unload()
{
    menu.unload();
}

void LevelSelectScreen::update()
{
    menu.update();
}

void LevelSelectScreen::draw() const
{
    menu.draw();
}