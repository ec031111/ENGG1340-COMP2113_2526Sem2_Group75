/*
 * =====================================================================
 * Project Name: Auto-Battler Arena
 * Group Number: ENGG1340-COMP2113_2526Sem2_Group75
 * File Type: Header File (.h)
 * File Description:
 *   Player class manages the human player's game state including:
 *   - Health points (HP) and gold currency
 *   - Unit bench (squad) for deploying units in battle
 *   - Win/loss streaks for bonus gold calculation
 *   - Round progression tracking
 *   Implements resource management and streak-based reward system.
 * =====================================================================
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "unit.h"
#include <vector>
#include <string>

// Maximum number of units a player can hold on the bench.
const int MAX_BENCH_SIZE = 8;

// Starting values
const int STARTING_HP   = 100;
const int STARTING_GOLD = 10;
const int GOLD_PER_ROUND = 5;
const int WIN_STREAK_BONUS = 3;   // 3 extra gold per consecutive win (only if streak >= 3)
const int LOSS_STREAK_BONUS = 2;  // 2 extra gold per consecutive loss (only if streak >= 3)
const int INTEREST_PER_10 = 1;    // earn 1 gold per 10 gold saved, max 5

// ---------------------------------------------------------------------
// Player
// ---------------------------------------------------------------------
// Tracks the human player's resources: HP, gold, bench (reserve units),
// and win/loss streak for bonus gold calculation.
// The Player OWNS the units on the bench and is responsible for
// deleting them when they are sold or the game ends.
// ---------------------------------------------------------------------
class Player {
public:
    // -----------------------------------------------------------------
    // Constructor
    // What it does : initialises the player with starting HP and gold.
    // Input  : name – player display name
    // Output : a fully initialised Player
    // -----------------------------------------------------------------
    Player(const std::string& name = "Player");

    // -----------------------------------------------------------------
    // Destructor
    // What it does : frees all dynamically allocated units on the bench.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    ~Player();

    // Getters ----------------------------------------------------------
    std::string getName() const;
    int  getHp() const;
    int  getGold() const;
    int  getBenchSize() const;
    int  getWinStreak() const;
    int  getLossStreak() const;
    int  getRoundsPlayed() const;
    bool isAlive() const;

    // -----------------------------------------------------------------
    // addGold / spendGold
    // What it does : adjusts the player's gold balance.
    // Input  : amount – positive integer
    // Output : spendGold returns false if insufficient funds
    // -----------------------------------------------------------------
    void addGold(int amount);
    bool spendGold(int amount);

    // -----------------------------------------------------------------
    // takeDamage
    // What it does : reduces player HP (floors at 0).
    // Input  : damage – positive integer
    // Output : none
    // -----------------------------------------------------------------
    void takeDamage(int damage);

    // -----------------------------------------------------------------
    // addToBench
    // What it does : adds a unit to the bench. The player takes
    //                ownership of the unit's memory.
    // Input  : unit – dynamically allocated Unit*
    // Output : true if bench has space, false otherwise
    // -----------------------------------------------------------------
    bool addToBench(Unit* unit);

    // -----------------------------------------------------------------
    // removeFromBench
    // What it does : removes a unit from the bench by index.
    //                Returns the pointer (caller takes ownership).
    // Input  : index – 0-based bench index
    // Output : Unit* on success, nullptr if invalid index
    // -----------------------------------------------------------------
    Unit* removeFromBench(int index);

    // -----------------------------------------------------------------
    // sellUnit
    // What it does : removes a unit from the bench, adds its sell price
    //                to gold, and deletes the unit.
    // Input  : index – bench index
    // Output : true on success
    // -----------------------------------------------------------------
    bool sellUnit(int index);

    // -----------------------------------------------------------------
    // getBenchUnit
    // What it does : peeks at a bench slot without removing.
    // Input  : index
    // Output : Unit* or nullptr
    // -----------------------------------------------------------------
    Unit* getBenchUnit(int index) const;

    // -----------------------------------------------------------------
    // displayBench
    // What it does : prints the bench contents to stdout.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void displayBench() const;

    // -----------------------------------------------------------------
    // displayStatus
    // What it does : prints HP, gold, and streak info.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void displayStatus() const;

    // -----------------------------------------------------------------
    // startNewRound
    // What it does : gives the player their round income (base gold +
    //                streak bonus) and increments the round counter.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void startNewRound();

    // -----------------------------------------------------------------
    // recordWin / recordLoss
    // What it does : updates the win/loss streak counters.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void recordWin();
    void recordLoss();

    // -----------------------------------------------------------------
    // getBench (const reference)
    // What it does : provides read access to the bench vector.
    // Input  : none
    // Output : const reference to bench_
    // -----------------------------------------------------------------
    const std::vector<Unit*>& getBench() const;

    // -----------------------------------------------------------------
    // loadState
    // What it does : restores player state from saved values.
    //                Clears current bench before loading.
    // Inputs : hp, gold, rounds, winStreak, lossStreak
    // Output : none
    // -----------------------------------------------------------------
    void loadState(int hp, int gold, int rounds, int winStreak, int lossStreak);

private:
    std::string name_;
    int hp_;
    int gold_;
    int winStreak_;
    int lossStreak_;
    int roundsPlayed_;
    std::vector<Unit*> bench_;   // dynamically allocated units
};

#endif // PLAYER_H
