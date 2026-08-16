#include "Virus.h"

Virus::Virus(VirusType type, sf::Vector2f position) : type(type) {
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