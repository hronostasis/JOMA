#include "LevelSelectScene.h"
#include "Game.h"
#include "PlayingScene.h"
#include "MainMenuScene.h"

LevelSelectScene::LevelSelectScene()
    : titleText(font),
    buttonLabels{ sf::Text(font), sf::Text(font), sf::Text(font), sf::Text(font), sf::Text(font), sf::Text(font) },
    buttonStatus{ sf::Text(font), sf::Text(font), sf::Text(font), sf::Text(font), sf::Text(font), sf::Text(font) }
{
    font.openFromFile("C:/Windows/Fonts/consola.ttf");

    titleText.setString("SELECT LEVEL");
    titleText.setCharacterSize(40);
    titleText.setFillColor(sf::Color(60, 220, 90));
    titleText.setPosition({ 480.f, 60.f });

    for (int i = 0; i < 6; i++) {
        int col = i % 3;
        int row = i / 3;
        float x = 260.f + col * 260.f;
        float y = 200.f + row * 160.f;

        buttons[i].setSize({ 220.f, 120.f });
        buttons[i].setFillColor(sf::Color(45, 42, 60));
        buttons[i].setOutlineThickness(2.f);
        buttons[i].setOutlineColor(sf::Color(120, 110, 220));
        buttons[i].setPosition({ x, y });

        buttonLabels[i].setCharacterSize(24);
        buttonLabels[i].setFillColor(sf::Color::White);
        buttonLabels[i].setPosition({ x + 40.f, y + 30.f });

        buttonStatus[i].setCharacterSize(16);
        buttonStatus[i].setPosition({ x + 60.f, y + 75.f });

        if (i < 5) {
            buttonLabels[i].setString("Level " + std::to_string(i + 1));
            bool available = (i + 1) <= unlockedLevel;
            buttonStatus[i].setString(available ? "AVAILABLE" : "LOCKED");
            buttonStatus[i].setFillColor(available ? sf::Color(80, 220, 100) : sf::Color(150, 150, 150));
        }
        else {
            buttonLabels[i].setString("Back to menu");
            buttonStatus[i].setString("");
        }
    }
}

void LevelSelectScene::handleEvent(const sf::Event& event, Game& game) {
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = game.getWindow().mapPixelToCoords(mp->position);

            for (int i = 0; i < 6; i++) {
                if (!buttons[i].getGlobalBounds().contains(pos)) continue;

                if (i == 5) {
                    game.changeScene(std::make_unique<MainMenuScene>());
                    return;
                }
                if ((i + 1) <= unlockedLevel) {
                    game.changeScene(std::make_unique<PlayingScene>(i + 1));
                    return;
                }
            }
        }
    }
}

void LevelSelectScene::update(float deltaTime, Game& game) {
    sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));

    for (int i = 0; i < 6; i++) {
        bool clickable = (i == 5) || ((i + 1) <= unlockedLevel);
        bool hovered = clickable && buttons[i].getGlobalBounds().contains(mousePos);
        buttons[i].setOutlineColor(hovered ? sf::Color(170, 160, 255) : sf::Color(120, 110, 220));
        buttons[i].setFillColor(hovered ? sf::Color(65, 60, 85) : sf::Color(45, 42, 60));
    }
}

void LevelSelectScene::render(sf::RenderWindow& window) {
    window.draw(titleText);
    for (int i = 0; i < 6; i++) {
        window.draw(buttons[i]);
        window.draw(buttonLabels[i]);
        window.draw(buttonStatus[i]);
    }
}