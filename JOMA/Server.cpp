#include "Server.h"

namespace {
    sf::Texture& coreIdleTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/entity_core.png"), true);
        return tex;
    }
    sf::Texture& coreDeathTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/core_death.png"), true);
        return tex;
    }
    sf::Texture& coreWoundTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/core_wound.png"), true);
        return tex;
    }
}

Server::Server(sf::Vector2f position)
    : currentHealth(maxHealth), sprite(coreIdleTexture())
{
    idleAnim = std::make_unique<Animation>(coreIdleTexture(), 1, 1, 1.f);
    deathAnim = std::make_unique<Animation>(coreDeathTexture(), 4, 5, 0.08f, false);
    woundAnim = std::make_unique<Animation>(coreWoundTexture(), 1, 2, 0.1f, false);

    idleAnim->applyTo(sprite);
    sf::IntRect rect = sprite.getTextureRect();
    sprite.setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
    sprite.setPosition(position);
}

void Server::switchTexture(sf::Texture& tex, Animation* anim) {
    sprite.setTexture(tex, false);
    anim->applyTo(sprite);
    sf::IntRect rect = sprite.getTextureRect();
    sprite.setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
    applyScale();
}

void Server::update(float deltaTime) {
    if (currentHealth <= 0.f) {
        if (&sprite.getTexture() != &coreDeathTexture()) {
            deathAnim->reset();
            switchTexture(coreDeathTexture(), deathAnim.get());
        }
        deathAnim->update(deltaTime);
        deathAnim->applyTo(sprite);
        return;
    }

    if (showWound) {
        if (&sprite.getTexture() != &coreWoundTexture()) {
            switchTexture(coreWoundTexture(), woundAnim.get());
        }
        woundAnim->update(deltaTime);
        woundAnim->applyTo(sprite);
        if (woundAnim->isFinished()) {
            showWound = false;
            switchTexture(coreIdleTexture(), idleAnim.get());
        }
        return;
    }

    if (&sprite.getTexture() != &coreIdleTexture()) {
        switchTexture(coreIdleTexture(), idleAnim.get());
    }
    idleAnim->update(deltaTime);
    idleAnim->applyTo(sprite);
}

void Server::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Server::setDesiredSize(float worldSize) {
    desiredSize = worldSize;
    applyScale();
}

void Server::applyScale() {
    sf::IntRect rect = sprite.getTextureRect();
    if (rect.size.x == 0 || desiredSize == 0.f) return;
    float scale = desiredSize / (float)rect.size.x;
    sprite.setScale({ scale, scale });
}

void Server::takeDamage(float amount) {
    currentHealth -= amount;
    if (currentHealth < 0.f) currentHealth = 0.f;
    if (currentHealth > 0.f) {
        showWound = true;
        woundAnim->reset();
    }
}
const sf::Texture* Server::getIconTexturePtr() const {
    return &coreIdleTexture();
}