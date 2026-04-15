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
const std::string SAVE_FILE = "docs/savegame.dat";
const std::string RECORD_FILE = "docs/records.txt";

// =====================================================================
// Record - Handles all save/load/record operations
// =====================================================================
class Record {
public:
    // -----------------------------------------------------------------
    // saveGame - save complete game state including board, shop, phases
    // -----------------------------------------------------------------
    static void saveGame(const Player& player,
                        const Board& board,
                        const Shop& shop,
                        const AI& ai,
                        GamePhase currentPhase,
                        EventType currentEvent);

    // -----------------------------------------------------------------
    // loadGame - load game state and restore board/shop
    // Returns: true if load successful
    // -----------------------------------------------------------------
    static bool loadGame(Player& player,
                         Board& board,
                         Shop& shop,
                         AI& ai,
                         GamePhase& currentPhase,
                         EventType& currentEvent,
                         bool& shouldResumeShopPhase);

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
    // -----------------------------------------------------------------
    static bool hasSaveFile();

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
