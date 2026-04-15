// =====================================================================
// PROJECT: AUTO-BATTLER ARENA
// GROUP: Group 75
// FILE TYPE: Header File
// FILE NAME: ai.h
// DESCRIPTION:
//   This file defines the AI class that controls computer opponents.
//   The AI generates an army each round based on difficulty level (EASY/HARD),
//   manages gold budget, and places units using difficulty-specific strategies.
//   - EASY: Buys cheapest units and places them randomly
//   - HARD: Prioritizes high-value units and uses tactical positioning
// =====================================================================

#ifndef AI_H
#define AI_H

#include "unit.h"
#include "board.h"
#include "shop.h"
#include <vector>
#include <string>

// Difficulty enum
enum Difficulty {
    EASY,
    HARD
};

// ---------------------------------------------------------------------
// AI
// ---------------------------------------------------------------------
// Controls the computer opponent. The AI has its own gold budget each
// round and uses different strategies depending on difficulty.
//   - EASY: buys cheapest units, places them randomly on the front row.
//   - HARD: prioritises high-value units using a score table and
//           arranges tanks in front, damage dealers in back.
// The AI OWNS its units and is responsible for deleting them.
// ---------------------------------------------------------------------
class AI {
public:
    // -----------------------------------------------------------------
    // Constructor
    // What it does : creates an AI opponent at the given difficulty.
    // Input  : difficulty – EASY or HARD
    // Output : an initialised AI
    // -----------------------------------------------------------------
    AI(Difficulty difficulty);

    // -----------------------------------------------------------------
    // Destructor
    // What it does : frees all dynamically allocated AI units.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    ~AI();

    // -----------------------------------------------------------------
    // generateArmy - Build AI army for current round based on difficulty
    // Calculates AI gold budget (scales with round), creates temporary
    // shop, and uses difficulty-specific buying strategy
    // Parameters: round - Current game round (affects AI gold budget)
    // Purpose: Generate opponent army for this combat round
    // -----------------------------------------------------------------
    void generateArmy(int round);

    // -----------------------------------------------------------------
    // placeUnits - Position AI army on board according to difficulty
    // Delegates to difficulty-specific placement logic for formation
    // Parameters: board - Game board to place units on
    // Purpose: Arrange AI army in strategic formation
    // -----------------------------------------------------------------
    void placeUnits(Board& board);

    // -----------------------------------------------------------------
    // clearArmy - Delete all units in AI army and reset inventory
    // Safely deletes each Unit pointer and clears army_ vector
    // Purpose: Clean up AI army before next round generation
    // -----------------------------------------------------------------
    void clearArmy();

    // -----------------------------------------------------------------
    // releaseArmy - Clear AI army vector without deleting units
    // Clears vector without freeing memory (ownership transferred)
    // Used when units have been transferred to board/combat
    // Purpose: Reset army list after units placed on board
    // -----------------------------------------------------------------
    void releaseArmy();

    // -----------------------------------------------------------------
    // getArmySize - Return number of units in AI army
    // Returns: Integer count of current army units
    // Purpose: Query AI army size for game logic and display
    // -----------------------------------------------------------------
    int getArmySize() const;

    // -----------------------------------------------------------------
    // getDifficultyString - Get human-readable difficulty level
    // Returns: "Easy" or "Hard" string
    // Purpose: Display current difficulty setting to player
    // -----------------------------------------------------------------
    std::string getDifficultyString() const;

private:
    Difficulty difficulty_;
    std::vector<Unit*> army_;  // dynamically allocated units
    int gold_;

    // -----------------------------------------------------------------
    // buyEasyStrategy - Purchase units using simple cheapest-first approach
    // Repeatedly refreshes shop and buys cheapest affordable units until
    // gold runs out or max army size reached
    // Parameters: shop - Temporary shop to purchase from
    // Purpose: Implement EASY difficulty AI purchasing behavior
    // -----------------------------------------------------------------
    void buyEasyStrategy(Shop& shop);

    // -----------------------------------------------------------------
    // buyHardStrategy - Purchase units using score-based optimization
    // Evaluates each unit with formula: ATK*2 + HP/5
    // Buys highest-scoring affordable units each iteration
    // Parameters: shop - Temporary shop to purchase from
    // Purpose: Implement HARD difficulty AI purchasing behavior
    // -----------------------------------------------------------------
    void buyHardStrategy(Shop& shop);

    // -----------------------------------------------------------------
    // placeEasy - Place units in front column sequentially
    // Places all units in front column (AI_MIN_COL), spreading across rows
    // No strategic positioning = easier for player
    // Parameters: board - Game board to place units on
    // Purpose: Implement EASY difficulty unit placement
    // -----------------------------------------------------------------
    void placeEasy(Board& board);

    // -----------------------------------------------------------------
    // placeHard - Place units in optimized formation
    // Separates units by class: tanks/warriors in front, damage in back
    // Maximizes defensive and offensive positioning for harder challenge
    // Parameters: board - Game board to place units on
    // Purpose: Implement HARD difficulty unit placement
    // -----------------------------------------------------------------
    void placeHard(Board& board);
};

#endif // AI_H
