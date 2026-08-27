#include "Virus.h"
#include "Tower.h"
#include <cmath>

namespace {
    sf::Texture& middleIdleTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/middle_idle.png"), true);
        return tex;
    }
    sf::Texture& middleAttackTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/middle_attack.png"), true);
        return tex;
    }
}
namespace {
    VirusStats getStatsForType(VirusType type) {
        switch (type) {
        case VirusType::Circle:   return { 50.f,  60.f, 1.0f, 10.f };
        case VirusType::Triangle: return { 25.f, 80.f, 1.8f,  8.f };
        case VirusType::Square:   return { 100.f,  30.f, 0.6f, 30.f };
        }
        return {};
    }
}

Virus::Virus(VirusType type, sf::Vector2f position)
    : type(type), stats(getStatsForType(type)), currentHealth(stats.health)
{
    if (type == VirusType::Circle) {
        useSprite = true;
        idleAnim = std::make_unique<Animation>(middleIdleTexture(), 1, 1, 1.f);
        attackAnim = std::make_unique<Animation>(middleAttackTexture(), 2, 2, 0.15f);

        sprite = std::make_unique<sf::Sprite>(middleIdleTexture());
        idleAnim->applyTo(*sprite);
        sf::IntRect rect = sprite->getTextureRect();
        sprite->setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
        sprite->setPosition(position);
    }
    else {
        if (type == VirusType::Triangle) {
            auto triangle = std::make_unique<sf::ConvexShape>();
            triangle->setPointCount(3);
            triangle->setPoint(0, { 0.f, -30.f });
            triangle->setPoint(1, { -30.f, 30.f });
            triangle->setPoint(2, { 30.f, 30.f });
            triangle->setFillColor(sf::Color(230, 210, 30));
            shape = std::move(triangle);
        }
        else {
            auto square = std::make_unique<sf::RectangleShape>(sf::Vector2f(60.f, 60.f));
            square->setFillColor(sf::Color(40, 90, 220));
            shape = std::move(square);
        }
        sf::FloatRect bounds = shape->getLocalBounds();
        shape->setOrigin({ bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });
        shape->setOutlineThickness(3.f);
        shape->setOutlineColor(sf::Color::Transparent);
        shape->setPosition(position);
    }
}

void Virus::draw(sf::RenderWindow& window) {
    if (useSprite) window.draw(*sprite);
    else window.draw(*shape);
}

sf::FloatRect Virus::getGlobalBounds() const {
    return useSprite ? sprite->getGlobalBounds() : shape->getGlobalBounds();
}

void Virus::setHovered(bool hovered) {
    if (useSprite) sprite->setColor(hovered ? sf::Color::White : sf::Color(210, 210, 210));
    else shape->setOutlineColor(hovered ? sf::Color::White : sf::Color::Transparent);
}

void Virus::setDesiredSize(float worldSize) {
    desiredSize = worldSize;
    if (useSprite) applySpriteScale();
    else shape->setScale({ worldSize / 60.f, worldSize / 60.f });
}

void Virus::applySpriteScale() {
    sf::IntRect rect = sprite->getTextureRect();
    if (rect.size.x == 0 || desiredSize == 0.f) return;
    float scale = desiredSize / (float)rect.size.x;
    sprite->setScale({ scale, scale });
}

sf::Vector2f Virus::getPosition() const {
    return useSprite ? sprite->getPosition() : shape->getPosition();
}

void Virus::setPosition(sf::Vector2f pos) {
    if (useSprite) sprite->setPosition(pos);
    else shape->setPosition(pos);
}

void Virus::setPath(std::vector<sf::Vector2f> newPath) {
    path = std::move(newPath);
    pathIndex = 1;
    reachedEnd = path.size() <= 1;
}

void Virus::setBlockers(std::vector<std::pair<Tower*, size_t>> blockersList) {
    blockers = std::move(blockersList);
    blockerCursor = 0;
}

void Virus::update(float deltaTime) {
    while (blockerCursor < blockers.size()) {
        Tower* t = blockers[blockerCursor].first;
        size_t idx = blockers[blockerCursor].second;
        if (!t || !t->isAlive() || pathIndex > idx) { blockerCursor++; continue; }
        break;
    }

    bool isFighting = false;
    if (blockerCursor < blockers.size() && pathIndex == blockers[blockerCursor].second) {
        isFighting = true;
    }

    if (useSprite) {
        Animation* anim = isFighting ? attackAnim.get() : idleAnim.get();
        sf::Texture& tex = isFighting ? middleAttackTexture() : middleIdleTexture();

        if (&sprite->getTexture() != &tex) {
            sprite->setTexture(tex, false);
            anim->applyTo(*sprite);
            sf::IntRect rect = sprite->getTextureRect();
            sprite->setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
            applySpriteScale();
        }
        anim->update(deltaTime);
        anim->applyTo(*sprite);
    }

    if (isFighting) {
        Tower* t = blockers[blockerCursor].first;
        t->setAttacking(true);
        attackTimer += deltaTime;
        if (attackTimer >= 1.f / stats.attackSpeed) {
            attackTimer = 0.f;
            t->takeDamage(stats.attackPower);
            takeDamage(t->getAttackPower());
        }
        return;
    }

    if (reachedEnd || pathIndex >= path.size()) { reachedEnd = true; return; }

    if (reachedEnd || pathIndex >= path.size()) { reachedEnd = true; return; }

    sf::Vector2f target = path[pathIndex];
    sf::Vector2f current = getPosition();
    sf::Vector2f diff = target - current;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    float step = stats.moveSpeed * deltaTime;

    if (step >= dist) {
        setPosition(target);
        pathIndex++;
        if (pathIndex >= path.size()) reachedEnd = true;
    }
    else {
        sf::Vector2f dir = diff / dist;
        setPosition(current + dir * step);
    }
}

void Virus::takeDamage(float amount) {
    currentHealth -= amount;
    if (currentHealth < 0.f) currentHealth = 0.f;
}