#pragma once
#include <SFML/Graphics.hpp>
#include "PathGenerator.h"
#include <vector>

class TileMap {
public:
    TileMap(float cellSize);
    void build(const std::vector<std::vector<GridPos>>& allPaths, GridPos serverPos);
    void draw(sf::RenderWindow& window);

private:
    float cellSize;
    sf::Texture texStraight, texCorner, texServer;
    std::vector<sf::Sprite> tileSprites;
    sf::Sprite serverSprite;

    sf::Vector2f toWorld(GridPos p) const;
};