#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iomanip>
//--- color define ---
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

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
// Constructor / Destructor
// =====================================================================
Game::Game(Difficulty difficulty)
    : ai_(difficulty), running_(true), skipCombat_(false),
      currentEvent_(EVENT_NONE) {}

Game::~Game() {}

// =====================================================================
// run - Master game loop
// =====================================================================
int Game::run() {
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +======================================+" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  |       AUTO-BATTLER ARENA             |" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  |       Difficulty: " << std::left << std::setw(19)
              << ai_.getDifficultyString() << "|" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  +======================================+" << RESET << std::endl;
    std::cout << "\n  Type 'help' for commands. 'save' to save game.\n" << std::endl;

    while (running_ && player_.isAlive()) {
        player_.startNewRound();

        // --- Random event ---
        currentEvent_ = Event::rollEvent(player_.getRoundsPlayed());
        handleEvent();

        player_.displayStatus();
        //--- Ask Quit when Gold is insufficent ---
            int min_cost = 999;
            for (int i = 0; i < SHOP_SIZE; i++) {
                Unit* u = shop_.getUnit(i);
                if (u != nullptr) {
                    int cost = u->getCost();
                    if (cost < min_cost) {
                        min_cost = cost;
                    }
                }
            }
            int my_gold = player_.getGold();
            if (my_gold < min_cost) {
                std::cout << "\n================================================================" << std::endl;
                std::cout << " You don't have enough gold to buy any champion." << std::endl;
                std::cout << " Do you want to quit the game? (yes/yo) > ";
                std::string answer;
                std::getline(std::cin, answer);
                if (answer == "yes" || answer == "y") {
                    running_ = false;
                    break;
                }
            }

        // --- Shop Phase ---
        shop_.refresh();
        shopPhase();
        if (!running_) break;

        // --- Battle Phase ---
        std::cout << BOLD << RED << "\n  ⚔️ ========== BATTLE PHASE ========== ⚔️" << RESET << std::endl;
        skipCombat_ = false;
        bool playerWon = battlePhase();

        if (playerWon) {
            player_.recordWin();
            std::cout << BOLD << YELLOW << "\n  >> 🥳YOU WON this round!🥳 <<" << RESET << std::endl;
        } else {
            player_.recordLoss();
            int damage = LOSS_DAMAGE_BASE
                + (ai_.getArmySize() * LOSS_DAMAGE_PER_SURVIVING);
            player_.takeDamage(damage);
            std::cout << BOLD << RED << "\n  >> ☠️YOU LOST this round☠️. You take "
                      << damage << " damage. <<" << RESET << std::endl;
        }

        board_.clear();
        currentEvent_ = EVENT_NONE;

        if (!player_.isAlive()) {
            const int GW = 35;
            std::cout << std::endl;
            std::cout << "  +" << std::string(GW, '=') << "+" << std::endl;
            printBoxTitle("GAME OVER", GW);
            std::string t2 = "You survived " + std::to_string(player_.getRoundsPlayed()) + " rounds!";
            printBoxTitle(t2, GW);
            std::cout << "  +" << std::string(GW, '=') << "+" << std::endl;
        } else {
            std::cout << "\n  [Press Enter to continue to next round...]";
            std::string dummy;
            std::getline(std::cin, dummy);
        }
    }

    saveRecord();
    return player_.getRoundsPlayed();
}

// =====================================================================
// handleEvent - Apply random event at start of round
// =====================================================================
void Game::handleEvent() {
    if (currentEvent_ == EVENT_NONE) return;

    std::string desc = Event::applyEvent(currentEvent_, player_);
    if (!desc.empty()) {
        std::cout << BOLD << BLUE << "\n  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << RESET << std::endl;
        std::cout << BOLD << BLUE << "  !  EVENT🫢: " << desc << RESET << std::endl;
        std::cout << BOLD << BLUE << "  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << RESET << std::endl;
    }

    // Give free unit if that event was rolled
    if (currentEvent_ == EVENT_RANDOM_FREE_UNIT) {
        giveRandomFreeUnit();
    }
}

// =====================================================================
// giveRandomFreeUnit
// =====================================================================
void Game::giveRandomFreeUnit() {
    // Create a temporary shop and grab one unit for free
    Shop tempShop;
    tempShop.refresh();
    Unit* freeUnit = tempShop.buyUnit(rand() % SHOP_SIZE);
    if (freeUnit != nullptr) {
        if (player_.addToBench(freeUnit)) {
            std::cout << "  You received a free " << freeUnit->getName() << "!" << std::endl;
        } else {
            std::cout << "  Bench is full! The free unit was lost." << std::endl;
            delete freeUnit;
        }
    }
}

// =====================================================================
// shopPhase - Buy, sell, place, merge, status, gold, then ready
// =====================================================================

void Game::shopPhase() {
    std::string line;
    bool ready = false;

    // Return board units to bench for repositioning
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c <= PLAYER_MAX_COL; ++c) {
            Unit* u = board_.getUnit(r, c);
            if (u != nullptr) {
                board_.removeUnit(r, c);
                player_.addToBench(u);
            }
        }

    // Check for auto-merges after returning units
    checkAndMerge();

    while (!ready && running_) {
        shop_.setPlayerGold(player_.getGold());
        shop_.display();
        player_.displayBench();

        std::vector<Unit*> placed = board_.getPlayerUnits();
        if (!placed.empty()) {
            board_.displayPlayerSide();
        }

        std::cout << BLUE << "  Command > " << RESET;
        if (!std::getline(std::cin, line)) {
            running_ = false;
            break;
        }

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "buy") {
            int slot;
            if (!(iss >> slot)) {
                std::cout << YELLOW << "  Usage: buy 1  (slot number 1-5)" << RESET << std::endl;
                continue;
            }
            slot--;
            Unit* u = shop_.getUnit(slot);
            if (u == nullptr) {
                std::cout << RED << "  That slot is empty." << RESET << std::endl;
                continue;
            }
            
            //purchase confirmation
            std::cout << BLUE << " Are you sure you want to buy " << WHITE << u->getName() << BLUE << "? (yes/no) > " << RESET;
            std::string confirm;
            std::getline(std::cin, confirm);
            if (confirm != "yes" && confirm != "y") {
                std::cout << YELLOW << " Purchase cancelled." << RESET << '\n';
                continue;
            }
            //=========================================================
            int cost = u->getCost();
            // Apply shop discount event
            if (currentEvent_ == EVENT_SHOP_DISCOUNT) {
                cost = (cost > 1) ? cost - 1 : 1;
            }
            if (!player_.spendGold(cost)) {
                std::cout << RED << "  Not enough gold! (need $" << cost
                          << ", have $" << player_.getGold() << ")" << RESET << std::endl;
                continue;
            }
            Unit* bought = shop_.buyUnit(slot);
            if (!player_.addToBench(bought)) {
                std::cout << RED << "  Bench is full! Refunding gold." << RESET << std::endl;
                player_.addGold(cost);
                delete bought;
                continue;
            }
            std::cout << GREEN << "  ✅ Purchased " << bought->getName() << "!" << RESET << std::endl;
            // Check for 3-merge after each purchase
            checkAndMerge();
            player_.displayStatus();

        } else if (cmd == "sell") {
            int idx;
            if (!(iss >> idx)) {
                std::cout << YELLOW << "  Usage: sell 1  (bench index)" << RESET << std::endl;
                continue;
            }
            idx--;
            Unit* Sold = player_.getBenchUnit(idx);
            if (Sold == nullptr) {
                std::cout << RED << " Invalid bench index." << RESET << std::endl;
                continue;
            }
            //selling confirmation
            std::cout << BLUE << " Sure you want to sell " << WHITE << Sold->getName() << BLUE << "? (yes/no) > " << RESET;
            std::string confirm;
            std::getline(std::cin, confirm);
            if (confirm != "yes" && confirm != "y") {
                std::cout << YELLOW << " Sell cancelled." << RESET << std::endl;
                continue;
            }
            if (!player_.sellUnit(idx)) {
                std::cout << RED << "  Invalid bench index." << RESET << std::endl;
            } else {
                std::cout << GREEN << " ✅ Sold successfully!" << RESET << std::endl;
            }
            player_.displayStatus();

        } else if (cmd == "place") {
            int idx, row, col;
            if (!(iss >> idx >> row >> col)) {
                std::cout << YELLOW << "  Usage: place 1 2 3  (bench_index row col)" << RESET << std::endl;
                continue;
            }
            idx--;
            if (col < 0 || col > PLAYER_MAX_COL || row < 0 || row >= BOARD_ROWS) {
                std::cout << RED << "  Position out of range. Row:0-4, Col:0-3" << RESET << std::endl;
                continue;
            }
            if (!board_.isEmpty(row, col)) {
                std::cout << RED << "  That cell is occupied!" << RESET << std::endl;
                continue;
            }
            Unit* unit = player_.removeFromBench(idx);
            if (unit == nullptr) {
                std::cout << RED << "  Invalid bench index." << RESET << std::endl;
                continue;
            }
            board_.placeUnit(unit, row, col);
            std::cout << GREEN << "  ✅ Placed " << unit->getName() << " at ("
                      << row << "," << col << ")." << RESET << std::endl;
            board_.displayPlayerSide();

        } else if (cmd == "remove") {
            int row, col;
            if (!(iss >> row >> col)) {
                std::cout << YELLOW << "  Usage: remove 2 3  (row col)" << RESET << std::endl;
                continue;
            }
            if (col < 0 || col > PLAYER_MAX_COL || row < 0 || row >= BOARD_ROWS) {
                std::cout << RED << "  Invalid position." << RESET << std::endl;
                continue;
            }
            Unit* unit = board_.getUnit(row, col);
            if (unit == nullptr || !unit->isPlayerUnit()) {
                std::cout << RED << "  No unit of yours at that position." << RESET << std::endl;
                continue;
            }
            board_.removeUnit(row, col);
            player_.addToBench(unit);
            std::cout << GREEN << "  ✅ Removed " << unit->getName() << " back to bench." << RESET << std::endl;

        } else if (cmd == "formation" || cmd == "form" || cmd == "f") {
            std::cout << MAGENTA << "\n 📊 Your Formation:" << RESET << std::endl;
            board_.displayPlayerSide();

        } else if (cmd == "auto") {
            int benchSize = player_.getBenchSize();
            int placed = 0;
            for (int i = 0; i < benchSize; ++i) {
                Unit* unit = player_.removeFromBench(0);
                if (unit == nullptr) continue;
                bool ok = false;
                for (int c = PLAYER_MAX_COL; c >= 0 && !ok; --c)
                    for (int r = 0; r < BOARD_ROWS && !ok; ++r)
                        if (board_.isEmpty(r, c)) {
                            board_.placeUnit(unit, r, c);
                            ok = true;
                            placed++;
                        }
                if (!ok) { player_.addToBench(unit); break; }
            }
            std::cout << GREEN << "  ✅ Auto-placed " << placed << " units." << RESET << std::endl;
            board_.displayPlayerSide();

        } else if (cmd == "refresh") {
            if (!player_.spendGold(shop_.getRefreshCost())) {
                std::cout << RED << "  Not enough gold! (need $" << shop_.getRefreshCost() << ")" << RESET << std::endl;
                continue;
            }
            shop_.refresh();
            std::cout << GREEN << "  ✅ Shop refreshed!" << RESET << std::endl;
            player_.displayStatus();


        } else if (cmd == "save") {
            saveGame();
            std::cout << GREEN << "  ✅ Game saved!" << RESET << std::endl;

        } else if (cmd == "ready") {
            // Auto-place bench leftovers
            int benchSize = player_.getBenchSize();
            for (int i = 0; i < benchSize; ++i) {
                Unit* unit = player_.removeFromBench(0);
                if (unit == nullptr) continue;
                bool ok = false;
                for (int c = PLAYER_MAX_COL; c >= 0 && !ok; --c)
                    for (int r = 0; r < BOARD_ROWS && !ok; ++r)
                        if (board_.isEmpty(r, c)) {
                            board_.placeUnit(unit, r, c);
                            ok = true;
                        }
                if (!ok) { player_.addToBench(unit); break; }
            }
            if (board_.getPlayerUnits().empty()) {
                std::cout << RED << "  No units! Buy some first." << RESET << std::endl;
                continue;
            }
            std::cout << BLUE << " 🎯 Ready for battle!" << RESET << std::endl;
            ready = true;

        } else if (cmd == "status") {
            player_.displayStatus();
            
        } else if (cmd == "gold") {
            std::cout << GREEN << " 💰 Gold: " << player_.getGold() << RESET << std::endl;
            
        } else if (cmd == "help") {
            printHelp();

        } else if (cmd == "quit" || cmd == "exit") {
            std::cout << YELLOW << "  Save before quitting? (y/n) > " << RESET;
            std::string ans;
            std::getline(std::cin, ans);
            if (ans == "y" || ans == "Y" || ans == "yes") {
                saveGame();
                std::cout << GREEN << "  ✅ Game saved!" << RESET << std::endl;
            }
            std::cout << BLUE << " 👋 Game exiting..." << RESET << std::endl;
            running_ = false;

        } else {
            std::cout << RED << "  Unknown command. Type 'help'." << RESET << std::endl;
        }
    }
}

// =====================================================================
// checkAndMerge - 3 same-name same-star units merge into star+1
// =====================================================================
void Game::checkAndMerge() {
    bool merged = true;
    while (merged) {
        merged = false;
        const std::vector<Unit*>& bench = player_.getBench();

        for (size_t i = 0; i < bench.size() && !merged; ++i) {
            std::string name = bench[i]->getBaseName();
            int star = bench[i]->getStarLevel();
            if (star >= MAX_STAR_LEVEL) continue;

            // Find 2 more units with same name and star level
            std::vector<int> matches;
            matches.push_back((int)i);
            for (size_t j = i + 1; j < bench.size(); ++j) {
                if (bench[j]->getBaseName() == name && bench[j]->getStarLevel() == star) {
                    matches.push_back((int)j);
                    if ((int)matches.size() >= MERGE_COUNT) break;
                }
            }

            if ((int)matches.size() >= MERGE_COUNT) {
                // Keep the first, upgrade it, delete the other two
                Unit* keeper = bench[matches[0]];
                keeper->upgrade();

                // Remove from bench in reverse order to preserve indices
                for (int k = MERGE_COUNT - 1; k >= 1; --k) {
                    Unit* removed = player_.removeFromBench(matches[k]);
                    if (removed != nullptr) delete removed;
                }

                std::cout << "  ** MERGE! 3x " << name << " -> "
                          << keeper->getDisplayName() << " (Star "
                          << keeper->getStarLevel() << ") **" << std::endl;
                merged = true;  // check again for chain merges
            }
        }
    }
}

// =====================================================================
// battlePhase
// =====================================================================
bool Game::battlePhase() {
    // Clear AI side
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = AI_MIN_COL; c < BOARD_COLS; ++c)
            board_.removeUnit(r, c);

    // Generate AI army
    ai_.generateArmy(player_.getRoundsPlayed());
    ai_.placeUnits(board_);
    ai_.releaseArmy();

    // Apply synergies to player units
    std::vector<Unit*> pUnits = board_.getPlayerUnits();
    std::vector<SynergyBonus> synergies = Synergy::calculateSynergies(pUnits);

    std::cout << "\n  Initial formation:" << std::endl;
    board_.display();

    // Show armies
    pUnits = board_.getPlayerUnits();
    std::vector<Unit*> aUnits = board_.getAIUnits();
    std::cout << "  YOUR ARMY (" << pUnits.size() << "): ";
    for (size_t i = 0; i < pUnits.size(); ++i) {
        std::cout << pUnits[i]->getName();
        if (i + 1 < pUnits.size()) std::cout << ", ";
    }
    std::cout << std::endl;
    std::cout << "  ENEMY (" << aUnits.size() << "):     ";
    for (size_t i = 0; i < aUnits.size(); ++i) {
        std::cout << aUnits[i]->getName();
        if (i + 1 < aUnits.size()) std::cout << ", ";
    }
    std::cout << std::endl;

    // Display active synergies
    Synergy::displaySynergies(synergies);

    std::cout << "  Press Enter to begin (or 's' to skip)...";
    std::string input;
    std::getline(std::cin, input);
    if (input == "s" || input == "S") skipCombat_ = true;

    bool playerWon = resolveCombat();

    // Clear synergy bonuses
    pUnits = board_.getPlayerUnits();
    Synergy::clearSynergies(pUnits);

    // Cleanup surviving units
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c) {
            Unit* u = board_.getUnit(r, c);
            if (u != nullptr) {
                board_.removeUnit(r, c);
                delete u;
            }
        }

    return playerWon;
}

// =====================================================================
// resolveCombat - tick-by-tick with abilities
// =====================================================================
bool Game::resolveCombat() {
    int totalPlayerKills = 0, totalAIKills = 0;
    int totalPlayerDmg = 0, totalAIDmg = 0;
    int lastTick = 0;

    for (int tick = 1; tick <= MAX_COMBAT_TICKS; ++tick) {
        std::vector<Unit*> allUnits = board_.getAllUnits();

        bool playerAlive = false, aiAlive = false;
        for (size_t i = 0; i < allUnits.size(); ++i) {
            if (allUnits[i]->isAlive()) {
                if (allUnits[i]->isPlayerUnit()) playerAlive = true;
                else aiAlive = true;
            }
        }
        if (!playerAlive || !aiAlive) break;
        lastTick = tick;

        if (!skipCombat_) {
            std::cout << "\n  -------- Tick " << tick << " --------" << std::endl;
            board_.display();
            std::vector<Unit*> p = board_.getPlayerUnits();
            std::vector<Unit*> a = board_.getAIUnits();
            std::cout << "  YOU: ";
            for (size_t i = 0; i < p.size(); ++i) {
                std::cout << p[i]->getSymbol();
                if (p[i]->getStarLevel() > 1) std::cout << p[i]->getStarLevel();
                std::cout << "(" << p[i]->getHp() << ")";
                if (i + 1 < p.size()) std::cout << " ";
            }
            std::cout << "  vs  ENEMY: ";
            for (size_t i = 0; i < a.size(); ++i) {
                std::cout << a[i]->getSymbol() << "(" << a[i]->getHp() << ")";
                if (i + 1 < a.size()) std::cout << " ";
            }
            std::cout << std::endl << std::endl;
        }

        for (size_t i = 0; i < allUnits.size(); ++i) {
            Unit* unit = allUnits[i];
            if (!unit->isAlive()) continue;

            Unit* enemy = board_.findNearestEnemy(unit);
            if (enemy == nullptr) continue;

            int dist = unit->getDistanceTo(enemy);

            if (dist <= unit->getAttackRange()) {
                int dmg = performAttack(unit, enemy);
                // Class ability triggers
                performAbility(unit, enemy, allUnits);

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
                              << " moves toward " << enemy->getName() << std::endl;
                }
            }
        }

        cleanupDeadUnits();

        if (!skipCombat_) {
            std::cout << "\n  [Enter] next | [s] skip > ";
            std::string in;
            std::getline(std::cin, in);
            if (in == "s" || in == "S") {
                skipCombat_ = true;
                std::cout << "  >> Fast-forwarding..." << std::endl;
            }
        }
    }

    // Battle report
    std::vector<Unit*> pUnits = board_.getPlayerUnits();
    std::vector<Unit*> aUnits = board_.getAIUnits();
    bool playerWon = !pUnits.empty() && aUnits.empty();

    const int BW = 39;
    std::cout << std::endl;
    std::cout << "  +" << std::string(BW, '=') << "+" << std::endl;
    printBoxTitle("BATTLE REPORT", BW);
    std::cout << "  +" << std::string(BW, '=') << "+" << std::endl;
    printBoxLine("  Result: " + std::string(playerWon ? "VICTORY!" : "DEFEAT..."), BW);
    printBoxLine("  Ticks:  " + std::to_string(lastTick), BW);
    std::cout << "  +" << std::string(BW, '-') << "+" << std::endl;
    printBoxLine("  Your damage dealt:  " + std::to_string(totalPlayerDmg), BW);
    printBoxLine("  Your kills:         " + std::to_string(totalPlayerKills), BW);
    printBoxLine("  Enemy damage dealt: " + std::to_string(totalAIDmg), BW);
    printBoxLine("  Enemy kills:        " + std::to_string(totalAIKills), BW);

    if (playerWon && !pUnits.empty()) {
        std::cout << "  +" << std::string(BW, '-') << "+" << std::endl;
        printBoxLine("  Survivors:", BW);
        for (size_t i = 0; i < pUnits.size(); ++i) {
            std::string line = "    " + pUnits[i]->getName() + "  HP: "
                + std::to_string(pUnits[i]->getHp()) + "/"
                + std::to_string(pUnits[i]->getMaxHp());
            printBoxLine(line, BW);
        }
    }
    std::cout << "  +" << std::string(BW, '=') << "+" << std::endl;

    return playerWon;
}

// =====================================================================
// performAttack - basic attack with crit
// =====================================================================
int Game::performAttack(Unit* attacker, Unit* defender) {
    int damage = attacker->getAtk();
    bool crit = false;

    int critChance = BASE_CRIT_CHANCE + attacker->getCritBonus();
    if (rand() % 100 < critChance) {
        damage = (int)(damage * CRIT_MULTIPLIER);
        crit = true;
    }

    // TANK ability: 25% chance to block 40% damage
    if (defender->getClass() == TANK && rand() % 100 < 25) {
        int blocked = damage * 40 / 100;
        damage -= blocked;
        if (!skipCombat_) {
            std::cout << "  [BLOCK] " << defender->getName()
                      << " blocks " << blocked << " dmg!" << std::endl;
        }
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
        if (!defender->isAlive()) std::cout << "  << KILLED >>";
        else std::cout << "  [" << defender->getHp() << " HP]";
        std::cout << std::endl;
    }

    return damage;
}

// =====================================================================
// performAbility - class-specific abilities trigger after attack
// =====================================================================
void Game::performAbility(Unit* attacker, Unit* defender, std::vector<Unit*>& allUnits) {
    UnitClass cls = attacker->getClass();

    switch (cls) {
        case MAGE: {
            // AOE: 30% chance to deal 50% splash to adjacent enemies
            if (rand() % 100 < 30) {
                int splash = attacker->getAtk() / 2;
                for (size_t i = 0; i < allUnits.size(); ++i) {
                    Unit* u = allUnits[i];
                    if (u == defender || !u->isAlive()) continue;
                    if (u->isPlayerUnit() == attacker->isPlayerUnit()) continue;
                    if (attacker->getDistanceTo(u) <= 2) {
                        u->takeDamage(splash);
                        if (!skipCombat_) {
                            std::cout << "  [AOE] " << attacker->getName()
                                      << " splashes " << u->getName()
                                      << " for " << splash << " dmg!" << std::endl;
                        }
                    }
                }
            }
            break;
        }
        case ASSASSIN: {
            // Backstab: 25% chance to deal extra 50% damage
            if (rand() % 100 < 25 && defender->isAlive()) {
                int bonus = attacker->getAtk() / 2;
                defender->takeDamage(bonus);
                if (!skipCombat_) {
                    std::cout << "  [BACKSTAB] " << attacker->getName()
                              << " strikes again for " << bonus << " dmg!" << std::endl;
                }
            }
            break;
        }
        case WARRIOR: {
            // Rage: below 50% HP, gain +30% ATK (applied once)
            if (attacker->getHp() < attacker->getMaxHp() / 2) {
                // The bonus is temporary for display - applied via synergy system
                if (!skipCombat_ && rand() % 100 < 10) {
                    std::cout << "  [RAGE] " << attacker->getName()
                              << " fights harder at low HP!" << std::endl;
                }
            }
            break;
        }
        case ARCHER: {
            // Double Shot: 20% chance to attack twice
            if (rand() % 100 < 20 && defender->isAlive()) {
                int dmg2 = attacker->getAtk();
                defender->takeDamage(dmg2);
                if (!skipCombat_) {
                    std::cout << "  [DOUBLE SHOT] " << attacker->getName()
                              << " fires again for " << dmg2 << " dmg!" << std::endl;
                }
            }
            break;
        }
        default:
            break;
    }
}

// =====================================================================
// cleanupDeadUnits
// =====================================================================
void Game::cleanupDeadUnits() {
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c) {
            Unit* u = board_.getUnit(r, c);
            if (u != nullptr && !u->isAlive()) {
                board_.removeUnit(r, c);
                delete u;
            }
        }
}

// =====================================================================
// saveRecord - append to records.txt
// =====================================================================
void Game::saveRecord() const {
    std::ofstream file(RECORD_FILE, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "  Warning: could not write to " << RECORD_FILE << std::endl;
        return;
    }
    file << player_.getName() << " "
         << player_.getRoundsPlayed() << " "
         << player_.getGold() << " "
         << ai_.getDifficultyString() << std::endl;
    file.close();
    std::cout << "\n  Record saved." << std::endl;
}

// =====================================================================
// saveGame - write full game state to savegame.dat
// =====================================================================
void Game::saveGame() const {
    std::ofstream file(SAVE_FILE);
    if (!file.is_open()) {
        std::cerr << "  Could not save game." << std::endl;
        return;
    }
    // Save player state
    file << player_.getHp() << " " << player_.getGold() << " "
         << player_.getRoundsPlayed() << " " << player_.getWinStreak() << " "
         << player_.getLossStreak() << std::endl;

    // Save bench units
    const std::vector<Unit*>& bench = player_.getBench();
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

    // Save difficulty
    file << ai_.getDifficultyString() << std::endl;
    file.close();
}

// =====================================================================
// loadGame - read full game state from savegame.dat
// =====================================================================
bool Game::loadGame() {
    std::ifstream file(SAVE_FILE);
    if (!file.is_open()) return false;

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
        // Create unit with base stats (before star scaling)
        // We store post-upgrade stats, so use them directly
        Unit* u = new Unit(name, (UnitClass)cls, maxHp, atk, cost, critBonus, range);
        // Set star level by calling upgrade() the right number of times
        // But since we saved post-upgrade maxHp/atk, just create with those stats
        // and manually set star level
        for (int s = 1; s < starLevel; ++s) {
            u->upgrade();
        }
        loadedUnits.push_back(u);
    }

    // Read difficulty
    std::string diff;
    file >> diff;
    file.close();

    // Apply loaded state to player
    player_.loadState(hp, gold, rounds, winStreak, lossStreak);

    // Add loaded units to bench
    for (size_t i = 0; i < loadedUnits.size(); ++i) {
        if (!player_.addToBench(loadedUnits[i])) {
            delete loadedUnits[i];  // bench full, shouldn't happen
        }
    }

    std::cout << "  Game loaded! Round " << rounds
              << ", HP: " << hp << ", Gold: " << gold
              << ", Bench: " << benchCount << " units" << std::endl;
    return true;
}

// =====================================================================
// displayLeaderboard
// =====================================================================
void Game::displayLeaderboard() {
    std::ifstream file(RECORD_FILE);
    if (!file.is_open()) {
        std::cout << BOLD << YELLOW << "\n  No records found. Play a game first!" << std::endl;
        return;
    }

    struct Record {
        std::string name;
        int rounds, gold;
        std::string difficulty;
    };

    std::vector<Record> records;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Record rec;
        if (iss >> rec.name >> rec.rounds >> rec.gold >> rec.difficulty)
            records.push_back(rec);
    }
    file.close();

    if (records.empty()) {
        std::cout << "\n  No records found." << std::endl;
        return;
    }

    for (size_t i = 0; i < records.size(); ++i)
        for (size_t j = i + 1; j < records.size(); ++j)
            if (records[j].rounds > records[i].rounds) {
                Record t = records[i]; records[i] = records[j]; records[j] = t;
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
// printHelp
// =====================================================================
void Game::printHelp() const {
    const int W = 50;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;

    std::cout << BOLD << CYAN;
    printBoxTitle("COMMANDS", W);
    std::cout << RESET;

    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;

    // SHOPPING
    std::cout << BOLD << YELLOW;
    printBoxLine("  SHOPPING:", W);
    std::cout << RESET;

    printBoxLine("    buy 1-5        Buy unit from shop slot", W);
    printBoxLine("    sell 1-N       Sell unit from bench", W);
    printBoxLine("    refresh        Re-roll shop ($" + std::to_string(shop_.getRefreshCost()) + ")", W);

    // BAGS
    std::cout << BOLD << YELLOW;
    printBoxLine("  BAGS:", W);
    std::cout << RESET;

    printBoxLine("    gold           Number of gold remained", W);
    printBoxLine("    status         HP, Gold, Round, Win Streak", W);

    // FORMATION
    std::cout << BOLD << YELLOW;
    printBoxLine("  FORMATION:", W);
    std::cout << RESET;

    printBoxLine("    place 1 2 3    Bench# -> row col", W);
    printBoxLine("    remove 2 3     Pick up unit at row col", W);
    printBoxLine("    auto           Auto-place all units", W);
    printBoxLine("    formation      Show current formation", W);

    // GAME
    std::cout << BOLD << YELLOW;
    printBoxLine("  GAME:", W);
    std::cout << RESET;

    printBoxLine("    ready          Start the battle!", W);
    printBoxLine("    save           Save game to file", W);
    printBoxLine("    quit           Exit", W);

    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    std::cout << BOLD << GREEN << "  SYNERGIES: 2+ same class = bonus. 3+ = stronger!" << RESET << std::endl;
    std::cout << BOLD << GREEN << "  MERGE: Buy 3 of same unit -> auto-upgrade to next star!" << RESET << std::endl;
    std::cout << BOLD << YELLOW << "  Tip: Tanks in col 3 (front), Archers in col 0 (back)" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
}
