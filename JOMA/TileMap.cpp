#include "TileMap.h"

TileMap::TileMap(float cellSize) : cellSize(cellSize), serverSprite(texServer) {
    texStraight.loadFromFile("assets/tiles/path_straight.png");
    texCorner.loadFromFile("assets/tiles/path_corner.png");
    texServer.loadFromFile("assets/tiles/entity_core.png");
    serverSprite.setTexture(texServer);
}

sf::Vector2f TileMap::toWorld(GridPos p) const {
    return { p.x * cellSize + cellSize / 2.f, p.y * cellSize + cellSize / 2.f };
}

int getDir(GridPos from, GridPos to) {
    if (to.x > from.x) return 0; //восток
    if (to.y > from.y) return 1; //юг
    if (to.x < from.x) return 2; //запад
    return 3; //север
}

void TileMap::build(const std::vector<std::vector<GridPos>>& allPaths, GridPos serverPos) {
    tileSprites.clear();

    for (const auto& path : allPaths) {
        for (size_t i = 0; i + 1 < path.size(); i++) {
            GridPos cell = path[i];
            if (cell.x == serverPos.x && cell.y == serverPos.y) continue;

            int inDir = (i == 0) ? getDir(path[i], path[i + 1]) : getDir(path[i - 1], path[i]);
            int outDir = getDir(path[i], path[i + 1]);

            sf::Sprite sprite(texStraight);
            float rotation = 0.f;

            if (inDir == outDir || (inDir + 2) % 4 == outDir) {
                sprite.setTexture(texStraight);
                rotation = (outDir % 2 == 0) ? 90.f : 0.f;
            }
            else {
                int enterFrom = (inDir + 2) % 4;
                int pair = enterFrom * 4 + outDir;
                if (pair == 3 * 4 + 0 || pair == 0 * 4 + 3) rotation = 0.f;
                else if (pair == 0 * 4 + 1 || pair == 1 * 4 + 0) rotation = 90.f;
                else if (pair == 1 * 4 + 2 || pair == 2 * 4 + 1) rotation = 180.f;
                else rotation = 270.f;
                sprite.setTexture(texCorner);
            }

            sf::Vector2f texSize = { (float)sprite.getTexture().getSize().x, (float)sprite.getTexture().getSize().y };
            sprite.setOrigin({ texSize.x / 2.f, texSize.y / 2.f });
            sprite.setPosition(toWorld(cell));
            sprite.setRotation(sf::degrees(rotation));
            tileSprites.push_back(sprite);
        }
    }

    sf::Vector2f serverTexSize = { (float)texServer.getSize().x, (float)texServer.getSize().y };
    serverSprite.setOrigin({ serverTexSize.x / 2.f, serverTexSize.y / 2.f });
    serverSprite.setPosition(toWorld(serverPos));
}

void TileMap::draw(sf::RenderWindow& window) {
    for (auto& sprite : tileSprites) window.draw(sprite);
    window.draw(serverSprite);
}