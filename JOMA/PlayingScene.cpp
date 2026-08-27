#include "PlayingScene.h"
#include "Game.h"
#include <cstdlib>
#include <random>
#include <algorithm>
#include <cmath>

PlayingScene::PlayingScene(int level)
    : levelNumber(level), tileMap(cellSize, sf::Vector2f(80.f, 45.f))
{
    (void)spawnTexture.loadFromFile("assets/tiles/spawn_virus.png");
    viruses.reserve(virusBudget);

    GridPos serverBlockOrigin = { cols / 2 - 1, rows / 2 - 1 };
    int pathCount = 2 + (rand() % 2);
    std::vector<SpawnSlot> spawns = pickSpawnSlots(cols, rows, pathCount);
    std::vector<std::vector<GridPos>> allPaths = generateAllPaths(spawns, serverBlockOrigin, cols, rows);

    tileMap.build(allPaths, serverBlockOrigin);

    std::vector<VirusType> allowedTypes;
    if (levelNumber == 1) allowedTypes = { VirusType::Circle };
    else allowedTypes = { VirusType::Circle, VirusType::Triangle, VirusType::Square };

    for (size_t p = 0; p < allPaths.size(); p++) {
        std::vector<sf::Vector2f> waypoints;
        for (const auto& cell : allPaths[p]) waypoints.push_back(tileMap.worldPos(cell));

        sf::Vector2f dirToBlock;
        switch (spawns[p].side) {
        case Side::North: dirToBlock = { 0.f, 1.f }; break;
        case Side::South: dirToBlock = { 0.f, -1.f }; break;
        case Side::West:  dirToBlock = { 1.f, 0.f }; break;
        case Side::East:  dirToBlock = { -1.f, 0.f }; break;
        }
        waypoints.push_back(waypoints.back() + dirToBlock * (cellSize / 2.f));

        VirusType type = allowedTypes[p % allowedTypes.size()];
        spawnPoints.emplace_back(spawnTexture, waypoints, type);

        SpawnPoint& sp = spawnPoints.back();
        sf::Vector2f texSize = { (float)spawnTexture.getSize().x, (float)spawnTexture.getSize().y };
        sp.sprite.setOrigin({ texSize.x / 2.f, texSize.y / 2.f });
        sp.sprite.setScale({ cellSize / texSize.x, cellSize / texSize.y });
        sp.sprite.setPosition(waypoints[0]);
    }

    // случайное число Файрволов (2-5), распределённых по случайным путям
    int totalFirewalls = 2 + (rand() % 4);

    std::vector<size_t> eligiblePaths;
    for (size_t p = 0; p < allPaths.size(); p++) if (allPaths[p].size() >= 4) eligiblePaths.push_back(p);

    std::vector<std::vector<size_t>> usedIndicesPerPath(allPaths.size());
    static std::mt19937 rng(std::random_device{}());

    int placed = 0, attempts = 0;
    while (placed < totalFirewalls && !eligiblePaths.empty() && attempts < totalFirewalls * 30) {
        attempts++;
        std::uniform_int_distribution<size_t> pathPick(0, eligiblePaths.size() - 1);
        size_t chosen = eligiblePaths[pathPick(rng)];
        auto& path = allPaths[chosen];

        std::uniform_int_distribution<size_t> idxPick(1, path.size() - 2);
        size_t idx = idxPick(rng);

        bool tooClose = false;
        for (size_t used : usedIndicesPerPath[chosen]) {
            if (std::abs((long long)used - (long long)idx) < 2) { tooClose = true; break; }
        }
        if (tooClose) continue;

        GridPos before = path[idx - 1];
        GridPos after = (idx + 1 < path.size()) ? path[idx + 1] : path[idx];
        bool horizontal = (before.y == after.y);

        sf::Vector2f pos = tileMap.worldPos(path[idx]);
        float rotation = horizontal ? 90.f : 0.f;

        WallInfo wall;
        wall.tower = std::make_unique<Tower>(pos, rotation);
        wall.tower->setDesiredSize(cellSize * 0.9f);
        wall.pathIndex = chosen;
        wall.blockIndex = idx;
        firewalls.push_back(std::move(wall));

        usedIndicesPerPath[chosen].push_back(idx);
        placed++;
    }
}

void PlayingScene::handleEvent(const sf::Event& event, Game& game) {
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = game.getWindow().mapPixelToCoords(mp->position);

            for (size_t i = 0; i < spawnPoints.size(); i++) {
                if (virusBudget <= 0) continue;
                SpawnPoint& sp = spawnPoints[i];
                if (sp.sprite.getGlobalBounds().contains(pos)) {
                    Virus virus(sp.type, sp.waypoints[0]);
                    virus.setDesiredSize(cellSize * 0.9f);
                    virus.setPath(sp.waypoints);

                    std::vector<std::pair<Tower*, size_t>> blockersForPath;
                    for (auto& w : firewalls) {
                        if (w.pathIndex == i) blockersForPath.push_back({ w.tower.get(), w.blockIndex });
                    }
                    std::sort(blockersForPath.begin(), blockersForPath.end(),
                        [](auto& a, auto& b) { return a.second < b.second; });
                    virus.setBlockers(std::move(blockersForPath));

                    viruses.push_back(std::move(virus));
                    virusBudget--;
                }
            }

            for (auto& virus : viruses) {
                if (virus.getGlobalBounds().contains(pos)) {
                    // выбрать этот вирус
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
    for (auto& w : firewalls) {
        if (w.tower && w.tower->isAlive()) w.tower->setAttacking(false);
    }

    sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
    for (auto& virus : viruses) {
        virus.setHovered(virus.getGlobalBounds().contains(mousePos));
        virus.update(deltaTime);
    }

    for (auto& w : firewalls) {
        if (w.tower && w.tower->isAlive()) w.tower->update(deltaTime);
    }

    viruses.erase(
        std::remove_if(viruses.begin(), viruses.end(), [](const Virus& v) { return !v.isAlive(); }),
        viruses.end()
    );
}

void PlayingScene::render(sf::RenderWindow& window) {
    tileMap.draw(window);
    for (auto& w : firewalls) {
        if (w.tower && w.tower->isAlive()) w.tower->draw(window);
    }
    for (auto& sp : spawnPoints) {
        sp.sprite.setColor(virusBudget > 0 ? sf::Color::White : sf::Color(255, 255, 255, 80));
        window.draw(sp.sprite);
    }
    for (auto& virus : viruses) virus.draw(window);
}