#include "SaveProgress.h"
#include <fstream>

namespace {
    const char* saveFilePath = "save.txt";
}

bool SaveProgress::hasSave() {
    std::ifstream file(saveFilePath);
    return file.good();
}

int SaveProgress::loadUnlockedLevel() {
    std::ifstream file(saveFilePath);
    if (!file.good()) return 1;
    int level = 1;
    file >> level;
    if (level < 1) level = 1;
    return level;
}

void SaveProgress::saveUnlockedLevel(int level) {
    int current = loadUnlockedLevel();
    if (level > current) {
        std::ofstream file(saveFilePath);
        file << level;
    }
}

void SaveProgress::resetProgress() {
    std::ofstream file(saveFilePath);
    file << 1;
}