//=============================================================================================
//Project: Auto-Battler Arena
//Group: ENGG1340-COMP2113_2526Sem2_Group75
//File Type: Source File
//File Name: record.cpp
//Description: Record class implementation. Handles game record management, leaderboard
//            display, game state serialization/deserialization, and player record tracking.
//==============================================================================================

 

#include "record.h"
#include "game.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

// Color codes (matching game.h)
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define BOLD        "\033[1m"
#define BR_RED      "\033[91m"
#define BR_YELLOW   "\033[93m"
#define BR_GREEN    "\033[92m"
#define BR_CYAN     "\033[96m"

// Helper: print a line padded to width W inside |...|
static void printBoxLine(const std::string& text, int W) {
    std::string s = text;
    if ((int)s.size() < W) s += std::string(W - s.size(), ' ');
    std::cout << "  |" << s << "|" << std::endl;
}

// Helper: print centered title
static void printBoxTitle(const std::string& title, int W) {
    int pad = (W - (int)title.size()) / 2;
    std::cout << "  |" << std::string(pad, ' ') << title
              << std::string(W - pad - (int)title.size(), ' ') << "|" << std::endl;
}

// Rcord::saveGame - serialize game state to file
// Purpose: Save complete game state to file for later resumption
// Input: player, board, shop, ai, currentPhase, currentEvent, slot
// Output: None (writes to save file)
void Record::saveGame(const Player& player,
                      const Board& board,
                      const Shop& /* shop */,
                      const AI& ai,
                      GamePhase currentPhase,
                      EventType currentEvent,
                      int aiHp,
                      int slot) {
    std::ofstream file(getSaveFilePath(slot));
    if (!file.is_open()) {
        std::cerr << "  Could not save game to slot " << slot << "." << std::endl;
        return;
    }
    
    // Save game phase (0=ROUND_START, 1=SHOP, 2=BATTLE)
    file << (int)currentPhase << std::endl;
    
    // Save player state
    file << player.getHp() << " " << player.getGold() << " "
         << player.getRoundsPlayed() << " " << player.getWinStreak() << " "
         << player.getLossStreak() << " " << player.getWinCount() << " "
         << player.getLossCount() << std::endl;

    // Save bench units
    const std::vector<Unit*>& bench = player.getBench();
    file << bench.size() << std::endl;
    for (size_t i = 0; i < bench.size(); ++i) {
        file << bench[i]->getBaseName() << " "
             << bench[i]->getClass() << " "
             << bench[i]->getMaxHp() << " "
             << bench[i]->getAtk() << " "
             << bench[i]->getCost() << " "
             << bench[i]->getCritBonus() << " "
             << bench[i]->getAttackRange() << " "
             << bench[i]->getStarLevel() << std::endl;
    }

    // Save board units (player side: rows 0-4, cols 0-3)
    int boardUnitCount = 0;
    std::vector<std::tuple<int, int, Unit*>> boardUnits;
    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c <= PLAYER_MAX_COL; ++c) {
            Unit* u = board.getUnit(r, c);
            if (u != nullptr) {
                boardUnits.push_back(std::make_tuple(r, c, u));
                boardUnitCount++;
            }
        }
    }
    file << boardUnitCount << std::endl;
    for (size_t i = 0; i < boardUnits.size(); ++i) {
        int r = std::get<0>(boardUnits[i]);
        int c = std::get<1>(boardUnits[i]);
        Unit* u = std::get<2>(boardUnits[i]);
        file << r << " " << c << " "
             << u->getBaseName() << " "
             << u->getClass() << " "
             << u->getMaxHp() << " "
             << u->getAtk() << " "
             << u->getCost() << " "
             << u->getCritBonus() << " "
             << u->getAttackRange() << " "
             << u->getStarLevel() << std::endl;
    }

    // Save difficulty
    file << ai.getDifficultyString() << std::endl;
    
    // Save current event
    file << (int)currentEvent << std::endl;

    // Save AI HP
    file << aiHp << std::endl;

    file.close();
    std::cout << GREEN << "  ✅ Game saved!" << RESET << std::endl;
}

// =====================================================================
// Record::loadGame - restore game state from save file
// =====================================================================
// Purpose: Load complete game state from save file
// Input: player, board, shop, ai, currentPhase, currentEvent, shouldResumeShop, slot
// Output: true if successful, false if file corrupted or invalid
bool Record::loadGame(Player& player,
                      Board& board,
                      Shop& /* shop */,
                      GamePhase& currentPhase,
                      EventType& currentEvent,
                      bool& shouldResumeShopPhase,
                      Difficulty& loadedDifficulty,
                      int& loadedAiHp,
                      int slot) {
    // Check if save file exists before attempting to load
    std::string saveFilePath = getSaveFilePath(slot);
    std::ifstream checkFile(saveFilePath);
    if (!checkFile.is_open()) {
        std::cout << "\n  " << BOLD << BR_RED << "ERROR" << RESET << ": Save file not found in slot " << slot << std::endl;
        std::cout << "  Please check that a previous game was saved to this slot." << std::endl;
        std::cout << "  Returning to main menu..." << std::endl;
        return false;
    }
    checkFile.close();

    // Now open the file for actual loading with error handling
    std::ifstream file(saveFilePath);
    if (!file.is_open()) {
        std::cout << "\n  " << BOLD << BR_RED << "ERROR" << RESET << ": Unable to open save file for reading." << std::endl;
        return false;
    }

    // Declare here (outside try) so catch block can clean up on parse failure
    std::vector<Unit*> loadedUnits;
    std::vector<std::tuple<int, int, Unit*>> boardUnits;

    try {
        // Read game phase
        int phaseInt;
        if (!(file >> phaseInt)) {
            throw std::runtime_error("Failed to read game phase");
        }
        if (phaseInt < 0 || phaseInt > 3) {
            throw std::runtime_error("Invalid game phase value");
        }
        currentPhase = (GamePhase)phaseInt;
        shouldResumeShopPhase = (currentPhase == PHASE_SHOP);

        // Read player state
        int hp, gold, rounds, winStreak, lossStreak, winCount = 0, lossCount = 0;
        if (!(file >> hp >> gold >> rounds >> winStreak >> lossStreak)) {
            throw std::runtime_error("Failed to read player state");
        }
        // Try reading win/loss counts (backward compat: default to 0 for old saves)
        file >> winCount >> lossCount;
        if (file.fail()) {
            winCount = 0;
            lossCount = 0;
            file.clear();
        }
        // Validate player state values
        if (hp < 0 || gold < 0 || rounds < 0 || winStreak < 0 || lossStreak < 0
            || winCount < 0 || lossCount < 0) {
            throw std::runtime_error("Invalid player state values");
        }

        // Read bench units
        int benchCount;
        if (!(file >> benchCount)) {
            throw std::runtime_error("Failed to read bench unit count");
        }
        if (benchCount < 0 || benchCount > MAX_BENCH_SIZE) {
            throw std::runtime_error("Invalid bench unit count");
        }

        for (int i = 0; i < benchCount; ++i) {
            std::string name;
            int cls, maxHp, atk, cost, critBonus, range, starLevel;
            if (!(file >> name >> cls >> maxHp >> atk >> cost >> critBonus >> range >> starLevel)) {
                throw std::runtime_error("Failed to read bench unit data");
            }
            
            // Validate unit data
            if (cls < 0 || cls >= NUM_CLASSES || maxHp <= 0 || atk < 0 || 
                cost < 0 || range < 0 || starLevel < 1 || starLevel > MAX_STAR_LEVEL) {
                throw std::runtime_error("Invalid unit data values");
            }
            
            // Create unit with saved post-upgrade stats directly.
            Unit* u = new Unit(name, (UnitClass)cls, maxHp, atk, cost, critBonus, range);
            // Manually set star level without re-scaling stats
            for (int s = 1; s < starLevel; ++s) {
                u->forceSetStarLevel(s + 1);
            }
            loadedUnits.push_back(u);
        }

        // Read board units
        int boardUnitCount;
        if (!(file >> boardUnitCount)) {
            throw std::runtime_error("Failed to read board unit count");
        }
        if (boardUnitCount < 0 || boardUnitCount > (BOARD_ROWS * BOARD_COLS)) {
            throw std::runtime_error("Invalid board unit count");
        }

        // Store board units temporarily (will apply after player state)
        for (int i = 0; i < boardUnitCount; ++i) {
            int r, c;
            std::string name;
            int cls, maxHp, atk, cost, critBonus, range, starLevel;
            if (!(file >> r >> c >> name >> cls >> maxHp >> atk >> cost >> critBonus >> range >> starLevel)) {
                throw std::runtime_error("Failed to read board unit data");
            }
            
            // Validate board position
            if (r < 0 || r >= BOARD_ROWS || c < 0 || c >= BOARD_COLS) {
                throw std::runtime_error("Invalid board unit position");
            }
            
            // Validate unit data
            if (cls < 0 || cls >= NUM_CLASSES || maxHp <= 0 || atk < 0 || 
                cost < 0 || range < 0 || starLevel < 1 || starLevel > MAX_STAR_LEVEL) {
                throw std::runtime_error("Invalid unit data values");
            }
            
            Unit* u = new Unit(name, (UnitClass)cls, maxHp, atk, cost, critBonus, range);
            for (int s = 1; s < starLevel; ++s) {
                u->forceSetStarLevel(s + 1);
            }
            boardUnits.push_back(std::make_tuple(r, c, u));
        }

        // Read difficulty
        std::string diff;
        if (!(file >> diff)) {
            // Difficulty is optional, continue
        }
        if (diff == "Hard" || diff == "HARD") {
            loadedDifficulty = HARD;
        } else {
            loadedDifficulty = EASY;  // default
        }
        
        // Read current event
        int eventInt;
        if (!(file >> eventInt)) {
            eventInt = EVENT_NONE;
        }
        if (eventInt < 0 || eventInt > 100) {
            eventInt = EVENT_NONE;
        }
        currentEvent = (EventType)eventInt;

        // Read AI HP (backward compat: default to 100 for old saves without this field)
        int aiHpRead;
        if (!(file >> aiHpRead)) {
            aiHpRead = 100;  // Old save without AI HP
        }
        if (aiHpRead < 0 || aiHpRead > 1000) {
            aiHpRead = 100;  // Invalid value
        }
        loadedAiHp = aiHpRead;

        file.close();

        // Apply loaded state to player
        player.loadState(hp, gold, rounds, winStreak, lossStreak, winCount, lossCount);

        // Add loaded units to bench
        for (size_t i = 0; i < loadedUnits.size(); ++i) {
            if (!player.addToBench(loadedUnits[i])) {
                delete loadedUnits[i];  // bench full, shouldn't happen
            }
        }

        // Place board units on the board
        for (size_t i = 0; i < boardUnits.size(); ++i) {
            int r = std::get<0>(boardUnits[i]);
            int c = std::get<1>(boardUnits[i]);
            Unit* u = std::get<2>(boardUnits[i]);
            board.placeUnit(u, r, c);
        }
        
        std::cout << "\n  " << BOLD << BR_GREEN << "✅ Save file loaded successfully!" << RESET << std::endl;
        return true;

    } catch (const std::exception& e) {
        file.close();
        // Clean up any partially loaded units to prevent memory leak
        for (size_t i = 0; i < loadedUnits.size(); ++i) delete loadedUnits[i];
        for (size_t i = 0; i < boardUnits.size(); ++i) delete std::get<2>(boardUnits[i]);

        std::cout << "\n  " << BOLD << BR_RED << "Save file corrupted" << RESET << std::endl;
        std::cout << "  Details: " << e.what() << std::endl;
        std::cout << "  This save file cannot be loaded. Please try another slot." << std::endl;

        return false;
    } catch (...) {
        file.close();
        // Clean up any partially loaded units to prevent memory leak
        for (size_t i = 0; i < loadedUnits.size(); ++i) delete loadedUnits[i];
        for (size_t i = 0; i < boardUnits.size(); ++i) delete std::get<2>(boardUnits[i]);

        std::cout << "\n  " << BOLD << BR_RED << "Save file corrupted" << RESET << std::endl;
        std::cout << "  Unknown error occurred while reading the save file." << std::endl;
        std::cout << "  This save file cannot be loaded. Please try another slot." << std::endl;

        return false;
    }
}

// =====================================================================
// Record::saveRecord - Save highest record for each player (max 50 records)
// What it does: Saves game result to records.txt, keeping only the highest
//               record (most rounds survived) for each player. When records
//               exceed 50 lines, removes oldest records from the beginning.
// Inputs: player (Player&) - player whose record to save
//         ai (AI&) - AI opponent with difficulty info
// Outputs: none (writes to RECORD_FILE, prints status to stdout)
// =====================================================================
void Record::saveRecord(const Player& player, const AI& ai) {
    const int MAX_RECORDS = 50;
    
    std::string playerName = player.getName();
    int roundsPlayed = player.getRoundsPlayed();
    int finalGold = player.getGold();
    std::string difficulty = ai.getDifficultyString();
    
    // Read all existing records and find if this player exists
    std::vector<std::string> allRecords;
    std::ifstream inFile(RECORD_FILE);
    std::string line;
    bool playerFound = false;
    int highestRounds = 0;
    
    if (inFile.is_open()) {
        while (std::getline(inFile, line)) {
            std::istringstream iss(line);
            std::string name;
            int rounds, gold;
            std::string diff;
            
            if (iss >> name >> rounds >> gold >> diff) {
                if (name == playerName) {
                    playerFound = true;
                    highestRounds = rounds;
                    // Only keep this record if new score is not better
                    if (roundsPlayed > highestRounds) {
                        // New score is better, don't add old record
                        continue;
                    } else {
                        // New score is not better, keep the old record
                        allRecords.push_back(line);
                        std::cout << "\n  Record not saved. " << playerName 
                                  << "'s best remains: " << highestRounds << " rounds." << std::endl;
                        return;
                    }
                } else {
                    allRecords.push_back(line);
                }
            }
        }
        inFile.close();
    }
    
    // Add the new record for this player
    std::ostringstream newRecord;
    newRecord << playerName << " " << roundsPlayed << " " << finalGold << " " << difficulty;
    allRecords.push_back(newRecord.str());
    
    // If total records exceed MAX_RECORDS, remove oldest records from the beginning
    while ((int)allRecords.size() > MAX_RECORDS) {
        allRecords.erase(allRecords.begin());
    }
    
    // Write all records back to file
    std::ofstream outFile(RECORD_FILE);
    if (!outFile.is_open()) {
        std::cerr << "  Warning: could not write to " << RECORD_FILE << std::endl;
        return;
    }
    
    for (const auto& rec : allRecords) {
        outFile << rec << std::endl;
    }
    
    outFile.close();
    
    if (playerFound && roundsPlayed > highestRounds) {
        std::cout << "\n  🎉 New personal best! " << playerName << ": " 
                  << roundsPlayed << " rounds (was " << highestRounds << ")" << std::endl;
    } else if (!playerFound) {
        std::cout << "\n  ✅ Record saved! " << playerName << ": " 
                  << roundsPlayed << " rounds." << std::endl;
    }
}

// =====================================================================
// Record::displayLeaderboard - show sorted leaderboard
// =====================================================================
void Record::displayLeaderboard() {
    std::ifstream file(RECORD_FILE);
    if (!file.is_open()) {
        std::cout << BOLD << YELLOW << "\n  No records found. Play a game first!" << RESET << std::endl;
        return;
    }

    struct RecordEntry {
        std::string name;
        int rounds, gold;
        std::string difficulty;
    };

    std::vector<RecordEntry> records;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        RecordEntry rec;
        if (iss >> rec.name >> rec.rounds >> rec.gold >> rec.difficulty)
            records.push_back(rec);
    }
    file.close();

    if (records.empty()) {
        std::cout << "\n  No records found." << std::endl;
        return;
    }

    // Sort by rounds (descending)
    for (size_t i = 0; i < records.size(); ++i)
        for (size_t j = i + 1; j < records.size(); ++j)
            if (records[j].rounds > records[i].rounds) {
                RecordEntry t = records[i]; records[i] = records[j]; records[j] = t;
            }

    const int W = 43;
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxTitle("LEADERBOARD", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;

    {
        std::ostringstream h;
        h << "  " << std::left << std::setw(6) << "Rank"
          << std::setw(12) << "Name" << std::setw(8) << "Rounds"
          << std::setw(7) << "Gold" << std::setw(6) << "Diff";
        printBoxLine(h.str(), W);
    }
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;

    int show = std::min((int)records.size(), 10);
    for (int i = 0; i < show; ++i) {
        std::ostringstream row;
        row << "  " << std::left << std::setw(6) << (i + 1)
            << std::setw(12) << records[i].name
            << std::setw(8) << records[i].rounds
            << std::setw(7) << records[i].gold
            << std::setw(6) << records[i].difficulty;
        printBoxLine(row.str(), W);
    }
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
}

// =====================================================================
// Record::hasSaveFile - check if save file exists for given slot
// Returns: true if save file exists and is readable, false otherwise
// =====================================================================
bool Record::hasSaveFile(int slot) {
    std::ifstream file(getSaveFilePath(slot));
    if (!file.is_open()) {
        return false;
    }
    file.close();
    return true;
}

// =====================================================================
// Record::showSavePreview - Display save file preview without loading
// Reads and displays: Round, HP, and Gold from save file
// Returns: true if successfully displayed preview, false if file error
// =====================================================================
bool Record::showSavePreview(int slot) {
    std::ifstream file(getSaveFilePath(slot));
    if (!file.is_open()) {
        return false;
    }

    try {
        // Skip game phase (line 1)
        int phase;
        if (!(file >> phase)) {
            throw std::runtime_error("Failed to read game phase");
        }
        if (phase < 0 || phase > 3) {
            throw std::runtime_error("Invalid game phase value");
        }

        // Read player state: HP, Gold, RoundsPlayed, WinStreak, LossStreak, WinCount, LossCount
        int hp, gold, rounds, winStreak, lossStreak, winCount = 0, lossCount = 0;
        if (!(file >> hp >> gold >> rounds >> winStreak >> lossStreak)) {
            throw std::runtime_error("Failed to read player state");
        }
        // Read win/loss counts (backward compat: optional in new saves)
        file >> winCount >> lossCount;
        if (file.fail()) {
            winCount = 0;
            lossCount = 0;
            file.clear();
        }

        // Validate values
        if (hp < 0 || gold < 0 || rounds < 0 || winStreak < 0 || lossStreak < 0
            || winCount < 0 || lossCount < 0) {
            throw std::runtime_error("Invalid player state values");
        }

        file.close();

        // Display preview with proper formatting
        std::cout << "  " << BOLD << BR_GREEN << "[Slot " << slot << "] Used" << RESET
                  << " | " << BR_YELLOW << "Round " << rounds << RESET
                  << " | " << BR_CYAN << "HP: " << hp << RESET
                  << " | " << BR_YELLOW << "Gold: " << gold << RESET << std::endl;

        return true;
    } catch (const std::exception& e) {
        file.close();
        std::cout << "  " << BOLD << BR_RED << "[Slot " << slot << "] Corrupted" << RESET
                  << std::string(20, ' ') << std::endl;
        return false;
    } catch (...) {
        file.close();
        std::cout << "  " << BOLD << BR_RED << "[Slot " << slot << "] Corrupted" << RESET
                  << std::string(20, ' ') << std::endl;
        return false;
    }
}

// =====================================================================
// Record::displayAllSlots - Display status of all 3 save slots
// Shows each slot with Empty/Used status and round number
// Purpose: Help player choose save/load slot
// =====================================================================
void Record::displayAllSlots() {
    std::cout << "\n";
    std::cout << BOLD << CYAN << "  +" << std::string(45, '-') << "+" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  |" << std::string(43, ' ') << "|" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  |  " << BOLD << BR_YELLOW << "SAVE SLOTS" << RESET
              << std::string(32, ' ') << BOLD << CYAN << "|" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  |" << std::string(43, ' ') << "|" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(45, '-') << "+" << RESET << std::endl;

    // Check each slot
    for (int slot = 1; slot <= 3; ++slot) {
        std::cout << BOLD << CYAN << "  |  " << RESET;
        
        std::ifstream file(getSaveFilePath(slot));
        if (!file.is_open()) {
            // Slot is empty
            std::cout << BOLD << BR_RED << "[Slot " << slot << "] Empty" << RESET;
            std::cout << std::string(28, ' ');
        } else {
            try {
                // Read round number and validate
                int phase;
                if (!(file >> phase)) {
                    throw std::runtime_error("Failed to read phase");
                }
                if (phase < 0 || phase > 3) {
                    throw std::runtime_error("Invalid phase value");
                }
                
                int hp, gold, rounds, winStreak, lossStreak, winCount = 0, lossCount = 0;
                if (!(file >> hp >> gold >> rounds >> winStreak >> lossStreak)) {
                    throw std::runtime_error("Failed to read player state");
                }
                // Read win/loss counts (backward compat: optional in new saves)
                file >> winCount >> lossCount;
                if (file.fail()) {
                    winCount = 0;
                    lossCount = 0;
                    file.clear();
                }

                // Validate player data
                if (hp < 0 || gold < 0 || rounds < 0 || winStreak < 0 || lossStreak < 0
                    || winCount < 0 || lossCount < 0) {
                    throw std::runtime_error("Invalid player state values");
                }
                
                file.close();

                // Slot is valid
                std::cout << BOLD << BR_GREEN << "[Slot " << slot << "] Used" << RESET
                          << " | " << BR_YELLOW << "Round " << rounds << RESET;
                
                // Padding to align right side
                std::string info = "[Slot " + std::to_string(slot) + "] Used | Round " + std::to_string(rounds);
                if (info.length() < 40) {
                    std::cout << std::string(40 - info.length(), ' ');
                }
            } catch (const std::exception& e) {
                file.close();
                std::cout << BOLD << BR_RED << "[Slot " << slot << "] Corrupted" << RESET;
                std::cout << std::string(24, ' ');
            } catch (...) {
                file.close();
                std::cout << BOLD << BR_RED << "[Slot " << slot << "] Corrupted" << RESET;
                std::cout << std::string(24, ' ');
            }
        }
        
        std::cout << BOLD << CYAN << "|" << RESET << std::endl;
    }
    
    std::cout << BOLD << CYAN << "  +" << std::string(45, '-') << "+" << RESET << std::endl;
    std::cout << std::endl;
}
