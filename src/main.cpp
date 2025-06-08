#include "raylib.h"   // <-- neu
#include "menu.h" 

enum class GameState { MENU, PLAY, EXIT };

int main() {
    InitWindow(960, 540, "Start Screen Demo");
    SetTargetFPS(60);

    GameState state = GameState::MENU;
    Menu menu;
    menu.load();

    while (!WindowShouldClose() && state != GameState::EXIT) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        switch (state) {
            case GameState::MENU: {
                MenuResult r = menu.update();
                menu.draw();
                if      (r == MenuResult::START_GAME) state = GameState::PLAY;
                else if (r == MenuResult::EXIT)       state = GameState::EXIT;
            } break;

            case GameState::PLAY:
                // Dein bisheriger Spiel-Code hier
                DrawText("Hier kommt dein Spiel!", 300, 250, 20, LIGHTGRAY);
                if (IsKeyPressed(KEY_ESCAPE)) state = GameState::MENU;
                break;

            default: break;
        }

        EndDrawing();
    }

    menu.unload();
    CloseWindow();
    return 0;
}
