#include "TileMap.h"
#include <cmath>

TileMap::TileMap(float cellSize, sf::Vector2f origin, int cols, int rows)
    : cols(cols), rows(rows), cellSize(cellSize), origin(origin), serverSprite(texServer)
{
    (void)texStraight.loadFromFile("assets/tiles/path_straight.png");
    (void)texCorner.loadFromFile("assets/tiles/path_corner.png");
    (void)texServer.loadFromFile("assets/tiles/entity_core.png");
    (void)texHidden.loadFromFile("assets/tiles/hidden_cell.png");
    serverSprite.setTexture(texServer, true);

    revealed.assign(cols, std::vector<bool>(rows, false));
}

sf::Vector2f TileMap::worldPos(GridPos p) const {
    return { origin.x + p.x * cellSize + cellSize / 2.f, origin.y + p.y * cellSize + cellSize / 2.f };
}

static int getDir(GridPos from, GridPos to) {
    if (to.x > from.x) return 0;
    if (to.y > from.y) return 1;
    if (to.x < from.x) return 2;
    return 3;
}

static int directionTowardBlock(GridPos cell, GridPos serverBlockOrigin) {
    if (cell.y < serverBlockOrigin.y) return 1;
    if (cell.y > serverBlockOrigin.y + 2) return 3;
    if (cell.x < serverBlockOrigin.x) return 0;
    return 2;
}

void TileMap::build(const std::vector<std::vector<GridPos>>& allPaths, GridPos serverBlockOrigin) {
    tileSprites.clear();

    for (const auto& path : allPaths) {
        for (size_t i = 1; i < path.size(); i++) {
            GridPos cell = path[i];

            int inDir = getDir(path[i - 1], path[i]);
            int outDir = (i + 1 < path.size()) ? getDir(path[i], path[i + 1]) : directionTowardBlock(cell, serverBlockOrigin);

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
            sprite.setScale({ cellSize / texSize.x, cellSize / texSize.y });
            sprite.setPosition(worldPos(cell));
            sprite.setRotation(sf::degrees(rotation));
            tileSprites.push_back(sprite);
        }
    }

    sf::Vector2f serverCenter = worldPos({ serverBlockOrigin.x + 1, serverBlockOrigin.y + 1 });
    sf::Vector2f serverTexSize = { (float)texServer.getSize().x, (float)texServer.getSize().y };
    float serverVisualSize = cellSize * 3.f;
    serverSprite.setOrigin({ serverTexSize.x / 2.f, serverTexSize.y / 2.f });
    serverSprite.setScale({ serverVisualSize / serverTexSize.x, serverVisualSize / serverTexSize.y });
    serverSprite.setPosition(serverCenter);
}

void TileMap::draw(sf::RenderWindow& window) {
    for (auto& sprite : tileSprites) window.draw(sprite);
    window.draw(serverSprite);
}

void TileMap::revealCell(GridPos p) {
    if (p.x < 0 || p.x >= cols || p.y < 0 || p.y >= rows) return;
    revealed[p.x][p.y] = true;
}

void TileMap::revealAround(sf::Vector2f worldPosition, int radiusCells) {
    int cx = (int)std::floor((worldPosition.x - origin.x) / cellSize);
    int cy = (int)std::floor((worldPosition.y - origin.y) / cellSize);

    for (int dx = -radiusCells; dx <= radiusCells; dx++) {
        for (int dy = -radiusCells; dy <= radiusCells; dy++) {
            if (dx * dx + dy * dy > radiusCells * radiusCells) continue;
            revealCell({ cx + dx, cy + dy });
        }
    }
}

void TileMap::drawFog(sf::RenderWindow& window) {
    sf::Sprite fogSprite(texHidden);
    sf::Vector2f texSize = { (float)texHidden.getSize().x, (float)texHidden.getSize().y };
    fogSprite.setScale({ cellSize / texSize.x, cellSize / texSize.y });

    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            if (revealed[x][y]) continue;
            fogSprite.setPosition({ origin.x + x * cellSize, origin.y + y * cellSize });
            window.draw(fogSprite);
        }
    }
}