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
#include <cstdio>

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

//displayMainMenu
// Purpose: Display main menu with game options to player
// Input: None
// Output: None (prints formatted menu to stdout)
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
    boxLine("  " + std::string(BR_YELLOW) + "5.  🗑️  Clear Data" + RESET, W);
    boxLine("  " + std::string(BR_WHITE) + "6.  🚪 Quit" + RESET, W);
    boxLine("", W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    std::cout << std::endl;
    std::cout << BOLD << BR_YELLOW << "  Select > " << RESET;
}

// selctPlayerName
// Purpose: Prompt and get player name for new game session
// Input: None
// Output: std::string - player name (defaults to "Player" if empty)
std::string selectPlayerName() {
    const int W = 43;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    boxLine(BOLD + std::string(BR_CYAN) + "  👤 Choose Your Name" + RESET, W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    boxLine("  Your name will be recorded on the", W);
    boxLine("  leaderboard for each victory.", W);
    boxLine("  (Leave empty for default: Player)", W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    std::cout << BOLD << BR_YELLOW << "  Enter name > " << RESET;
    
    std::string playerName;
    std::getline(std::cin, playerName);
    
    // Trim leading/trailing whitespace
    size_t start = playerName.find_first_not_of(" \t\n\r");
    size_t end = playerName.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos) {
        playerName = "Player";  // Default name if empty
    } else {
        playerName = playerName.substr(start, end - start + 1);
    }
    
    std::cout << BR_GREEN << "  ✅ Welcome, " << playerName << "!" << RESET << std::endl << std::endl;
    return playerName;
}

// selectDifficulty
// Purpose: Prompt and get difficulty selection from player with input validation
// Input: None
// Output: Difficulty enum - EASY or HARD (validates 1 or 2 input, retries on invalid)
Difficulty selectDifficulty() {
    const int W = 43;
    int choice = -1;
    
    while (choice != 1 && choice != 2) {
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

        // Read integer input
        std::cin >> choice;
        
        // Check for input stream errors
        if (std::cin.fail()) {
            std::cin.clear();                           // Clear error flag
            std::cin.ignore(10000, '\n');               // Discard invalid input
            std::cout << BR_RED << "  ❌ Invalid input. Please enter 1 or 2." << RESET << std::endl;
            choice = -1;
            continue;
        }
        
        // Discard any remaining characters in input buffer
        std::cin.ignore(10000, '\n');
        
        // Validate range
        if (choice != 1 && choice != 2) {
            std::cout << BR_RED << "  ❌ Invalid input. Please enter 1 or 2." << RESET << std::endl;
            choice = -1;
            continue;
        }
    }
    
    if (choice == 2) {
        std::cout << BR_RED << "  Difficulty set to HARD. Good luck!" << RESET << std::endl;
        return HARD;
    }
    std::cout << BR_GREEN << "  Difficulty set to EASY." << RESET << std::endl;
    return EASY;
}

// clearDataMenu
// Purpose: Display data clearing options and handle deletion of game records and saves
// Input: None
// Output: None (modifies files on disk, prints status to stdout)
void clearDataMenu() {
    const int W = 52;
    bool valid = false;
    
    while (!valid) {
        std::cout << std::endl;
        std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
        boxLine(BOLD + std::string(BR_YELLOW) + "  🗑️  Clear Game Data" + RESET, W);
        std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
        boxLine("  " + std::string(BR_RED) + "1. Delete Leaderboard Records (records.txt)" + RESET, W);
        boxLine("", W);
        boxLine("  " + std::string(BR_RED) + "2. Delete Save Slot 1 (save1.dat)" + RESET, W);
        boxLine("", W);
        boxLine("  " + std::string(BR_RED) + "3. Delete Save Slot 2 (save2.dat)" + RESET, W);
        boxLine("", W);
        boxLine("  " + std::string(BR_RED) + "4. Delete Save Slot 3 (save3.dat)" + RESET, W);
        boxLine("", W);
        boxLine("  " + std::string(BR_RED) + "5. Delete ALL Data (Everything)" + RESET, W);
        boxLine("", W);
        boxLine("  " + std::string(BR_GREEN) + "6. Cancel" + RESET, W);
        std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
        std::cout << BOLD << BR_YELLOW << "  Select > " << RESET;

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            // Delete records.txt
            std::cout << BR_YELLOW << "  ⚠️  Clear leaderboard? (yes/no) > " << RESET;
            std::string confirm;
            std::getline(std::cin, confirm);
            
            if (confirm == "yes") {
                if (remove("docs/records.txt") == 0) {
                    std::cout << BR_GREEN << "  ✅ Leaderboard records deleted." << RESET << std::endl;
                    // Recreate empty records file
                    std::ofstream file("docs/records.txt");
                    file.close();
                } else {
                    std::cout << BR_RED << "  ❌ Error deleting records file." << RESET << std::endl;
                }
            } else {
                std::cout << BR_YELLOW << "  ⏭️  Cancelled." << RESET << std::endl;
            }
            valid = true;

        } else if (choice == "2" || choice == "3" || choice == "4") {
            // Delete specific save slot
            int slot = std::stoi(choice) - 1;  // Convert 2->1, 3->2, 4->3
            std::cout << BR_YELLOW << "  ⚠️  Delete save slot " << (slot + 1) << "? (yes/no) > " << RESET;
            std::string confirm;
            std::getline(std::cin, confirm);
            
            if (confirm == "yes") {
                std::string filepath = getSaveFilePath(slot + 1);
                if (remove(filepath.c_str()) == 0) {
                    std::cout << BR_GREEN << "  ✅ Save slot " << (slot + 1) << " deleted." << RESET << std::endl;
                } else {
                    std::cout << BR_YELLOW << "  ℹ️  Save slot " << (slot + 1) << " is empty." << RESET << std::endl;
                }
            } else {
                std::cout << BR_YELLOW << "  ⏭️  Cancelled." << RESET << std::endl;
            }
            valid = true;

        } else if (choice == "5") {
            // Delete all data
            std::cout << BR_RED << "  ⚠️⚠️⚠️  WARNING: Delete EVERYTHING? (yes/no) > " << RESET;
            std::string confirm;
            std::getline(std::cin, confirm);
            
            if (confirm == "yes") {
                // Delete records
                remove("docs/records.txt");
                std::ofstream file("docs/records.txt");
                file.close();
                
                // Delete all saves
                for (int i = 1; i <= 3; ++i) {
                    remove(getSaveFilePath(i).c_str());
                }
                
                std::cout << BR_GREEN << "  ✅ All game data deleted." << RESET << std::endl;
            } else {
                std::cout << BR_YELLOW << "  ⏭️  Cancelled." << RESET << std::endl;
            }
            valid = true;

        } else if (choice == "6") {
            std::cout << BR_YELLOW << "  Returning to main menu..." << RESET << std::endl;
            valid = true;

        } else {
            std::cout << BR_RED << "  ❌ Invalid option. Please enter 1-6." << RESET << std::endl;
        }
    }
}

// main
// Purpose: Program entry point - seed RNG and manage game main loop
// Input: None (argc/argv not used)
// Output: 0 on normal exit
int main() {
    // Seed the random number generator once at program start.
    srand((unsigned)time(nullptr));

    bool running = true;

    while (running) {
        displayMainMenu();

        std::string choice;
        if (!std::getline(std::cin, choice)) break;

        if (choice == "1") {
            // New Game - input player name, select difficulty, and start
            std::string playerName = selectPlayerName();
            Difficulty diff = selectDifficulty();
            Game game(diff, playerName);
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

        } else if (choice == "5") {
            // Clear Data - open data management menu
            clearDataMenu();

        } else if (choice == "5" || choice == "quit" || choice == "exit") {
            std::cout << "\n  Thanks for playing Auto-Battler Arena! Goodbye.\n"
                      << std::endl;
            running = false;

        } else {
            std::cout << "  Invalid option. Please enter 1-6." << std::endl;
        }
    }

    return 0;
}
