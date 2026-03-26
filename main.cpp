#include "game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

// -----------------------------------------------------------------
// displayMainMenu
// What it does : prints the main menu options to stdout.
// Input  : none
// Output : none (prints to stdout)
// -----------------------------------------------------------------
void displayMainMenu() {
    std::cout << std::endl;
    std::cout << "  +---------------------------------------+" << std::endl;
    std::cout << "  |                                       |" << std::endl;
    std::cout << "  |       AUTO-BATTLER ARENA              |" << std::endl;
    std::cout << "  |                                       |" << std::endl;
    std::cout << "  +---------------------------------------+" << std::endl;
    std::cout << "  |                                       |" << std::endl;
    std::cout << "  |   1.  New Game                        |" << std::endl;
    std::cout << "  |   2.  Leaderboard                     |" << std::endl;
    std::cout << "  |   3.  Quit                            |" << std::endl;
    std::cout << "  |                                       |" << std::endl;
    std::cout << "  +---------------------------------------+" << std::endl;
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
    std::cout << std::endl;
    std::cout << "  +-----------------------------------+" << std::endl;
    std::cout << "  |  Select Difficulty                 |" << std::endl;
    std::cout << "  +-----------------------------------+" << std::endl;
    std::cout << "  |  1. Easy                           |" << std::endl;
    std::cout << "  |     - AI buys cheap units          |" << std::endl;
    std::cout << "  |     - Random placement             |" << std::endl;
    std::cout << "  |  2. Hard                           |" << std::endl;
    std::cout << "  |     - AI uses smart strategy       |" << std::endl;
    std::cout << "  |     - Optimised formation          |" << std::endl;
    std::cout << "  +-----------------------------------+" << std::endl;
    std::cout << "  Select > ";

    std::string input;
    std::getline(std::cin, input);

    if (input == "2") {
        std::cout << "  Difficulty set to HARD. Good luck!" << std::endl;
        return HARD;
    }
    std::cout << "  Difficulty set to EASY." << std::endl;
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
            Difficulty diff = selectDifficulty();
            Game game(diff);
            game.run();

        } else if (choice == "2") {
            Game::displayLeaderboard();

        } else if (choice == "3" || choice == "quit" || choice == "exit") {
            std::cout << "\n  Thanks for playing Auto-Battler Arena! Goodbye.\n"
                      << std::endl;
            running = false;

        } else {
            std::cout << "  Invalid option. Please enter 1, 2, or 3." << std::endl;
        }
    }

    return 0;
}
