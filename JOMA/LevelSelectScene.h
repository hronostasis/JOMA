#pragma once
#include "Scene.h"
#include <array>

class LevelSelectScene : public Scene {
public:
    LevelSelectScene();
    void handleEvent(const sf::Event& event, Game& game) override;
    void update(float deltaTime, Game& game) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Font font;
    sf::Text titleText;
    std::array<sf::RectangleShape, 6> buttons; //5 уровней + Назад в меню
    std::array<sf::Text, 6> buttonLabels;
    std::array<sf::Text, 6> buttonStatus;
    int unlockedLevel = 1; //хардкожаная заглушка
};