#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Animation.h"
#include "Damageable.h"

enum class TowerKind { Firewall, Spider };

struct TowerStats {
    float health;
    float attackPower;
};

class Tower : public Damageable {
public:
    Tower(TowerKind kind, sf::Vector2f position, float rotationDegrees);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void setDesiredSize(float worldSize);
    void setAttacking(bool value) override { isAttacking = value; }

    void takeDamage(float amount) override;
    bool isAlive() const override { return currentHealth > 0.f; }
    bool isDeathFinished() const { return deathAnim && deathAnim->isFinished(); }
    float getAttackPower() const override { return stats.attackPower; }

private:
    TowerKind kind;
    TowerStats stats;
    float currentHealth;

    sf::Sprite sprite;
    std::unique_ptr<Animation> idleAnim, attackAnim, deathAnim;
    bool isAttacking = false;
    float desiredSize = 0.f;

    void applyScale();
};