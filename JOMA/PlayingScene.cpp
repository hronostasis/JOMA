#include "PlayingScene.h"
#include "Game.h"
#include <cstdlib>

PlayingScene::PlayingScene()
    : tileMap(cellSize, sf::Vector2f(80.f, 45.f))
{
    GridPos server = { cols / 2, rows / 2 };
    int pathCount = 2 + (rand() % 2);
    std::vector<GridPos> spawns = pickSpawnPoints(cols, rows, pathCount);
    std::vector<std::vector<GridPos>> allPaths = generateAllPaths(spawns, server, cols, rows);

    tileMap.build(allPaths, server);

    VirusType types[] = { VirusType::Circle, VirusType::Triangle, VirusType::Square };
    int typeIndex = 0;

    for (const auto& gridPath : allPaths) {
        std::vector<sf::Vector2f> waypoints;
        for (const auto& cell : gridPath) waypoints.push_back(tileMap.worldPos(cell));

        Virus virus(types[typeIndex % 3], waypoints[0]);
        virus.setPath(waypoints);
        viruses.push_back(std::move(virus));
        typeIndex++;
    }
}

void PlayingScene::handleEvent(const sf::Event& event, Game& game) {
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = game.getWindow().mapPixelToCoords(mp->position);
            for (auto& virus : viruses) {
                if (virus.getGlobalBounds().contains(pos)) {
                    //выбрать этот вирус
                }
            }
        }
    }
    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Num1) { /* выбрать вирус 1 */ }
        if (kp->code == sf::Keyboard::Key::Num2) { /* выбрать вирус 2 */ }
        if (kp->code == sf::Keyboard::Key::Num3) { /* выбрать вирус 3 */ }
    }
}

void PlayingScene::update(float deltaTime, Game& game) {
    sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
    for (auto& virus : viruses) {
        virus.setHovered(virus.getGlobalBounds().contains(mousePos));
        virus.update(deltaTime);
    }
}

void PlayingScene::render(sf::RenderWindow& window) {
    tileMap.draw(window);
    for (auto& virus : viruses) virus.draw(window);
}