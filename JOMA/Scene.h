#pragma once
#include <SFML/Graphics.hpp>

class Game;

class Scene {
public:
    virtual ~Scene() = default;
    virtual void handleEvent(const sf::Event& event, Game& game) = 0;
    virtual void update(float deltaTime, Game& game) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};