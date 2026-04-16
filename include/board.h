/*
================================================================================
Project:        Auto-Battler Arena (ENGG1340-COMP2113, 2024-2025 Semester 2)
Group:          Group 75
File Type:      Header File
File Name:      board.h
Description:    Manages the 5x8 battle grid. Handles unit placement, removal,
                rendering, and spatial queries. The board stores Unit pointers
                but does not own them; memory management is handled by Player
                and Game classes.
================================================================================
*/

#ifndef BOARD_H
#define BOARD_H

#include "unit.h"
#include <vector>
#include <string>

// Board dimensions
const int BOARD_ROWS = 5;
const int BOARD_COLS = 8;

// The dividing column: columns 0-3 belong to the player,
// columns 4-7 belong to the AI.
const int PLAYER_MAX_COL = 3;
const int AI_MIN_COL = 4;

// ---------------------------------------------------------------------
// Board
// ---------------------------------------------------------------------
// Manages the 5x8 battle grid using a 2D vector of Unit pointers.
// Handles placement, removal, rendering, and spatial queries.
// The board does NOT own the Unit objects — it only stores pointers.
// Memory management of Units is handled by Player / Game.
// ---------------------------------------------------------------------
class Board {
public:
    // -----------------------------------------------------------------
    // Constructor
    // Purpose: Creates an empty BOARD_ROWS x BOARD_COLS grid with all cells set to nullptr
    // Input: none
    // Output: Initialised Board object
    // -----------------------------------------------------------------
    Board();

    // -----------------------------------------------------------------
    // placeUnit
    // Purpose: Places a unit pointer at (row, col) and updates unit's position
    // Input: unit (Unit* pointer), row, col (target coordinates)
    // Output: true if succeeded, false if occupied or out of bounds
    // -----------------------------------------------------------------
    bool placeUnit(Unit* unit, int row, int col);

    // -----------------------------------------------------------------
    // removeUnit
    // Purpose: Remove a unit from grid by setting cell to nullptr (does NOT delete unit)
    // Input: row, col (cell to clear)
    // Output: none (modifies grid state)
    // -----------------------------------------------------------------
    void removeUnit(int row, int col);

    // -----------------------------------------------------------------
    // getUnit
    // Purpose: Retrieve the Unit pointer at the given cell
    // Input: row, col (cell coordinates)
    // Output: Unit* pointer or nullptr if empty or out of bounds
    // -----------------------------------------------------------------
    Unit* getUnit(int row, int col) const;

    // -----------------------------------------------------------------
    // isValidPosition
    // Purpose: Check if (row, col) is within grid bounds
    // Input: row, col (cell coordinates)
    // Output: true if valid coordinates, false otherwise
    // -----------------------------------------------------------------
    bool isValidPosition(int row, int col) const;

    // -----------------------------------------------------------------
    // isEmpty
    // Purpose: Check if a cell is empty (nullptr)
    // Input: row, col (cell coordinates)
    // Output: true if empty or out of bounds, false otherwise
    // -----------------------------------------------------------------
    bool isEmpty(int row, int col) const;

    // -----------------------------------------------------------------
    // clear
    // Purpose: Set every cell to nullptr (does NOT delete units)
    // Input: none
    // Output: none (modifies grid state)
    // -----------------------------------------------------------------
    void clear();

    // -----------------------------------------------------------------
    // display
    // Purpose: Print the board as ASCII grid with player units on left, AI on right
    // Input: none
    // Output: none (prints visual representation to stdout)
    // -----------------------------------------------------------------
    void display() const;

    // -----------------------------------------------------------------
    // displayPlayerSide
    // Purpose: Print only player's half (cols 0-3) for formation/deploy phase
    // Input: none
    // Output: none (prints formation visualization to stdout)
    // -----------------------------------------------------------------
    void displayPlayerSide() const;

    // -----------------------------------------------------------------
    // findNearestEnemy
    // Purpose: Scan grid for closest enemy unit using Manhattan distance
    // Input: unit (Unit* looking for a target)
    // Output: Unit* pointer to nearest enemy, or nullptr if none found
    // -----------------------------------------------------------------
    Unit* findNearestEnemy(const Unit* unit) const;

    // -----------------------------------------------------------------
    // moveToward
    // Purpose: Move a unit one step closer to target position (tries row first, then col)
    // Input: unit (Unit* to move), targetRow, targetCol (destination)
    // Output: none (modifies grid and unit's internal position)
    // -----------------------------------------------------------------
    void moveToward(Unit* unit, int targetRow, int targetCol);

    // -----------------------------------------------------------------
    // getAllUnits
    // What it does : collects all non-null unit pointers on the board.
    // Input  : playerOnly – if true, only player units; if false, only
    //          AI units. If not specified, returns all units.
    // Output : vector of Unit pointers
    // -----------------------------------------------------------------
    std::vector<Unit*> getPlayerUnits() const;
    std::vector<Unit*> getAIUnits() const;
    std::vector<Unit*> getAllUnits() const;

private:
    // 2D vector grid — the core data structure for the battlefield.
    std::vector<std::vector<Unit*>> grid_;
};

#endif // BOARD_H
