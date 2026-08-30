#include "PlayingScene.h"
#include "Game.h"
#include <cstdlib>
#include <random>
#include <algorithm>
#include <cmath>
#include "LevelSelectScene.h"

PlayingScene::PlayingScene(int level)
    : levelNumber(level), tileMap(cellSize, sf::Vector2f(80.f, 45.f), cols, rows),
    btnAText(bannerFont), btnBText(bannerFont)
{
    (void)spawnTexture.loadFromFile("assets/tiles/spawn_virus.png");
    (void)bannerFont.openFromFile("C:/Windows/Fonts/consola.ttf");
    viruses.reserve(virusBudget);

    GridPos serverBlockOrigin = { cols / 2 - 1, rows / 2 - 1 };
    int pathCount = 2 + (rand() % 2);
    std::vector<SpawnSlot> spawns = pickSpawnSlots(cols, rows, pathCount);
    std::vector<std::vector<GridPos>> allPaths = generateAllPaths(spawns, serverBlockOrigin, cols, rows);

    tileMap.build(allPaths, serverBlockOrigin);

    sf::Vector2f serverCenter = tileMap.worldPos({ serverBlockOrigin.x + 1, serverBlockOrigin.y + 1 });
    server = std::make_unique<Server>(serverCenter);
    server->setDesiredSize(cellSize * 3.f);

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

        tileMap.revealCell(spawns[p].pos);
    }

    // расстановка Файрволов (2-5, все уровни)
    int totalFirewalls = 2 + (rand() % 4);
    std::vector<size_t> eligiblePaths;
    for (size_t p = 0; p < allPaths.size(); p++) if (allPaths[p].size() >= 3) eligiblePaths.push_back(p);

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
        wall.tower = std::make_unique<Tower>(TowerKind::Firewall, pos, rotation);
        wall.tower->setDesiredSize(cellSize * 0.9f);
        wall.pathIndex = chosen;
        wall.blockIndex = idx;
        firewalls.push_back(std::move(wall));

        usedIndicesPerPath[chosen].push_back(idx);
        placed++;
    }

    //расстановка Спайдеров (с уровня 2 максимум 2 на карту, обычно 1 на путь)
    if (levelNumber >= 2) {
        int totalSpiders = rand() % 3; //0, 1 или 2
        std::vector<size_t> pathsWithoutSpider;
        for (size_t p = 0; p < allPaths.size(); p++) if (allPaths[p].size() >= 3) pathsWithoutSpider.push_back(p);

        int spidersPlaced = 0, spiderAttempts = 0;
        while (spidersPlaced < totalSpiders && !pathsWithoutSpider.empty() && spiderAttempts < totalSpiders * 30) {
            spiderAttempts++;
            std::uniform_int_distribution<size_t> pathPick(0, pathsWithoutSpider.size() - 1);
            size_t pickIdx = pathPick(rng);
            size_t chosen = pathsWithoutSpider[pickIdx];
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
            wall.tower = std::make_unique<Tower>(TowerKind::Spider, pos, rotation);
            wall.tower->setDesiredSize(cellSize * 0.9f);
            wall.pathIndex = chosen;
            wall.blockIndex = idx;
            firewalls.push_back(std::move(wall));

            usedIndicesPerPath[chosen].push_back(idx);
            pathsWithoutSpider.erase(pathsWithoutSpider.begin() + pickIdx); //обычно 1 на путь
            spidersPlaced++;
        }
    }

    setupBanner();
}

void PlayingScene::setupBanner() {
    btnA.setSize({ 240.f, 60.f });
    btnA.setFillColor(sf::Color(45, 42, 60));
    btnA.setOutlineThickness(2.f);
    btnA.setOutlineColor(sf::Color(120, 110, 220));
    btnA.setPosition({ 1280.f / 2.f - 260.f, 620.f });

    btnB.setSize({ 240.f, 60.f });
    btnB.setFillColor(sf::Color(45, 42, 60));
    btnB.setOutlineThickness(2.f);
    btnB.setOutlineColor(sf::Color(120, 110, 220));
    btnB.setPosition({ 1280.f / 2.f + 20.f, 620.f });

    btnAText.setString("Level panel");
    btnAText.setFillColor(sf::Color::White);
    centerAndFitText(btnAText, btnA);

    btnBText.setFillColor(sf::Color::White);
}

void PlayingScene::checkWinLoseConditions() {
    if (result != LevelResult::None) return;

    if (!server->isAlive() && server->isDeathFinished()) {
        result = LevelResult::Victory;
        btnBText.setString("Next level");
        centerAndFitText(btnBText, btnB);
        return;
    }

    if (virusBudget <= 0 && viruses.empty() && server->isAlive()) {
        result = LevelResult::Defeat;
        btnBText.setString("Again");
        centerAndFitText(btnBText, btnB);
        return;
    }
}

void PlayingScene::handleEvent(const sf::Event& event, Game& game) {
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = game.getWindow().mapPixelToCoords(mp->position);

            if (result != LevelResult::None) {
                if (btnA.getGlobalBounds().contains(pos)) {
                    game.changeScene(std::make_unique<LevelSelectScene>());
                    return;
                }
                if (btnB.getGlobalBounds().contains(pos)) {
                    if (result == LevelResult::Victory)
                        game.changeScene(std::make_unique<PlayingScene>(levelNumber + 1));
                    else
                        game.changeScene(std::make_unique<PlayingScene>(levelNumber));
                    return;
                }
                return;
            }

            for (size_t i = 0; i < spawnPoints.size(); i++) {
                if (virusBudget <= 0) continue;
                SpawnPoint& sp = spawnPoints[i];
                if (sp.sprite.getGlobalBounds().contains(pos)) {
                    Virus virus(sp.type, sp.waypoints[0]);
                    virus.setDesiredSize(cellSize * 0.9f);
                    virus.setPath(sp.waypoints);

                    std::vector<std::pair<Damageable*, size_t>> blockersForPath;
                    for (auto& w : firewalls) {
                        if (w.pathIndex == i) blockersForPath.push_back({ w.tower.get(), w.blockIndex });
                    }
                    std::sort(blockersForPath.begin(), blockersForPath.end(),
                        [](auto& a, auto& b) { return a.second < b.second; });

                    blockersForPath.push_back({ server.get(), sp.waypoints.size() - 1 });

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
    if (result != LevelResult::None) {
        sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
        bool hoverA = btnA.getGlobalBounds().contains(mousePos);
        bool hoverB = btnB.getGlobalBounds().contains(mousePos);
        btnA.setFillColor(hoverA ? sf::Color(70, 65, 110) : sf::Color(45, 42, 60));
        btnB.setFillColor(hoverB ? sf::Color(70, 65, 110) : sf::Color(45, 42, 60));
        server->update(deltaTime);
        return;
    }

    for (auto& w : firewalls) {
        if (w.tower && w.tower->isAlive()) w.tower->setAttacking(false);
    }

    sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
    for (auto& virus : viruses) {
        virus.setHovered(virus.getGlobalBounds().contains(mousePos));
        virus.update(deltaTime);
        if (virus.isAlive()) tileMap.revealAround(virus.getWorldPosition(), fogRevealRadius);
    }

    for (auto& w : firewalls) {
        if (w.tower && (w.tower->isAlive() || !w.tower->isDeathFinished())) w.tower->update(deltaTime);
    }

    server->update(deltaTime);

    viruses.erase(
        std::remove_if(viruses.begin(), viruses.end(), [](const Virus& v) { return v.isRemovable(); }),
        viruses.end()
    );

    checkWinLoseConditions();
}

void PlayingScene::render(sf::RenderWindow& window) {
    tileMap.draw(window);
    for (auto& w : firewalls) {
        if (w.tower && (w.tower->isAlive() || !w.tower->isDeathFinished())) w.tower->draw(window);
    }
    server->draw(window);
    for (auto& sp : spawnPoints) {
        sp.sprite.setColor(virusBudget > 0 ? sf::Color::White : sf::Color(255, 255, 255, 80));
        window.draw(sp.sprite);
    }
    for (auto& virus : viruses) virus.draw(window);
    tileMap.drawFog(window);

    if (result != LevelResult::None) {
        sf::RectangleShape overlay({ 1280.f, 900.f });
        overlay.setFillColor(sf::Color(0, 0, 0, 160));
        window.draw(overlay);

        sf::Color neon = (result == LevelResult::Victory) ? sf::Color(60, 200, 255) : sf::Color(230, 40, 60);
        std::string title = (result == LevelResult::Victory) ? "result: VICTORY" : "result: DEFEAT";
        drawNeonText(window, bannerFont, title, 90, { 1280.f / 2.f, 380.f }, neon);

        window.draw(btnA);
        window.draw(btnAText);
        window.draw(btnB);
        window.draw(btnBText);
    }
}