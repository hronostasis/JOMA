#pragma once
#include "Scene.h"
#include "Virus.h"
#include "Tower.h"
#include "Server.h"
#include "TileMap.h"
#include "PathGenerator.h"
#include "UI.h"
#include <vector>
#include <memory>

struct SpawnPoint {
    std::vector<sf::Vector2f> waypoints;
    VirusType type;
    Side side;
    sf::Sprite sprite;

    SpawnPoint(sf::Texture& tex, std::vector<sf::Vector2f> wp, VirusType t, Side s)
        : waypoints(std::move(wp)), type(t), side(s), sprite(tex) {
    }
};

struct WallInfo {
    std::unique_ptr<Tower> tower;
    size_t pathIndex = 0;
    size_t blockIndex = 0;
};

enum class LevelResult { None, Victory, Defeat };

class PlayingScene : public Scene {
public:
    PlayingScene(int levelNumber = 1);
    void handleEvent(const sf::Event& event, Game& game) override;
    void update(float deltaTime, Game& game) override;
    void render(sf::RenderWindow& window) override;

private:
    static constexpr int cols = 16;
    static constexpr int rows = 12;
    static constexpr float cellSize = 70.f;
    static constexpr int fogRevealRadius = 2;

    int levelNumber;
    TileMap tileMap;
    std::unique_ptr<Server> server;
    std::vector<SpawnPoint> spawnPoints;
    std::vector<Virus> viruses;
    int virusBudget = 10;
    int selectedSpawnIndex = -1;

    std::vector<WallInfo> firewalls;

    LevelResult result = LevelResult::None;
    sf::Font bannerFont;
    sf::RectangleShape btnA, btnB;
    sf::Text btnAText, btnBText;

    void setupBanner();
    void checkWinLoseConditions();
    void trySpawnVirus(size_t i);
};