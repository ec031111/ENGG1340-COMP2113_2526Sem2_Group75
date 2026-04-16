//=====================================================================
//Project Name: Auto-Battler Arena
//Group Number: ENGG1340-COMP2113_2526Sem2_Group75
//File Type: Header File (.h)
//File Description:
//  Player class manages the human player's game state including:
//  - Health points (HP) and gold currency
//  - Unit bench (squad) for deploying units in battle
//  - Win/loss streaks for bonus gold calculation
//  - Round progression tracking
//  Implements resource management and streak-based reward system.
// =====================================================================


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
    // Purpose: Initialize player with starting HP and gold
    // Input: name (const string reference) - player display name
    // Output: Fully initialized Player object
    Player(const std::string& name = "Player");

    // Purpose: Free all dynamically allocated units on the bench
    // Input: none
    // Output: none
    ~Player();

    // Purpose: Get player's name
    // Input: none
    // Output: const string reference - player name
    std::string getName() const;

    // Purpose: Get player's current HP
    // Input: none
    // Output: int - current health points
    int  getHp() const;

    // Purpose: Get player's current gold
    // Input: none
    // Output: int - current gold amount
    int  getGold() const;

    // Purpose: Get number of units on bench
    // Input: none
    // Output: int - bench size
    int  getBenchSize() const;

    // Purpose: Get current win streak counter
    // Input: none
    // Output: int - win streak count
    int  getWinStreak() const;

    // Purpose: Get current loss streak counter
    // Input: none
    // Output: int - loss streak count
    int  getLossStreak() const;

    // Purpose: Get total rounds played
    // Input: none
    // Output: int - rounds played count
    int  getRoundsPlayed() const;

    // Purpose: Check if player is still alive
    // Input: none
    // Output: bool - true if HP > 0
    bool isAlive() const;

    // Purpose: Add gold to player's balance
    // Input: amount (int) - gold to add
    // Output: none
    void addGold(int amount);

    // Purpose: Spend gold from player's balance
    // Input: amount (int) - gold to spend
    // Output: bool - false if insufficient funds, true otherwise
    bool spendGold(int amount);

    // Purpose: Reduce player HP by damage amount (minimum 0)
    // Input: damage (int) - damage to apply
    // Output: none
    void takeDamage(int damage);

    // Purpose: Add unit to bench (player takes ownership of memory)
    // Input: unit (Unit* pointer) - dynamically allocated unit
    // Output: bool - true if added, false if bench is full
    bool addToBench(Unit* unit);

    // Purpose: Remove unit from bench by index (caller takes ownership)
    // Input: index (int) - 0-based bench index
    // Output: Unit* - pointer to removed unit or nullptr if invalid
    Unit* removeFromBench(int index);

    // Purpose: Sell unit from bench and add sell price to gold
    // Input: index (int) - bench index
    // Output: bool - true if sold successfully, false if invalid index
    bool sellUnit(int index);

    // Purpose: Get unit from bench without removing
    // Input: index (int) - bench index
    // Output: Unit* - pointer to unit or nullptr if invalid
    Unit* getBenchUnit(int index) const;

    // Purpose: Display all units on bench
    // Input: none
    // Output: none (prints to stdout)
    void displayBench() const;

    // Purpose: Display player status (HP, gold, streaks)
    // Input: none
    // Output: none (prints to stdout)
    void displayStatus() const;

    // Purpose: Award round income (base gold + streak bonus) and increment round counter
    // Input: none
    // Output: none
    void startNewRound();

    // Purpose: Record a win and update win/loss streak
    // Input: none
    // Output: none
    void recordWin();

    // Purpose: Record a loss and update win/loss streak
    // Input: none
    // Output: none
    void recordLoss();

    // Purpose: Get read-only access to bench vector
    // Input: none
    // Output: const reference to bench_ vector
    const std::vector<Unit*>& getBench() const;

    // Purpose: Restore player state from saved values (clears bench first)
    // Input: hp, gold, rounds, winStreak, lossStreak (int) - saved state values
    // Output: none
    void loadState(int hp, int gold, int rounds, int winStreak, int lossStreak);

private:
    std::string name_;           // Player's display name
    int hp_;                     // Current health points
    int gold_;                   // Current gold amount
    int winStreak_;              // Consecutive wins
    int lossStreak_;             // Consecutive losses
    int roundsPlayed_;           // Total rounds survived
    std::vector<Unit*> bench_;   // Dynamically allocated units
};

#endif // PLAYER_H
