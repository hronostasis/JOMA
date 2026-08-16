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