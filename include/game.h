#pragma once
#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "shop.h"
#include "player.h"
#include "ai.h"
#include "synergy.h"
#include "event.h"
#include <string>
#include <vector>
//---- word type ----
#define BOLD        "\033[1m"
#define ITALIC      "\033[3m"    
#define UNDERLINE   "\033[4m"    
//--- color define ---
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define GRAY        "\033[90m"
#define BR_RED      "\033[91m"
#define BR_GREEN    "\033[92m"
#define BR_YELLOW   "\033[93m"
#define BR_BLUE     "\033[94m"
#define BR_PURPLE   "\033[95m"
#define BR_CYAN     "\033[96m"
#define BR_WHITE    "\033[97m"

// Damage dealt to the losing player after each battle round.
const int LOSS_DAMAGE_BASE = 5;
const int LOSS_DAMAGE_PER_SURVIVING = 2;

// Maximum units player can deploy per round
// Round 1: 3, Round 2: 4, Round 3: 5, Round 4+: 6
const int MAX_DEPLOY_UNITS[] = {3, 4, 5, 6, 6, 6, 6, 6, 6, 6,  // rounds 1-10
                                 6, 6, 6, 6, 6, 6, 6, 6, 6, 6}; // rounds 11-20

// Base critical hit chance (percentage, 0-100).
const int BASE_CRIT_CHANCE = 20;
const double CRIT_MULTIPLIER = 1.5;

// Maximum combat iterations to prevent infinite loops.
const int MAX_COMBAT_TICKS = 200;

// Save file name
const std::string SAVE_FILE = "docs/savegame.dat";
const std::string RECORD_FILE = "docs/records.txt";

// ---------------------------------------------------------------------
// Game
// ---------------------------------------------------------------------
// The master class that orchestrates all game systems: shop, combat,
// synergies, events, unit merging, class abilities, and file I/O.
// ---------------------------------------------------------------------
class Game {
public:
    Game(Difficulty difficulty);
    ~Game();

    // Main game loop. Returns final score (rounds survived).
    // showIntro: if true, prints the story introduction (new games only).
    int run(bool showIntro = true);

    // File I/O
    void saveRecord() const;
    void saveGame() const;
    bool loadGame();
    static void displayLeaderboard();

private:
    Board   board_;
    Shop    shop_;
    Player  player_;
    AI      ai_;
    bool    running_;
    bool    skipCombat_;
    EventType currentEvent_;  // event active this round

    // --- Phase handlers ---
    void shopPhase();
    bool battlePhase();

    // --- Combat ---
    bool resolveCombat(std::vector<Unit*>& deadUnits);
    int  performAttack(Unit* attacker, Unit* defender);
    void performAbility(Unit* attacker, Unit* defender, std::vector<Unit*>& allUnits);
    void cleanupDeadUnits(std::vector<Unit*>& deadUnits);

    // --- Unit merging (3 -> 1 upgrade) ---
    void checkAndMerge();

    // --- Events ---
    void handleEvent();

    // --- Free unit for EVENT_RANDOM_FREE_UNIT ---
    void giveRandomFreeUnit();

    // --- UI ---
    void printHelp() const;
    void printCommandTips() const;
    void printStatusBar() const;  // Compact status display
    void printDeployLimit() const;  // Show deploy limit for current round
    void printFormation() const;  // Show formation with HP bars
    void showIntro() const;
    int getMaxDeployUnits() const;  // Get max units for current round
};

#endif // GAME_H
