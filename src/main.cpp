#include "raylib.h"

int main() {
    const int screenW = 960;
    const int screenH = 540;
    InitWindow(screenW, screenH, "Jump-and-Run-Prototype");
    SetTargetFPS(60);

    Texture2D playerTex = LoadTexture("assets/player.png");

    // --- Spielerzustand --------------------------------------------------
    Vector2 pos = {100, 380};       // Startposition
    Vector2 vel = {0, 0};           // Geschwindigkeit (x,y)
    const float runSpeed  = 200.0f; // px/s seitwärts
    const float jumpSpeed = 350.0f; // Start-Impuls nach oben
    const float gravity   = 800.0f; // px/s²
    const float groundY   = 380;    // einfaches „Boden-Level“
    bool onGround = true;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- Eingabe ------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT)) vel.x =  runSpeed;
        else if (IsKeyDown(KEY_LEFT)) vel.x = -runSpeed;
        else vel.x = 0;

        if (IsKeyPressed(KEY_SPACE) && onGround) {
            vel.y = -jumpSpeed;     // negativer Wert ⇒ nach oben
            onGround = false;
        }

        // --- Physik -------------------------------------------------------
        vel.y += gravity * dt;      // Schwerkraft
        pos.x += vel.x * dt;
        pos.y += vel.y * dt;

        // --- Boden-Kollision ---------------------------------------------
        if (pos.y >= groundY) {
            pos.y = groundY;
            vel.y = 0;
            onGround = true;
        }

        // --- Zeichnen -----------------------------------------------------
        BeginDrawing();
        ClearBackground({30, 30, 30, 255});
        DrawTexture(playerTex, (int)pos.x, (int)pos.y, WHITE);
        DrawText("Pfeile = Laufen, SPACE = Springen, ESC = Beenden", 20, 20, 20, RAYWHITE);
        EndDrawing();
    }

    UnloadTexture(playerTex);
    CloseWindow();
    return 0;
}
