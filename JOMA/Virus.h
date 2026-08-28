#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Animation.h"

enum class VirusType { Circle, Triangle, Square };

struct VirusStats {
    float health;
    float moveSpeed;
    float attackSpeed;
    float attackPower;
};

class Tower;

class Virus {
public:
    Virus(VirusType type, sf::Vector2f position);

    void draw(sf::RenderWindow& window);
    sf::FloatRect getGlobalBounds() const;
    void setHovered(bool hovered);
    void setDesiredSize(float worldSize);

    void setPath(std::vector<sf::Vector2f> newPath);
    void update(float deltaTime);
    bool hasReachedEnd() const { return reachedEnd; }

    float getHealth() const { return currentHealth; }
    float getMaxHealth() const { return stats.health; }
    float getMoveSpeed() const { return stats.moveSpeed; }
    float getAttackSpeed() const { return stats.attackSpeed; }
    float getAttackPower() const { return stats.attackPower; }
    sf::Vector2f getWorldPosition() const { return getPosition(); }
    bool isAlive() const { return currentHealth > 0.f; }
    bool isRemovable() const;

    void takeDamage(float amount);

    void setBlockers(std::vector<std::pair<Tower*, size_t>> blockersList);

private:
    VirusType type;
    VirusStats stats;
    float currentHealth;

    std::unique_ptr<sf::Shape> shape;

    bool useSprite = false;
    std::unique_ptr<sf::Sprite> sprite;
    std::unique_ptr<Animation> idleAnim, attackAnim, deathAnim;
    float desiredSize = 0.f;

    std::vector<sf::Vector2f> path;
    size_t pathIndex = 0;
    bool reachedEnd = false;

    std::vector<std::pair<Tower*, size_t>> blockers;
    size_t blockerCursor = 0;
    float attackTimer = 0.f;

    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);
    void applySpriteScale();
};