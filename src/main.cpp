#include "game.h"
#include "tutorial.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

// ============== ANSI COLOR CODES ==============
const std::string ANSI_RED     = "\033[31m";
const std::string ANSI_GREEN   = "\033[32m";
const std::string ANSI_YELLOW  = "\033[33m";
const std::string ANSI_BLUE    = "\033[34m";
const std::string ANSI_BRIGHT_CYAN    = "\033[96m";
const std::string ANSI_RESET   = "\033[0m";

// Helper: print a padded line inside |...|
static void boxLine(const std::string& text, int W) {
    std::string s = text;
    if ((int)s.size() < W) s += std::string(W - s.size(), ' ');
    std::cout << "  |" << s << "|" << std::endl;
}

// -----------------------------------------------------------------
// displayMainMenu
// What it does : prints the main menu options to stdout.
// Input  : none
// Output : none (prints to stdout)
// -----------------------------------------------------------------
void displayMainMenu() {
    const int W = 42;
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '=') << "+" << std::endl;
    boxLine("", W);
    boxLine(ANSI_BRIGHT_CYAN + "   ⚔️  AUTO-BATTLER ARENA  🏆" + ANSI_RESET, W);
    boxLine("", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    boxLine("", W);
    boxLine(ANSI_GREEN + "  🎮 1.  New Game" + ANSI_RESET, W);
    boxLine(ANSI_BLUE + "  📚 2.  Tutorial" + ANSI_RESET, W);
    boxLine(ANSI_YELLOW + "  🏅 3.  Leaderboard" + ANSI_RESET, W);
    boxLine(ANSI_RED + "  💾 4.  Load Game" + ANSI_RESET, W);
    boxLine(ANSI_RED + "  🚪 5.  Quit" + ANSI_RESET, W);
    boxLine("", W);
    std::cout << "  +" << std::string(W, '=') << "+" << std::endl;
    std::cout << std::endl;
    std::cout << "  Select > ";
}

// -----------------------------------------------------------------
// selectDifficulty
// What it does : prompts the user to choose Easy or Hard difficulty.
// Input  : none
// Output : the chosen Difficulty enum value
// -----------------------------------------------------------------
Difficulty selectDifficulty() {
    const int W = 40;
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '=') << "+" << std::endl;
    std::string title = "⚙️  SELECT DIFFICULTY  ⚙️";
    int pad = (W - (int)title.size()) / 2;
    std::cout << "  |" << std::string(pad, ' ') << title 
              << std::string(W - pad - (int)title.size(), ' ') << "|" << std::endl;
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    boxLine("", W);
    boxLine(ANSI_GREEN + "  😊 1. EASY" + ANSI_RESET, W);
    boxLine("     • AI buys cheap units", W);
    boxLine("     • Random placement", W);
    boxLine("", W);
    boxLine(ANSI_RED + "  😈 2. HARD" + ANSI_RESET, W);
    boxLine("     • AI uses smart strategy", W);
    boxLine("     • Optimised formation", W);
    boxLine("", W);
    std::cout << "  +" << std::string(W, '=') << "+" << std::endl;
    std::cout << "  Select > ";

    std::string input;
    std::getline(std::cin, input);

    if (input == "2") {
        std::cout << ANSI_RED << "  😈 Difficulty set to HARD. Good luck warrior!" << ANSI_RESET << std::endl;
        return HARD;
    }
    std::cout << ANSI_GREEN << "  😊 Difficulty set to EASY." << ANSI_RESET << std::endl;
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
            Game::displayLeaderboard();

        } else if (choice == "4") {
            // Load game - try to read save file and resume
            // Need to read difficulty from save file first
            std::ifstream checkFile("docs/savegame.dat");
            if (!checkFile.is_open()) {
                std::cout << "  No save file found." << std::endl;
            } else {
                // Peek at difficulty (last line of save)
                // Default to EASY, loadGame will handle the rest
                checkFile.close();
                Difficulty diff = EASY;
                Game game(diff);
                if (game.loadGame()) {
                    game.run(false);
                } else {
                    std::cout << "  Save file is corrupted." << std::endl;
                }
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
