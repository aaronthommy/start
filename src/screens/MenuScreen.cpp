#include "screens/MenuScreen.h"
#include <cmath>   

void MenuScreen::load(const char *titlePath,
                      const std::vector<Item> &items)
{
    title = LoadTexture(titlePath);
    font  = LoadFont("assets/fonts/first_font.ttf");
    sfxSelect = LoadSound("assets/sfx/game_select_new.wav");
    entries   = items;
}

void MenuScreen::unload()
{
    UnloadTexture(title);
    UnloadFont(font);
    UnloadSound(sfxGame);
    UnloadSound(sfxSelect);
}

MenuScreen::Style MenuScreen::defaultStyle{};

void MenuScreen::update()
{

    // Tastatur
    if (IsKeyPressed(KEY_UP))
    {
        selected = (selected + entries.size() - 1) % entries.size();
       
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        selected = (selected + 1) % entries.size();
        
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
        float totalH = entries.size() * style.vGap;
        float startY = (GetScreenHeight() - totalH) * 0.6f + style.origin.y;

        Vector2 textSize = MeasureTextEx(font, entries[i].text.c_str(), 32, 0);
        float textX = (GetScreenWidth() - textSize.x) / 2;
        float y = startY + i * style.vGap;

        Rectangle r{textX - 10, y - 5, textSize.x + 20, textSize.y + 10}; // Padding für bessere Klickbarkeit

        if (CheckCollisionPointRec(m, r))
        {
            if (selected != (int)i)
            {
                selected = (int)i;
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
    // Titel horizontal UND vertikal besser zentrieren
    int tx = (GetScreenWidth() - title.width) / 2;
    int ty = GetScreenHeight() / 6; // Statt fixer 90px
    DrawTexture(title, tx, ty, WHITE);

    // Menü-Einträge mit besserem Spacing
    float totalH = entries.size() * style.vGap;
    float startY = (GetScreenHeight() - totalH) * 0.6f + style.origin.y; // 0.6f statt 0.5f

    for (size_t i = 0; i < entries.size(); ++i)
    {
        Color col = (i == selected) ? style.active : style.normal;

        // Text zentrieren
        Vector2 textSize = MeasureTextEx(font, entries[i].text.c_str(), 32, 0);
        float textX = (GetScreenWidth() - textSize.x) / 2;
        Vector2 pos{textX, startY + i * style.vGap};

        // Hover-Effekt mit leichter Animation
        if (i == selected)
        {
            float pulse = sinf(GetTime() * 5.0f) * 0.1f + 1.0f;
            DrawTextEx(font, entries[i].text.c_str(), pos, 32 * pulse, 0, col);
        }
        else
        {
            DrawTextEx(font, entries[i].text.c_str(), pos, 32, 0, col);
        }
    }
}