#pragma once
#include <SFML/Graphics.hpp>

inline void centerAndFitText(sf::Text& text, const sf::RectangleShape& button, unsigned maxSize = 28, float padding = 16.f) {
    unsigned size = maxSize;
    text.setCharacterSize(size);
    while (size > 8 &&
        (text.getLocalBounds().size.x > button.getSize().x - padding ||
            text.getLocalBounds().size.y > button.getSize().y - padding)) {
        size -= 2;
        text.setCharacterSize(size);
    }
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({ bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });
    text.setPosition({ button.getPosition().x + button.getSize().x / 2.f, button.getPosition().y + button.getSize().y / 2.f });
}
inline void drawNeonText(sf::RenderWindow& window, sf::Font& font, const std::string& str,
    unsigned size, sf::Vector2f center, sf::Color color)
{
    sf::Text glow(font, str, size);
    glow.setFillColor(sf::Color::Transparent);
    glow.setOutlineColor(sf::Color(color.r, color.g, color.b, 60));

    for (float thickness : { 10.f, 6.f, 3.f }) {
        glow.setOutlineThickness(thickness);
        sf::FloatRect b = glow.getLocalBounds();
        glow.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f });
        glow.setPosition(center);
        window.draw(glow);
    }

    sf::Text core(font, str, size);
    core.setFillColor(sf::Color::White);
    core.setOutlineColor(color);
    core.setOutlineThickness(2.f);
    sf::FloatRect b = core.getLocalBounds();
    core.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f });
    core.setPosition(center);
    window.draw(core);
}