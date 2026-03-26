#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iomanip>

// -----------------------------------------------------------------
// Constructor
// What it does : initialises all subsystems with the chosen difficulty.
// Input  : difficulty
// Output : a Game ready to run()
// -----------------------------------------------------------------
Game::Game(Difficulty difficulty)
    : ai_(difficulty), running_(true), skipCombat_(false) {}

// -----------------------------------------------------------------
// Destructor
// What it does : board doesn't own units, so nothing extra to free here.
//                Player and AI destructors handle their own memory.
// Input  : none
// Output : none
// -----------------------------------------------------------------
Game::~Game() {}

// -----------------------------------------------------------------
// run
// What it does : the master game loop. Each iteration = one round
//                consisting of a shop phase followed by a battle phase.
//                Ends when the player dies or types "quit".
// Input  : none
// Output : final score (rounds survived)
// -----------------------------------------------------------------
int Game::run() {
    std::cout << std::endl;
    std::cout << "  +======================================+" << std::endl;
    std::cout << "  |       AUTO-BATTLER ARENA             |" << std::endl;
    std::cout << "  |       Difficulty: " << std::left << std::setw(19)
              << ai_.getDifficultyString() << "|" << std::endl;
    std::cout << "  +======================================+" << std::endl;
    std::cout << "\n  Type 'help' during the shop phase to see commands.\n"
              << std::endl;

    while (running_ && player_.isAlive()) {
        // --- Start of round ---
        player_.startNewRound();
        player_.displayStatus();

        // --- Shop Phase ---
        shop_.refresh();
        shopPhase();

        if (!running_) break;

        // --- Battle Phase ---
        std::cout << "\n  ========== BATTLE PHASE ==========" << std::endl;

        skipCombat_ = false;
        bool playerWon = battlePhase();

        if (playerWon) {
            player_.recordWin();
            std::cout << "\n  >> YOU WON this round! <<" << std::endl;
        } else {
            player_.recordLoss();
            // Calculate damage to player
            int damage = LOSS_DAMAGE_BASE
                + (ai_.getArmySize() * LOSS_DAMAGE_PER_SURVIVING);
            player_.takeDamage(damage);
            std::cout << "\n  >> YOU LOST this round. You take "
                      << damage << " damage. <<" << std::endl;
        }

        // Clean up the board for the next round
        board_.clear();

        if (!player_.isAlive()) {
            const int GW = 35;
            std::cout << std::endl;
            std::cout << "  +" << std::string(GW, '=') << "+" << std::endl;
            std::string t1 = "GAME OVER";
            int p1 = (GW - (int)t1.size()) / 2;
            std::cout << "  |" << std::string(p1, ' ') << t1
                      << std::string(GW - p1 - (int)t1.size(), ' ') << "|" << std::endl;
            std::string t2 = "You survived " + std::to_string(player_.getRoundsPlayed()) + " rounds!";
            int p2 = (GW - (int)t2.size()) / 2;
            std::cout << "  |" << std::string(p2, ' ') << t2
                      << std::string(GW - p2 - (int)t2.size(), ' ') << "|" << std::endl;
            std::cout << "  +" << std::string(GW, '=') << "+" << std::endl;
        } else {
            std::cout << std::endl;
            std::cout << "  [Press Enter to continue to next round...]";
            std::string dummy;
            std::getline(std::cin, dummy);
        }
    }

    saveRecord();
    return player_.getRoundsPlayed();
}

// -----------------------------------------------------------------
// shopPhase
// What it does : interactive command loop. Displays shop and bench,
//                reads user commands until "ready" or "quit".
// Input  : none
// Output : none
// -----------------------------------------------------------------
void Game::shopPhase() {
    std::string line;
    bool ready = false;

    // Clear player side of the board for deployment
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c <= PLAYER_MAX_COL; ++c) {
            Unit* u = board_.getUnit(r, c);
            if (u != nullptr) {
                board_.removeUnit(r, c);
                player_.addToBench(u);
            }
        }

    while (!ready && running_) {
        shop_.setPlayerGold(player_.getGold());
        shop_.display();
        player_.displayBench();

        // Show current formation if any units are placed
        std::vector<Unit*> placed = board_.getPlayerUnits();
        if (!placed.empty()) {
            board_.displayPlayerSide();
        }

        std::cout << "  Command > ";
        if (!std::getline(std::cin, line)) {
            running_ = false;
            break;
        }

        // Parse command
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "buy") {
            int slot;
            if (!(iss >> slot)) {
                std::cout << "  Usage: buy 1  (slot number 1-5)" << std::endl;
                continue;
            }
            slot--;
            Unit* u = shop_.getUnit(slot);
            if (u == nullptr) {
                std::cout << "  That slot is empty." << std::endl;
                continue;
            }
            if (!player_.spendGold(u->getCost())) {
                std::cout << "  Not enough gold! (need $" << u->getCost()
                          << ", have $" << player_.getGold() << ")" << std::endl;
                continue;
            }
            Unit* bought = shop_.buyUnit(slot);
            if (!player_.addToBench(bought)) {
                std::cout << "  Bench is full! Refunding gold." << std::endl;
                player_.addGold(bought->getCost());
                delete bought;
                continue;
            }
            std::cout << "  + Purchased " << bought->getName() << "!" << std::endl;
            player_.displayStatus();

        } else if (cmd == "sell") {
            int idx;
            if (!(iss >> idx)) {
                std::cout << "  Usage: sell 1  (bench index)" << std::endl;
                continue;
            }
            idx--;
            if (!player_.sellUnit(idx)) {
                std::cout << "  Invalid bench index." << std::endl;
            }
            player_.displayStatus();

        } else if (cmd == "place") {
            int idx, row, col;
            if (!(iss >> idx >> row >> col)) {
                std::cout << "  Usage: place 1 2 3  (bench_index row col)" << std::endl;
                std::cout << "  Row: 0-4, Col: 0-3 (your side only)" << std::endl;
                continue;
            }
            idx--;
            if (col < 0 || col > PLAYER_MAX_COL) {
                std::cout << "  Column must be 0-" << PLAYER_MAX_COL
                          << " (your side)." << std::endl;
                continue;
            }
            if (row < 0 || row >= BOARD_ROWS) {
                std::cout << "  Row must be 0-" << (BOARD_ROWS - 1) << "." << std::endl;
                continue;
            }
            if (!board_.isEmpty(row, col)) {
                std::cout << "  That cell is already occupied!" << std::endl;
                continue;
            }
            Unit* unit = player_.removeFromBench(idx);
            if (unit == nullptr) {
                std::cout << "  Invalid bench index." << std::endl;
                continue;
            }
            board_.placeUnit(unit, row, col);
            std::cout << "  Placed " << unit->getName() << " at ("
                      << row << "," << col << ")." << std::endl;
            board_.displayPlayerSide();

        } else if (cmd == "remove") {
            int row, col;
            if (!(iss >> row >> col)) {
                std::cout << "  Usage: remove 2 3  (row col)" << std::endl;
                continue;
            }
            if (col < 0 || col > PLAYER_MAX_COL || row < 0 || row >= BOARD_ROWS) {
                std::cout << "  Invalid position." << std::endl;
                continue;
            }
            Unit* unit = board_.getUnit(row, col);
            if (unit == nullptr) {
                std::cout << "  No unit at that position." << std::endl;
                continue;
            }
            if (!unit->isPlayerUnit()) {
                std::cout << "  That's not your unit." << std::endl;
                continue;
            }
            board_.removeUnit(row, col);
            player_.addToBench(unit);
            std::cout << "  Removed " << unit->getName()
                      << " back to bench." << std::endl;

        } else if (cmd == "formation" || cmd == "form" || cmd == "f") {
            board_.displayPlayerSide();

        } else if (cmd == "refresh") {
            if (!player_.spendGold(shop_.getRefreshCost())) {
                std::cout << "  Not enough gold to refresh! (need $"
                          << shop_.getRefreshCost() << ")" << std::endl;
                continue;
            }
            shop_.refresh();
            std::cout << "  Shop refreshed!" << std::endl;
            player_.displayStatus();

        } else if (cmd == "auto") {
            // Auto-place all bench units for convenience
            int benchSize = player_.getBenchSize();
            int placed = 0;
            for (int i = 0; i < benchSize; ++i) {
                Unit* unit = player_.removeFromBench(0);
                if (unit == nullptr) continue;
                bool ok = false;
                // Fill from col 3 (front) to col 0 (back), top to bottom
                for (int c = PLAYER_MAX_COL; c >= 0 && !ok; --c) {
                    for (int r = 0; r < BOARD_ROWS && !ok; ++r) {
                        if (board_.isEmpty(r, c)) {
                            board_.placeUnit(unit, r, c);
                            ok = true;
                            placed++;
                        }
                    }
                }
                if (!ok) {
                    player_.addToBench(unit);
                    break;
                }
            }
            std::cout << "  Auto-placed " << placed << " units." << std::endl;
            board_.displayPlayerSide();

        } else if (cmd == "ready") {
            // Check: need at least one unit on the board
            std::vector<Unit*> onBoard = board_.getPlayerUnits();
            if (onBoard.empty() && player_.getBenchSize() == 0) {
                std::cout << "  You have no units! Buy some first." << std::endl;
                continue;
            }
            // Auto-place any remaining bench units
            if (!onBoard.empty() || player_.getBenchSize() > 0) {
                int benchSize = player_.getBenchSize();
                for (int i = 0; i < benchSize; ++i) {
                    Unit* unit = player_.removeFromBench(0);
                    if (unit == nullptr) continue;
                    bool ok = false;
                    for (int c = PLAYER_MAX_COL; c >= 0 && !ok; --c) {
                        for (int r = 0; r < BOARD_ROWS && !ok; ++r) {
                            if (board_.isEmpty(r, c)) {
                                board_.placeUnit(unit, r, c);
                                ok = true;
                            }
                        }
                    }
                    if (!ok) {
                        player_.addToBench(unit);
                        break;
                    }
                }
            }
            onBoard = board_.getPlayerUnits();
            if (onBoard.empty()) {
                std::cout << "  No units on the board! Use 'place' or 'auto'." << std::endl;
                continue;
            }
            ready = true;

        } else if (cmd == "help") {
            printHelp();

        } else if (cmd == "quit" || cmd == "exit") {
            running_ = false;

        } else {
            std::cout << "  Unknown command. Type 'help' for options." << std::endl;
        }
    }
}

// -----------------------------------------------------------------
// battlePhase
// What it does : places player and AI units on the board, then runs
//                automated combat.
// Input  : none
// Output : true if the player won
// -----------------------------------------------------------------
bool Game::battlePhase() {
    // Player units are already placed on the board from the shop phase.
    // Only need to clear the AI side and generate AI army.
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = AI_MIN_COL; c < BOARD_COLS; ++c)
            board_.removeUnit(r, c);

    // Generate and place AI army
    ai_.generateArmy(player_.getRoundsPlayed());
    ai_.placeUnits(board_);
    // Board now owns the AI units -- release AI's pointers without deleting
    ai_.releaseArmy();

    std::cout << "\n  Initial formation:" << std::endl;
    board_.display();

    // Show both armies before combat starts
    std::vector<Unit*> pUnits = board_.getPlayerUnits();
    std::vector<Unit*> aUnits = board_.getAIUnits();
    std::cout << "  YOUR ARMY (" << pUnits.size() << " units):  ";
    for (size_t i = 0; i < pUnits.size(); ++i) {
        std::cout << pUnits[i]->getName();
        if (i + 1 < pUnits.size()) std::cout << ", ";
    }
    std::cout << std::endl;
    std::cout << "  ENEMY ARMY (" << aUnits.size() << " units): ";
    for (size_t i = 0; i < aUnits.size(); ++i) {
        std::cout << aUnits[i]->getName();
        if (i + 1 < aUnits.size()) std::cout << ", ";
    }
    std::cout << std::endl;

    std::cout << "\n  Press Enter to begin combat (or 's' to skip to result)...";
    std::string input;
    std::getline(std::cin, input);
    if (input == "s" || input == "S") {
        skipCombat_ = true;
    }

    // Run combat
    bool playerWon = resolveCombat();

    // Clean up all surviving units still on the board.
    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            Unit* u = board_.getUnit(r, c);
            if (u != nullptr) {
                board_.removeUnit(r, c);
                delete u;
            }
        }
    }

    return playerWon;
}

// -----------------------------------------------------------------
// placePlayerUnits
// What it does : auto-places all bench units onto the player's side
//                (cols 0 to PLAYER_MAX_COL), filling column by column.
// Input  : none
// Output : none
// -----------------------------------------------------------------
void Game::placePlayerUnits() {
    int col = PLAYER_MAX_COL;
    int row = 0;

    // We move units out of the bench and onto the board.
    int benchSize = player_.getBenchSize();
    for (int i = 0; i < benchSize; ++i) {
        Unit* unit = player_.removeFromBench(0);
        if (unit == nullptr) continue;

        bool placed = false;
        while (!placed && col >= 0) {
            if (board_.isEmpty(row, col)) {
                board_.placeUnit(unit, row, col);
                placed = true;
            }
            row++;
            if (row >= BOARD_ROWS) {
                row = 0;
                col--;
            }
        }
        if (!placed) {
            // No space -- shouldn't happen with normal bench sizes
            delete unit;
        }
    }
}

// -----------------------------------------------------------------
// resolveCombat
// What it does : the tick-by-tick combat engine with paced output.
//   Each tick:
//   1. Display the board and a tick header.
//   2. Each alive unit finds nearest enemy, attacks or moves.
//   3. Print a compact summary of this tick's actions.
//   4. Remove dead units.
//   5. Wait for player input (Enter) or skip if requested.
//   6. Check win condition.
//   After combat, print a final battle report.
// Input  : none
// Output : true if player side won
// -----------------------------------------------------------------
bool Game::resolveCombat() {
    int totalPlayerKills = 0;
    int totalAIKills = 0;
    int totalPlayerDmg = 0;
    int totalAIDmg = 0;
    int lastTick = 0;

    for (int tick = 1; tick <= MAX_COMBAT_TICKS; ++tick) {
        std::vector<Unit*> allUnits = board_.getAllUnits();

        // Check end conditions
        bool playerAlive = false;
        bool aiAlive = false;
        for (size_t i = 0; i < allUnits.size(); ++i) {
            if (allUnits[i]->isAlive()) {
                if (allUnits[i]->isPlayerUnit()) playerAlive = true;
                else aiAlive = true;
            }
        }

        if (!playerAlive || !aiAlive) break;

        lastTick = tick;

        if (!skipCombat_) {
            // --- Tick header + board ---
            std::cout << "\n  -------- Tick " << tick << " --------" << std::endl;
            board_.display();

            // --- Unit HP status bar ---
            std::vector<Unit*> pUnits = board_.getPlayerUnits();
            std::vector<Unit*> aUnits = board_.getAIUnits();
            std::cout << "  YOU: ";
            for (size_t i = 0; i < pUnits.size(); ++i) {
                std::cout << pUnits[i]->getSymbol() << "("
                          << pUnits[i]->getHp() << ")";
                if (i + 1 < pUnits.size()) std::cout << " ";
            }
            std::cout << "  vs  ENEMY: ";
            for (size_t i = 0; i < aUnits.size(); ++i) {
                std::cout << aUnits[i]->getSymbol() << "("
                          << aUnits[i]->getHp() << ")";
                if (i + 1 < aUnits.size()) std::cout << " ";
            }
            std::cout << std::endl << std::endl;
        }

        // --- Actions this tick ---
        for (size_t i = 0; i < allUnits.size(); ++i) {
            Unit* unit = allUnits[i];
            if (!unit->isAlive()) continue;

            Unit* enemy = board_.findNearestEnemy(unit);
            if (enemy == nullptr) continue;

            int dist = unit->getDistanceTo(enemy);

            if (dist <= unit->getAttackRange()) {
                int dmg = performAttack(unit, enemy);
                if (unit->isPlayerUnit()) {
                    totalPlayerDmg += dmg;
                    if (!enemy->isAlive()) totalPlayerKills++;
                } else {
                    totalAIDmg += dmg;
                    if (!enemy->isAlive()) totalAIKills++;
                }
            } else {
                board_.moveToward(unit, enemy->getRow(), enemy->getCol());
                if (!skipCombat_) {
                    std::cout << "  " << unit->getName()
                              << " moves toward " << enemy->getName()
                              << std::endl;
                }
            }
        }

        // Remove dead units
        cleanupDeadUnits();

        // --- Pause ---
        if (!skipCombat_) {
            std::cout << "\n  [Enter] next tick  |  [s] skip to result > ";
            std::string input;
            std::getline(std::cin, input);
            if (input == "s" || input == "S") {
                skipCombat_ = true;
                std::cout << "  >> Fast-forwarding..." << std::endl;
            }
        }
    }

    // --- Final state ---
    std::vector<Unit*> pUnits = board_.getPlayerUnits();
    std::vector<Unit*> aUnits = board_.getAIUnits();

    bool playerWon = !pUnits.empty() && aUnits.empty();

    // --- Battle Report ---
    const int BW = 39;
    std::cout << std::endl;
    std::cout << "  +" << std::string(BW, '=') << "+" << std::endl;

    {
        std::string t = "BATTLE REPORT";
        int p = (BW - (int)t.size()) / 2;
        std::cout << "  |" << std::string(p, ' ') << t
                  << std::string(BW - p - (int)t.size(), ' ') << "|" << std::endl;
    }

    std::cout << "  +" << std::string(BW, '=') << "+" << std::endl;

    auto printRow = [&](const std::string& text) {
        std::string s = text;
        if ((int)s.size() < BW) s += std::string(BW - s.size(), ' ');
        std::cout << "  |" << s << "|" << std::endl;
    };

    printRow("  Result: " + std::string(playerWon ? "VICTORY!" : "DEFEAT..."));
    printRow("  Ticks:  " + std::to_string(lastTick));
    std::cout << "  +" << std::string(BW, '-') << "+" << std::endl;
    printRow("  Your damage dealt:  " + std::to_string(totalPlayerDmg));
    printRow("  Your kills:         " + std::to_string(totalPlayerKills));
    printRow("  Enemy damage dealt: " + std::to_string(totalAIDmg));
    printRow("  Enemy kills:        " + std::to_string(totalAIKills));

    if (playerWon && !pUnits.empty()) {
        std::cout << "  +" << std::string(BW, '-') << "+" << std::endl;
        printRow("  Survivors:");
        for (size_t i = 0; i < pUnits.size(); ++i) {
            std::string line = "    " + pUnits[i]->getName() + "  HP: "
                + std::to_string(pUnits[i]->getHp()) + "/"
                + std::to_string(pUnits[i]->getMaxHp());
            printRow(line);
        }
    }
    std::cout << "  +" << std::string(BW, '=') << "+" << std::endl;

    return playerWon;
}

// -----------------------------------------------------------------
// performAttack
// What it does : calculates damage (with crit chance), applies it,
//                and prints a combat log line.
// Inputs : attacker, defender - Unit pointers
// Output : integer damage dealt
// -----------------------------------------------------------------
int Game::performAttack(Unit* attacker, Unit* defender) {
    int damage = attacker->getAtk();
    bool crit = false;

    // Critical hit check: base 20% + unit's crit bonus
    int critChance = BASE_CRIT_CHANCE + attacker->getCritBonus();
    if (rand() % 100 < critChance) {
        damage = (int)(damage * CRIT_MULTIPLIER);
        crit = true;
    }

    defender->takeDamage(damage);

    if (!skipCombat_) {
        if (crit) {
            std::cout << "  ** " << attacker->getName() << " CRITS "
                      << defender->getName() << " for " << damage << " dmg!";
        } else {
            std::cout << "  " << attacker->getName() << " -> "
                      << defender->getName() << " " << damage << " dmg";
        }

        if (!defender->isAlive()) {
            std::cout << "  << KILLED >>";
        } else {
            std::cout << "  [" << defender->getHp() << " HP]";
        }
        std::cout << std::endl;
    }

    return damage;
}

// -----------------------------------------------------------------
// cleanupDeadUnits
// What it does : scans the board for dead units, removes them from
//                the grid, and frees their memory with delete.
// Input  : none
// Output : none
// -----------------------------------------------------------------
void Game::cleanupDeadUnits() {
    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            Unit* u = board_.getUnit(r, c);
            if (u != nullptr && !u->isAlive()) {
                board_.removeUnit(r, c);
                delete u;
            }
        }
    }
}

// -----------------------------------------------------------------
// saveRecord
// What it does : appends a line to records.txt with the player's
//                name, rounds survived, and difficulty.
// Input  : none
// Output : none (writes to file)
// -----------------------------------------------------------------
void Game::saveRecord() const {
    std::ofstream file("records.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "  Warning: could not open records.txt for writing."
                  << std::endl;
        return;
    }
    file << player_.getName() << " "
         << player_.getRoundsPlayed() << " "
         << player_.getGold() << " "
         << ai_.getDifficultyString() << std::endl;
    file.close();
    std::cout << "\n  Record saved to records.txt." << std::endl;
}

// -----------------------------------------------------------------
// displayLeaderboard (static)
// What it does : reads records.txt line by line, sorts by rounds
//                survived (descending), and prints the top 10.
// Input  : none
// Output : none (reads file, prints to stdout)
// -----------------------------------------------------------------
void Game::displayLeaderboard() {
    std::ifstream file("records.txt");
    if (!file.is_open()) {
        std::cout << "\n  No records found. Play a game first!" << std::endl;
        return;
    }

    struct Record {
        std::string name;
        int rounds;
        int gold;
        std::string difficulty;
    };

    std::vector<Record> records;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Record rec;
        if (iss >> rec.name >> rec.rounds >> rec.gold >> rec.difficulty) {
            records.push_back(rec);
        }
    }
    file.close();

    if (records.empty()) {
        std::cout << "\n  No records found. Play a game first!" << std::endl;
        return;
    }

    // Sort by rounds descending
    for (size_t i = 0; i < records.size(); ++i) {
        for (size_t j = i + 1; j < records.size(); ++j) {
            if (records[j].rounds > records[i].rounds) {
                Record temp = records[i];
                records[i] = records[j];
                records[j] = temp;
            }
        }
    }

    const int W = 43;
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;

    std::string title = "LEADERBOARD";
    int pad = (W - (int)title.size()) / 2;
    std::cout << "  |" << std::string(pad, ' ') << title
              << std::string(W - pad - (int)title.size(), ' ') << "|" << std::endl;

    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;

    // Header
    {
        std::ostringstream h;
        h << "  " << std::left << std::setw(6) << "Rank"
          << std::setw(12) << "Name"
          << std::setw(8) << "Rounds"
          << std::setw(7) << "Gold"
          << std::setw(6) << "Diff";
        std::string s = h.str();
        if ((int)s.size() < W) s += std::string(W - s.size(), ' ');
        std::cout << "  |" << s << "|" << std::endl;
    }
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;

    int show = (int)records.size();
    if (show > 10) show = 10;

    for (int i = 0; i < show; ++i) {
        std::ostringstream row;
        row << "  " << std::left << std::setw(6) << (i + 1)
            << std::setw(12) << records[i].name
            << std::setw(8) << records[i].rounds
            << std::setw(7) << records[i].gold
            << std::setw(6) << records[i].difficulty;
        std::string s = row.str();
        if ((int)s.size() < W) s += std::string(W - s.size(), ' ');
        std::cout << "  |" << s << "|" << std::endl;
    }
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
}

// -----------------------------------------------------------------
// printHelp
// What it does : displays the list of available shop phase commands.
// Input  : none
// Output : none (stdout)
// -----------------------------------------------------------------
void Game::printHelp() const {
    std::cout << std::endl;
    std::cout << "  +------------------------------------------------+" << std::endl;
    std::cout << "  |  COMMANDS                                      |" << std::endl;
    std::cout << "  +------------------------------------------------+" << std::endl;
    std::cout << "  |  SHOPPING:                                     |" << std::endl;
    std::cout << "  |    buy 1-5        Buy unit from shop slot      |" << std::endl;
    std::cout << "  |    sell 1-N       Sell unit from bench         |" << std::endl;
    std::cout << "  |    refresh        Re-roll shop ($"
              << shop_.getRefreshCost() << ")              |" << std::endl;
    std::cout << "  |  FORMATION:                                    |" << std::endl;
    std::cout << "  |    place 1 2 3    Place bench unit at row,col  |" << std::endl;
    std::cout << "  |    remove 2 3     Remove unit at row,col       |" << std::endl;
    std::cout << "  |    auto           Auto-place all bench units   |" << std::endl;
    std::cout << "  |    formation      Show your current formation  |" << std::endl;
    std::cout << "  |  GAME:                                         |" << std::endl;
    std::cout << "  |    ready          Start the battle!            |" << std::endl;
    std::cout << "  |    quit           Exit the game                |" << std::endl;
    std::cout << "  +------------------------------------------------+" << std::endl;
    std::cout << "  Tip: Put Tanks in col 3 (front), Archers in col 0 (back)" << std::endl;
}
