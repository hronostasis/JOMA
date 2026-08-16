#pragma once
#include "Scene.h"
#include "Virus.h"
#include <vector>

class PlayingScene : public Scene {
public:
    PlayingScene();
    void handleEvent(const sf::Event& event, Game& game) override;
    void update(float deltaTime, Game& game) override;
    void render(sf::RenderWindow& window) override;

private:
    std::vector<Virus> viruses;
};