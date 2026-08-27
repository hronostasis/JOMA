#pragma once
#include "Scene.h"
#include "Virus.h"
#include "Tower.h"
#include "TileMap.h"
#include "PathGenerator.h"
#include <vector>
#include <memory>

struct SpawnPoint {
    std::vector<sf::Vector2f> waypoints;
    VirusType type;
    sf::Sprite sprite;

    SpawnPoint(sf::Texture& tex, std::vector<sf::Vector2f> wp, VirusType t)
        : waypoints(std::move(wp)), type(t), sprite(tex) {
    }
};

class PlayingScene : public Scene {
public:
    PlayingScene(int levelNumber = 1);
    void handleEvent(const sf::Event& event, Game& game) override;
    void update(float deltaTime, Game& game) override;
    void render(sf::RenderWindow& window) override;

private:
    static constexpr int cols = 16;
    static constexpr int rows = 9;
    static constexpr float cellSize = 70.f;

    int levelNumber;
    TileMap tileMap;
    sf::Texture spawnTexture;
    std::vector<SpawnPoint> spawnPoints;
    std::vector<Virus> viruses;
    int virusBudget = 10;

    std::unique_ptr<Tower> firewall;
    size_t firewallPathIndex = SIZE_MAX;
    size_t firewallBlockIndex = SIZE_MAX;
};