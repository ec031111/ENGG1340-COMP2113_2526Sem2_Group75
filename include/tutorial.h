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

    // -----------------------------------------------------------------
    // run - Main entry point for tutorial system
    // Description: Displays menu for skipping or starting tutorial.
    //              Guides players through game mechanics sequentially.
    // Returns: true if tutorial completed or skipped, false if quit
    // -----------------------------------------------------------------
    bool run();

    // Individual lesson methods (return false if user quits to menu)
    // -----------------------------------------------------------------
    // lessonIntro - Welcome and objective introduction
    // Explains game goal: survive as many rounds as possible
    // -----------------------------------------------------------------
    bool lessonIntro();

    // -----------------------------------------------------------------
    // lessonShop - Learn shop system mechanics
    // Teaching: buying, selling, refreshing shop, gold management
    // -----------------------------------------------------------------
    bool lessonShop();

    // -----------------------------------------------------------------
    // lessonUnits - Understand unit system and classes
    // Teaching: unit attributes, classes, abilities, star levels
    // -----------------------------------------------------------------
    bool lessonUnits();

    // -----------------------------------------------------------------
    // lessonPlacement - Learn unit deployment and formation
    // Teaching: placing units on board, formation strategy
    // -----------------------------------------------------------------
    bool lessonPlacement();

    // -----------------------------------------------------------------
    // lessonCombat - Understand combat mechanics
    // Teaching: attack resolution, damage, abilities in combat
    // -----------------------------------------------------------------
    bool lessonCombat();

    // -----------------------------------------------------------------
    // lessonSynergy - Learn synergy system and team composition
    // Teaching: class synergies, bonuses for matching unit types
    // -----------------------------------------------------------------
    bool lessonSynergy();

    // -----------------------------------------------------------------
    // lessonGoldManagement - Master gold economy and resources
    // Teaching: income, interest, spending strategy, streaks
    // -----------------------------------------------------------------
    bool lessonGoldManagement();

    // -----------------------------------------------------------------
    // lessonLevelUp - Understand unit merging and upgrading
    // Teaching: how to merge 3 units into 1 higher star, progression
    // -----------------------------------------------------------------
    bool lessonLevelUp();

    // -----------------------------------------------------------------
    // lessonCompletion - Final congratulations and tips
    // Summary of key concepts and encouragement to play
    // -----------------------------------------------------------------
    bool lessonCompletion();

private:
    std::vector<TutorialLesson> lessons_;
    int currentLessonIndex_;  // Track current lesson for progress display

    // Helper methods
    // -----------------------------------------------------------------
    // displayLesson - Show lesson content with formatted display
    // Parameters: title - Lesson title, content - Paragraphs of text
    //             current - Progress index, total - Total lessons
    // -----------------------------------------------------------------
    void displayLesson(const std::string& title, const std::vector<std::string>& content, int current = 0, int total = 9);

    // -----------------------------------------------------------------
    // printBox - Print text in formatted box with borders
    // Parameters: text - Content to display, width - Box width
    // -----------------------------------------------------------------
    void printBox(const std::string& text, int width = 60);

    // -----------------------------------------------------------------
    // clearScreen - Clear terminal display (cross-platform)
    // Platform: Windows (cls) and Linux/macOS (clear)
    // -----------------------------------------------------------------
    void clearScreen() const;

    // -----------------------------------------------------------------
    // getUserChoice - Prompt for user input with menu option hint
    // Returns: User input string (exit via 'menu' command)
    // -----------------------------------------------------------------
    std::string getUserChoice();

    // -----------------------------------------------------------------
    // waitForInput - Pause execution until user presses Enter
    // Purpose: Control pacing during tutorial lessons
    // -----------------------------------------------------------------
    void waitForInput();

    // -----------------------------------------------------------------
    // printEncouragement - Display motivational messages
    // Purpose: Encourage player progress through lessons
    // -----------------------------------------------------------------
    void printEncouragement();

    // -----------------------------------------------------------------
    // printProgressBar - Show tutorial progress (X of Y lessons)
    // Parameters: current - Completed lesson count, total - Total lessons
    // -----------------------------------------------------------------
    void printProgressBar(int current, int total);

    // -----------------------------------------------------------------
    // demonstrateFullGameplay - Show sample game round automatically
    // Purpose: Demonstrate real gameplay flow for player learning
    // -----------------------------------------------------------------
    void demonstrateFullGameplay();

    // -----------------------------------------------------------------
    // getDisplayWidth - Calculate console display width excluding ANSI codes
    // Handles: Color codes, emoji, UTF-8 multi-byte characters
    // Returns: Visual width for alignment and formatting
    // -----------------------------------------------------------------
    int getDisplayWidth(const std::string& str) const;
};

#endif
