// =====================================================================
// Project: AUTO-BATTLER ARENA
// Group: Group 75
// File Type: Source File
// File Name: ai.cpp
// Description:
//   This file implements the AI class that controls computer opponents.
//   Contains methods for generating AI armies with difficulty-specific strategies,
//   buying units based on difficulty levels, placing units tactically,
//   and managing the AI's unit roster and resources.
// =====================================================================

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
// generateArmy - Build AI army for current round based on difficulty
// Description: Calculates AI gold budget (scales with round),
//              creates temporary shop, and uses difficulty-specific
//              buying strategy to purchase units for this round.
// Parameters: round - Current game round (affects AI gold budget)
// Process: 1. Clear previous army
//          2. Calculate round-based AI gold budget
//          3. Use EASY or HARD strategy to buy units
// Purpose: Generate opponent army for this combat round
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
// buyEasyStrategy - Purchase units using simple cheapest-first approach
// Description: Repeatedly refreshes shop and buys cheapest affordable
//              units until gold runs out or max army size reached.
//              Simple strategy = easier difficulty for player.
// Parameters: shop - Temporary shop to purchase from
// Process: 1. Refresh shop up to 5 times
//          2. Find cheapest affordable unit each iteration
//          3. Buy and add to army_ until gold runs out
// Purpose: Implement EASY difficulty AI purchasing behavior
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
// buyHardStrategy - Purchase units using score-based optimization
// Description: Evaluates each unit with formula: ATK*2 + HP/5.
//              Buys highest-scoring affordable units each iteration.
//              Smarter strategy = harder difficulty for player.
// Parameters: shop - Temporary shop to purchase from
// Process: 1. Refresh shop up to 5 times
//          2. Score all affordable units (attack priority)
//          3. Buy highest-scoring unit each iteration
// Purpose: Implement HARD difficulty AI purchasing behavior (optimized team)
// -----------------------------------------------------------------

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
// placeUnits - Position AI army on board according to difficulty strategy
// Description: Delegates to difficulty-specific placement logic.
//              Determines optimal formation for combat.
// Parameters: board - Game board to place units on
// Purpose: Dispatch to appropriate army placement strategy based on difficulty
// -----------------------------------------------------------------
void AI::placeUnits(Board& board) {
    if (difficulty_ == EASY) {
        placeEasy(board);
    } else {
        placeHard(board);
    }
}

// -----------------------------------------------------------------
// placeEasy - Place units in front column sequentially (simple formation)
// Description: Places all AI units in front column (AI_MIN_COL),
//              filling rows left-to-right. Spills to next column if full.
//              No strategic positioning = easier for player.
// Parameters: board - Game board to place units on
// Process: Iterate through army_, place each unit in order
// Purpose: Implement EASY difficulty unit placement (predictable)
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
// placeHard - Place units in optimized formation (tanks front, damage back)
// Description: Separates units by class: tanks/warriors in front,
//              ranged/mages in back. Maximizes defensive and offensive
//              positioning for harder challenge.
// Parameters: board - Game board to place units on
// Process: 1. Split army into tanks and damage dealers
//          2. Place tanks in front (AI_MIN_COL)
//          3. Place damage in back (right columns)
// Purpose: Implement HARD difficulty unit placement (strategic formation)
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
// clearArmy - Delete all units in AI army and reset inventory
// Description: Safely deletes each Unit pointer and clears army_ vector.
//              Prevents memory leaks before generating new army.
// Purpose: Clean up AI army before next round generation
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
// releaseArmy - Clear AI army vector without deleting units (ownership transferred)
// Description: Clears army_ vector without freeing memory.
//              Used when units have been transferred to board/combat.
// Purpose: Reset army list after units placed on board
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
