#include "core/Player.h"
#include "SpriteUtils.h"
#include "core/abilities/ProjectileAbility.h" // <- Die konkrete Fähigkeit einbinden
#include "core/CombatSystem.h"
#include "config.h" // <-- WICHTIG: Einbinden, um die virtuellen Dimensionen zu kennen
#include "raymath.h"      // NEU: Für Vector2Subtract
#include <cmath> 

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

void Player::takeDamage(float damage) {
    // Rufe die Basis-Implementation auf
    Character::takeDamage(damage);
    
    // NEU: Setze Blink-Timer bei Schaden
    if (damage > 0) {  // Nur bei echtem Schaden, nicht bei Heilung
        damageBlinkTime = 0.5f;  // 0.5 Sekunden blinken
    }
}


void Player::update(float delta, const std::vector<Rectangle> &platforms)
{
    // === COOLDOWN-TIMER ===
    if (primaryAbilityCooldownTimer > 0)
    {
        primaryAbilityCooldownTimer -= delta;
    }
    
    // === KNOCKBACK-TIMER ===
    bool inKnockback = (knockbackTime > 0.0f);
    if (inKnockback) {
        knockbackTime -= delta;
        TraceLog(LOG_DEBUG, "Knockback aktiv: %.3f Sekunden verbleibend", knockbackTime);
    }
    
    // === EINGABE VERARBEITEN (nur wenn nicht im Knockback) ===
    if (!inKnockback) {
        vel.x = 0;  // Nur überschreiben wenn kein Knockback aktiv
        bool isMoving = false;
        
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            vel.x = -MOVE_SPEED;
            facingRight = false;
            isMoving = true;
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            vel.x = MOVE_SPEED;
            facingRight = true;
            isMoving = true;
        }

        if (IsKeyPressed(KEY_SPACE) && canJump)
        {
            vel.y = JUMP_SPEED;
            canJump = false;
        }
    } else {
        // Während Knockback: Reduziere die Knockback-Geschwindigkeit langsam
        vel.x *= 0.95f;  // Damping für natürlicheren Effekt
    }

    // === ANIMATIONS-LOGIK (unverändert) ===
    AnimState previousAnimState = currentAnimState;

    if (!canJump)
    {
        currentAnimState = AnimState::JUMPING;
    }
    else if (abs(vel.x) > 10.0f)  // Bewegung auch während Knockback berücksichtigen
    {
        currentAnimState = AnimState::RUNNING;
    }
    else
    {
        currentAnimState = AnimState::IDLE;
    }

    if (currentAnimState != previousAnimState)
    {
        currentFrame = 0;
        frameTimer = 0.0f;
        switch (currentAnimState)
        {
        case AnimState::IDLE:
            frameCount = 1;
            break;
        case AnimState::RUNNING:
            frameCount = 8;
            break;
        case AnimState::JUMPING:
            frameCount = 12;
            break;
        }
    }

    if (frameCount > 1)
    {
        frameTimer += delta;
        if (frameTimer >= frameSpeed)
        {
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1) % frameCount;
        }
    }

    // === BEWEGUNG & KOLLISION (unverändert) ===
    // 1. Horizontale Bewegung
    pos.x += vel.x * delta;
    Rectangle playerBounds = getBounds();
    for (const auto &platform : platforms)
    {
        if (CheckCollisionRecs(playerBounds, platform))
        {
            if (vel.x > 0)
            {
                pos.x = platform.x - playerBounds.width;
            }
            else if (vel.x < 0)
            {
                pos.x = platform.x + platform.width;
            }
        }
    }

    // 2. Vertikale Bewegung
    pos.y += vel.y * delta;
    playerBounds = getBounds();
    canJump = false;

    for (const auto &platform : platforms)
    {
        if (CheckCollisionRecs(playerBounds, platform))
        {
            if (vel.y > 0)
            {
                pos.y = platform.y - playerBounds.height;
                vel.y = 0;
                canJump = true;
            }
            else if (vel.y < 0)
            {
                pos.y = platform.y + platform.height;
                vel.y = 0;
            }
        }
    }

    // 3. Gravitation (immer anwenden)
    vel.y += GRAVITY * delta;

    if (vel.y > JUMP_SPEED * -2)
    {
        vel.y = JUMP_SPEED * -2;
    }

     if (damageBlinkTime > 0.0f) {
        damageBlinkTime -= delta;
    }
}

void Player::applyKnockback(Vector2 enemyPosition) {
    // Berechne die Positionen
    Vector2 playerCenter = {pos.x + size.x / 2.0f, pos.y + size.y / 2.0f};
    
    // === HORIZONTALER KNOCKBACK ===
    float horizontalDirection = playerCenter.x - enemyPosition.x;
    
    // Normalisiere die horizontale Richtung
    if (horizontalDirection > 0) {
        horizontalDirection = 1.0f;  // Player ist rechts vom Enemy -> nach rechts stoßen
    } else if (horizontalDirection < 0) {
        horizontalDirection = -1.0f; // Player ist links vom Enemy -> nach links stoßen
    } else {
        // Fallback: Zufällige Richtung
        horizontalDirection = (rand() % 2 == 0) ? 1.0f : -1.0f;
    }
    
    // === SETZE KNOCKBACK-GESCHWINDIGKEIT ===
    float horizontalForce = 250.0f;
    float verticalForce = -300.0f;  // Stärker nach oben
    
    vel.x = horizontalDirection * horizontalForce;
    vel.y = verticalForce;
    
    // === AKTIVIERE KNOCKBACK-ZUSTAND ===
    knockbackTime = KNOCKBACK_DURATION;  // 0.3 Sekunden Knockback
    
    TraceLog(LOG_INFO, "Knockback aktiviert! H=%.1f, V=%.1f, Dauer=%.2f", 
             vel.x, vel.y, knockbackTime);
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
            columns = 3;
            rows = 3;
            break;
        case AnimState::JUMPING:
            currentSprite = spriteJump;
            columns = 3;
            rows = 4;
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
    
    // NEU: Damage Blink Effect
    Color tint = WHITE;
    if (damageBlinkTime > 0.0f) {
        // Blinke zwischen rot und weiß
        float blinkSpeed = 15.0f;  // Wie schnell blinken
        if (sinf(GetTime() * blinkSpeed) > 0) {
            tint = {255, 100, 100, 255};  // Rötlicher Tint
        }
    }
    
    DrawTexturePro(currentSprite, sourceRec, destRec, {0, 0}, 0, tint);

    DrawCircleV(getCenter(), 5, RED);
}

