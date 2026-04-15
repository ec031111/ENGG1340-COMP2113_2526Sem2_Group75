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
    // What it does : creates an empty BOARD_ROWS x BOARD_COLS grid
    //                with all cells set to nullptr.
    // Input  : none
    // Output : an initialised Board object
    // -----------------------------------------------------------------
    Board();

    // -----------------------------------------------------------------
    // placeUnit
    // What it does : places a unit pointer at (row, col) on the grid
    //                and updates the unit's internal position.
    // Inputs : unit – pointer to the Unit to place
    //          row, col – target cell coordinates
    // Output : true if placement succeeded, false if cell is occupied
    //          or coordinates are out of bounds.
    // -----------------------------------------------------------------
    bool placeUnit(Unit* unit, int row, int col);

    // -----------------------------------------------------------------
    // removeUnit
    // What it does : removes a unit from the grid (sets cell to nullptr).
    //                Does NOT delete the unit.
    // Inputs : row, col – cell to clear
    // Output : none
    // -----------------------------------------------------------------
    void removeUnit(int row, int col);

    // -----------------------------------------------------------------
    // getUnit
    // What it does : returns the Unit pointer at the given cell.
    // Inputs : row, col – cell coordinates
    // Output : Unit* or nullptr if empty / out of bounds
    // -----------------------------------------------------------------
    Unit* getUnit(int row, int col) const;

    // -----------------------------------------------------------------
    // isValidPosition
    // What it does : checks if (row, col) is within grid bounds.
    // Inputs : row, col
    // Output : true if valid
    // -----------------------------------------------------------------
    bool isValidPosition(int row, int col) const;

    // -----------------------------------------------------------------
    // isEmpty
    // What it does : checks if a cell is empty (nullptr).
    // Inputs : row, col
    // Output : true if empty or out of bounds
    // -----------------------------------------------------------------
    bool isEmpty(int row, int col) const;

    // -----------------------------------------------------------------
    // clear
    // What it does : sets every cell to nullptr (does NOT delete units).
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void clear();

    // -----------------------------------------------------------------
    // display
    // What it does : prints the board as an ASCII grid to stdout.
    //                Player units on the left, AI units on the right,
    //                separated by a vertical bar.
    // Input  : none
    // Output : none (prints to stdout)
    // -----------------------------------------------------------------
    void display() const;

    // -----------------------------------------------------------------
    // displayPlayerSide
    // What it does : prints only the player's half (cols 0-3) for
    //                the formation/deploy phase.
    // Input  : none
    // Output : none (prints to stdout)
    // -----------------------------------------------------------------
    void displayPlayerSide() const;

    // -----------------------------------------------------------------
    // findNearestEnemy
    // What it does : scans the grid for the closest enemy unit
    //                (by Manhattan distance) to the given unit.
    // Input  : unit – the unit looking for a target
    // Output : pointer to the nearest enemy, or nullptr if none found
    // -----------------------------------------------------------------
    Unit* findNearestEnemy(const Unit* unit) const;

    // -----------------------------------------------------------------
    // moveToward
    // What it does : moves a unit one step closer to a target position
    //                on the grid. Tries row movement first, then column.
    // Inputs : unit – the unit to move
    //          targetRow, targetCol – destination coordinates
    // Output : none (modifies grid and unit position)
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
