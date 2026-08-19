#include "PathGenerator.h"
#include <random>
#include <algorithm>

std::vector<GridPos> pickSpawnPoints(int cols, int rows, int count) {
    std::vector<GridPos> slots = {
        { cols / 2, 0 }, { cols / 2, rows - 1 },
        { 0, rows / 2 }, { cols - 1, rows / 2 },
        { 1, 1 }, { cols - 2, rows - 2 }
    };
    static std::mt19937 rng(std::random_device{}());
    std::shuffle(slots.begin(), slots.end(), rng);
    return std::vector<GridPos>(slots.begin(), slots.begin() + count);
}

std::vector<std::vector<GridPos>> generateAllPaths(const std::vector<GridPos>& spawns, GridPos server, int cols, int rows) {
    std::vector<std::vector<GridPos>> allPaths;
    std::vector<std::vector<bool>> occupied(cols, std::vector<bool>(rows, false));
    static std::mt19937 rng(std::random_device{}());

    for (const auto& spawn : spawns) {
        std::vector<GridPos> path;
        GridPos current = spawn;
        path.push_back(current);
        occupied[current.x][current.y] = true;

        while (!(current == server)) {
            int dist = std::abs(current.x - server.x) + std::abs(current.y - server.y);
            std::vector<GridPos> candidates;

            for (auto dir : { GridPos{1,0}, GridPos{-1,0}, GridPos{0,1}, GridPos{0,-1} }) {
                GridPos next = { current.x + dir.x, current.y + dir.y };
                if (next.x < 0 || next.x >= cols || next.y < 0 || next.y >= rows) continue;
                int nextDist = std::abs(next.x - server.x) + std::abs(next.y - server.y);
                bool blocked = occupied[next.x][next.y] && !(next == server);
                if (nextDist <= dist && !blocked) candidates.push_back(next);
            }

            if (candidates.empty()) {
                for (auto dir : { GridPos{1,0}, GridPos{-1,0}, GridPos{0,1}, GridPos{0,-1} }) {
                    GridPos next = { current.x + dir.x, current.y + dir.y };
                    if (next.x < 0 || next.x >= cols || next.y < 0 || next.y >= rows) continue;
                    int nextDist = std::abs(next.x - server.x) + std::abs(next.y - server.y);
                    if (nextDist <= dist) candidates.push_back(next);
                }
            }

            std::uniform_int_distribution<int> dist2(0, (int)candidates.size() - 1);
            current = candidates[dist2(rng)];
            path.push_back(current);
            occupied[current.x][current.y] = true;
        }
        allPaths.push_back(path);
    }
    return allPaths;
}