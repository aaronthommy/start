#include "screens/MenuScreen.h"

void MenuScreen::load(const char *titlePath,
                      const std::vector<Item> &items)
{
    title = LoadTexture(titlePath);
    font = LoadFont("assets/fonts/first_font.ttf");
    sfxMove = LoadSound("assets/sfx/menu_move.wav");
    sfxSelect = LoadSound("assets/sfx/menu_select.wav");
    entries = items;
}

void MenuScreen::unload()
{
    UnloadTexture(title);
    UnloadFont(font);
    UnloadSound(sfxMove);
    UnloadSound(sfxSelect);
}

MenuScreen::Style MenuScreen::defaultStyle{};

void MenuScreen::update()
{
    // Tastatur
    if (IsKeyPressed(KEY_UP))
    {
        selected = (selected + entries.size() - 1) % entries.size();
        PlaySound(sfxMove);
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        selected = (selected + 1) % entries.size();
        PlaySound(sfxMove);
    }
    if (IsKeyPressed(KEY_ENTER))
    {
        PlaySound(sfxSelect);
        entries[selected].onSelect();
    }

    // Maus
    Vector2 m = GetMousePosition();
    for (size_t i = 0; i < entries.size(); ++i)
    {
        int tx = (GetScreenWidth() - title.width) / 2;
        float totalH = entries.size() * style.vGap;
        float startY = (GetScreenHeight() - totalH) * 0.5f + style.origin.y;
        float x = tx + 20; // gleicher X-Versatz wie beim Text
        float y = startY + i * style.vGap;
        Vector2 size = MeasureTextEx(font, entries[i].text.c_str(), 32, 0);
        Rectangle r{x, y, size.x, size.y};
        if (CheckCollisionPointRec(m, r))
        {
            if (selected != (int)i)
            {
                selected = (int)i;
                PlaySound(sfxMove);
            }
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                PlaySound(sfxSelect);
                entries[i].onSelect();
            }
        }
    }
}

void MenuScreen::draw() const
{
    int tx = (GetScreenWidth() - title.width) / 2;
    DrawTexture(title, tx, 90, WHITE); // etwas höher

    float totalH = entries.size() * style.vGap;
    float startY = (GetScreenHeight() - totalH) * 0.5f + style.origin.y;

    for (size_t i = 0; i < entries.size(); ++i)
    {
        Color col = (i == selected) ? style.active : style.normal;
        Vector2 pos{(float)tx + 20, startY + i * style.vGap};
        DrawTextEx(font, entries[i].text.c_str(), pos, 32, 0, col);
    }
}