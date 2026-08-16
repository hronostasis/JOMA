#include "MainMenuScene.h"
#include "Game.h"
#include "PlayingScene.h"
#include "LevelSelectScene.h"
#include "UI.h"

MainMenuScene::MainMenuScene()
    : titleJust(font), titleOne(font), titleMore(font), titleAttack(font),
    startText(font), exitText(font)
{
    font.openFromFile("C:/Windows/Fonts/consola.ttf");

    titleJust.setString("Just ");
    titleJust.setCharacterSize(48);
    titleJust.setFillColor(sf::Color(70, 130, 240));

    titleOne.setString("One ");
    titleOne.setCharacterSize(48);
    titleOne.setFillColor(sf::Color(220, 60, 60));

    titleMore.setString("More ");
    titleMore.setCharacterSize(48);
    titleMore.setFillColor(sf::Color(230, 200, 40));

    titleAttack.setString("Attack");
    titleAttack.setCharacterSize(48);
    titleAttack.setFillColor(sf::Color(60, 190, 100));

    float totalWidth = titleJust.getLocalBounds().size.x
        + titleOne.getLocalBounds().size.x
        + titleMore.getLocalBounds().size.x
        + titleAttack.getLocalBounds().size.x;
    float x = (1280.f - totalWidth) / 2.f;
    float y = 150.f;

    titleJust.setPosition({ x, y });
    x += titleJust.getLocalBounds().size.x;
    titleOne.setPosition({ x, y });
    x += titleOne.getLocalBounds().size.x;
    titleMore.setPosition({ x, y });
    x += titleMore.getLocalBounds().size.x;
    titleAttack.setPosition({ x, y });

    startButton.setSize({ 220.f, 60.f });
    startButton.setFillColor(sf::Color::Transparent);
    startButton.setOutlineThickness(2.f);
    startButton.setOutlineColor(sf::Color(120, 110, 220));
    startButton.setPosition({ 530.f, 340.f });

    startText.setString("START GAME");
    startText.setFillColor(sf::Color::White);
    centerAndFitText(startText, startButton);

    exitButton.setSize({ 220.f, 60.f });
    exitButton.setFillColor(sf::Color::Transparent);
    exitButton.setOutlineThickness(2.f);
    exitButton.setOutlineColor(sf::Color(120, 110, 220));
    exitButton.setPosition({ 530.f, 420.f });

    exitText.setString("EXIT GAME");
    exitText.setFillColor(sf::Color::White);
    centerAndFitText(exitText, exitButton);
}

void MainMenuScene::handleEvent(const sf::Event& event, Game& game) {
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = game.getWindow().mapPixelToCoords(mp->position);
            if (startButton.getGlobalBounds().contains(pos)) {
                game.changeScene(std::make_unique<LevelSelectScene>());
                return;
            }
            if (exitButton.getGlobalBounds().contains(pos)) {
                game.getWindow().close();
            }
        }
    }
}

void MainMenuScene::update(float deltaTime, Game& game) {
    sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));

    bool startHovered = startButton.getGlobalBounds().contains(mousePos);
    startButton.setOutlineColor(startHovered ? sf::Color(170, 160, 255) : sf::Color(120, 110, 220));
    startButton.setFillColor(startHovered ? sf::Color(70, 65, 110) : sf::Color::Transparent);

    bool exitHovered = exitButton.getGlobalBounds().contains(mousePos);
    exitButton.setOutlineColor(exitHovered ? sf::Color(170, 160, 255) : sf::Color(120, 110, 220));
    exitButton.setFillColor(exitHovered ? sf::Color(70, 65, 110) : sf::Color::Transparent);
}

void MainMenuScene::render(sf::RenderWindow& window) {
    window.draw(titleJust);
    window.draw(titleOne);
    window.draw(titleMore);
    window.draw(titleAttack);
    window.draw(startButton);
    window.draw(startText);
    window.draw(exitButton);
    window.draw(exitText);
}