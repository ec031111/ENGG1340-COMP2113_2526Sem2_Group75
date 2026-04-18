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
// Process: 1. Clear previous army
//          2. Calculate round-based AI gold budget
//          3. Use EASY or HARD strategy to buy units
// Purpose: Generate opponent army for this combat round
// Input: round (int) - current game round affecting gold budget
// Output: None (populates army_ vector with purchased units)
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

// Core AI strategy
// Purpose: Implement EASY difficulty AI unit purchasing (cheapest-first strategy)
// Input: shop (Shop&) - temporary shop reference to purchase units from
// Output: None (populates army_ vector with purchased units)
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

// Core AI strategy
// Purpose: Implement HARD difficulty AI unit purchasing (score-optimized strategy)
// Input: shop (Shop&) - temporary shop reference to purchase units from
// Output: None (populates army_ vector with high-value units)
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
// Purpose: Dispatch to appropriate army placement strategy based on difficulty
// Input: board (Board&) - game board reference to place units on
// Output: None (places all units on board via difficulty-specific strategies)
// -----------------------------------------------------------------
void AI::placeUnits(Board& board) {
    if (difficulty_ == EASY) {
        placeEasy(board);
    } else {
        placeHard(board);
    }
}

// Deployment function
// Purpose: Deploy AI units in simple sequential formation (EASY difficulty)
// Input: board (Board&) - game board reference to place units on
// Output: None (places all army_ units on board)
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

// Deployment function
// Purpose: Deploy AI units in optimized strategic formation (HARD difficulty)
// Input: board (Board&) - game board reference to place units on
// Output: None (places army_ units in strategic positions by class)
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
// Input: None
// Output: None (clears and deallocates all units in army_)
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
// Input: None
// Output: None (clears army_ vector without deallocating units)
// -----------------------------------------------------------------
void AI::releaseArmy() {
    army_.clear();
}

// Getter function
// Purpose: Get the number of units currently in AI army
// Input: None
// Output: int - size of army_ vector
int AI::getArmySize() const {
    return (int)army_.size();
}

// What: Getter function
// Purpose: Get the string representation of AI difficulty level
// Input: None
// Output: std::string - "Easy" or "Hard" based on difficulty_
std::string AI::getDifficultyString() const {
    return (difficulty_ == EASY) ? "Easy" : "Hard";
}
