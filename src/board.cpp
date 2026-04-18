//================================================================================
//Project:        Auto-Battler Arena (ENGG1340-COMP2113, 2024-2025 Semester 2)
//Group:          Group 75
//File Type:      Source File
//File Name:      board.cpp
//Description:    Implementation of the Board class. Provides functionality for
//                managing unit placement on a 5x8 grid, checking valid positions,
//                rendering the board state, and performing spatial queries to
//                retrieve units by position or ownership (player/AI).
//================================================================================


#include "board.h"
#include <iostream>
#include <iomanip>
#include <climits>
#include <string>
#include <utility>

static const std::string ANSI_RESET = "\033[0m";
static const std::string ANSI_RED = "\033[31m";
static const std::string ANSI_BLUE = "\033[34m";
static const std::string ANSI_GREEN = "\033[32m";
static const std::string ANSI_MAGENTA = "\033[35m";
static const std::string ANSI_YELLOW = "\033[33m";
static const std::string ANSI_CYAN = "\033[36m";
static const std::string ANSI_BOLD = "\033[1m";

static std::pair<std::string, std::string> getClassColorEmoji(UnitClass cls) {
    switch (cls) {
        case WARRIOR: return {ANSI_RED, "⚔️"};
        case MAGE: return {ANSI_BLUE, "🔮"};
        case TANK: return {ANSI_GREEN, "🛡️"};
        case ASSASSIN: return {ANSI_MAGENTA, "🗡️"};
        case ARCHER: return {ANSI_YELLOW, "🏹"};
        default: return {ANSI_CYAN, "•"};
    }
}

// What: Constructor
// Purpose: Initialize a BOARD_ROWS x BOARD_COLS grid of nullptrs
// Input: None
// Output: A Board ready for unit placement
Board::Board()
    : grid_(BOARD_ROWS, std::vector<Unit*>(BOARD_COLS, nullptr)) {}

// What: Core placement
// Purpose: Place a unit on the grid if the cell is valid and empty
// Input: unit (Unit*), row (int), col (int)
// Output: true if successful, false otherwise
bool Board::placeUnit(Unit* unit, int row, int col) {
    if (!isValidPosition(row, col)) return false;
    if (grid_[row][col] != nullptr) return false;
    grid_[row][col] = unit;
    unit->setPosition(row, col);
    return true;
}

// What: Removal operation
// Purpose: Clear the cell at (row, col) by removing the unit
// Input: row (int), col (int)
// Output: None
void Board::removeUnit(int row, int col) {
    if (isValidPosition(row, col)) {
        grid_[row][col] = nullptr;
    }
}

// What: Getter
// Purpose: Retrieve the unit pointer at the specified grid position
// Input: row (int), col (int)
// Output: Unit pointer if present, nullptr otherwise
Unit* Board::getUnit(int row, int col) const {
    if (!isValidPosition(row, col)) return nullptr;
    return grid_[row][col];
}

// What: Validation
// Purpose: Verify if the given position is within valid grid bounds
// Input: row (int), col (int)
// Output: true if within bounds, false otherwise
bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < BOARD_ROWS && col >= 0 && col < BOARD_COLS;
}

// What: Validation
// Purpose: Check if a cell is empty (no unit present)
// Input: row (int), col (int)
// Output: true if cell is empty or out of bounds, false if occupied
bool Board::isEmpty(int row, int col) const {
    if (!isValidPosition(row, col)) return true;
    return grid_[row][col] == nullptr;
}

// What: Reset
// Purpose: Reset all grid cells to empty state
// Input: None
// Output: None
void Board::clear() {
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c)
            grid_[r][c] = nullptr;
}

// What: Display
// Purpose: Display the full game board with player and AI armies
// Input: None
// Output: None (prints to stdout with ANSI colors and dividing bar)
void Board::display() const {
    std::cout << std::endl;
    std::cout << ANSI_BOLD << ANSI_CYAN << "  +---------+---------+" << ANSI_RESET << std::endl;
    std::cout << "  |" << ANSI_BOLD << ANSI_CYAN << " YOUR ARMY" << ANSI_RESET
              << " |" << ANSI_BOLD << ANSI_RED << " ENEMY ARMY" << ANSI_RESET << "|" << std::endl;
    std::cout << "  | 0 1 2 3 | 4 5 6 7 |" << std::endl;
    std::cout << ANSI_BOLD << ANSI_CYAN << "  +---------+---------+" << ANSI_RESET << std::endl;

    for (int r = 0; r < BOARD_ROWS; ++r) {
        std::cout << " " << r << "|";
        for (int c = 0; c < BOARD_COLS; ++c) {
            if (c == AI_MIN_COL) std::cout << "|";
            std::cout << " ";
            if (grid_[r][c] != nullptr) {
                auto pair_ = getClassColorEmoji(grid_[r][c]->getClass());
                std::cout << pair_.first << pair_.second << ANSI_RESET;
            } else {
                std::cout << ". ";
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << ANSI_BOLD << ANSI_CYAN << "  +---------+---------+" << ANSI_RESET << std::endl;
    std::cout << std::endl;
}

// What: Display
// Purpose: Display only the player's side of the board during deployment
// Input: None
// Output: None (prints player formation to stdout)
void Board::displayPlayerSide() const {
    std::cout << std::endl;
    std::cout << ANSI_BOLD << ANSI_CYAN << "  +---------------+" << ANSI_RESET << std::endl;
    std::cout << "  |" << ANSI_BOLD << ANSI_CYAN << " YOUR FORMATION " << ANSI_RESET << "|" << std::endl;
    std::cout << "  |    0 1 2 3     |" << std::endl;
    std::cout << ANSI_BOLD << ANSI_CYAN << "  +---------------+" << ANSI_RESET << std::endl;

    for (int r = 0; r < BOARD_ROWS; ++r) {
        std::cout << " " << r << " |";
        for (int c = 0; c <= PLAYER_MAX_COL; ++c) {
            std::cout << " ";
            if (grid_[r][c] != nullptr) {
                auto pair_ = getClassColorEmoji(grid_[r][c]->getClass());
                std::cout << pair_.first << pair_.second << ANSI_RESET;
            } else {
                std::cout << ". ";
            }
        }
        std::cout << " |" << std::endl;
    }
    std::cout << ANSI_BOLD << ANSI_CYAN << "  +---------------+" << ANSI_RESET << std::endl;
    std::cout << std::endl;
}

// -----------------------------------------------------------------
// What: Core combat
// Purpose: Find the closest enemy unit using Manhattan distance
// Input: unit (const Unit*) - unit searching for a target
// Output: Pointer to nearest living enemy, or nullptr if none exist
Unit* Board::findNearestEnemy(const Unit* unit) const {
    Unit* nearest = nullptr;
    int minDist = INT_MAX;

    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            Unit* candidate = grid_[r][c];
            if (candidate == nullptr) continue;
            if (candidate->isPlayerUnit() == unit->isPlayerUnit()) continue;
            if (!candidate->isAlive()) continue;

            int dist = unit->getDistanceTo(candidate);
            if (dist < minDist) {
                minDist = dist;
                nearest = candidate;
            }
        }
    }
    return nearest;
}

// What: Core movement
// Purpose: Move a unit one cell closer to target, preferring row then column
// Input: unit (Unit*), targetRow (int), targetCol (int)
// Output: None (modifies grid state and unit position)
void Board::moveToward(Unit* unit, int targetRow, int targetCol) {
    int curRow = unit->getRow();
    int curCol = unit->getCol();

    int newRow = curRow;
    int newCol = curCol;

    // Try row movement first
    if (targetRow > curRow)      newRow = curRow + 1;
    else if (targetRow < curRow) newRow = curRow - 1;

    if (newRow != curRow && isEmpty(newRow, curCol)) {
        removeUnit(curRow, curCol);
        placeUnit(unit, newRow, curCol);
        return;
    }

    // Try column movement
    if (targetCol > curCol)      newCol = curCol + 1;
    else if (targetCol < curCol) newCol = curCol - 1;

    if (newCol != curCol && isEmpty(curRow, newCol)) {
        removeUnit(curRow, curCol);
        placeUnit(unit, curRow, newCol);
        return;
    }

    // Try diagonal if both straight moves are blocked
    if (newRow != curRow && newCol != curCol && isEmpty(newRow, newCol)) {
        removeUnit(curRow, curCol);
        placeUnit(unit, newRow, newCol);
    }
}

// What: Getter
// Purpose: Retrieve all player-owned units currently on the board
// Input: None
// Output: Vector of Unit pointers owned by the player
std::vector<Unit*> Board::getPlayerUnits() const {
    std::vector<Unit*> units;
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c)
            if (grid_[r][c] != nullptr && grid_[r][c]->isPlayerUnit())
                units.push_back(grid_[r][c]);
    return units;
}

// What: Getter
// Purpose: Retrieve all AI-owned units currently on the board
// Input: None
// Output: Vector of Unit pointers owned by the AI
std::vector<Unit*> Board::getAIUnits() const {
    std::vector<Unit*> units;
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c)
            if (grid_[r][c] != nullptr && !grid_[r][c]->isPlayerUnit())
                units.push_back(grid_[r][c]);
    return units;
}

// What: Getter
// Purpose: Retrieve all units currently on the board
// Input: None
// Output: Vector of all Unit pointers on the board
std::vector<Unit*> Board::getAllUnits() const {
    std::vector<Unit*> units;
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c)
            if (grid_[r][c] != nullptr)
                units.push_back(grid_[r][c]);
    return units;
}
