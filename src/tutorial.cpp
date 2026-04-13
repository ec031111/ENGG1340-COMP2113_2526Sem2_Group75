#include "tutorial.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

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
void Tutorial::printBox(const std::string& text, int width) {
    std::string line = text;
    // If text is longer than width, truncate it
    if ((int)line.size() > width) {
        line = line.substr(0, width);
    }
    // Pad with spaces to match the width
    if ((int)line.size() < width) {
        line += std::string(width - line.size(), ' ');
    }
    std::cout << "  |" << line << "|" << std::endl;
}

// -----------------------------------------------------------------
// Helper: Get user input with menu option
// -----------------------------------------------------------------
std::string Tutorial::getUserChoice() {
    std::string input;
    std::cout << "  (Type 'menu' to return to main menu, or press Enter)" << std::endl;
    std::cout << "  > ";
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
        "Great job! You're learning fast!",
        "Excellent! You're getting the hang of it!",
        "Perfect! You're a quick learner!",
        "Well done! Keep it up!",
        "Fantastic! You're doing amazing!",
        "Superb! You're ready for the battlefield!",
        "Wonderful! You're progressing nicely!",
        "Outstanding! You're becoming a true commander!"
    };
    
    int idx = rand() % messages.size();
    std::cout << "\n  [SUCCESS] " << messages[idx] << std::endl;
}

// -----------------------------------------------------------------
// Helper: Display tutorial progress bar (9 lessons)
// -----------------------------------------------------------------
void Tutorial::printProgressBar(int current, int total) {
    // Progress visualization: 9 boxes, filled ones are completed
    std::cout << std::endl;
    std::cout << "  Tutorial Progress: ";
    
    for (int i = 1; i <= total; i++) {
        if (i < current) {
            std::cout << "[■]";  // Filled box for completed lessons
        } else if (i == current) {
            std::cout << "[◆]";  // Diamond for current lesson
        } else {
            std::cout << "[□]";  // Empty box for remaining lessons
        }
    }
    
    std::cout << "  (Lesson " << current << " of " << total << ")" << std::endl;
    std::cout << std::endl;
}

// -----------------------------------------------------------------
// Helper: Display a lesson with content
// -----------------------------------------------------------------
void Tutorial::displayLesson(const std::string& title, 
                              const std::vector<std::string>& content) {
    clearScreen();
    
    const int W = 70;
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBox("  TUTORIAL: " + title, W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBox("", W);
    
    for (const auto& line : content) {
        printBox("  " + line, W);
    }
    
    printBox("", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    std::cout << std::endl;
}

// -----------------------------------------------------------------
// Lesson 1: Introduction
// -----------------------------------------------------------------
bool Tutorial::lessonIntro() {
    printProgressBar(1, 9);  // 1 of 9 lessons
    
    const std::vector<std::string> content = {
        "Welcome, Commander!",
        "",
        "You are the last hope of the war-torn Kingdom of Aethelia.",
        "Dark forces threaten to consume your homeland. You must",
        "recruit heroes, build a powerful army, and defend the realm",
        "against the darkness.",
        "",
        "Your objective is simple: survive as many rounds as possible!",
        "",
        "Each round consists of:",
        "  1. SHOPPING - Buy units and arrange your formation",
        "  2. COMBAT - Your army fights automatically",
        "",
        "Destroy all enemy units to win!"
    };
    
    displayLesson("Welcome, Commander!", content);
    
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
    printProgressBar(2, 9);  // 2 of 9 lessons
    
    const std::vector<std::string> content = {
        "THE SHOP SYSTEM",
        "",
        "Each round, the shop offers 5 randomly generated units.",
        "",
        "KEY ACTIONS:",
        "  • buy <1-5>      - Purchase a unit using gold",
        "  • sell <1-8>     - Sell a unit from your bench",
        "  • refresh <cost> - Refresh shop (costs 2 gold)",
        "",
        "GOLD MANAGEMENT:",
        "  • Start with 15 gold per game",
        "  • Earn 5 gold at the start of each new round",
        "  • Interest: Every 10 gold gives 1 bonus gold",
        "  • Winning/losing streaks give bonus gold",
        "",
        "STRATEGY TIP: Balance spending on units vs. saving for",
        "interest bonuses!"
    };
    
    displayLesson("The Shop System", content);
    
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
        // Interactive: Show practice
        clearScreen();
        std::cout << std::endl;
        std::cout << "  Let's practice with a sample shop!" << std::endl;
        std::cout << std::endl;
        
        // Display shop in format matching game.cpp
        const int W = 58;
        std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
        std::string shopTitle = "SHOP  (refresh: $2)";
        int pad = (W - (int)shopTitle.size()) / 2;
        std::cout << "  |" << std::string(pad, ' ') << shopTitle
                  << std::string(W - pad - (int)shopTitle.size(), ' ') << "|" << std::endl;
        std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
        
        // Sample shop units
        std::cout << "  |  [1] Warrior   WarriorHP:20  ATK:8  R:1 [Rage] $5        |" << std::endl;
        std::cout << "  |  [2] Mage      MageHP:15  ATK:12 R:3 [AOE] $6            |" << std::endl;
        std::cout << "  |  [3] Tank      TankHP:30  ATK:5  R:1 [Block] $7          |" << std::endl;
        std::cout << "  |  [4] Assassin  AssassinHP:14  ATK:11 R:1 [Crit] $6       |" << std::endl;
        std::cout << "  |  [5] Archer    ArcherHP:16  ATK:9  R:3 [DblShot] $4      |" << std::endl;
        
        std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
        std::cout << "  Your Gold: 15" << std::endl;
        std::cout << std::endl;
        
        std::cout << "  SHOP COMMANDS:" << std::endl;
        std::cout << "  • buy <1-5>     - Buy a unit" << std::endl;
        std::cout << "  • info shop N   - See unit details" << std::endl;
        std::cout << "  • refresh       - Re-roll shop" << std::endl;
        std::cout << std::endl;
        
        bool validInput = false;
        while (!validInput) {
            std::cout << "  Try buying a unit! Type: buy 1" << std::endl;
            std::cout << "  (Then press Enter)" << std::endl;
            std::cout << std::endl;
            std::cout << "  > ";
            
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
    printProgressBar(3, 9);  // 3 of 9 lessons
    
    const std::vector<std::string> content = {
        "UNIT CLASSES & SPECIAL ABILITIES",
        "",
        "There are 5 unit classes in the game, each with unique",
        "abilities and attributes: (ATK = Attack)",
        "",
        "WARRIOR  - High HP, mid ATK",
        "          Ability: Rage - +30% ATK when HP < 50%",
        "",
        "MAGE     - Low HP, high ATK",
        "          Ability: AOE - 30% chance to deal 50% splash dmg",
        "",
        "TANK     - Super high HP, low ATK",
        "          Ability: Block - 25% chance to block 40% dmg",
        "",
        "ASSASSIN - Mid HP, high ATK, high crit.",
        "          Ability: Backstab - 25% chance +50% extra dmg",
        "",
        "ARCHER   - Mid HP, mid ATK, support",
        "          Ability: Double Shot - 20% chance to attack twice",
        "",
        "Each unit has: Name, Class, HP, ATK, Crit%, Level (star)"
    };
    
    displayLesson("Units and Special Abilities", content);
    
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
    printProgressBar(4, 9);  // 4 of 9 lessons
    
    const std::vector<std::string> content = {
        "BOARD PLACEMENT & FORMATION",
        "",
        "Your bench can hold up to 8 units. You can place units",
        "on the 5x8 battle grid (your side has 4 columns).",
        "",
        "PLACEMENT STRATEGIES:",
        "",
        "FRONT LINE (Column 0-1):",
        "  • Place TANKS and WARRIORS here",
        "  • They take the most damage",
        "",
        "BACK LINE (Column 2-3):",
        "  • Place MAGES, ARCHERS, and ASSASSINS",
        "  • Stay safe from enemies while dealing damage",
        "",
        "FORMATION TIPS:",
        "  • Have a balanced frontline and backline",
        "  • Consider unit synergies (more on that later!)",
        "  • Use 'place <idx> <row> <col>' command to position",
        "  • Use 'auto' command for automatic smart placement"
    };
    
    displayLesson("Board Placement & Formation", content);
    
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
        // Interactive: Show practice placement
        clearScreen();
        std::cout << std::endl;
        std::cout << "  Let's practice placing units on the battlefield!" << std::endl;
        std::cout << std::endl;
        std::cout << "  Your Bench:" << std::endl;
        std::cout << "  [1] Warrior" << std::endl;
        std::cout << "  [2] Archer" << std::endl;
        std::cout << std::endl;
        std::cout << "  +---------------+" << std::endl;
        std::cout << "  | YOUR FORMATION |" << std::endl;
        std::cout << "  |    0 1 2 3     |" << std::endl;
        std::cout << "  +---------------+" << std::endl;
        std::cout << " 0 | . . . . |" << std::endl;
        std::cout << " 1 | . . . . |" << std::endl;
        std::cout << " 2 | . . . . |" << std::endl;
        std::cout << " 3 | . . . . |" << std::endl;
        std::cout << " 4 | . . . . |" << std::endl;
        std::cout << "  +---------------+" << std::endl;
        std::cout << std::endl;
        std::cout << "  PLACEMENT COMMANDS:" << std::endl;
        std::cout << "  • place <idx> <row> <col>  - Place unit at position" << std::endl;
        std::cout << "  • remove <row> <col>       - Remove unit from board" << std::endl;
        std::cout << "  • auto                     - Auto-place all units" << std::endl;
        std::cout << "  • formation                - Show current formation" << std::endl;
        std::cout << std::endl;
        
        bool validInput = false;
        while (!validInput) {
            std::cout << "  Try placing the Warrior at Row 2, Column 0!" << std::endl;
            std::cout << "  Type: place 1 2 0" << std::endl;
            std::cout << "  (Then press Enter)" << std::endl;
            std::cout << std::endl;
            std::cout << "  > ";
            
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
    printProgressBar(5, 9);  // 5 of 9 lessons
    
    const std::vector<std::string> content = {
        "HOW COMBAT WORKS",
        "",
        "Once you've placed your units and clicked 'ready', the",
        "battle begins! Combat is AUTOMATIC and fully visual.",
        "",
        "COMBAT FLOW:",
        "  1. Your units and enemy units spawn on the grid",
        "  2. Every tick (turn), each unit finds the nearest enemy",
        "  3. If in range, units attack; otherwise, they move",
        "  4. Each attack may trigger a critical hit (1.5x damage)",
        "  5. Each attack may trigger class abilities",
        "  6. Dead units are removed from the battlefield",
        "",
        "COMBAT ENDS when:",
        "  • All your units are defeated (you lose)",
        "  • All enemy units are defeated (you win)",
        "  • 200 ticks have passed (tie, you take minimal damage)",
        "",
        "LOSING DAMAGE: 5 + (num surviving enemies * 2)",
        "",
        "You can watch the combat and press [Enter] to skip"
    };
    
    displayLesson("Combat System", content);
    
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
    printProgressBar(6, 9);  // 6 of 9 lessons
    
    const std::vector<std::string> content = {
        "SYNERGY EFFECTS",
        "",
        "Units of the same class provide powerful bonuses!",
        "When you have 2 or more units of the same class,",
        "automatic synergy effects activate.",
        "",
        "SYNERGY BONUSES:",
        "",
        "WARRIOR Synergy:",
        "  Lv1 (2x): +5 ATK to all",
        "  Lv2 (3x): +10 ATK to all",
        "",
        "MAGE Synergy: Lv1: +15% crit  Lv2: +30% crit",
        "",
        "TANK Synergy: Lv1: +20% HP recovery  Lv2: to all",
        "",
        "ASSASSIN Synergy: Lv1: +10 ATK  Lv2: +20 ATK, +10% crit",
        "",
        "ARCHER Synergy: Lv1: +3 ATK  Lv2: +5 ATK",
        "",
        "STRATEGY: Build themed armies around synergies!"
    };
    
    displayLesson("Synergy Effects", content);
    
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
    printProgressBar(7, 9);  // 7 of 9 lessons
    
    const std::vector<std::string> content = {
        "GOLD & RESOURCE MANAGEMENT",
        "",
        "Gold is your most precious resource!",
        "",
        "GOLD SOURCES:",
        "  • Battle rewards: Win streak = more gold",
        "  • Loss penalty: Lose streak = reduced income",
        "  • Interest: Every 10 gold = 1 bonus gold (max 5/turn)",
        "  • Special events: May grant gold",
        "",
        "GOLD SPENDING:",
        "  • Buy units from shop",
        "  • Refresh shop (2 gold per refresh)",
        "  • No other direct costs",
        "",
        "WINNING STRATEGY:",
        "  • Early game (rounds 1-3): Buy aggressively",
        "  • Mid game (rounds 4-7): Balance buying and saving",
        "  • Late game: Save for interest bonuses and power units",
        "",
        "PRO TIP: 50 gold generates 5 free gold per round!"
    };
    
    displayLesson("Gold & Resource Management", content);
    
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
    printProgressBar(8, 9);  // 8 of 9 lessons
    
    const std::vector<std::string> content = {
        "UNIT LEVELING & MERGING",
        "",
        "Units can be leveled up to become more powerful!",
        "",
        "HOW MERGING WORKS:",
        "  When you have 3 units of the SAME name at the SAME",
        "  level, they automatically merge into 1 upgraded unit.",
        "",
        "LEVEL PROGRESSION:",
        "  Level 1 ★     -> Level 2 ★★",
        "    • HP × 1.8        • ATK × 1.5",
        "",
        "  Level 2 ★★    -> Level 3 ★★★ (Max Level)",
        "    • HP × 3.0        • ATK × 2.5",
        "",
        "STRATEGY:",
        "  • Focus on getting 3 copies of strong units",
        "  • Leveled units are MUCH more powerful",
        "  • A 3-star unit can carry an entire army!",
        "",
        "NOTE: Merging is automatic - no action needed from you!"
    };
    
    displayLesson("Unit Leveling & Merging", content);
    
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
    std::cout << "  [PHASE 1: SHOPPING]" << std::endl;
    std::cout << std::endl;
    std::cout << "  Your Status:" << std::endl;
    std::cout << "    • HP: 100" << std::endl;
    std::cout << "    • Gold: 15" << std::endl;
    std::cout << std::endl;
    std::cout << "  +" << std::string(58, '-') << "+" << std::endl;
    std::cout << "  |" << std::string(16, ' ') << "SHOP  (refresh: $1)" << std::string(17, ' ') << "|" << std::endl;
    std::cout << "  +" << std::string(58, '-') << "+" << std::endl;
    std::cout << "  |  [1] Warrior   WarriorHP:20  ATK:8  R:1 [Rage] $5        |" << std::endl;
    std::cout << "  |  [2] Mage      MageHP:15  ATK:12 R:3 [AOE] $6            |" << std::endl;
    std::cout << "  |  [3] Tank      TankHP:30  ATK:5  R:1 [Block] $7          |" << std::endl;
    std::cout << "  |  [4] Archer    ArcherHP:16  ATK:9  R:3 [DblShot] $4      |" << std::endl;
    std::cout << "  |  [5] Assassin  AssassinHP:14  ATK:11 R:1 [Crit] $6       |" << std::endl;
    std::cout << "  +" << std::string(58, '-') << "+" << std::endl;
    std::cout << std::endl;
    std::cout << "  Buying: Warrior, Tank, Archer" << std::endl;
    std::cout << "  Total Cost: 5 + 7 + 4 = 16 gold" << std::endl;
    std::cout << "  Remaining Gold: 15 + 5 (interest) = 20 -> 4 gold after purchase" << std::endl;
    std::cout << std::endl;
    waitForInput();
    
    // PHASE 2: PLACEMENT
    clearScreen();
    std::cout << std::endl;
    std::cout << "  [PHASE 2: ARRANGING FORMATION]" << std::endl;
    std::cout << std::endl;
    std::cout << "  Your Bench:" << std::endl;
    std::cout << "    [1] Warrior" << std::endl;
    std::cout << "    [2] Tank" << std::endl;
    std::cout << "    [3] Archer" << std::endl;
    std::cout << std::endl;
    std::cout << "  +---------------+" << std::endl;
    std::cout << "  | YOUR FORMATION |" << std::endl;
    std::cout << "  |    0 1 2 3     |" << std::endl;
    std::cout << "  +---------------+" << std::endl;
    std::cout << " 0 | Tn .  .  Ar |" << std::endl;
    std::cout << " 1 | .  .  .  .  |" << std::endl;
    std::cout << " 2 | Wa .  .  .  |" << std::endl;
    std::cout << " 3 | .  .  .  .  |" << std::endl;
    std::cout << " 4 | .  .  .  .  |" << std::endl;
    std::cout << "  +---------------+" << std::endl;
    std::cout << std::endl;
    std::cout << "  Strategy: Tank in front + high-damage Warrior in middle + Archer for support" << std::endl;
    std::cout << std::endl;
    waitForInput();
    
    // PHASE 3: COMBAT
    clearScreen();
    std::cout << std::endl;
    std::cout << "  [PHASE 3: BATTLE!]" << std::endl;
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
            std::cout << "  ✓ VICTORY! All enemy units defeated!" << std::endl;
        }
        std::cout << std::endl;
    }
    
    waitForInput();
    
    // PHASE 4: RESULTS
    clearScreen();
    std::cout << std::endl;
    std::cout << "  [ROUND RESULTS]" << std::endl;
    std::cout << std::endl;
    std::cout << "  Battle Outcome: ✓ VICTORY!" << std::endl;
    std::cout << std::endl;
    std::cout << "  Combat Statistics:" << std::endl;
    std::cout << "    • Your Units Remaining: 3" << std::endl;
    std::cout << "    • Enemy Units Eliminated: 5" << std::endl;
    std::cout << "    • Your Damage Taken: 5 HP" << std::endl;
    std::cout << "    • Battle Duration: 5 ticks" << std::endl;
    std::cout << std::endl;
    std::cout << "  Gold Income:" << std::endl;
    std::cout << "    • Base Income: 5 gold" << std::endl;
    std::cout << "    • Victory Bonus: 3 gold" << std::endl;
    std::cout << "    • Win Streak Bonus: 2 gold" << std::endl;
    std::cout << "    • Total Gold This Round: 10 gold" << std::endl;
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
    waitForInput();
}

// -----------------------------------------------------------------
bool Tutorial::lessonCompletion() {
    printProgressBar(9, 9);  // 9 of 9 lessons
    
    // First, show a complete game demo
    demonstrateFullGameplay();
    
    clearScreen();
    printProgressBar(9, 9);
    
    const std::vector<std::string> content = {
        "",
        "CONGRATULATIONS, COMMANDER!",
        "",
        "You have completed the tutorial!",
        "",
        "You now understand:",
        "  ✓ How to navigate the shop and buy units",
        "  ✓ The 5 unit classes and their special abilities",
        "  ✓ How to arrange your formation strategically",
        "  ✓ How combat works automatically",
        "  ✓ How synergies provide powerful bonuses",
        "  ✓ How to manage gold wisely",
        "  ✓ How to level up and merge your units",
        "  ✓ How a complete game round flows",
        "",
        "You are ready to enter the battlefield!",
        "",
        "Remember: Every decision matters. Build wisely, fight",
        "hard, and claim victory!",
        "",
        "May fortune favor the bold, Commander!"
    };
    
    displayLesson("Tutorial Complete!", content);
    printEncouragement();
    std::cout << "\n  You will now return to the main menu..." << std::endl;
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
