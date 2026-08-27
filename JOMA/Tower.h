#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Animation.h"

struct TowerStats {
    float health;
    float attackPower;
};

class Tower {
public:
    Tower(sf::Vector2f position, float rotationDegrees);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void setDesiredSize(float worldSize);
    void setAttacking(bool value) { isAttacking = value; }

    void takeDamage(float amount);
    bool isAlive() const { return currentHealth > 0.f; }
    float getAttackPower() const { return stats.attackPower; }

private:
    TowerStats stats;
    float currentHealth;

    sf::Sprite sprite;
    std::unique_ptr<Animation> idleAnim, attackAnim;
    bool isAttacking = false;
    float desiredSize = 0.f;

    void applyScale();
};