 //==========================================================================================
 // Project: Auto-Battler Arena
 // Group: ENGG1340-COMP2113_2526Sem2_Group75
 // File Type: Source File (main entry point)
 // File Name: main.cpp
 // Description: Main program entry point. Initializes the game, handles the main menu,
 //              and manages game flow including new games, tutorials, leaderboard,
 //              and game loading/saving functionality.
 //==========================================================================================
 

#include "game.h"
#include "record.h"
#include "tutorial.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

static int visibleWidth(const std::string& text) {
    int width = 0;
    bool inEscape = false;
    for (char ch : text) {
        if (ch == '\033') {
            inEscape = true;
            continue;
        }
        if (inEscape) {
            if (ch == 'm') inEscape = false;
            continue;
        }
        ++width;
    }
    return width;
}

// Helper: print a padded line inside |...|
static void boxLine(const std::string& text, int W) {
    std::string s = text;
    int width = visibleWidth(s);
    if (width < W) s += std::string(W - width, ' ');
    std::cout << "  |" << s << "|" << std::endl;
}

// -----------------------------------------------------------------
// displayMainMenu
// What it does : prints the main menu options to stdout.
// Input  : none
// Output : none (prints to stdout)
// -----------------------------------------------------------------
void displayMainMenu() {
    const int W = 43;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    boxLine("", W);
    boxLine(BOLD + std::string(BR_CYAN) + "   ⚔️  AUTO-BATTLER ARENA  🏆" + RESET, W);
    boxLine("", W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    boxLine("", W);
    boxLine("  " + std::string(BR_GREEN) + "1.  🎮 New Game" + RESET, W);
    boxLine("  " + std::string(BR_BLUE) + "2.  📚 Tutorial" + RESET, W);
    boxLine("  " + std::string(BR_YELLOW) + "3.  🏅 Leaderboard" + RESET, W);
    boxLine("  " + std::string(BR_RED) + "4.  💾 Load Game" + RESET, W);
    boxLine("  " + std::string(BR_WHITE) + "5.  🚪 Quit" + RESET, W);
    boxLine("", W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    std::cout << std::endl;
    std::cout << BOLD << BR_YELLOW << "  Select > " << RESET;
}

// -----------------------------------------------------------------
// selectDifficulty
// What it does : prompts the user to choose Easy or Hard difficulty.
// Input  : none
// Output : the chosen Difficulty enum value
// -----------------------------------------------------------------
Difficulty selectDifficulty() {
    const int W = 43;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    boxLine(BOLD + std::string(BR_CYAN) + "  🎯 Select Difficulty" + RESET, W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    boxLine("  " + std::string(BR_GREEN) + "1. Easy  😊" + RESET, W);
    boxLine("     - AI buys cheap units", W);
    boxLine("     - Random placement", W);
    boxLine("  " + std::string(BR_RED) + "2. Hard  😈" + RESET, W);
    boxLine("     - AI uses smart strategy", W);
    boxLine("     - Optimised formation", W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    std::cout << BOLD << BR_YELLOW << "  Select > " << RESET;

    std::string input;
    std::getline(std::cin, input);

    if (input == "2") {
        std::cout << BR_RED << "  Difficulty set to HARD. Good luck!" << RESET << std::endl;
        return HARD;
    }
    std::cout << BR_GREEN << "  Difficulty set to EASY." << RESET << std::endl;
    return EASY;
}

// -----------------------------------------------------------------
// main
// What it does : program entry point. Seeds the random number generator,
//                displays the main menu, and dispatches to the
//                appropriate function based on user input.
// Input  : none (standard argc/argv not used)
// Output : 0 on normal exit
// -----------------------------------------------------------------
int main() {
    // Seed the random number generator once at program start.
    srand((unsigned)time(nullptr));

    bool running = true;

    while (running) {
        displayMainMenu();

        std::string choice;
        if (!std::getline(std::cin, choice)) break;

        if (choice == "1") {
            // New Game - directly select difficulty and start
            Difficulty diff = selectDifficulty();
            Game game(diff);
            game.run(true);

        } else if (choice == "2") {
            // Tutorial - run standalone tutorial
            Tutorial tutorial;
            tutorial.run();

        } else if (choice == "3") {
            Record::displayLeaderboard();

        } else if (choice == "4") {
            // Load game with slot selection
            // Display all slots with their status
            Record::displayAllSlots();
            
            // Check if any slots have save files
            int availableSlots = 0;
            for (int i = 1; i <= 3; ++i) {
                if (Record::hasSaveFile(i)) {
                    availableSlots++;
                }
            }
            
            if (availableSlots == 0) {
                std::cout << "  No save files found. Returning to main menu..." << std::endl;
                continue;
            }
            
            std::cout << "  Select slot (1-3) > ";
            std::string slotChoice;
            std::getline(std::cin, slotChoice);
            
            int slot = 1;
            if (slotChoice == "1" || slotChoice == "2" || slotChoice == "3") {
                slot = std::stoi(slotChoice);
            } else {
                std::cout << "  Invalid slot. Using slot 1." << std::endl;
            }
            
            if (!Record::hasSaveFile(slot)) {
                std::cout << "\n  Slot " << slot << " is empty. Returning to main menu..." << std::endl;
                continue;
            }

            Difficulty diff = EASY;
            Game game(diff);
            
            GamePhase phase = PHASE_ROUND_START;
            EventType event = EVENT_NONE;
            bool shouldResumeShop = false;
            
            if (Record::loadGame(game.getPlayer(), game.getBoard(), game.getShop(), 
                                 const_cast<AI&>(game.getAI()), phase, event, shouldResumeShop, slot)) {
                game.setCurrentPhase(phase);
                game.setCurrentEvent(event);
                game.setShouldResumeShop(shouldResumeShop);
                game.run(false);
            } else {
                std::cout << "  Save file is corrupted." << std::endl;
            }

        } else if (choice == "5" || choice == "quit" || choice == "exit") {
            std::cout << "\n  Thanks for playing Auto-Battler Arena! Goodbye.\n"
                      << std::endl;
            running = false;

        } else {
            std::cout << "  Invalid option. Please enter 1, 2, 3, or 4." << std::endl;
        }
    }

    return 0;
}
