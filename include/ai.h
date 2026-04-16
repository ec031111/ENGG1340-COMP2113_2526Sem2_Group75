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
    // Purpose: Create AI opponent at given difficulty level
    // Input: difficulty (enum) - EASY or HARD
    // Output: Initialized AI object
    AI(Difficulty difficulty);

    // Purpose: Free all dynamically allocated AI units
    // Input: none
    // Output: none
    ~AI();

    // Purpose: Generate AI army for current round based on difficulty
    // Input: round (int) - current game round (affects AI gold budget)
    // Output: none
    void generateArmy(int round);

    // Purpose: Position AI army on board in difficulty-specific formation
    // Input: board (Board reference) - game board to place units on
    // Output: none
    void placeUnits(Board& board);

    // Purpose: Delete all units in AI army and reset inventory
    // Input: none
    // Output: none
    void clearArmy();

    // Purpose: Clear AI army list without deleting units (ownership transferred)
    // Input: none
    // Output: none
    void releaseArmy();

    // Purpose: Get count of units in AI army
    // Input: none
    // Output: int - number of units in army
    int getArmySize() const;

    // Purpose: Get human-readable difficulty level string
    // Input: none
    // Output: string - "Easy" or "Hard"
    std::string getDifficultyString() const;

private:
    Difficulty difficulty_;              // AI difficulty level (EASY/HARD)
    std::vector<Unit*> army_;            // Dynamically allocated unit army
    int gold_;                           // AI gold budget for this round

    // Purpose: Purchase units using cheapest-first strategy
    // Input: shop (Shop reference) - temporary shop to purchase from
    // Output: none
    void buyEasyStrategy(Shop& shop);

    // Purpose: Purchase units using score-based optimization strategy
    // Input: shop (Shop reference) - temporary shop to purchase from
    // Output: none
    void buyHardStrategy(Shop& shop);

    // Purpose: Place units in front column formation (easy difficulty)
    // Input: board (Board reference) - game board to place units on
    // Output: none
    void placeEasy(Board& board);

    // Purpose: Place units in strategic formation (hard difficulty)
    // Input: board (Board reference) - game board to place units on
    // Output: none
    void placeHard(Board& board);
};

#endif // AI_H
