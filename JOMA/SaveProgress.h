#pragma once

class SaveProgress {
public:
    static bool hasSave();
    static int loadUnlockedLevel();
    static void saveUnlockedLevel(int level);
    static void resetProgress();
};