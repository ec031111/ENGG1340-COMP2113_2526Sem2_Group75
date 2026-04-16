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
    // -----------------------------------------------------------------
    // saveGame - save complete game state including board, shop, phases
    // Parameters: slot - save slot number (1, 2, or 3)
    // -----------------------------------------------------------------
    static void saveGame(const Player& player,
                        const Board& board,
                        const Shop& shop,
                        const AI& ai,
                        GamePhase currentPhase,
                        EventType currentEvent,
                        int slot = 1);

    // -----------------------------------------------------------------
    // loadGame - load game state and restore board/shop
    // Parameters: slot - save slot number (1, 2, or 3)
    // Returns: true if load successful
    // -----------------------------------------------------------------
    static bool loadGame(Player& player,
                         Board& board,
                         Shop& shop,
                         AI& ai,
                         GamePhase& currentPhase,
                         EventType& currentEvent,
                         bool& shouldResumeShopPhase,
                         int slot = 1);

    // -----------------------------------------------------------------
    // saveRecord - append game result to leaderboard
    // -----------------------------------------------------------------
    static void saveRecord(const Player& player, const AI& ai);

    // -----------------------------------------------------------------
    // displayLeaderboard - show all records sorted by rounds
    // -----------------------------------------------------------------
    static void displayLeaderboard();

    // -----------------------------------------------------------------
    // hasSaveFile - check if save file exists
    // Parameters: slot - save slot number (1, 2, or 3)
    // -----------------------------------------------------------------
    static bool hasSaveFile(int slot = 1);

    // -----------------------------------------------------------------
    // showSavePreview - Display save file preview without loading
    // Reads Round, HP, and Gold from save file and displays summary
    // Parameters: slot - save slot number (1, 2, or 3)
    // Returns: true if preview displayed successfully
    // -----------------------------------------------------------------
    static bool showSavePreview(int slot = 1);

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
