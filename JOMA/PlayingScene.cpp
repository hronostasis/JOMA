#include "PlayingScene.h"
#include "Game.h"

PlayingScene::PlayingScene() {
    viruses.emplace_back(VirusType::Circle, sf::Vector2f(200.f, 300.f));
    viruses.emplace_back(VirusType::Triangle, sf::Vector2f(400.f, 300.f));
    viruses.emplace_back(VirusType::Square, sf::Vector2f(600.f, 300.f));
}

void PlayingScene::handleEvent(const sf::Event& event, Game& game) {
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = game.getWindow().mapPixelToCoords(mp->position);
            for (auto& virus : viruses) {
                if (virus.getGlobalBounds().contains(pos)) {
                    //выбрать этот вирус
                }
            }
        }
    }
    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Num1) { /* выбрать вирус 1 */ }
        if (kp->code == sf::Keyboard::Key::Num2) { /* выбрать вирус 2 */ }
        if (kp->code == sf::Keyboard::Key::Num3) { /* выбрать вирус 3 */ }
    }
}

void PlayingScene::update(float deltaTime, Game& game) {
    sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
    for (auto& virus : viruses) {
        virus.setHovered(virus.getGlobalBounds().contains(mousePos));
    }
}

void PlayingScene::render(sf::RenderWindow& window) {
    for (auto& virus : viruses) virus.draw(window);
}