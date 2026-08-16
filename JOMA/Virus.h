#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

enum class VirusType { Circle, Triangle, Square };

class Virus {
public:
    Virus(VirusType type, sf::Vector2f position);

    void draw(sf::RenderWindow& window);
    sf::FloatRect getGlobalBounds() const;
    void setHovered(bool hovered);

private:
    VirusType type;
    std::unique_ptr<sf::Shape> shape;
};