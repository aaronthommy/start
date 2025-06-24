#include "core/Enemy.h"
#include "SpriteUtils.h"
#include "config.h"      // <-- HIER IST DIE FEHLENDE ZEILE!

Enemy::Enemy(const nlohmann::json& def, Vector2 startPos)
    : Character(Texture2D{}, startPos) // Ruft den Konstruktor der Basisklasse auf
{
    // Lade alle Eigenschaften aus der JSON-Definition
    std::string spritePath = def.value("sprite_path", "");
    int frameCountFromJson = def.value("frame_count", 1);
    stats.maxHealth = def.value("health", 100);
    stats.currentHealth = stats.maxHealth;
    patrolSpeed = def.value("speed", 50.0f);
    hasGravity = def.value("gravity", true);

    float width = def.value("width", 50.0f);
    float height = def.value("height", 50.0f);

    // Lade das Sprite mit unserer Helfer-Funktion
    if (!spritePath.empty()) {
        this->sprite = loadSprite(spritePath.c_str(), frameCountFromJson);
    }
    this->frameCount = frameCountFromJson;

    // Setze Position und Größe
    this->pos = startPos;
    this->bounds = { startPos.x, startPos.y, width, height };

    // Gegner starten standardmäßig nach rechts laufend
    this->vel = { patrolSpeed, 0 };
    this->facingRight = true;
}

void Enemy::update(float dt, const std::vector<Rectangle>& platforms) {
    // Führe die KI aus
    patrol(dt, platforms);

    // Aktualisiere die Animation des Gegners
    frameTimer += dt;
    if (frameTimer >= frameSpeed) {
        frameTimer = 0.0f;
        currentFrame = (currentFrame + 1) % frameCount;
    }
}

void Enemy::patrol(float dt, const std::vector<Rectangle>& platforms) {
    // --- Horizontale Bewegung & Kollision ---
    pos.x += vel.x * dt;
    bounds.x = pos.x;

    bool shouldTurn = false;
    for (const auto& platform : platforms) {
        if (CheckCollisionRecs(getBounds(), platform)) {
            // Bei Kollision mit einer Wand, umdrehen
            pos.x -= vel.x * dt; // Bewegung rückgängig machen
            shouldTurn = true;
            break;
        }
    }

    // --- Vertikale Bewegung & Schwerkraft ---
    if (hasGravity) {
        vel.y += GRAVITY * dt;
        pos.y += vel.y * dt;
        bounds.y = pos.y;

        for (const auto& platform : platforms) {
            if (CheckCollisionRecs(getBounds(), platform)) {
                if (vel.y > 0) { // Fällt nach unten
                    pos.y = platform.y - bounds.height;
                    vel.y = 0;
                } else if (vel.y < 0) { // Stößt von unten an eine Plattform
                    pos.y = platform.y + platform.height;
                    vel.y = 0;
                }
            }
        }
    }

    // --- Kanten-Erkennung ---
    // Prüfe, ob der Boden unter der nächsten Schrittposition noch da ist.
    Rectangle groundCheckRect = getBounds();
    groundCheckRect.y += 5; // Schaue etwas nach unten
    // Passe die Prüfposition an die Bewegungsrichtung an
    if (facingRight) {
        groundCheckRect.x += bounds.width;
    } else {
        groundCheckRect.x -= 1;
    }

    bool groundAhead = false;
    for (const auto& platform : platforms) {
        if (CheckCollisionRecs(groundCheckRect, platform)) {
            groundAhead = true;
            break;
        }
    }
    // Wenn vor dem Gegner kein Boden ist, umdrehen
    if (!groundAhead && vel.y == 0) {
        shouldTurn = true;
    }
    
    // Wenn der Gegner umdrehen soll, ändere seine Richtung
    if (shouldTurn) {
        vel.x *= -1;
        facingRight = !facingRight;
    }
}