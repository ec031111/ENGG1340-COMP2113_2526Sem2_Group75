/*
=======================================================================================
 Project: Auto-Battler Arena
 Group: ENGG1340-COMP2113_2526Sem2_Group75
 File Type: Header File
 File Name: record.h
 Description: Record class declaration for managing game record data, leaderboard,
              game saving/loading functionality, and player statistics tracking.
=======================================================================================
 */

#pragma once
#ifndef RECORD_H
#define RECORD_H

#include "game.h"
#include "player.h"
#include "board.h"
#include "shop.h"
#include "ai.h"
#include <string>
#include <vector>

// File paths
const std::string SAVE_FILE_1 = "docs/save1.dat";
const std::string SAVE_FILE_2 = "docs/save2.dat";
const std::string SAVE_FILE_3 = "docs/save3.dat";
const std::string RECORD_FILE = "docs/records.txt";

// Helper function to get save file path based on slot (1-3)
inline std::string getSaveFilePath(int slot) {
    if (slot == 1) return SAVE_FILE_1;
    if (slot == 2) return SAVE_FILE_2;
    if (slot == 3) return SAVE_FILE_3;
    return SAVE_FILE_1; // default to slot 1
}

// =====================================================================
// Record - Handles all save/load/record operations
// =====================================================================
class Record {
public:
    // Purpose: Save complete game state to file
    // Input: player, board, shop, ai, currentPhase, currentEvent (references), slot (int)
    // Output: none
    static void saveGame(const Player& player,
                        const Board& board,
                        const Shop& shop,
                        const AI& ai,
                        GamePhase currentPhase,
                        EventType currentEvent,
                        int slot = 1);

    // Purpose: Load game state from file and restore board/shop
    // Input: player, board, shop, ai, currentPhase, currentEvent, shouldResumeShopPhase (references), slot (int)
    // Output: bool - true if load successful
    static bool loadGame(Player& player,
                         Board& board,
                         Shop& shop,
                         AI& ai,
                         GamePhase& currentPhase,
                         EventType& currentEvent,
                         bool& shouldResumeShopPhase,
                         int slot = 1);

    // Purpose: Append game result to leaderboard file
    // Input: player, ai (const references)
    // Output: none
    static void saveRecord(const Player& player, const AI& ai);

    // Purpose: Display all records sorted by rounds played
    // Input: none
    // Output: none (prints to stdout)
    static void displayLeaderboard();

    // Purpose: Check if save file exists in specified slot
    // Input: slot (int) - save slot number 1-3
    // Output: bool - true if save file exists
    static bool hasSaveFile(int slot = 1);

    // Purpose: Display save file preview without loading
    // Input: slot (int) - save slot number 1-3
    // Output: bool - true if preview displayed successfully
    static bool showSavePreview(int slot = 1);

    // Purpose: Display status of all 3 save slots
    // Input: none
    // Output: none (prints to stdout)
    static void displayAllSlots();

private:
    // Helper structures for serialization
    struct SavedUnit {
        std::string name;
        int cls;
        int maxHp;
        int atk;
        int cost;
        int critBonus;
        int range;
        int starLevel;
    };

    struct BoardUnitData {
        int row, col;
        SavedUnit unit;
    };
};

#endif // RECORD_H
