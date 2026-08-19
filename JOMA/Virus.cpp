#include "Virus.h"
#include <cmath>
namespace {
    VirusStats getStatsForType(VirusType type) {
        switch (type) {
        case VirusType::Circle:   return { 50.f,  80.f, 1.0f, 10.f }; //средний брат
        case VirusType::Triangle: return { 25.f, 140.f, 1.8f,  4.f }; //быстрый, хрупкий
        case VirusType::Square:   return { 200.f,  40.f, 0.6f, 50.f }; //танк, медленный
        }
        return {};
    }
}

Virus::Virus(VirusType type, sf::Vector2f position)
    : type(type), stats(getStatsForType(type)), currentHealth(stats.health)
{
    switch (type) {
    case VirusType::Circle: {
        auto circle = std::make_unique<sf::CircleShape>(30.f);
        circle->setFillColor(sf::Color(220, 40, 40));
        shape = std::move(circle);
        break;
    }
    case VirusType::Triangle: {
        auto triangle = std::make_unique<sf::ConvexShape>();
        triangle->setPointCount(3);
        triangle->setPoint(0, { 0.f, -30.f });
        triangle->setPoint(1, { -30.f, 30.f });
        triangle->setPoint(2, { 30.f, 30.f });
        triangle->setFillColor(sf::Color(230, 210, 30));
        shape = std::move(triangle);
        break;
    }
    case VirusType::Square: {
        auto square = std::make_unique<sf::RectangleShape>(sf::Vector2f(60.f, 60.f));
        square->setFillColor(sf::Color(40, 90, 220));
        square->setOrigin({ 30.f, 30.f });
        shape = std::move(square);
        break;
    }
    }
    shape->setOutlineThickness(3.f);
    shape->setOutlineColor(sf::Color::Transparent);
    shape->setPosition(position);
}

void Virus::draw(sf::RenderWindow& window) {
    window.draw(*shape);
}

sf::FloatRect Virus::getGlobalBounds() const {
    return shape->getGlobalBounds();
}

void Virus::setHovered(bool hovered) {
    shape->setOutlineColor(hovered ? sf::Color::White : sf::Color::Transparent);
}

void Virus::takeDamage(float amount) {
    currentHealth -= amount;
    if (currentHealth < 0.f) currentHealth = 0.f;
}
void Virus::setPath(std::vector<sf::Vector2f> newPath) {
    path = std::move(newPath);
    pathIndex = 1; //индекс 0 — стартовая точка, вирус там уже стоит
    reachedEnd = path.size() <= 1;
}

void Virus::update(float deltaTime) {
    if (reachedEnd || pathIndex >= path.size()) { reachedEnd = true; return; }

    sf::Vector2f target = path[pathIndex];
    sf::Vector2f current = shape->getPosition();
    sf::Vector2f diff = target - current;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    float step = stats.moveSpeed * deltaTime;

    if (step >= dist) {
        shape->setPosition(target);
        pathIndex++;
        if (pathIndex >= path.size()) reachedEnd = true;
    }
    else {
        sf::Vector2f dir = diff / dist;
        shape->setPosition(current + dir * step);
    }
}