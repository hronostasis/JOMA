#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Animation.h"
#include "Damageable.h"

class Server : public Damageable {
public:
    Server(sf::Vector2f position);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void setDesiredSize(float worldSize);

    void takeDamage(float amount) override;
    bool isAlive() const override { return currentHealth > 0.f; }
    bool isDeathFinished() const { return deathAnim && deathAnim->isFinished(); }
    float getHealth() const { return currentHealth; }
    float getMaxHealth() const { return maxHealth; }
    const sf::Texture* getIconTexturePtr() const;

private:
    float maxHealth = 500.f;
    float currentHealth;

    sf::Sprite sprite;
    std::unique_ptr<Animation> idleAnim, deathAnim, woundAnim;
    bool showWound = false;
    float desiredSize = 0.f;

    void applyScale();
    void switchTexture(sf::Texture& tex, Animation* anim);
};