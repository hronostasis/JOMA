#pragma once
#include <SFML/Graphics.hpp>
#include "PathGenerator.h"
#include <vector>

class TileMap {
public:
    TileMap(float cellSize, sf::Vector2f origin, int cols, int rows);
    void build(const std::vector<std::vector<GridPos>>& allPaths, GridPos serverBlockOrigin);
    void draw(sf::RenderWindow& window);
    void drawFog(sf::RenderWindow& window);
    sf::Vector2f worldPos(GridPos p) const;

    void revealCell(GridPos p);
    void revealAround(sf::Vector2f worldPosition, int radiusCells);

private:
    int cols, rows;
    float cellSize;
    sf::Vector2f origin;
    sf::Texture texStraight, texCorner, texServer, texHidden;
    std::vector<sf::Sprite> tileSprites;
    sf::Sprite serverSprite;
    std::vector<std::vector<bool>> revealed;
};