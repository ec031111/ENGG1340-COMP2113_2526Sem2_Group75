//============================================================================
// Project: ENGG1340/COMP2113 - Auto-Battler Arena
// Group: 75
// File Type: Header File (.h)
// File Description:
//   Defines the Tutorial class for interactive gameplay tutorials.
//   Provides structured lessons covering game mechanics:
//   shop system, units, board placement, combat, synergies, resource
//   management, and unit leveling/merging. Tracks progression through
//   lessons and handles content delivery for new players.
// ============================================================================


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

    // Purpose: Display main tutorial menu and guide through lessons
    // Input: none
    // Output: bool - true if completed/skipped, false if quit
    bool run();

    // Individual lesson methods (return false if user quits to menu)
    // Purpose: Teach game goal and introductory concepts
    // Input: none
    // Output: bool - false if user quits to menu
    bool lessonIntro();

    // Purpose: Teach shop system mechanics and purchasing
    // Input: none
    // Output: bool - false if user quits to menu
    bool lessonShop();

    // Purpose: Teach unit attributes, classes, abilities, and star levels
    // Input: none
    // Output: bool - false if user quits to menu
    bool lessonUnits();

    // Purpose: Teach unit deployment and formation strategy
    // Input: none
    // Output: bool - false if user quits to menu
    bool lessonPlacement();

    // Purpose: Teach combat mechanics and attack resolution
    // Input: none
    // Output: bool - false if user quits to menu
    bool lessonCombat();

    // Purpose: Teach synergy system and class bonuses
    // Input: none
    // Output: bool - false if user quits to menu
    bool lessonSynergy();

    // Purpose: Teach gold economy, income, and spending strategy
    // Input: none
    // Output: bool - false if user quits to menu
    bool lessonGoldManagement();

    // Purpose: Teach unit merging mechanic and progression
    // Input: none
    // Output: bool - false if user quits to menu
    bool lessonLevelUp();

    // Purpose: Display congratulations and final tips
    // Input: none
    // Output: bool - false if user quits to menu
    bool lessonCompletion();

private:
    std::vector<TutorialLesson> lessons_;    // List of all tutorial lessons
    int currentLessonIndex_;                 // Track current lesson for progress display

    // Helper methods
    // Purpose: Display formatted lesson content with progress bar
    // Input: title (string), content (vector of strings), current (int), total (int)
    // Output: none (prints to stdout)
    void displayLesson(const std::string& title, const std::vector<std::string>& content, int current = 0, int total = 9);

    // Purpose: Print text in formatted box with borders
    // Input: text (const string), width (int) - box width
    // Output: none (prints to stdout)
    void printBox(const std::string& text, int width = 60);

    // Purpose: Clear terminal display (cross-platform)
    // Input: none
    // Output: none
    void clearScreen() const;

    // Purpose: Get user input string with menu option hint
    // Input: none
    // Output: string - user input (enter 'menu' to exit)
    std::string getUserChoice();

    // Purpose: Pause execution until user presses Enter
    // Input: none
    // Output: none
    void waitForInput();

    // Purpose: Display motivational messages to encourage progress
    // Input: none
    // Output: none (prints to stdout)
    void printEncouragement();

    // Purpose: Display tutorial progress bar
    // Input: current (int) - completed lesson count, total (int) - total lessons
    // Output: none (prints to stdout)
    void printProgressBar(int current, int total);

    // Purpose: Automatically demonstrate real gameplay flow
    // Input: none
    // Output: none (displays sample game round)
    void demonstrateFullGameplay();

    // Purpose: Calculate visual display width excluding ANSI codes and UTF-8
    // Input: str (const string) - string to measure
    // Output: int - visual display width
    int getDisplayWidth(const std::string& str) const;
};

#endif
