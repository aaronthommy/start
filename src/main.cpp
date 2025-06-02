#include "raylib.h"

int main() {
    const int screenW = 960;
    const int screenH = 540;

    InitWindow(screenW, screenH, "Jump-and-Run-Prototype");
    SetTargetFPS(60);

    Texture2D player = LoadTexture("assets/player.png");

    /* Startposition und Geschwindigkeit */
    Vector2 pos   = { 100, 380 };   // links unten
    const float speed = 200.0f;     // Pixel pro Sekunde

    /* Haupt-Loop ----------------------------------------------------*/
    while (!WindowShouldClose()) {

        /* 1) LOGIK-UPDATE */
        float dt = GetFrameTime();              // Sekunden seit letztem Frame
        if (IsKeyDown(KEY_RIGHT)) pos.x += speed * dt;
        if (IsKeyDown(KEY_LEFT))  pos.x -= speed * dt;

        /* 2) ZEICHNEN */
        BeginDrawing();
        ClearBackground((Color){ 30, 30, 30, 255 });   // dunkles Grau
        DrawTexture(player, (int)pos.x, (int)pos.y, WHITE);
        DrawText("ESC beendet", 20, 20, 20, RAYWHITE);
        EndDrawing();
    }

    /* Aufräumen */
    UnloadTexture(player);
    CloseWindow();
    return 0;
}
