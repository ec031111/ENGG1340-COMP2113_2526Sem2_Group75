#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "shop.h"
#include "player.h"
#include "ai.h"
#include <string>
#include <vector>

// Damage dealt to the losing player after each battle round.
const int LOSS_DAMAGE_BASE = 5;
const int LOSS_DAMAGE_PER_SURVIVING = 2;

// Base critical hit chance (percentage, 0-100).
const int BASE_CRIT_CHANCE = 20;
const double CRIT_MULTIPLIER = 1.5;

// Maximum combat iterations to prevent infinite loops.
const int MAX_COMBAT_TICKS = 200;

// ---------------------------------------------------------------------
// Game
// ---------------------------------------------------------------------
// The master class that ties everything together. Manages the game loop
// (shop phase -> battle phase -> repeat), combat resolution, and
// file I/O for the leaderboard.
// ---------------------------------------------------------------------
class Game {
public:
    // -----------------------------------------------------------------
    // Constructor
    // What it does : creates the game with the given difficulty.
    // Input  : difficulty - EASY or HARD
    // Output : a Game ready to run
    // -----------------------------------------------------------------
    Game(Difficulty difficulty);

    // -----------------------------------------------------------------
    // Destructor
    // What it does : cleans up board units that may remain.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    ~Game();

    // -----------------------------------------------------------------
    // run
    // What it does : starts and loops the main game until the player
    //                dies or quits. Returns the final score.
    // Input  : none
    // Output : integer final score (rounds survived)
    // -----------------------------------------------------------------
    int run();

    // -----------------------------------------------------------------
    // saveRecord
    // What it does : appends the game result to records.txt.
    // Input  : none
    // Output : none (writes to file)
    // -----------------------------------------------------------------
    void saveRecord() const;

    // -----------------------------------------------------------------
    // displayLeaderboard  (static)
    // What it does : reads records.txt and prints the top scores.
    // Input  : none
    // Output : none (reads file, prints to stdout)
    // -----------------------------------------------------------------
    static void displayLeaderboard();

private:
    Board   board_;
    Shop    shop_;
    Player  player_;
    AI      ai_;
    bool    running_;
    bool    skipCombat_;  // player pressed 's' to fast-forward

    // -----------------------------------------------------------------
    // shopPhase
    // What it does : handles one shop round - display shop, read
    //                commands, let the player buy/sell/place/refresh.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void shopPhase();

    // -----------------------------------------------------------------
    // battlePhase
    // What it does : places player + AI units on the board and runs
    //                the auto-combat loop until one side is eliminated.
    // Input  : none
    // Output : true if the player won the battle, false otherwise
    // -----------------------------------------------------------------
    bool battlePhase();

    // -----------------------------------------------------------------
    // placePlayerUnits
    // What it does : places all bench units onto the player's side of
    //                the board (cols 0 to PLAYER_MAX_COL).
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void placePlayerUnits();

    // -----------------------------------------------------------------
    // resolveCombat
    // What it does : runs the tick-based combat loop with paced output.
    // Input  : none
    // Output : true if the player's side won
    // -----------------------------------------------------------------
    bool resolveCombat();

    // -----------------------------------------------------------------
    // performAttack
    // What it does : one unit attacks another, applying crit logic.
    // Inputs : attacker, defender - Unit pointers
    // Output : integer damage dealt
    // -----------------------------------------------------------------
    int performAttack(Unit* attacker, Unit* defender);

    // -----------------------------------------------------------------
    // cleanupDeadUnits
    // What it does : removes dead units from the board.
    //                Dead units are deleted (freed).
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void cleanupDeadUnits();

    // -----------------------------------------------------------------
    // printHelp
    // What it does : shows available commands during the shop phase.
    // Input  : none
    // Output : none (stdout)
    // -----------------------------------------------------------------
    void printHelp() const;
};

#endif // GAME_H
