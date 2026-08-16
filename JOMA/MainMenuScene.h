#pragma once
#include "Scene.h"

class MainMenuScene : public Scene {
public:
    MainMenuScene();
    void handleEvent(const sf::Event& event, Game& game) override;
    void update(float deltaTime, Game& game) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Font font;
    sf::Text titleJust, titleOne, titleMore, titleAttack;
    sf::RectangleShape startButton;
    sf::Text startText;
    sf::RectangleShape exitButton;
    sf::Text exitText;
};