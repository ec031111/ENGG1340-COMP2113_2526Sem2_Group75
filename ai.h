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
    // generateArmy
    // What it does : spends the round's gold budget to buy units from
    //                a private shop and stores them in army_.
    // Input  : round – current round number (affects budget)
    // Output : none
    // -----------------------------------------------------------------
    void generateArmy(int round);

    // -----------------------------------------------------------------
    // placeUnits
    // What it does : places all army units onto the AI side of the board
    //                (columns AI_MIN_COL to BOARD_COLS-1).
    // Input  : board – reference to the game board
    // Output : none
    // -----------------------------------------------------------------
    void placeUnits(Board& board);

    // -----------------------------------------------------------------
    // clearArmy
    // What it does : deletes all units in army_ and clears the vector.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void clearArmy();

    // -----------------------------------------------------------------
    // releaseArmy
    // What it does : clears the army vector WITHOUT deleting the units.
    //                Use when ownership has been transferred (e.g. to board).
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void releaseArmy();

    // -----------------------------------------------------------------
    // getArmySize
    // What it does : returns how many units the AI currently has.
    // Input  : none
    // Output : integer size
    // -----------------------------------------------------------------
    int getArmySize() const;

    // -----------------------------------------------------------------
    // getDifficultyString
    // What it does : returns "Easy" or "Hard".
    // Input  : none
    // Output : string
    // -----------------------------------------------------------------
    std::string getDifficultyString() const;

private:
    Difficulty difficulty_;
    std::vector<Unit*> army_;  // dynamically allocated units
    int gold_;

    // -----------------------------------------------------------------
    // buyEasyStrategy
    // What it does : buys the cheapest available units until gold runs out.
    // Input  : shop – reference to a temporary shop
    // Output : none
    // -----------------------------------------------------------------
    void buyEasyStrategy(Shop& shop);

    // -----------------------------------------------------------------
    // buyHardStrategy
    // What it does : evaluates units by a score (ATK + HP/10) and buys
    //                the highest-scoring affordable units.
    // Input  : shop – reference to a temporary shop
    // Output : none
    // -----------------------------------------------------------------
    void buyHardStrategy(Shop& shop);

    // -----------------------------------------------------------------
    // placeEasy
    // What it does : places all units in the front row (col = AI_MIN_COL).
    // Input  : board
    // Output : none
    // -----------------------------------------------------------------
    void placeEasy(Board& board);

    // -----------------------------------------------------------------
    // placeHard
    // What it does : tanks go front (AI_MIN_COL), damage dealers in back
    //                (BOARD_COLS - 1).
    // Input  : board
    // Output : none
    // -----------------------------------------------------------------
    void placeHard(Board& board);
};

#endif // AI_H
