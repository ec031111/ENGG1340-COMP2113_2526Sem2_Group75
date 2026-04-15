#include "ai.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------
// Constructor
// What it does : stores the chosen difficulty level.
// Input  : difficulty
// Output : initialised AI with empty army
// -----------------------------------------------------------------
AI::AI(Difficulty difficulty)
    : difficulty_(difficulty), gold_(0) {}

// -----------------------------------------------------------------
// Destructor
// What it does : frees all remaining army units.
// Input  : none
// Output : none
// -----------------------------------------------------------------
AI::~AI() {
    clearArmy();
}

// -----------------------------------------------------------------
// generateArmy
// What it does : calculates the AI's gold budget for the round,
//                creates a temporary shop, and uses the appropriate
//                buying strategy to fill army_.
// Input  : round – current round number
// Output : none (populates army_)
// -----------------------------------------------------------------
void AI::generateArmy(int round) {
    clearArmy();

    // AI gold scales with the round number
    // Base budget of 10, plus 2 per round
    gold_ = 10 + round * 2;
    if (difficulty_ == HARD) {
        gold_ += 3;  // Hard AI gets a small gold advantage
    }

    // Create a temporary shop for the AI to buy from
    Shop tempShop;

    if (difficulty_ == EASY) {
        buyEasyStrategy(tempShop);
    } else {
        buyHardStrategy(tempShop);
    }

    // Mark all units as AI-owned
    for (size_t i = 0; i < army_.size(); ++i) {
        army_[i]->setPlayerUnit(false);
    }
}

// -----------------------------------------------------------------
// buyEasyStrategy
// What it does : refreshes the shop and always buys the cheapest
//                unit first, repeating until gold runs out or the
//                shop is empty.
// Input  : shop
// Output : none (adds units to army_)
// -----------------------------------------------------------------
void AI::buyEasyStrategy(Shop& shop) {
    // Buy from multiple shop refreshes
    int maxUnits = 5;
    int bought = 0;

    while (bought < maxUnits && gold_ > 0) {
        shop.refresh();

        // Find cheapest unit in the shop
        int cheapestIdx = -1;
        int cheapestCost = 9999;
        for (int i = 0; i < SHOP_SIZE; ++i) {
            Unit* u = shop.getUnit(i);
            if (u != nullptr && u->getCost() <= gold_ && u->getCost() < cheapestCost) {
                cheapestCost = u->getCost();
                cheapestIdx = i;
            }
        }

        if (cheapestIdx == -1) break;  // can't afford anything

        Unit* unit = shop.buyUnit(cheapestIdx);
        if (unit != nullptr) {
            army_.push_back(unit);
            gold_ -= unit->getCost();
            bought++;
        }
    }
}

// -----------------------------------------------------------------
// buyHardStrategy
// What it does : refreshes the shop and evaluates each unit with a
//                score function (ATK * 2 + HP / 5). Buys the highest
//                scoring affordable unit each iteration.
// Input  : shop
// Output : none (adds units to army_)
// -----------------------------------------------------------------
void AI::buyHardStrategy(Shop& shop) {
    int maxUnits = 5;
    int bought = 0;

    while (bought < maxUnits && gold_ > 0) {
        shop.refresh();

        // Score each unit and pick the best affordable one
        int bestIdx = -1;
        int bestScore = -1;
        for (int i = 0; i < SHOP_SIZE; ++i) {
            Unit* u = shop.getUnit(i);
            if (u == nullptr) continue;
            if (u->getCost() > gold_) continue;

            int score = u->getAtk() * 2 + u->getMaxHp() / 5;
            if (score > bestScore) {
                bestScore = score;
                bestIdx = i;
            }
        }

        if (bestIdx == -1) break;

        Unit* unit = shop.buyUnit(bestIdx);
        if (unit != nullptr) {
            army_.push_back(unit);
            gold_ -= unit->getCost();
            bought++;
        }
    }
}

// -----------------------------------------------------------------
// placeUnits
// What it does : delegates to the difficulty-specific placement logic.
// Input  : board
// Output : none (units placed on the board)
// -----------------------------------------------------------------
void AI::placeUnits(Board& board) {
    if (difficulty_ == EASY) {
        placeEasy(board);
    } else {
        placeHard(board);
    }
}

// -----------------------------------------------------------------
// placeEasy
// What it does : places all units on the front AI column (AI_MIN_COL),
//                spreading across rows. If the front column is full,
//                spills into the next column.
// Input  : board
// Output : none
// -----------------------------------------------------------------
void AI::placeEasy(Board& board) {
    int col = AI_MIN_COL;
    int row = 0;

    for (size_t i = 0; i < army_.size(); ++i) {
        bool placed = false;
        while (!placed && col < BOARD_COLS) {
            if (board.isEmpty(row, col)) {
                board.placeUnit(army_[i], row, col);
                placed = true;
            }
            row++;
            if (row >= BOARD_ROWS) {
                row = 0;
                col++;
            }
        }
    }
}

// -----------------------------------------------------------------
// placeHard
// What it does : separates units into tanks (front) and damage dealers
//                (back). Tanks go on AI_MIN_COL, damage dealers on
//                the rightmost column.
// Input  : board
// Output : none
// -----------------------------------------------------------------
void AI::placeHard(Board& board) {
    std::vector<Unit*> tanks;
    std::vector<Unit*> damage;

    for (size_t i = 0; i < army_.size(); ++i) {
        if (army_[i]->getClass() == TANK || army_[i]->getClass() == WARRIOR) {
            tanks.push_back(army_[i]);
        } else {
            damage.push_back(army_[i]);
        }
    }

    // Place tanks in front column (AI_MIN_COL)
    for (size_t i = 0; i < tanks.size(); ++i) {
        bool placed = false;
        for (int r = 0; r < BOARD_ROWS && !placed; ++r) {
            if (board.isEmpty(r, AI_MIN_COL)) {
                board.placeUnit(tanks[i], r, AI_MIN_COL);
                placed = true;
            }
        }
        // Fallback: try next column
        if (!placed) {
            for (int r = 0; r < BOARD_ROWS && !placed; ++r) {
                if (board.isEmpty(r, AI_MIN_COL + 1)) {
                    board.placeUnit(tanks[i], r, AI_MIN_COL + 1);
                    placed = true;
                }
            }
        }
    }

    // Place damage dealers in back column (BOARD_COLS - 1)
    for (size_t i = 0; i < damage.size(); ++i) {
        bool placed = false;
        // Try back column first
        for (int r = 0; r < BOARD_ROWS && !placed; ++r) {
            if (board.isEmpty(r, BOARD_COLS - 1)) {
                board.placeUnit(damage[i], r, BOARD_COLS - 1);
                placed = true;
            }
        }
        // Fallback: try middle columns
        if (!placed) {
            for (int c = BOARD_COLS - 2; c >= AI_MIN_COL && !placed; --c) {
                for (int r = 0; r < BOARD_ROWS && !placed; ++r) {
                    if (board.isEmpty(r, c)) {
                        board.placeUnit(damage[i], r, c);
                        placed = true;
                    }
                }
            }
        }
    }
}

// -----------------------------------------------------------------
// clearArmy
// What it does : deletes all army units and empties the vector.
// Input  : none
// Output : none (frees dynamic memory)
// -----------------------------------------------------------------
void AI::clearArmy() {
    for (size_t i = 0; i < army_.size(); ++i) {
        if (army_[i] != nullptr) {
            delete army_[i];
            army_[i] = nullptr;
        }
    }
    army_.clear();
}

// -----------------------------------------------------------------
// releaseArmy
// What it does : clears the vector without freeing memory.
//                Ownership has been transferred elsewhere.
// Input  : none
// Output : none
// -----------------------------------------------------------------
void AI::releaseArmy() {
    army_.clear();
}

int AI::getArmySize() const {
    return (int)army_.size();
}

std::string AI::getDifficultyString() const {
    return (difficulty_ == EASY) ? "Easy" : "Hard";
}
