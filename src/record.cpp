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

// =====================================================================
// Record::saveGame - save complete game state
// =====================================================================
void Record::saveGame(const Player& player,
                      const Board& board,
                      const Shop& /* shop */,
                      const AI& ai,
                      GamePhase currentPhase,
                      EventType currentEvent) {
    std::ofstream file(SAVE_FILE);
    if (!file.is_open()) {
        std::cerr << "  Could not save game." << std::endl;
        return;
    }
    
    // Save game phase (0=ROUND_START, 1=SHOP, 2=BATTLE)
    file << (int)currentPhase << std::endl;
    
    // Save player state
    file << player.getHp() << " " << player.getGold() << " "
         << player.getRoundsPlayed() << " " << player.getWinStreak() << " "
         << player.getLossStreak() << std::endl;

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
    
    file.close();
    std::cout << GREEN << "  ✅ Game saved!" << RESET << std::endl;
}

// =====================================================================
// Record::loadGame - restore game state from save file
// =====================================================================
bool Record::loadGame(Player& player,
                      Board& board,
                      Shop& /* shop */,
                      AI& /* ai */,
                      GamePhase& currentPhase,
                      EventType& currentEvent,
                      bool& shouldResumeShopPhase) {
    std::ifstream file(SAVE_FILE);
    if (!file.is_open()) return false;

    // Read game phase
    int phaseInt;
    if (!(file >> phaseInt)) {
        file.close();
        return false;
    }
    currentPhase = (GamePhase)phaseInt;
    shouldResumeShopPhase = (currentPhase == PHASE_SHOP);

    // Read player state
    int hp, gold, rounds, winStreak, lossStreak;
    if (!(file >> hp >> gold >> rounds >> winStreak >> lossStreak)) {
        file.close();
        return false;
    }

    // Read bench units
    int benchCount;
    if (!(file >> benchCount)) {
        file.close();
        return false;
    }

    std::vector<Unit*> loadedUnits;
    for (int i = 0; i < benchCount; ++i) {
        std::string name;
        int cls, maxHp, atk, cost, critBonus, range, starLevel;
        if (!(file >> name >> cls >> maxHp >> atk >> cost >> critBonus >> range >> starLevel)) {
            // Cleanup on error
            for (size_t j = 0; j < loadedUnits.size(); ++j) delete loadedUnits[j];
            file.close();
            return false;
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
        // Cleanup on error
        for (size_t j = 0; j < loadedUnits.size(); ++j) delete loadedUnits[j];
        file.close();
        return false;
    }

    // Store board units temporarily (will apply after player state)
    std::vector<std::tuple<int, int, Unit*>> boardUnits;
    for (int i = 0; i < boardUnitCount; ++i) {
        int r, c;
        std::string name;
        int cls, maxHp, atk, cost, critBonus, range, starLevel;
        if (!(file >> r >> c >> name >> cls >> maxHp >> atk >> cost >> critBonus >> range >> starLevel)) {
            // Cleanup on error
            for (size_t j = 0; j < loadedUnits.size(); ++j) delete loadedUnits[j];
            for (size_t j = 0; j < boardUnits.size(); ++j) delete std::get<2>(boardUnits[j]);
            file.close();
            return false;
        }
        Unit* u = new Unit(name, (UnitClass)cls, maxHp, atk, cost, critBonus, range);
        for (int s = 1; s < starLevel; ++s) {
            u->forceSetStarLevel(s + 1);
        }
        boardUnits.push_back(std::make_tuple(r, c, u));
    }

    // Read difficulty
    std::string diff;
    file >> diff;
    
    // Read current event
    int eventInt;
    if (!(file >> eventInt)) {
        eventInt = EVENT_NONE;
    }
    currentEvent = (EventType)eventInt;
    
    file.close();

    // Apply loaded state to player
    player.loadState(hp, gold, rounds, winStreak, lossStreak);

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
    
    return true;
}

// =====================================================================
// Record::saveRecord - append game result to leaderboard
// =====================================================================
void Record::saveRecord(const Player& player, const AI& ai) {
    std::ofstream file(RECORD_FILE, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "  Warning: could not write to " << RECORD_FILE << std::endl;
        return;
    }
    file << player.getName() << " "
         << player.getRoundsPlayed() << " "
         << player.getGold() << " "
         << ai.getDifficultyString() << std::endl;
    file.close();
    std::cout << "\n  Record saved." << std::endl;
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
// Record::hasSaveFile - check if save file exists
// =====================================================================
bool Record::hasSaveFile() {
    std::ifstream file(SAVE_FILE);
    return file.is_open();
}
