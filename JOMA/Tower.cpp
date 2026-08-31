#include "Tower.h"
#include <cmath>

namespace {
    sf::Texture& firewallIdleTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/firewall_idle.png"), true);
        return tex;
    }
    sf::Texture& firewallAttackTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/firewall_attack.png"), true);
        return tex;
    }
    sf::Texture& firewallDeathTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/firewall_death.png"), true);
        return tex;
    }

    sf::Texture& spiderIdleTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/spider_idle.png"), true);
        return tex;
    }
    sf::Texture& spiderAttackTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/spider_attack.png"), true);
        return tex;
    }
    sf::Texture& spiderDeathTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/spider_death.png"), true);
        return tex;
    }

    sf::Texture& idleTextureFor(TowerKind k) { return k == TowerKind::Spider ? spiderIdleTexture() : firewallIdleTexture(); }
    sf::Texture& attackTextureFor(TowerKind k) { return k == TowerKind::Spider ? spiderAttackTexture() : firewallAttackTexture(); }
    sf::Texture& deathTextureFor(TowerKind k) { return k == TowerKind::Spider ? spiderDeathTexture() : firewallDeathTexture(); }

    TowerStats statsFor(TowerKind k) {
        if (k == TowerKind::Spider) return { 100.f, 10.f }; //прикидка, баланс подберём отдельно
        return { 120.f, 12.f };
    }
}

Tower::Tower(TowerKind kind, sf::Vector2f position, sf::Vector2f travelDirection)
    : kind(kind), stats(statsFor(kind)), currentHealth(stats.health), sprite(idleTextureFor(kind))
{
    idleAnim = std::make_unique<Animation>(idleTextureFor(kind), 1, 1, 1.f);

    if (kind == TowerKind::Spider) {
        attackAnim = std::make_unique<Animation>(attackTextureFor(kind), 3, 3, 0.12f);
        deathAnim = std::make_unique<Animation>(deathTextureFor(kind), 3, 4, 0.1f, false);
    }
    else {
        attackAnim = std::make_unique<Animation>(attackTextureFor(kind), 2, 3, 0.15f);
        deathAnim = std::make_unique<Animation>(deathTextureFor(kind), 2, 2, 0.12f, false);
    }

    idleAnim->applyTo(sprite);
    sf::IntRect rect = sprite.getTextureRect();
    sprite.setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
    sprite.setPosition(position);

    float rotation = 0.f;
    if (kind == TowerKind::Firewall) {
        rotation = (travelDirection.x != 0.f) ? 90.f : 0.f;
    }
    else {
        sf::Vector2f facing = -travelDirection;
        rotation = std::atan2(facing.y, facing.x) * 180.f / 3.14159265f + 90.f;
    }
    sprite.setRotation(sf::degrees(rotation));
}

void Tower::update(float deltaTime) {
    if (currentHealth <= 0.f) {
        sf::Texture& deathTex = deathTextureFor(kind);
        if (&sprite.getTexture() != &deathTex) {
            sprite.setTexture(deathTex, false);
            deathAnim->reset();
            deathAnim->applyTo(sprite);
            sf::IntRect rect = sprite.getTextureRect();
            sprite.setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
            applyScale();
        }
        deathAnim->update(deltaTime);
        deathAnim->applyTo(sprite);
        return;
    }

    Animation* anim = isAttacking ? attackAnim.get() : idleAnim.get();
    sf::Texture& tex = isAttacking ? attackTextureFor(kind) : idleTextureFor(kind);

    if (&sprite.getTexture() != &tex) {
        sprite.setTexture(tex, false);
        anim->applyTo(sprite);
        sf::IntRect rect = sprite.getTextureRect();
        sprite.setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
        applyScale();
    }

    anim->update(deltaTime);
    anim->applyTo(sprite);
}

void Tower::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Tower::setDesiredSize(float worldSize) {
    desiredSize = worldSize;
    applyScale();
}

void Tower::applyScale() {
    sf::IntRect rect = sprite.getTextureRect();
    if (rect.size.x == 0 || desiredSize == 0.f) return;
    float scale = desiredSize / (float)rect.size.x;
    sprite.setScale({ scale, scale });
}

void Tower::takeDamage(float amount) {
    currentHealth -= amount;
    if (currentHealth < 0.f) currentHealth = 0.f;
}