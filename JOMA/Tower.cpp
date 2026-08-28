#include "Tower.h"

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
}

Tower::Tower(sf::Vector2f position, float rotationDegrees)
    : stats{ 150.f, 15.f }, currentHealth(stats.health), sprite(firewallIdleTexture())
{
    idleAnim = std::make_unique<Animation>(firewallIdleTexture(), 1, 1, 1.f);
    attackAnim = std::make_unique<Animation>(firewallAttackTexture(), 2, 3, 0.15f);
    deathAnim = std::make_unique<Animation>(firewallDeathTexture(), 2, 2, 0.12f, false);

    idleAnim->applyTo(sprite);
    sf::IntRect rect = sprite.getTextureRect();
    sprite.setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
    sprite.setPosition(position);
    sprite.setRotation(sf::degrees(rotationDegrees));
}

void Tower::update(float deltaTime) {
    if (currentHealth <= 0.f) {
        if (&sprite.getTexture() != &firewallDeathTexture()) {
            sprite.setTexture(firewallDeathTexture(), false);
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
    sf::Texture& tex = isAttacking ? firewallAttackTexture() : firewallIdleTexture();

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