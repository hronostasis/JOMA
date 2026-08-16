#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Virus.h"

enum class GameState { MainMenu, Playing, Paused, Victory, Defeat };

class Game {
public:
    Game();
    void run();

private:
    void handleEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow window;
    sf::Clock clock;
    GameState state;
    std::vector<Virus> viruses;
};