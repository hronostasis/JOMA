#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

enum class VirusType { Circle, Triangle, Square };

struct VirusStats {
    float health;
    float moveSpeed;    //пиксели/сек
    float attackSpeed;  //урон/сек
    float attackPower;  //урон за тычку
};

class Virus {
public:
    Virus(VirusType type, sf::Vector2f position);

    void draw(sf::RenderWindow& window);
    sf::FloatRect getGlobalBounds() const;
    void setHovered(bool hovered);

    float getHealth() const { return currentHealth; }
    float getMaxHealth() const { return stats.health; }
    float getMoveSpeed() const { return stats.moveSpeed; }
    float getAttackSpeed() const { return stats.attackSpeed; }
    float getAttackPower() const { return stats.attackPower; }
    bool isAlive() const { return currentHealth > 0.f; }

    void takeDamage(float amount);

    void setPath(std::vector<sf::Vector2f> newPath);
    void update(float deltaTime);
    bool hasReachedEnd() const { return reachedEnd; }
    void setScale(float scale) { shape->setScale({ scale, scale }); }
private:
    VirusType type;
    std::unique_ptr<sf::Shape> shape;
    VirusStats stats;
    float currentHealth;
    std::vector<sf::Vector2f> path;
    size_t pathIndex = 0;
    bool reachedEnd = false;
};