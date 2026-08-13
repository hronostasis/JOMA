#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1280u, 720u }), "JOMA");

    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();

        window.clear(sf::Color(10, 10, 20)); //тёмный фон под неон
        window.display();
    }

    return 0;
}