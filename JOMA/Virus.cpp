#include "Virus.h"
#include <cmath>

namespace {
    VirusStats getStatsForType(VirusType type) {
        switch (type) {
        case VirusType::Circle:   return { 100.f,  80.f, 1.0f, 15.f };
        case VirusType::Triangle: return { 50.f, 140.f, 1.8f,  8.f };
        case VirusType::Square:   return { 200.f,  40.f, 0.6f, 30.f };
        }
        return {};
    }

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
    sf::Texture& middleDeathTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/middle_death.png"), true);
        return tex;
    }

    sf::Texture& arrowIdleTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/arrow_idle.png"), true);
        return tex;
    }
    sf::Texture& arrowAttackTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/arrow_attack.png"), true);
        return tex;
    }
    sf::Texture& arrowDeathTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/arrow_death.png"), true);
        return tex;
    }

    sf::Texture& idleTextureFor(VirusType t) { return t == VirusType::Triangle ? arrowIdleTexture() : middleIdleTexture(); }
    sf::Texture& attackTextureFor(VirusType t) { return t == VirusType::Triangle ? arrowAttackTexture() : middleAttackTexture(); }
    sf::Texture& deathTextureFor(VirusType t) { return t == VirusType::Triangle ? arrowDeathTexture() : middleDeathTexture(); }
}

Virus::Virus(VirusType type, sf::Vector2f position)
    : type(type), stats(getStatsForType(type)), currentHealth(stats.health)
{
    if (type == VirusType::Circle || type == VirusType::Triangle) {
        useSprite = true;

        idleAnim = std::make_unique<Animation>(idleTextureFor(type), 1, 1, 1.f);
        if (type == VirusType::Circle) {
            attackAnim = std::make_unique<Animation>(attackTextureFor(type), 2, 2, 0.15f);
            deathAnim = std::make_unique<Animation>(deathTextureFor(type), 3, 3, 0.12f, false);
        }
        else {
            attackAnim = std::make_unique<Animation>(attackTextureFor(type), 1, 2, 0.15f);
            deathAnim = std::make_unique<Animation>(deathTextureFor(type), 2, 2, 0.12f, false);
        }

        sprite = std::make_unique<sf::Sprite>(idleTextureFor(type));
        idleAnim->applyTo(*sprite);
        sf::IntRect rect = sprite->getTextureRect();
        sprite->setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
        sprite->setPosition(position);
    }
    else {
        auto square = std::make_unique<sf::RectangleShape>(sf::Vector2f(60.f, 60.f));
        square->setFillColor(sf::Color(40, 90, 220));
        shape = std::move(square);

        sf::FloatRect bounds = shape->getLocalBounds();
        shape->setOrigin({ bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });
        shape->setOutlineThickness(3.f);
        shape->setOutlineColor(sf::Color::Transparent);
        shape->setPosition(position);
    }
}

void Virus::draw(sf::RenderWindow& window) {
    if (useSprite) window.draw(*sprite);
    else if (isAlive()) window.draw(*shape);
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

void Virus::setBlockers(std::vector<std::pair<Damageable*, size_t>> blockersList) {
    blockers = std::move(blockersList);
    blockerCursor = 0;
}

bool Virus::isRemovable() const {
    if (currentHealth > 0.f) return false;
    if (!useSprite) return true;
    return deathAnim && deathAnim->isFinished();
}

void Virus::update(float deltaTime) {
    if (currentHealth <= 0.f) {
        if (useSprite) {
            sf::Texture& deathTex = deathTextureFor(type);
            if (&sprite->getTexture() != &deathTex) {
                sprite->setTexture(deathTex, false);
                deathAnim->reset();
                deathAnim->applyTo(*sprite);
                sf::IntRect rect = sprite->getTextureRect();
                sprite->setOrigin({ rect.size.x / 2.f, rect.size.y / 2.f });
                applySpriteScale();
            }
            deathAnim->update(deltaTime);
            deathAnim->applyTo(*sprite);
        }
        return;
    }

    while (blockerCursor < blockers.size()) {
        Damageable* d = blockers[blockerCursor].first;
        size_t idx = blockers[blockerCursor].second;
        if (!d || !d->isAlive() || pathIndex > idx) { blockerCursor++; continue; }
        break;
    }

    bool isFighting = false;
    if (blockerCursor < blockers.size() && pathIndex == blockers[blockerCursor].second) {
        isFighting = true;
    }

    if (useSprite) {
        Animation* anim = isFighting ? attackAnim.get() : idleAnim.get();
        sf::Texture& tex = isFighting ? attackTextureFor(type) : idleTextureFor(type);

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
        Damageable* d = blockers[blockerCursor].first;
        d->setAttacking(true);
        attackTimer += deltaTime;
        if (attackTimer >= 1.f / stats.attackSpeed) {
            attackTimer = 0.f;
            d->takeDamage(stats.attackPower);
            takeDamage(d->getAttackPower());
        }
        return;
    }

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