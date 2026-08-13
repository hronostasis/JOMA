#include <SFML/Graphics.hpp>

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    Victory,
    Defeat
};
int main() {
    sf::RenderWindow window(sf::VideoMode({ 1280u, 720u }), "JOMA");

    sf::Clock clock;
    //Красный круг (вирус 1)
    sf::CircleShape circle(30.f);
    circle.setFillColor(sf::Color(220, 40, 40));
    circle.setPosition({ 200.f, 300.f });

    //Жёлтый треугольник (вирус 2)
    sf::ConvexShape triangle;
    triangle.setPointCount(3);
    triangle.setPoint(0, { 0.f, -30.f });
    triangle.setPoint(1, { -30.f, 30.f });
    triangle.setPoint(2, { 30.f, 30.f });
    triangle.setFillColor(sf::Color(230, 210, 30));
    triangle.setPosition({ 400.f, 300.f });

    //Синий квадрат (вирус 3)
    sf::RectangleShape square({ 60.f, 60.f });
    square.setFillColor(sf::Color(40, 90, 220));
    square.setOrigin({ 30.f, 30.f });
    square.setPosition({ 600.f, 300.f });

    circle.setOutlineThickness(3.f);
    triangle.setOutlineThickness(3.f);
    square.setOutlineThickness(3.f);
    circle.setOutlineColor(sf::Color::Transparent);
    triangle.setOutlineColor(sf::Color::Transparent);
    square.setOutlineColor(sf::Color::Transparent);

    GameState state = GameState::Playing;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* mp = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mp->button == sf::Mouse::Button::Left) {
                    sf::Vector2f pos = window.mapPixelToCoords(mp->position);
                    if (circle.getGlobalBounds().contains(pos)) { /* выбрать вирус 1 */ }
                    if (triangle.getGlobalBounds().contains(pos)) { /* выбрать вирус 2 */ }
                    if (square.getGlobalBounds().contains(pos)) { /* выбрать вирус 3 */ }
                }
            }

            if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Num1) { /* выбрать вирус 1 */ }
                if (kp->code == sf::Keyboard::Key::Num2) { /* выбрать вирус 2 */ }
                if (kp->code == sf::Keyboard::Key::Num3) { /* выбрать вирус 3 */ }
            }
        }
        float deltaTime = clock.restart().asSeconds();

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        circle.setOutlineColor(circle.getGlobalBounds().contains(mousePos) ? sf::Color::White : sf::Color::Transparent);
        triangle.setOutlineColor(triangle.getGlobalBounds().contains(mousePos) ? sf::Color::White : sf::Color::Transparent);
        square.setOutlineColor(square.getGlobalBounds().contains(mousePos) ? sf::Color::White : sf::Color::Transparent);

        window.clear(sf::Color(10, 10, 20));

        switch (state) {
        case GameState::MainMenu:
            //тут потом отрисовка кнопки "Играть" и тд
            break;
        case GameState::Playing:
            window.draw(circle);
            window.draw(triangle);
            window.draw(square);
            break;
        case GameState::Paused:
            //затемнение + текст "Пауза"
            break;
        case GameState::Victory:
            //экран победы
            break;
        case GameState::Defeat:
            //экран поражения
            break;
        }

        window.display();
    }
    return 0;
}