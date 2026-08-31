#include "PathGenerator.h"
#include <random>
#include <algorithm>
#include <cstdlib>

std::vector<SpawnSlot> pickSpawnSlots(int cols, int rows, int count) {
    std::vector<SpawnSlot> slots = {
        { { cols / 2, 0 }, Side::North },
        { { cols / 2, rows - 1 }, Side::South },
        { { 0, rows / 2 }, Side::West },
        { { cols - 1, rows / 2 }, Side::East }
    };
    static std::mt19937 rng(std::random_device{}());
    std::shuffle(slots.begin(), slots.end(), rng);
    count = std::min(count, 4);
    return std::vector<SpawnSlot>(slots.begin(), slots.begin() + count);
}

static GridPos dockingCell(Side side, GridPos serverBlockOrigin) {
    switch (side) {
    case Side::North: return { serverBlockOrigin.x + 1, serverBlockOrigin.y - 1 };
    case Side::South: return { serverBlockOrigin.x + 1, serverBlockOrigin.y + 3 };
    case Side::West:  return { serverBlockOrigin.x - 1, serverBlockOrigin.y + 1 };
    case Side::East:  return { serverBlockOrigin.x + 3, serverBlockOrigin.y + 1 };
    }
    return serverBlockOrigin;
}

static int manhattan(GridPos a, GridPos b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<std::vector<GridPos>> generateAllPaths(const std::vector<SpawnSlot>& spawns, GridPos serverBlockOrigin, int cols, int rows, bool addTurns) {
    std::vector<std::vector<GridPos>> allPaths;
    std::vector<std::vector<bool>> occupied(cols, std::vector<bool>(rows, false));

    for (int dx = 0; dx < 3; dx++)
        for (int dy = 0; dy < 3; dy++) {
            int gx = serverBlockOrigin.x + dx, gy = serverBlockOrigin.y + dy;
            if (gx >= 0 && gx < cols && gy >= 0 && gy < rows) occupied[gx][gy] = true;
        }

    static std::mt19937 rng(std::random_device{}());

    for (const auto& spawn : spawns) {
        GridPos target = dockingCell(spawn.side, serverBlockOrigin);

        std::vector<GridPos> path;
        GridPos current = spawn.pos;
        path.push_back(current);
        occupied[current.x][current.y] = true;

        int safety = cols * rows * 4;
        int detoursUsed = 0;
        const int maxDetours = 4;

        while (!(current == target) && safety-- > 0) {
            int dist = manhattan(current, target);

            auto tryTier = [&](auto predicate) {
                std::vector<GridPos> result;
                for (auto dir : { GridPos{1,0}, GridPos{-1,0}, GridPos{0,1}, GridPos{0,-1} }) {
                    GridPos next = { current.x + dir.x, current.y + dir.y };
                    if (next.x < 0 || next.x >= cols || next.y < 0 || next.y >= rows) continue;
                    int nextDist = manhattan(next, target);
                    bool free = !occupied[next.x][next.y] || (next == target);
                    if (predicate(nextDist, dist, free)) result.push_back(next);
                }
                return result;
                };

            std::vector<GridPos> candidates;

            if (addTurns && detoursUsed < maxDetours && dist > 2 && (rng() % 100) < 35) {
                candidates = tryTier([](int nd, int d, bool free) { return free && nd <= d + 1 && nd != d - 1; });
                if (!candidates.empty()) detoursUsed++;
            }

            if (candidates.empty())
                candidates = tryTier([](int nd, int d, bool free) { return nd < d && free; });
            if (candidates.empty())
                candidates = tryTier([](int nd, int d, bool free) { return nd <= d && free; });
            if (candidates.empty())
                candidates = tryTier([](int, int, bool free) { return free; });
            if (candidates.empty())
                candidates = tryTier([](int, int, bool) { return true; });

            std::uniform_int_distribution<int> distPick(0, (int)candidates.size() - 1);
            current = candidates[distPick(rng)];
            path.push_back(current);
            occupied[current.x][current.y] = true;
        }
        allPaths.push_back(path);
    }
    return allPaths;
}