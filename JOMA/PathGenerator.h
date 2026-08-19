#pragma once
#include <vector>

struct GridPos {
    int x, y;
    bool operator==(const GridPos& other) const { return x == other.x && y == other.y; }
};

std::vector<GridPos> pickSpawnPoints(int cols, int rows, int count);
std::vector<std::vector<GridPos>> generateAllPaths(const std::vector<GridPos>& spawns, GridPos server, int cols, int rows);