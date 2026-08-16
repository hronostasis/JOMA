#pragma once
#include <SFML/Graphics.hpp>
#include "Scene.h"
#include <memory>

class Game {
public:
    Game();
    void run();
    void changeScene(std::unique_ptr<Scene> newScene);
    sf::RenderWindow& getWindow() { return window; }

private:
    sf::RenderWindow window;
    sf::Clock clock;
    std::unique_ptr<Scene> currentScene;
};