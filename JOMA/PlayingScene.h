#pragma once
#include "Scene.h"
#include "Virus.h"
#include "TileMap.h"
#include "PathGenerator.h"
#include <vector>

class PlayingScene : public Scene {
public:
    PlayingScene();
    void handleEvent(const sf::Event& event, Game& game) override;
    void update(float deltaTime, Game& game) override;
    void render(sf::RenderWindow& window) override;

private:
    static constexpr int cols = 16;
    static constexpr int rows = 9;
    static constexpr float cellSize = 70.f;

    TileMap tileMap;
    std::vector<Virus> viruses;
};