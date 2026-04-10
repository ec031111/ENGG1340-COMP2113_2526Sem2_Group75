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

// Damage dealt to the losing player after each battle round.
const int LOSS_DAMAGE_BASE = 5;
const int LOSS_DAMAGE_PER_SURVIVING = 2;

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
    bool resolveCombat();
    int  performAttack(Unit* attacker, Unit* defender);
    void performAbility(Unit* attacker, Unit* defender, std::vector<Unit*>& allUnits);
    void cleanupDeadUnits();

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
    void showIntro() const;
};

#endif // GAME_H
