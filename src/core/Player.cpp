#include "core/Player.h"
#include "SpriteUtils.h"
#include "config.h" // <-- WICHTIG: Einbinden, um die virtuellen Dimensionen zu kennen

Player::Player()
{
    reset();
}

void Player::reset()
{
    position = {100, 100}; // Startposition
    velocity = {0, 0};     // Keine Anfangsgeschwindigkeit
    canJump = false;       // Kann am Anfang nicht springen (erst bei Bodenkontakt)

    frameCount = 4; // Dein Player-Sprite hat 20 Frames
    currentFrame = 0;
    frameTimer = 0.0f;
    frameSpeed = 1.0f / 12.0f; // 12 Bilder pro Sekunde
    facingRight = true;
}

Vector2 Player::getPosition() const
{
    return position;
}

void Player::setPosition(Vector2 newPosition)
{
    position = newPosition;
}

void Player::load()
{
    // Wir nehmen den Sprite, der schon in main geladen wurde
    sprite = loadSprite("assets/sprites/player/player.png", 20);
}

void Player::unload()
{
    UnloadTexture(sprite);
}

void Player::update(float delta, const std::vector<Rectangle> &platforms)
{
    // --- Eingabe verarbeiten ---
    velocity.x = 0;
    bool isMoving = false;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        velocity.x = -MOVE_SPEED;
        facingRight = false; // Schaut nach links
        isMoving = true;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        velocity.x = MOVE_SPEED;
        facingRight = true; // Schaut nach rechts
        isMoving = true;
    }

    if (IsKeyPressed(KEY_SPACE) && canJump)
    {
        velocity.y = JUMP_SPEED;
        canJump = false;
    }

    // 2. Animations-Logik
    if (isMoving)
    {
        frameTimer += delta;
        if (frameTimer >= frameSpeed)
        {
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1) % frameCount;
        }
    }
    else
    {
        currentFrame = 0; // Zurück zum ersten Frame, wenn der Spieler steht
    }
    // --- Bewegung & Kollision anwenden ---

    // 1. Horizontale Bewegung
    position.x += velocity.x * delta;
    Rectangle playerBounds = getBounds();
    for (const auto &platform : platforms)
    {
        if (CheckCollisionRecs(playerBounds, platform))
        {
            // Bei Kollision, bewege Spieler aus der Plattform heraus
            if (velocity.x > 0)
            { // Bewegt sich nach rechts
                position.x = platform.x - playerBounds.width;
            }
            else if (velocity.x < 0)
            { // Bewegt sich nach links
                position.x = platform.x + platform.width;
            }
        }
    }

    // 2. Vertikale Bewegung
    position.y += velocity.y * delta;
    playerBounds = getBounds(); // Position aktualisieren
    canJump = false;            // Zurücksetzen, Landung wird weiter unten geprüft

    for (const auto &platform : platforms)
    {
        if (CheckCollisionRecs(playerBounds, platform))
        {
            if (velocity.y > 0)
            {                                                  // Fällt nach unten
                position.y = platform.y - playerBounds.height; // Auf Plattform landen
                velocity.y = 0;
                canJump = true; // Kann wieder springen
            }
            else if (velocity.y < 0)
            {                                              // Springt nach oben
                position.y = platform.y + platform.height; // An Unterseite stoppen
                velocity.y = 0;
            }
        }
    }

    // 3. Gravitation anwenden (wenn nicht auf einer Plattform)
    velocity.y += GRAVITY * delta;

    // Optional: Verhindere, dass die Gravitation zu extrem wird
    if (velocity.y > JUMP_SPEED * -2)
    {
        velocity.y = JUMP_SPEED * -2;
    }
}

void Player::draw() const
{
    // Frame-Größe (angenommen, jeder Frame ist 100x100 Pixel)
    float frameWidth = (float)sprite.width / frameCount;
    float frameHeight = (float)sprite.height;

    // Definiere den Ausschnitt (source rectangle)
    Rectangle sourceRec = {currentFrame * frameWidth, 0, frameWidth, frameHeight};

    // NEU: Spiegel den Sprite, wenn er nach links schaut
    if (!facingRight)
    {
        sourceRec.width *= -1;
    }

    Rectangle destRec = getBounds();
    DrawTexturePro(sprite, sourceRec, destRec, {0, 0}, 0, WHITE);
}

Rectangle Player::getBounds() const
{
    return {position.x, position.y, 100, 100};
}