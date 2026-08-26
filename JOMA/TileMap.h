#pragma once
#include <SFML/Graphics.hpp>
#include "PathGenerator.h"
#include <vector>

class TileMap {
public:
    TileMap(float cellSize, sf::Vector2f origin);
    void build(const std::vector<std::vector<GridPos>>& allPaths, GridPos serverBlockOrigin);
    void draw(sf::RenderWindow& window);
    sf::Vector2f worldPos(GridPos p) const;

private:
    float cellSize;
    sf::Vector2f origin;
    sf::Texture texStraight, texCorner, texServer;
    std::vector<sf::Sprite> tileSprites;
    sf::Sprite serverSprite;
};