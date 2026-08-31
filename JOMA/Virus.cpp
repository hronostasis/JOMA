#include "Virus.h"
#include <cmath>

namespace {
    VirusStats getStatsForType(VirusType type) {
        switch (type) {
        case VirusType::Circle:   return { 50.f,  40.f, 1.0f, 10.f };
        case VirusType::Triangle: return { 25.f, 80.f, 1.8f,  8.f };
        case VirusType::Square:   return { 100.f,  30.f, 0.6f, 30.f };
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

    sf::Texture& tankIdleTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/tank_idle.png"), true);
        return tex;
    }
    sf::Texture& tankAttackTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/tank_attack.png"), true);
        return tex;
    }
    sf::Texture& tankDeathTexture() {
        static sf::Texture tex;
        static bool loaded = ((void)tex.loadFromFile("assets/tiles/tank_death.png"), true);
        return tex;
    }

    sf::Texture& idleTextureFor(VirusType t) {
        if (t == VirusType::Circle) return middleIdleTexture();
        if (t == VirusType::Triangle) return arrowIdleTexture();
        return tankIdleTexture();
    }
    sf::Texture& attackTextureFor(VirusType t) {
        if (t == VirusType::Circle) return middleAttackTexture();
        if (t == VirusType::Triangle) return arrowAttackTexture();
        return tankAttackTexture();
    }
    sf::Texture& deathTextureFor(VirusType t) {
        if (t == VirusType::Circle) return middleDeathTexture();
        if (t == VirusType::Triangle) return arrowDeathTexture();
        return tankDeathTexture();
    }
}

Virus::Virus(VirusType type, sf::Vector2f position)
    : type(type), stats(getStatsForType(type)), currentHealth(stats.health)
{
    idleAnim = std::make_unique<Animation>(idleTextureFor(type), 1, 1, 1.f);

    if (type == VirusType::Circle) {
        attackAnim = std::make_unique<Animation>(attackTextureFor(type), 2, 1, 0.15f);
        deathAnim = std::make_unique<Animation>(deathTextureFor(type), 3, 3, 0.12f, false);
    }
    else if (type == VirusType::Triangle) {
        attackAnim = std::make_unique<Animation>(attackTextureFor(type), 1, 2, 0.15f);
        deathAnim = std::make_unique<Animation>(deathTextureFor(type), 2, 2, 0.12f, false);
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

void Virus::draw(sf::RenderWindow& window) {
    window.draw(*sprite);
}

sf::FloatRect Virus::getGlobalBounds() const {
    return sprite->getGlobalBounds();
}

void Virus::setHovered(bool hovered) {
    sprite->setColor(hovered ? sf::Color::White : sf::Color(210, 210, 210));
}

void Virus::setDesiredSize(float worldSize) {
    desiredSize = worldSize;
    applySpriteScale();
}

void Virus::applySpriteScale() {
    sf::IntRect rect = sprite->getTextureRect();
    if (rect.size.x == 0 || desiredSize == 0.f) return;
    float scale = desiredSize / (float)rect.size.x;
    sprite->setScale({ scale, scale });
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
    return deathAnim && deathAnim->isFinished();
}

void Virus::update(float deltaTime) {
    if (currentHealth <= 0.f) {
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
    sf::Vector2f current = sprite->getPosition();
    sf::Vector2f diff = target - current;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    float step = stats.moveSpeed * deltaTime;

    if (step >= dist) {
        sprite->setPosition(target);
        pathIndex++;
        if (pathIndex >= path.size()) reachedEnd = true;
    }
    else {
        sf::Vector2f dir = diff / dist;
        sprite->setPosition(current + dir * step);
    }
}

void Virus::takeDamage(float amount) {
    currentHealth -= amount;
    if (currentHealth < 0.f) currentHealth = 0.f;
}