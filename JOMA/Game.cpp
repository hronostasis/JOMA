#include "Game.h"

Game::Game()
    : window(sf::VideoMode({ 1280u, 720u }), "JOMA"),
    state(GameState::Playing) //временно Playing, чтобы сразу видеть фигуры
{
    viruses.emplace_back(VirusType::Circle, sf::Vector2f(200.f, 300.f));
    viruses.emplace_back(VirusType::Triangle, sf::Vector2f(400.f, 300.f));
    viruses.emplace_back(VirusType::Square, sf::Vector2f(600.f, 300.f));
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        float deltaTime = clock.restart().asSeconds();
        update(deltaTime);
        render();
    }
}

void Game::handleEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            window.close();

        if (const auto* mp = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mp->button == sf::Mouse::Button::Left) {
                sf::Vector2f pos = window.mapPixelToCoords(mp->position);
                for (auto& virus : viruses) {
                    if (virus.getGlobalBounds().contains(pos)) {
                        //выбрать этот вирус
                    }
                }
            }
        }

        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Num1) { /* выбрать вирус 1 */ }
            if (kp->code == sf::Keyboard::Key::Num2) { /* выбрать вирус 2 */ }
            if (kp->code == sf::Keyboard::Key::Num3) { /* выбрать вирус 3 */ }
        }
    }
}

void Game::update(float deltaTime) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (auto& virus : viruses) {
        virus.setHovered(virus.getGlobalBounds().contains(mousePos));
    }
}

void Game::render() {
    window.clear(sf::Color(10, 10, 20));

    switch (state) {
    case GameState::Playing:
        for (auto& virus : viruses) virus.draw(window);
        break;
    default:
        break;
    }

    window.display();
}