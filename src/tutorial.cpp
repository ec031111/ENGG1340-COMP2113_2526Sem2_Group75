 // ============================================================================
 // Project: ENGG1340/COMP2113 - Auto-Battler Arena
 // Group: 75
 // File Type: Implementation File (.cpp)
 // File Description:
 //   Implements the Tutorial class providing interactive gameplay tutorials.
 //   Contains lesson content, progression tracking, and display logic for
 //   teaching new players the game mechanics through structured lessons.
 //   Manages tutorial state, lesson navigation, and completion tracking.
 // ============================================================================
 

#include "tutorial.h"
#include "game.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <unistd.h>

// -----------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------
Tutorial::Tutorial() {
    lessons_ = {
        TutorialLesson::INTRO,
        TutorialLesson::SHOP,
        TutorialLesson::UNITS,
        TutorialLesson::PLACEMENT,
        TutorialLesson::COMBAT,
        TutorialLesson::SYNERGY,
        TutorialLesson::GOLD_MANAGEMENT,
        TutorialLesson::LEVEL_UP,
        TutorialLesson::COMPLETED
    };
    currentLessonIndex_ = 0;
}

// -----------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------
Tutorial::~Tutorial() {}

// -----------------------------------------------------------------
// Helper: Clear screen
// -----------------------------------------------------------------
void Tutorial::clearScreen() const {
    // Works on both macOS/Linux and Windows
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// -----------------------------------------------------------------
// Helper: Print a boxed line
// -----------------------------------------------------------------
int Tutorial::getDisplayWidth(const std::string& str) const {
    int width = 0;
    bool inEscape = false;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '\033') {  // ESC character starts ANSI sequence
            inEscape = true;
        } else if (inEscape && str[i] == 'm') {  // 'm' ends ANSI sequence
            inEscape = false;
        } else if (!inEscape) {
            // Check for multi-byte UTF-8 characters (emoji, etc)
            unsigned char c = (unsigned char)str[i];
            if ((c & 0x80) == 0) {
                // Single byte ASCII: 1 column
                width++;
            } else if ((c & 0xE0) == 0xC0) {
                // 2-byte UTF-8
                width += 2;  // Most emoji are 2 columns wide
                i++;  // Skip next byte
            } else if ((c & 0xF0) == 0xE0) {
                // 3-byte UTF-8
                width += 2;  // Eastern fonts are typically 2 columns
                i += 2;  // Skip next 2 bytes
            } else if ((c & 0xF8) == 0xF0) {
                // 4-byte UTF-8 (emoji)
                width += 2;  // Emoji are typically 2 columns
                i += 3;  // Skip next 3 bytes
            }
        }
    }
    return width;
}

void Tutorial::printBox(const std::string& text, int width) {
    std::string line = text;
    int displayWidth = getDisplayWidth(text);
    
    // Pad with spaces to match the desired width
    if (displayWidth < width) {
        line += std::string(width - displayWidth, ' ');
    }
    std::cout << "  |" << line << "|" << std::endl;
}

// -----------------------------------------------------------------
// Helper: Get user input with menu option
// -----------------------------------------------------------------
std::string Tutorial::getUserChoice() {
    std::string input;
    std::cout << BOLD << YELLOW << "  (Type 'menu' to return to main menu, or press Enter)" << RESET << std::endl;
    std::cout << BOLD << BR_YELLOW << "  > " << RESET;
    if (!std::getline(std::cin, input)) {
        return "";
    }
    return input;
}

// -----------------------------------------------------------------
// Helper: Wait for user to press enter
// -----------------------------------------------------------------
void Tutorial::waitForInput() {
    std::string temp;
    std::getline(std::cin, temp);
}

// -----------------------------------------------------------------
// Helper: Print encouragement message
// -----------------------------------------------------------------
void Tutorial::printEncouragement() {
    const std::vector<std::string> messages = {
        "🌟 Great job! You're learning fast!",
        "⭐ Excellent! You're getting the hang of it!",
        "✨ Perfect! You're a quick learner!",
        "🎯 Well done! Keep it up!",
        "🚀 Fantastic! You're doing amazing!",
        "🏆 Superb! You're ready for the battlefield!",
        "💫 Wonderful! You're progressing nicely!",
        "👑 Outstanding! You're becoming a true commander!"
    };
    
    int idx = rand() % messages.size();
    std::cout << BOLD << BR_GREEN << "\n  ✓ " << messages[idx] << RESET << std::endl;
}

// -----------------------------------------------------------------
// Helper: Display tutorial progress bar (9 lessons)
// -----------------------------------------------------------------
void Tutorial::printProgressBar(int current, int total) {
    // Progress visualization: 9 boxes, filled ones are completed
    std::cout << std::endl;
    std::cout << BOLD << BR_CYAN << "  Tutorial Progress: ";
    
    for (int i = 1; i <= total; i++) {
        if (i < current) {
            std::cout << "[■]";  // Filled box for completed lessons
        } else if (i == current) {
            std::cout << "[◆]";  // Diamond for current lesson
        } else {
            std::cout << "[□]";  // Empty box for remaining lessons
        }
    }
    
    std::cout << RESET << "  (Lesson " << current << " of " << total << ")" << std::endl;
    std::cout << std::endl;
}

// -----------------------------------------------------------------
// Helper: Display a lesson with content
// -----------------------------------------------------------------
void Tutorial::displayLesson(const std::string& title, 
                              const std::vector<std::string>& content,
                              int current, int total) {
    clearScreen();
    
    // Display progress bar if current > 0
    if (current > 0) {
        printProgressBar(current, total);
    }
    
    const int W = 70;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '=') << "+" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  | 📚 TUTORIAL: " << std::left << std::setw(W - 17) << title << "|" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    printBox("", W);
    
    for (const auto& line : content) {
        printBox("  " + line, W);
    }
    
    printBox("", W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '=') << "+" << RESET << std::endl;
    std::cout << std::endl;
}

// -----------------------------------------------------------------
// Lesson 1: Introduction
// -----------------------------------------------------------------
bool Tutorial::lessonIntro() {
    
    const std::vector<std::string> content = {
        BOLD BR_CYAN "Welcome, Commander!" RESET,
        "",
        "You are the " BOLD BR_PURPLE "last hope" RESET " of the war-torn Kingdom of Aethelia.",
        "Dark forces threaten to consume your homeland. You " BOLD BR_PURPLE "must" RESET,
        "" BR_GREEN "recruit heroes" RESET ", " BR_GREEN "build a powerful army" RESET ", and " BR_GREEN "defend the realm" RESET,
        "against the darkness.",
        "",
        "Your objective is " BR_YELLOW "simple" RESET ": " BOLD BR_YELLOW "survive as many rounds as possible" RESET "!",
        "",
        "Each round consists of:",
        "  1. " BR_CYAN "SHOPPING" RESET " - Buy units and arrange your formation",
        "  2. " BR_CYAN "COMBAT" RESET " - Your army fights automatically",
        "",
        "" BR_RED "Destroy all enemy units to win!" RESET
    };
    
    displayLesson("Welcome, Commander! 👑", content, 1, 9);
    
    std::string choice = getUserChoice();
    if (choice == "menu" || choice == "MENU") {
        std::cout << "  Returning to main menu..." << std::endl;
        std::cout << "  Press Enter to continue" << std::endl;
        waitForInput();
        return false;
    }
    return true;
}

// -----------------------------------------------------------------
// Lesson 2: Shop System (with interactive practice)
// -----------------------------------------------------------------
bool Tutorial::lessonShop() {
    
    const std::vector<std::string> content = {
        BOLD BR_YELLOW "THE SHOP SYSTEM" RESET,
        "",
        "Each round, the shop offers " BR_YELLOW "5 randomly generated units" RESET ".",
        "",
        BOLD BR_CYAN "KEY ACTIONS:" RESET,
        "  • " BR_YELLOW "buy <1-5>" RESET "      - Purchase a unit using gold",
        "  • " BR_YELLOW "sell <1-8>" RESET "     - Sell a unit from your bench",
        "  • " BR_YELLOW "refresh <cost>" RESET " - Refresh shop (costs " BR_YELLOW "2 gold" RESET ")",
        "",
        BOLD BR_GREEN "GOLD MANAGEMENT:" RESET,
        "  • Start with " BR_YELLOW "15 gold" RESET " per game",
        "  • Earn " BR_YELLOW "5 gold" RESET " at the start of each new round",
        "  • Interest: Every " BR_YELLOW "10 gold" RESET " gives " BR_YELLOW "1 bonus gold" RESET,
        "  • Winning/losing streaks give " BR_YELLOW "bonus gold" RESET,
        "",
        "💡 " BOLD BR_YELLOW "STRATEGY TIP: " RESET "Balance spending on units vs. saving for",
        "interest bonuses!"
    };
    
    displayLesson("The Shop System 🏪", content, 2, 9);
    
    std::string choice = getUserChoice();
    if (choice == "menu" || choice == "MENU") {
        std::cout << "  Returning to main menu..." << std::endl;
        std::cout << "  Press Enter to continue" << std::endl;
        waitForInput();
        return false;
    }
    
    std::cout << std::endl;
    
    if (choice == "skip" || choice == "SKIP") {
        std::cout << "  Skipping this lesson..." << std::endl;
    } else {
        // Interactive: Show practice with real game format
        clearScreen();
        printProgressBar(2, 9);  // Show progress at top
        std::cout << std::endl;
        std::cout << "  Let's practice with a sample shop!" << std::endl;
        std::cout << "  (You have 15 gold to spend)" << std::endl;
        std::cout << std::endl;
        
        // Display shop in EXACT format matching game.cpp
        const int W = 68;
        std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
        
        std::string title = BOLD + std::string(BR_YELLOW) + "🏪 SHOP  (refresh: $2) 🏪" + RESET;
        int pad = (W - getDisplayWidth(title)) / 2;
        if (pad < 0) pad = 0;
        std::cout << "  |" << std::string(pad, ' ') << title
                  << std::string(std::max(0, W - pad - getDisplayWidth(title)), ' ') << "|" << std::endl;
        
        std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
        
        // Sample shop units matching real format
        // Format: ✓/✗ [#] 🎨 Name ClassHP:XX ATK:X R:X [Ability] $Cost
        int playerGold = 15;
        
        // Unit 1: Warrior - can afford
        std::cout << "  |" << (playerGold >= 5 ? GREEN "  ✓ " RESET : RED "  ✗ " RESET)
                  << "[1] " << "⚔️ " << std::left << std::setw(10) << "Warrior"
                  << std::setw(9) << "Warrior"
                  << "HP:" << std::setw(4) << "20"
                  << "ATK:" << std::setw(3) << "8"
                  << "R:" << "1"
                  << " [Rage] "
                  << (playerGold >= 5 ? GREEN : RED) << "$5" << RESET
                  << std::string(W - 60, ' ') << "|" << std::endl;
        
        // Unit 2: Mage - can afford
        std::cout << "  |" << (playerGold >= 6 ? GREEN "  ✓ " RESET : RED "  ✗ " RESET)
                  << "[2] " << "✨ " << std::left << std::setw(10) << "Mage"
                  << std::setw(9) << "Mage"
                  << "HP:" << std::setw(4) << "15"
                  << "ATK:" << std::setw(3) << "12"
                  << "R:" << "3"
                  << " [AOE]  "
                  << (playerGold >= 6 ? GREEN : RED) << "$6" << RESET
                  << std::string(W - 60, ' ') << "|" << std::endl;
        
        // Unit 3: Tank - can afford
        std::cout << "  |" << (playerGold >= 7 ? GREEN "  ✓ " RESET : RED "  ✗ " RESET)
                  << "[3] " << "🛡️ " << std::left << std::setw(10) << "Tank"
                  << std::setw(9) << "Tank"
                  << "HP:" << std::setw(4) << "30"
                  << "ATK:" << std::setw(3) << "5"
                  << "R:" << "1"
                  << " [Block] "
                  << (playerGold >= 7 ? GREEN : RED) << "$7" << RESET
                  << std::string(W - 60, ' ') << "|" << std::endl;
        
        // Unit 4: Assassin - can afford
        std::cout << "  |" << (playerGold >= 6 ? GREEN "  ✓ " RESET : RED "  ✗ " RESET)
                  << "[4] " << "🗡️ " << std::left << std::setw(10) << "Assassin"
                  << std::setw(9) << "Assassin"
                  << "HP:" << std::setw(4) << "14"
                  << "ATK:" << std::setw(3) << "11"
                  << "R:" << "1"
                  << " [Crit]  "
                  << (playerGold >= 6 ? GREEN : RED) << "$6" << RESET
                  << std::string(W - 60, ' ') << "|" << std::endl;
        
        // Unit 5: Archer - can afford
        std::cout << "  |" << (playerGold >= 4 ? GREEN "  ✓ " RESET : RED "  ✗ " RESET)
                  << "[5] " << "🏹 " << std::left << std::setw(10) << "Archer"
                  << std::setw(9) << "Archer"
                  << "HP:" << std::setw(4) << "16"
                  << "ATK:" << std::setw(3) << "9"
                  << "R:" << "3"
                  << " [DblShot]"
                  << (playerGold >= 4 ? GREEN : RED) << "$4" << RESET
                  << std::string(W - 60, ' ') << "|" << std::endl;
        
        std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
        std::cout << BOLD << BR_YELLOW << "  💰 Your Gold: 15" << RESET << std::endl;
        std::cout << std::endl;
        
        std::cout << "  SHOP COMMANDS:" << std::endl;
        std::cout << "  • " << BR_CYAN << "buy <1-5>" << RESET << "      - Purchase a unit" << std::endl;
        std::cout << "  • " << BR_CYAN << "info shop N" << RESET << "  - See unit details" << std::endl;
        std::cout << "  • " << BR_CYAN << "refresh" << RESET << "        - Re-roll shop" << std::endl;
        std::cout << std::endl;
        
        bool validInput = false;
        while (!validInput) {
            std::cout << BOLD << BR_YELLOW << "  Try buying a unit! Type: buy 1" << RESET << std::endl;
            std::cout << "  (Then press Enter)" << std::endl;
            std::cout << std::endl;
            std::cout << BOLD << BR_YELLOW << "  > " << RESET;
            
            std::string input;
            std::getline(std::cin, input);
            
            // Check if user wants to return to menu
            if (input == "menu" || input == "MENU") {
                std::cout << std::endl;
                std::cout << "  Returning to main menu..." << std::endl;
                std::cout << "  Press Enter to continue" << std::endl;
                waitForInput();
                return false;
            }
            
            std::cout << std::endl;
            // Parse command: "buy <1-5>"
            std::istringstream iss(input);
            std::string cmd;
            int unitNum = -1;
            bool validCmd = false;
            
            if (iss >> cmd >> unitNum && cmd == "buy" && unitNum >= 1 && unitNum <= 5) {
                validCmd = true;
            }
            
            if (validCmd) {
                std::cout << "  ✓ Great! You've bought a Warrior!" << std::endl;
                std::cout << "  ✓ It's now in your bench (workbench)." << std::endl;
                printEncouragement();
                std::cout << "  Press Enter to continue" << std::endl;
                waitForInput();
                validInput = true;
            } else {
                std::cout << "  Oops! That's not quite right." << std::endl;
                std::cout << "  You can try commands like 'buy 1' or 'buy 2'" << std::endl;
                std::cout << std::endl;
            }
        }
    }
    return true;
}

// -----------------------------------------------------------------
// Lesson 3: Units and Attributes
// -----------------------------------------------------------------
bool Tutorial::lessonUnits() {
    
    const std::vector<std::string> content = {
        BOLD BR_CYAN "UNIT CLASSES & SPECIAL ABILITIES" RESET,
        "",
        "There are 5 unit classes in the game, each with unique",
        "abilities and attributes: (" BOLD BR_BLUE "ATK" RESET " = Attack)",
        "",
        BOLD BR_GREEN "WARRIOR" RESET "  - High " BR_BLUE "HP" RESET ", mid " BR_BLUE "ATK" RESET,
        "          Ability: " BR_CYAN "Rage" RESET " - Below " BR_YELLOW "50%" RESET " " BR_BLUE "HP" RESET ": " BR_YELLOW "+30% ATK" RESET " (once/combat)",
        "",
        BOLD BR_GREEN "MAGE" RESET "     - Low " BR_BLUE "HP" RESET ", high " BR_BLUE "ATK" RESET,
        "          Ability: " BR_CYAN "AOE" RESET " - " BR_YELLOW "30% chance" RESET " to deal " BR_YELLOW "50% splash" RESET " dmg",
        "",
        BOLD BR_GREEN "TANK" RESET "     - Super high " BR_BLUE "HP" RESET ", low " BR_BLUE "ATK" RESET,
        "          Ability: " BR_CYAN "Block" RESET " - " BR_YELLOW "25% chance" RESET " to block " BR_YELLOW "40%" RESET " dmg",
        "",
        BOLD BR_GREEN "ASSASSIN" RESET " - Mid " BR_BLUE "HP" RESET ", high " BR_BLUE "ATK" RESET ", high " BR_BLUE "crit" RESET ".",
        "          Ability: " BR_CYAN "Backstab" RESET " - " BR_YELLOW "25% chance" RESET " " BR_YELLOW "+50% extra dmg" RESET,
        "",
        BOLD BR_GREEN "ARCHER" RESET "   - Mid " BR_BLUE "HP" RESET ", mid " BR_BLUE "ATK" RESET ", support",
        "          Ability: " BR_CYAN "Double Shot" RESET " - " BR_YELLOW "20% chance" RESET " to attack (can " BR_BLUE "crit" RESET ")",
        "",
        "Each unit has: Name, Class, " BR_BLUE "HP" RESET ", " BR_BLUE "ATK" RESET ", " BR_BLUE "Crit%" RESET ", Level (star)"
    };
    
    displayLesson("Units and Special Abilities 🧩", content, 3, 9);
    
    std::string choice = getUserChoice();
    if (choice == "menu" || choice == "MENU") {
        std::cout << "  Returning to main menu..." << std::endl;
        std::cout << "  Press Enter to continue" << std::endl;
        waitForInput();
        return false;
    }
    if (choice == "skip" || choice == "SKIP") {
        std::cout << "  Skipping this lesson..." << std::endl;
    }
    return true;
}

// -----------------------------------------------------------------
// Lesson 4: Board Placement (with interactive practice)
// -----------------------------------------------------------------
bool Tutorial::lessonPlacement() {
    
    const std::vector<std::string> content = {
        BOLD BR_YELLOW "BOARD PLACEMENT & FORMATION" RESET,
        "",
        "Your bench can hold up to " BR_YELLOW "8 units" RESET ". You can place units",
        "on the " BR_YELLOW "5x8 battle grid" RESET " (your side has " BR_YELLOW "4 columns" RESET ").",
        "",
        BOLD BR_YELLOW "PLACEMENT STRATEGIES:" RESET,
        "",
        BOLD BR_YELLOW "FRONT LINE (Column 0-1):" RESET,
        "  • Place " BR_GREEN "TANKS" RESET " and " BR_GREEN "WARRIORS" RESET " here",
        "  • They " BR_RED "take the most damage" RESET,
        "",
        BOLD BR_YELLOW "BACK LINE (Column 2-3):" RESET,
        "  • Place " BR_GREEN "MAGES" RESET ", " BR_GREEN "ARCHERS" RESET ", and " BR_GREEN "ASSASSINS" RESET,
        "  • Stay " BR_GREEN "safe from enemies" RESET " while dealing damage",
        "",
        BOLD BR_YELLOW "FORMATION TIPS:" RESET,
        "  • Have a " BR_GREEN "balanced frontline and backline" RESET,
        "  • Consider " BR_GREEN "unit synergies" RESET " (more on that later!)",
        "  • Use " BR_YELLOW "'place <idx> <row> <col>'" RESET " command to position",
        "  • Use " BR_YELLOW "'auto'" RESET " command for automatic smart placement",
        "",
        "Strategic Example:",
        "  " WHITE "┌───────────────────┐" RESET,
        "  " WHITE "│  BACK  │  FRONT  │" RESET,
        "  " WHITE "│  0  1  │  2  3   │" RESET,
        "  " WHITE "├───────────────────┤" RESET,
        "  " WHITE "│ Ma .   │ Ta WA   │" RESET,
        "  " WHITE "│ Ar .   │ . .    │" RESET,
        "  " WHITE "└───────────────────┘" RESET,
        "  (Ma=Mage, Ar=Archer, Ta=Tank, WA=Warrior)"
    };
    
    displayLesson("Board Placement & Formation 🗺️", content, 4, 9);
    
    std::string choice = getUserChoice();
    if (choice == "menu" || choice == "MENU") {
        std::cout << "  Returning to main menu..." << std::endl;
        std::cout << "  Press Enter to continue" << std::endl;
        waitForInput();
        return false;
    }
    std::cout << std::endl;
    
    if (choice == "skip" || choice == "SKIP") {
        std::cout << "  Skipping this lesson..." << std::endl;
    } else {
        // Interactive: Show practice placement with real game format
        clearScreen();
        printProgressBar(4, 9);  // Show progress at top
        std::cout << std::endl;
        std::cout << "  Let's practice placing units on the battlefield!" << std::endl;
        std::cout << std::endl;
        std::cout << "  Your Bench:" << std::endl;
        std::cout << "  [1] " << "⚔️  Warrior" << std::endl;
        std::cout << "  [2] " << "🏹 Archer" << std::endl;
        std::cout << std::endl;
        
        // Your Formation board - matching real game format
        const int FORM_W = 17;
        std::cout << BOLD << CYAN << "  +" << std::string(FORM_W, '-') << "+" << RESET << std::endl;
        std::cout << "  |" << BOLD << CYAN << " YOUR FORMATION " << RESET << "|" << std::endl;
        std::cout << "  |    0 1 2 3     |" << std::endl;
        std::cout << BOLD << CYAN << "  +" << std::string(FORM_W, '-') << "+" << RESET << std::endl;
        
        std::string formRows[] = {
            " 0 | .  .  .  .  |",
            " 1 | .  .  .  .  |",
            " 2 | .  .  .  .  |",
            " 3 | .  .  .  .  |",
            " 4 | .  .  .  .  |"
        };
        
        for (const auto& row : formRows) {
            std::cout << " " << row << std::endl;
        }
        std::cout << BOLD << CYAN << "  +" << std::string(FORM_W, '-') << "+" << RESET << std::endl;
        std::cout << std::endl;
        std::cout << "  No units deployed." << std::endl;
        std::cout << std::endl;
        std::cout << "  " << BR_CYAN << "PLACEMENT COMMANDS:" << RESET << std::endl;
        std::cout << "  • " << BR_CYAN << "place <idx> <row> <col>" << RESET << "  - Place unit at position" << std::endl;
        std::cout << "  • " << BR_CYAN << "remove <row> <col>" << RESET << "       - Remove unit from board" << std::endl;
        std::cout << "  • " << BR_CYAN << "auto" << RESET << "                     - Auto-place all units" << std::endl;
        std::cout << "  • " << BR_CYAN << "formation" << RESET << "                - Show current formation" << std::endl;
        std::cout << std::endl;
        
        bool validInput = false;
        while (!validInput) {
            std::cout << BOLD << BR_YELLOW << "  Try placing the Warrior at Row 2, Column 0!" << RESET << std::endl;
            std::cout << "  Type: place 1 2 0" << std::endl;
            std::cout << "  (Then press Enter)" << std::endl;
            std::cout << std::endl;
            std::cout << BOLD << BR_YELLOW << "  > " << RESET;
            
            std::string input;
            std::getline(std::cin, input);
            
            // Check if user wants to return to menu
            if (input == "menu" || input == "MENU") {
                std::cout << std::endl;
                std::cout << "  Returning to main menu..." << std::endl;
                std::cout << "  Press Enter to continue" << std::endl;
                waitForInput();
                return false;
            }
            
            std::cout << std::endl;
            // Parse command: "place <1-2> <0-4> <0-7>"
            std::istringstream iss(input);
            std::string cmd;
            int idx, row, col;
            bool validCmd = false;
            
            if (iss >> cmd >> idx >> row >> col && 
                cmd == "place" && idx >= 1 && idx <= 2 && 
                row >= 0 && row <= 4 && col >= 0 && col <= 7) {
                validCmd = true;
            }
            
            if (validCmd) {
                std::cout << "  ✓ Perfect! You've placed the Warrior on the grid!" << std::endl;
                std::cout << "  ✓ Defensive units like Warriors belong in the front." << std::endl;
                printEncouragement();
                std::cout << "  Press Enter to continue" << std::endl;
                waitForInput();
                validInput = true;
            } else {
                std::cout << "  Oops! That's not quite right." << std::endl;
                std::cout << "  Remember the command format: place <idx> <row> <col>" << std::endl;
                std::cout << std::endl;
            }
        }
    }
    return true;
}

// -----------------------------------------------------------------
// Lesson 5: Combat System
// -----------------------------------------------------------------
bool Tutorial::lessonCombat() {
    
    const std::vector<std::string> content = {
        BOLD BR_YELLOW "HOW COMBAT WORKS" RESET,
        "",
        "Once you've placed your units and clicked " BR_YELLOW "'ready'" RESET ", the",
        "battle begins! Combat is " BOLD BR_YELLOW "AUTOMATIC" RESET " and " BR_GREEN "fully visual" RESET ".",
        "",
        BOLD BR_YELLOW "COMBAT FLOW:" RESET,
        "  1. Your units and enemy units " BR_GREEN "spawn on the grid" RESET,
        "  2. Every " BR_YELLOW "tick (turn)" RESET ", each unit " BR_YELLOW "finds the nearest enemy" RESET,
        "  3. If in range, " BR_YELLOW "units attack" RESET "; otherwise, they move",
        "  4. Each attack may trigger " BR_YELLOW "critical hit" RESET " (" BR_YELLOW "1.5x damage" RESET ")",
        "  5. Each attack may trigger " BR_YELLOW "class abilities" RESET,
        "  6. " BR_RED "Dead units are removed" RESET " from the battlefield",
        "",
        BOLD BR_YELLOW "COMBAT ENDS when:" RESET,
        "  • " BR_RED "All your units are defeated" RESET " (you " BR_RED "lose" RESET ")",
        "  • " BR_GREEN "All enemy units are defeated" RESET " (you " BR_GREEN "win" RESET ")",
        "  • " BR_YELLOW "200 ticks have passed" RESET " (tie, you take minimal damage)",
        "",
        "⚠ " BOLD BR_RED "LOSING DAMAGE:" RESET " " BR_YELLOW "5 + (num surviving enemies * 2)" RESET,
        "",
        "You can watch the combat and press " BR_YELLOW "[Enter]" RESET " to skip"
    };
    
    displayLesson("Combat System ⚔️", content, 5, 9);
    
    std::string choice = getUserChoice();
    if (choice == "menu" || choice == "MENU") {
        std::cout << "  Returning to main menu..." << std::endl;
        std::cout << "  Press Enter to continue" << std::endl;
        waitForInput();
        return false;
    }
    if (choice == "skip" || choice == "SKIP") {
        std::cout << "  Skipping this lesson..." << std::endl;
    }
    return true;
}

// -----------------------------------------------------------------
// Lesson 6: Synergy Effects
// -----------------------------------------------------------------
bool Tutorial::lessonSynergy() {
    
    const std::vector<std::string> content = {
        BOLD BR_YELLOW "SYNERGY EFFECTS" RESET " - Power Through Teamwork!",
        "",
        "Same class units give automatic bonuses (stacking):",
        "",
        "⚔ " BOLD BR_GREEN "WARRIOR" RESET ":  " BR_CYAN "Lv1(2x)" RESET " " BR_YELLOW "+5 ATK" RESET "  |  " BR_CYAN "Lv2(3x)" RESET " " BR_YELLOW "+10 ATK" RESET,
        "🔥 " BOLD BR_GREEN "MAGE" RESET ":     " BR_CYAN "Lv1(2x)" RESET " " BR_YELLOW "+15% crit" RESET "  |  " BR_CYAN "Lv2(3x)" RESET " " BR_YELLOW "+30% crit" RESET,
        "🛡 " BOLD BR_GREEN "TANK" RESET ":     " BR_CYAN "Lv1(2x)" RESET " " BR_YELLOW "+20% HP" RESET "  |  " BR_CYAN "Lv2(3x)" RESET " More resilience",
        "🗡 " BOLD BR_GREEN "ASSASSIN" RESET ": " BR_CYAN "Lv1(2x)" RESET " " BR_YELLOW "+10 ATK" RESET "  |  " BR_CYAN "Lv2(3x)" RESET " " BR_YELLOW "+20 ATK, +10% crit" RESET,
        "🏹 " BOLD BR_GREEN "ARCHER" RESET ":   " BR_CYAN "Lv1(2x)" RESET " " BR_YELLOW "+3 ATK" RESET "  |  " BR_CYAN "Lv2(3x)" RESET " " BR_YELLOW "+5 ATK" RESET,
        "",
        "💡 " BOLD BR_YELLOW "STRATEGY" RESET ": Build themed armies for maximum power!"
    };
    
    displayLesson("Synergy Effects ✨", content, 6, 9);
    
    std::string choice = getUserChoice();
    if (choice == "menu" || choice == "MENU") {
        std::cout << "  Returning to main menu..." << std::endl;
        std::cout << "  Press Enter to continue" << std::endl;
        waitForInput();
        return false;
    }
    if (choice == "skip" || choice == "SKIP") {
        std::cout << "  Skipping this lesson..." << std::endl;
    }
    return true;
}

// -----------------------------------------------------------------
// Lesson 7: Gold Management
// -----------------------------------------------------------------
bool Tutorial::lessonGoldManagement() {
    
    const std::vector<std::string> content = {
        BOLD BR_YELLOW "GOLD & RESOURCE MANAGEMENT" RESET,
        "",
        "Gold is your " BOLD BR_YELLOW "most precious resource" RESET "!",
        "",
        BOLD BR_YELLOW "GOLD SOURCES:" RESET,
        "  • Battle rewards: " BR_YELLOW "Win streak = more gold" RESET,
        "  • Loss penalty: " BR_RED "Lose streak = reduced income" RESET,
        "  • Interest: " BR_YELLOW "Every 10 gold = 1 bonus gold" RESET " (max " BR_YELLOW "5/turn" RESET ")",
        "  • Special events: " BR_GREEN "May grant gold" RESET,
        "",
        BOLD BR_YELLOW "GOLD SPENDING:" RESET,
        "  • " BR_YELLOW "Buy units" RESET " from shop",
        "  • " BR_YELLOW "Refresh shop" RESET " (" BR_YELLOW "2 gold" RESET " per refresh)",
        "  • " BR_CYAN "No other direct costs" RESET,
        "",
        BOLD BR_YELLOW "WINNING STRATEGY:" RESET,
        "  • Early game (rounds 1-3): " BR_YELLOW "Buy aggressively" RESET,
        "  • Mid game (rounds 4-7): " BR_YELLOW "Balance buying and saving" RESET,
        "  • Late game: " BR_YELLOW "Save for interest bonuses" RESET " and power units",
        "",
        "💰 " BOLD BR_YELLOW "PRO TIP: " RESET "" BR_YELLOW "50 gold generates 5 free gold per round!" RESET
    };
    
    displayLesson("Gold & Resource Management 💰", content, 7, 9);
    
    std::string choice = getUserChoice();
    if (choice == "menu" || choice == "MENU") {
        std::cout << "  Returning to main menu..." << std::endl;
        std::cout << "  Press Enter to continue" << std::endl;
        waitForInput();
        return false;
    }
    if (choice == "skip" || choice == "SKIP") {
        std::cout << "  Skipping this lesson..." << std::endl;
    }
    return true;
}

// -----------------------------------------------------------------
// Lesson 8: Leveling Up (Unit Merging)
// -----------------------------------------------------------------
bool Tutorial::lessonLevelUp() {
    
    const std::vector<std::string> content = {
        BOLD BR_YELLOW "UNIT LEVELING & MERGING" RESET,
        "",
        "Units can be " BR_GREEN "leveled up" RESET " to become " BOLD BR_YELLOW "more powerful" RESET "!",
        "",
        BOLD BR_YELLOW "HOW MERGING WORKS:" RESET,
        "  When you have " BR_YELLOW "3 units" RESET " of the " BR_YELLOW "SAME name" RESET " at the " BR_YELLOW "SAME" RESET,
        "  level, they " BR_GREEN "automatically merge" RESET " into " BR_GREEN "1 upgraded unit" RESET ".",
        "",
        BOLD BR_YELLOW "LEVEL PROGRESSION:" RESET,
        "  Level 1 " BR_YELLOW "★" RESET "     -> Level 2 " BR_YELLOW "★★" RESET,
        "    • " BR_RED "HP × 1.8" RESET "        • " BR_RED "ATK × 1.5" RESET,
        "",
        "  Level 2 " BR_YELLOW "★★" RESET "    -> Level 3 " BR_YELLOW "★★★" RESET " (" BR_RED "Max Level" RESET ")",
        "    • " BR_GREEN "HP × 3.0" RESET "        • " BR_GREEN "ATK × 2.5" RESET,
        "",
        BOLD BR_YELLOW "STRATEGY:" RESET,
        "  • Focus on getting " BR_YELLOW "3 copies of strong units" RESET,
        "  • Leveled units are " BOLD BR_YELLOW "MUCH more powerful" RESET,
        "  • A " BR_GREEN "3-star unit" RESET " can " BR_RED "carry an entire army!" RESET,
        "",
        "ℹ️  " BOLD BR_RED "NOTE:" RESET " Merging is " BR_GREEN "automatic" RESET " - no action needed from you!"
    };
    
    displayLesson("Unit Leveling & Merging ⭐", content, 8, 9);
    
    std::string choice = getUserChoice();
    if (choice == "menu" || choice == "MENU") {
        std::cout << "  Returning to main menu..." << std::endl;
        std::cout << "  Press Enter to continue" << std::endl;
        waitForInput();
        return false;
    }
    if (choice == "skip" || choice == "SKIP") {
        std::cout << "  Skipping this lesson..." << std::endl;
    }
    return true;
}

// -----------------------------------------------------------------
// Demonstrate Full Gameplay
// -----------------------------------------------------------------
void Tutorial::demonstrateFullGameplay() {
    clearScreen();
    std::cout << std::endl;
    std::cout << "  ========== GAMEPLAY DEMONSTRATION ==========" << std::endl;
    std::cout << std::endl;
    
    // PHASE 1: SHOPPING
    std::cout << BOLD << YELLOW << "  [PHASE 1: SHOPPING]" << RESET << std::endl;
    std::cout << std::endl;
    std::cout << "  Your Status:" << std::endl;
    std::cout << "    • HP: 100" << std::endl;
    std::cout << BOLD << BR_YELLOW << "    • Gold: 15" << RESET << std::endl;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(58, '-') << "+" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  |" << std::string(16, ' ') << "SHOP  (refresh: $1)" << std::string(17, ' ') << "|" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(58, '-') << "+" << RESET << std::endl;
    std::cout << "  |  [1] Warrior   WarriorHP:20  ATK:8  R:1 [Rage] $5        |" << std::endl;
    std::cout << "  |  [2] Mage      MageHP:15  ATK:12 R:3 [AOE] $6            |" << std::endl;
    std::cout << "  |  [3] Tank      TankHP:30  ATK:5  R:1 [Block] $7          |" << std::endl;
    std::cout << "  |  [4] Archer    ArcherHP:16  ATK:9  R:3 [DblShot] $4      |" << std::endl;
    std::cout << "  |  [5] Assassin  AssassinHP:14  ATK:11 R:1 [Crit] $6       |" << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(58, '-') << "+" << RESET << std::endl;
    std::cout << std::endl;
    std::cout << "  Buying: Warrior, Tank, Archer" << std::endl;
    std::cout << "  Total Cost: 5 + 7 + 4 = 16 gold" << std::endl;
    std::cout << "  Remaining Gold: 15 + 5 (interest) = 20 -> 4 gold after purchase" << std::endl;
    std::cout << std::endl;
    std::cout << BOLD << BR_YELLOW << "  Press Enter to continue..." << RESET << std::endl;
    waitForInput();
    
    // PHASE 2: PLACEMENT
    clearScreen();
    std::cout << std::endl;
    std::cout << BOLD << BR_YELLOW << "  [PHASE 2: ARRANGING FORMATION]" << RESET << std::endl;
    std::cout << std::endl;
    std::cout << "  Your Bench:" << std::endl;
    std::cout << "    [1] " << "⚔️  Warrior" << std::endl;
    std::cout << "    [2] " << "🛡️  Tank" << std::endl;
    std::cout << "    [3] " << "🏹 Archer" << std::endl;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +---------------+" << RESET << std::endl;
    std::cout << "  |" << BOLD << CYAN << " YOUR FORMATION " << RESET << "|" << std::endl;
    std::cout << "  |    0 1 2 3     |" << std::endl;
    std::cout << BOLD << CYAN << "  +---------------+" << RESET << std::endl;
    std::cout << " 0 | 🛡️  .  .  🏹 |" << std::endl;
    std::cout << " 1 | .  .  .  .  |" << std::endl;
    std::cout << " 2 | ⚔️  .  .  .  |" << std::endl;
    std::cout << " 3 | .  .  .  .  |" << std::endl;
    std::cout << " 4 | .  .  .  .  |" << std::endl;
    std::cout << BOLD << CYAN << "  +---------------+" << RESET << std::endl;
    std::cout << std::endl;
    std::cout << "  Strategy: Tank in front + high-damage Warrior in middle + Archer for support" << std::endl;
    std::cout << std::endl;
    std::cout << BOLD << BR_YELLOW << "  Press Enter to continue..." << RESET << std::endl;
    waitForInput();
    
    // PHASE 3: COMBAT
    clearScreen();
    std::cout << std::endl;
    std::cout << BOLD << BR_RED << "  [PHASE 3: BATTLE!]" << RESET << std::endl;
    std::cout << std::endl;
    
    // Simulate a few combat ticks
    for (int tick = 1; tick <= 5; tick++) {
        std::cout << "  --- Tick " << tick << " ---" << std::endl;
        
        if (tick == 1) {
            std::cout << "  Your Tank moves toward enemy..." << std::endl;
            std::cout << "  Enemy Warrior attacks your Tank! (Damage: 8)" << std::endl;
        } else if (tick == 2) {
            std::cout << "  Your Warrior attacks enemy! (Damage: 8)" << std::endl;
            std::cout << "  Your Archer shoots! (Damage: 9, CRITICAL HIT! 13.5) " << std::endl;
            std::cout << "  ✓ Enemy Archer defeated!" << std::endl;
        } else if (tick == 3) {
            std::cout << "  Your Tank blocks 40% damage! (Blocked: 3)" << std::endl;
            std::cout << "  Your Warrior uses Rage! (ATK +30%, Damage: 10)" << std::endl;
        } else if (tick == 4) {
            std::cout << "  Your units are winning!" << std::endl;
            std::cout << "  Enemy ranks are falling..." << std::endl;
        } else if (tick == 5) {
            std::cout << BOLD << BR_GREEN << "  ✓ VICTORY! All enemy units defeated!" << RESET << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << BOLD << BR_YELLOW << "  Press Enter to see results..." << RESET << std::endl;
    waitForInput();
    
    // PHASE 4: RESULTS
    clearScreen();
    std::cout << std::endl;
    std::cout << BOLD << BR_YELLOW << "  [ROUND RESULTS]" << RESET << std::endl;
    std::cout << std::endl;
    std::cout << BOLD << BR_GREEN << "  Battle Outcome: ✓ VICTORY!" << RESET << std::endl;
    std::cout << std::endl;
    std::cout << "  Combat Statistics:" << std::endl;
    std::cout << "    • Your Units Remaining: 3" << std::endl;
    std::cout << "    • Enemy Units Eliminated: 5" << std::endl;
    std::cout << BOLD << BR_RED << "    • Your Damage Taken: 5 HP" << RESET << std::endl;
    std::cout << "    • Battle Duration: 5 ticks" << std::endl;
    std::cout << std::endl;
    std::cout << "  " BOLD BR_YELLOW "Gold Income:" RESET << std::endl;
    std::cout << "    • " BR_YELLOW "Base Income: 5 gold" RESET << std::endl;
    std::cout << "    • " BR_YELLOW "Victory Bonus: 3 gold" RESET << std::endl;
    std::cout << "    • " BR_YELLOW "Win Streak Bonus: 2 gold" RESET << std::endl;
    std::cout << "    • " BOLD BR_YELLOW "Total Gold This Round: 10 gold" RESET << std::endl;
    std::cout << std::endl;
    std::cout << "  Your Status After Round:" << std::endl;
    std::cout << "    • HP: 95 (was 100)" << std::endl;
    std::cout << "    • Gold: 14" << std::endl;
    std::cout << "    • Win Streak: 1 round" << std::endl;
    std::cout << std::endl;
    std::cout << "  This process repeats every round!" << std::endl;
    std::cout << std::endl;
    std::cout << "  Ready to jump into your first REAL battle and test your strategy?" << std::endl;
    std::cout << std::endl;
    std::cout << BOLD << BR_YELLOW << "  (Type 'ready' to proceed)" << RESET << std::endl;
    
    bool readyInput = false;
    while (!readyInput) {
        std::cout << BOLD << BR_YELLOW << "  > " << RESET;
        std::string input;
        if (!std::getline(std::cin, input)) break;
        
        if (input == "ready" || input == "READY") {
            readyInput = true;
        }
    }
}

// -----------------------------------------------------------------
bool Tutorial::lessonCompletion() {
    
    // First, show a complete game demo
    demonstrateFullGameplay();
    
    const std::vector<std::string> content = {
        "",
        BOLD BR_YELLOW "CONGRATULATIONS, COMMANDER!" RESET,
        "",
        "You have " BR_GREEN "completed the tutorial" RESET "!",
        "",
        BOLD BR_YELLOW "You now understand:" RESET,
        "  ✓ How to " BR_YELLOW "navigate the shop and buy units" RESET,
        "  ✓ The " BR_YELLOW "5 unit classes" RESET " and their " BR_YELLOW "special abilities" RESET,
        "  ✓ How to " BR_YELLOW "arrange your formation strategically" RESET,
        "  ✓ How " BR_YELLOW "combat works automatically" RESET,
        "  ✓ How " BR_YELLOW "synergies provide powerful bonuses" RESET,
        "  ✓ How to " BR_YELLOW "manage gold wisely" RESET,
        "  ✓ How to " BR_YELLOW "level up and merge your units" RESET,
        "  ✓ How a " BR_YELLOW "complete game round flows" RESET,
        "",
        BOLD BR_GREEN "You are ready to enter the battlefield!" RESET,
        "",
        "Remember: " BOLD BR_YELLOW "Every decision matters" RESET ". " BR_YELLOW "Build wisely" RESET ", " BR_YELLOW "fight",
        "hard" RESET ", and " BR_YELLOW "claim victory" RESET "!",
        "",
        BOLD BR_YELLOW "May fortune favor the bold, Commander!" RESET
    };
    
    displayLesson("Tutorial Complete! 🏁", content, 9, 9);
    printEncouragement();
    std::cout << "\n  You will now return to the main menu..." << std::endl;
    
    // Auto-return to main menu after 0.5 seconds
    usleep(500000);  // 0.5 seconds = 500000 microseconds
    waitForInput();
    return true;
}

// -----------------------------------------------------------------
// Main Tutorial Run
// -----------------------------------------------------------------
bool Tutorial::run() {
    clearScreen();
    std::cout << std::endl;
    std::cout << "  Excellent! Let's begin your journey..." << std::endl;
    std::cout << "  Press Enter to continue" << std::endl;
    waitForInput();
    
    if (!lessonIntro()) return false;
    if (!lessonShop()) return false;
    if (!lessonUnits()) return false;
    if (!lessonPlacement()) return false;
    if (!lessonCombat()) return false;
    if (!lessonSynergy()) return false;
    if (!lessonGoldManagement()) return false;
    if (!lessonLevelUp()) return false;
    if (!lessonCompletion()) return false;
    
    return true;
}
