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
}

Server::Server(sf::Vector2f position)
    : currentHealth(maxHealth), sprite(coreIdleTexture())
{
    idleAnim = std::make_unique<Animation>(coreIdleTexture(), 1, 1, 1.f);
    deathAnim = std::make_unique<Animation>(coreDeathTexture(), 4, 5, 0.08f, false);

    idleAnim->applyTo(sprite);
    sf::IntRect rect = sprite.getTextureRect();
    sprite.setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
    sprite.setPosition(position);
}

void Server::update(float deltaTime) {
    if (currentHealth <= 0.f) {
        if (&sprite.getTexture() != &coreDeathTexture()) {
            sprite.setTexture(coreDeathTexture(), false);
            deathAnim->reset();
            deathAnim->applyTo(sprite);
            sf::IntRect rect = sprite.getTextureRect();
            sprite.setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
            applyScale();
        }
        deathAnim->update(deltaTime);
        deathAnim->applyTo(sprite);
    }
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
}