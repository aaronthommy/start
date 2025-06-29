#include "core/Player.h"
#include "SpriteUtils.h"
#include "core/abilities/ProjectileAbility.h" // <- Die konkrete Fähigkeit einbinden
#include "core/CombatSystem.h"
#include "config.h" // <-- WICHTIG: Einbinden, um die virtuellen Dimensionen zu kennen

Player::Player() : Character(Texture2D{}, Vector2{}) // Rufe den Character-Konstruktor mit leeren Werten auf
{
    reset();
}

void Player::load()
{
    // Wir nehmen den Sprite, der schon in main geladen wurde
    spriteIdle = loadSprite("assets/sprites/player/player.png", 1);
    spriteRun = loadSprite("assets/sprites/player/run_anim.png", 8);
    spriteJump = loadSprite("assets/sprites/player/jump_anim.png", 12);
}

void Player::unload()
{
    UnloadTexture(spriteIdle);
    UnloadTexture(spriteRun);
    UnloadTexture(spriteJump);
}


void Player::reset()
{
    pos = {100, 100}; // Startposition
    vel = {0, 0};     // Keine Anfangsgeschwindigkeit
    canJump = false;  // Kann am Anfang nicht springen (erst bei Bodenkontakt)

    size = {100, 100};

    currentAnimState = AnimState::IDLE;
    frameCount = 1; // Dein Player-Sprite hat 20 Frames
    currentFrame = 0;
    frameTimer = 0.0f;
    frameSpeed = 1.0f / 12.0f; // 12 Bilder pro Sekunde
    facingRight = true;

    primaryAbility = std::make_unique<ProjectileAbility>();
    hp = base.maxHP;
}

Vector2 Player::getPosition() const
{
    return pos;
}

void Player::setPosition(Vector2 newPosition)
{
    pos = newPosition;
}

void Player::usePrimaryAbility(CombatSystem &combatSystem, Vector2 target)
{
    if (primaryAbility)
    {
        if (primaryAbilityCooldownTimer <= 0)
        { // Nur ausführen, wenn der Timer bei 0 ist
            // Führe die Fähigkeit aus
            primaryAbility->execute(combatSystem, *this, target);

            // Setze den Timer auf die Cooldown-Zeit der Fähigkeit zurück
            primaryAbilityCooldownTimer = primaryAbility->cooldown;
        }
    }
}

void Player::update(float delta, const std::vector<Rectangle> &platforms)
{
    if (primaryAbilityCooldownTimer > 0)
    {
        primaryAbilityCooldownTimer -= delta;
    }
    // --- Eingabe verarbeiten ---
    vel.x = 0;
    bool isMoving = false;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        vel.x = -MOVE_SPEED;
        facingRight = false; // Schaut nach links
        isMoving = true;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        vel.x = MOVE_SPEED;
        facingRight = true; // Schaut nach rechts
        isMoving = true;
    }


    if (IsKeyPressed(KEY_SPACE) && canJump)
    {
        vel.y = JUMP_SPEED;
        canJump = false;
    }


    AnimState previousAnimState = currentAnimState;

    // 1. Zustand bestimmen
    if (!canJump)
    {
        currentAnimState = AnimState::JUMPING;
    }
    else if (isMoving)
    {
        currentAnimState = AnimState::RUNNING;
    }
    else
    {
        currentAnimState = AnimState::IDLE;
    }

    // 2. Wenn sich der Zustand geändert hat, Animation zurücksetzen
    if (currentAnimState != previousAnimState)
    {
        currentFrame = 0;
        frameTimer = 0.0f;
        switch (currentAnimState)
        {
        case AnimState::IDLE:
            frameCount = 1;
            break; // 4 Frames im alten Sprite
        case AnimState::RUNNING:
            frameCount = 8;
            break;
        case AnimState::JUMPING:
            frameCount = 12;
            break;
        }
    }

    // 3. Animation basierend auf dem aktuellen Zustand updaten
    // Nur animieren, wenn es nötig ist (mehr als 1 Frame)
    if (frameCount > 1)
    {
        frameTimer += delta;
        if (frameTimer >= frameSpeed)
        {
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1) % frameCount;
        }
    }
    // --- Bewegung & Kollision anwenden ---

    // 1. Horizontale Bewegung
    pos.x += vel.x * delta;
    Rectangle playerBounds = getBounds();
    for (const auto &platform : platforms)
    {
        if (CheckCollisionRecs(playerBounds, platform))
        {
            // Bei Kollision, bewege Spieler aus der Plattform heraus
            if (vel.x > 0)
            { // Bewegt sich nach rechts
                pos.x = platform.x - playerBounds.width;
            }
            else if (vel.x < 0)
            { // Bewegt sich nach links
                pos.x = platform.x + platform.width;
            }
        }
    }

    // 2. Vertikale Bewegung
    pos.y += vel.y * delta;
    playerBounds = getBounds(); // Position aktualisieren
    canJump = false;            // Zurücksetzen, Landung wird weiter unten geprüft

    for (const auto &platform : platforms)
    {
        if (CheckCollisionRecs(playerBounds, platform))
        {
            if (vel.y > 0)
            {                                             // Fällt nach unten
                pos.y = platform.y - playerBounds.height; // Auf Plattform landen
                vel.y = 0;
                canJump = true; // Kann wieder springen
            }
            else if (vel.y < 0)
            {                                         // Springt nach oben
                pos.y = platform.y + platform.height; // An Unterseite stoppen
                vel.y = 0;
            }
        }
    }

    // 3. Gravitation anwenden (wenn nicht auf einer Plattform)
    vel.y += GRAVITY * delta;

    // Optional: Verhindere, dass die Gravitation zu extrem wird
    if (vel.y > JUMP_SPEED * -2)
    {
        vel.y = JUMP_SPEED * -2;
    }
}

void Player::draw() const
{
    Texture2D currentSprite;
    int columns = 1;
    int rows = 1;

    switch (currentAnimState) {
        case AnimState::IDLE:
            currentSprite = spriteIdle;
            break;
        case AnimState::RUNNING:
            currentSprite = spriteRun;
            columns = 3; // 3 Frames pro Zeile
            rows = 3;    // 3 Zeilen
            break;
        case AnimState::JUMPING:
            currentSprite = spriteJump;
            columns = 3; // 3 Frames pro Zeile
            rows = 4;    // 4 Zeilen
            break;
    }

    float frameWidth = (float)currentSprite.width / columns;
    float frameHeight = (float)currentSprite.height / rows;

    int currentRow = currentFrame / columns;
    int currentCol = currentFrame % columns;

    Rectangle sourceRec = {
        (float)currentCol * frameWidth,
        (float)currentRow * frameHeight,
        frameWidth,
        frameHeight
    };

    if (!facingRight)
    {
        sourceRec.width *= -1;
    }

    Rectangle destRec = getBounds();
    DrawTexturePro(currentSprite, sourceRec, destRec, {0, 0}, 0, WHITE);

    DrawCircleV(getCenter(), 5, RED);
}
