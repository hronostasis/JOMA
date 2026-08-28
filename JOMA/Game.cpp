#include "Game.h"
#include "MainMenuScene.h"

Game::Game() : window(sf::VideoMode({ 1280u, 900u }), "JOMA") {
    currentScene = std::make_unique<MainMenuScene>();
}

void Game::run() {
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            currentScene->handleEvent(*event, *this);
        }
        float deltaTime = clock.restart().asSeconds();
        currentScene->update(deltaTime, *this);

        window.clear(sf::Color(10, 10, 20));
        currentScene->render(window);
        window.display();
    }
}

void Game::changeScene(std::unique_ptr<Scene> newScene) {
    currentScene = std::move(newScene);
}