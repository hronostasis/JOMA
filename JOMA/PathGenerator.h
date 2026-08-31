#pragma once
#include <vector>

struct GridPos {
    int x, y;
    bool operator==(const GridPos& other) const { return x == other.x && y == other.y; }
};

enum class Side { North, South, West, East };

struct SpawnSlot {
    GridPos pos;
    Side side;
};

std::vector<SpawnSlot> pickSpawnSlots(int cols, int rows, int count);
std::vector<std::vector<GridPos>> generateAllPaths(const std::vector<SpawnSlot>& spawns, GridPos serverBlockOrigin, int cols, int rows, bool addTurns = false);