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

// -----------------------------------------------------------------
// Constructor
// What it does : initialises a BOARD_ROWS x BOARD_COLS grid of nullptrs.
// Input  : none
// Output : a Board ready for unit placement
// -----------------------------------------------------------------
Board::Board()
    : grid_(BOARD_ROWS, std::vector<Unit*>(BOARD_COLS, nullptr)) {}

// -----------------------------------------------------------------
// placeUnit
// What it does : puts a unit on the grid if the cell is valid & empty.
// Inputs : unit, row, col
// Output : true on success, false otherwise
// -----------------------------------------------------------------
bool Board::placeUnit(Unit* unit, int row, int col) {
    if (!isValidPosition(row, col)) return false;
    if (grid_[row][col] != nullptr) return false;
    grid_[row][col] = unit;
    unit->setPosition(row, col);
    return true;
}

// -----------------------------------------------------------------
// removeUnit
// What it does : clears the cell at (row, col).
// Inputs : row, col
// Output : none
// -----------------------------------------------------------------
void Board::removeUnit(int row, int col) {
    if (isValidPosition(row, col)) {
        grid_[row][col] = nullptr;
    }
}

// -----------------------------------------------------------------
// getUnit
// What it does : returns the unit pointer at (row, col).
// Inputs : row, col
// Output : Unit* or nullptr
// -----------------------------------------------------------------
Unit* Board::getUnit(int row, int col) const {
    if (!isValidPosition(row, col)) return nullptr;
    return grid_[row][col];
}

// -----------------------------------------------------------------
// isValidPosition
// What it does : bounds-checks (row, col).
// Inputs : row, col
// Output : true if within grid
// -----------------------------------------------------------------
bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < BOARD_ROWS && col >= 0 && col < BOARD_COLS;
}

// -----------------------------------------------------------------
// isEmpty
// What it does : checks whether a cell has no unit.
// Inputs : row, col
// Output : true if cell is empty or out of bounds
// -----------------------------------------------------------------
bool Board::isEmpty(int row, int col) const {
    if (!isValidPosition(row, col)) return true;
    return grid_[row][col] == nullptr;
}

// -----------------------------------------------------------------
// clear
// What it does : resets every cell to nullptr.
// Input  : none
// Output : none
// -----------------------------------------------------------------
void Board::clear() {
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c)
            grid_[r][c] = nullptr;
}

// -----------------------------------------------------------------
// display
// What it does : prints the grid to stdout with a dividing bar between
//                player (cols 0-3) and AI (cols 4-7) territory.
//                Uses the unit's symbol or '.' for empty cells.
// Input  : none
// Output : none (prints to stdout)
// -----------------------------------------------------------------
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

// -----------------------------------------------------------------
// displayPlayerSide
// What it does : prints only the player's half of the board (cols 0-3)
//                so the player can see their formation while deploying.
// Input  : none
// Output : none (prints to stdout)
// -----------------------------------------------------------------
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
// findNearestEnemy
// What it does : iterates over the grid to find the closest enemy unit
//                (by Manhattan distance). "Enemy" = different ownership.
// Input  : unit - the unit searching for a target
// Output : pointer to the nearest enemy, or nullptr if none exist
// -----------------------------------------------------------------
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

// -----------------------------------------------------------------
// moveToward
// What it does : moves a unit one cell closer to (targetRow, targetCol).
//                Prefers row movement; falls back to column movement
//                if row movement is blocked.
// Inputs : unit, targetRow, targetCol
// Output : none (modifies grid and unit position)
// -----------------------------------------------------------------
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

// -----------------------------------------------------------------
// getPlayerUnits / getAIUnits / getAllUnits
// What it does : collects unit pointers from the grid by ownership.
// Input  : none
// Output : vector of Unit*
// -----------------------------------------------------------------
std::vector<Unit*> Board::getPlayerUnits() const {
    std::vector<Unit*> units;
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c)
            if (grid_[r][c] != nullptr && grid_[r][c]->isPlayerUnit())
                units.push_back(grid_[r][c]);
    return units;
}

std::vector<Unit*> Board::getAIUnits() const {
    std::vector<Unit*> units;
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c)
            if (grid_[r][c] != nullptr && !grid_[r][c]->isPlayerUnit())
                units.push_back(grid_[r][c]);
    return units;
}

std::vector<Unit*> Board::getAllUnits() const {
    std::vector<Unit*> units;
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c)
            if (grid_[r][c] != nullptr)
                units.push_back(grid_[r][c]);
    return units;
}
