#ifndef TUTORIAL_H
#define TUTORIAL_H

#include <string>
#include <vector>

// Tutorial Lesson types
enum class TutorialLesson {
    INTRO,              // Introduction and game objective
    SHOP,               // Shop system
    UNITS,              // Units and their attributes
    PLACEMENT,          // Board placement and formation
    COMBAT,             // Combat system
    SYNERGY,            // Synergy effects
    GOLD_MANAGEMENT,    // Gold and resource management
    LEVEL_UP,           // Unit merging and leveling
    COMPLETED           // Tutorial completion
};

// -----------------------------------------------------------------
// Tutorial
// -----------------------------------------------------------------
// Provides an interactive tutorial system to help new players learn
// the game mechanics. Each lesson can be skipped individually, and
// the entire tutorial can be skipped at the start.
// -----------------------------------------------------------------
class Tutorial {
public:
    Tutorial();
    ~Tutorial();

    // Main entry point: run the entire tutorial
    // Returns true if player completed or skipped tutorials
    // Returns false if player quit
    bool run();

    // Individual lesson methods (return false if user quits to menu)
    bool lessonIntro();
    bool lessonShop();
    bool lessonUnits();
    bool lessonPlacement();
    bool lessonCombat();
    bool lessonSynergy();
    bool lessonGoldManagement();
    bool lessonLevelUp();
    bool lessonCompletion();

private:
    std::vector<TutorialLesson> lessons_;
    int currentLessonIndex_;  // Track current lesson for progress display

    // Helper methods
    void displayLesson(const std::string& title, const std::vector<std::string>& content);
    void printBox(const std::string& text, int width = 60);
    void clearScreen() const;
    std::string getUserChoice();
    void waitForInput();
    void printEncouragement();
    void printProgressBar(int current, int total);  // Display tutorial progress
    void demonstrateFullGameplay();  // Demo a complete game round
};

#endif
