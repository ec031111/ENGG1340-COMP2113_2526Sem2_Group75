#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iomanip>

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

// Helper: build an HP bar string of given width
// e.g. hpBar(current, max, 10) => "===       "
static std::string hpBar(int hp, int maxHp, int width) {
    if (maxHp <= 0) return std::string(width, ' ');
    int filled = (hp * width) / maxHp;
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;
    return std::string(filled, '=') + std::string(width - filled, ' ');
}

// Helper: preferred column for smart placement by class
static int smartPlaceCol(UnitClass cls) {
    switch (cls) {
        case TANK:     return 3;  // front line
        case WARRIOR:  return 3;
        case ARCHER:   return 0;  // back line
        case MAGE:     return 0;
        case ASSASSIN: return 1;  // middle
        default:       return 2;
    }
}

// Helper: smart-place a unit on the player side
static bool smartPlaceUnit(Board& board, Unit* unit) {
    int pc = smartPlaceCol(unit->getClass());
    for (int off = 0; off <= PLAYER_MAX_COL; ++off) {
        for (int d = 0; d < 2; ++d) {
            int c = (d == 0) ? pc - off : pc + off;
            if (c < 0 || c > PLAYER_MAX_COL) continue;
            if (off == 0 && d == 1) continue;
            for (int r = 0; r < BOARD_ROWS; ++r) {
                if (board.isEmpty(r, c)) {
                    board.placeUnit(unit, r, c);
                    return true;
                }
            }
        }
    }
    return false;
}

// Helper: ability name for shop display
static std::string abilityName(UnitClass cls) {
    switch (cls) {
        case WARRIOR:  return "Rage";
        case MAGE:     return "AOE";
        case TANK:     return "Block";
        case ASSASSIN: return "Crit";
        case ARCHER:   return "DblShot";
        default:       return "?";
    }
}

// Helper: ability description for info command
static std::string abilityDesc(UnitClass cls) {
    switch (cls) {
        case WARRIOR:  return "Below 50% HP: +30% ATK (once per combat)";
        case MAGE:     return "30% chance: 50% splash to nearby enemies";
        case TANK:     return "25% chance: block 40% of incoming damage";
        case ASSASSIN: return "25% chance: extra 50% damage (backstab)";
        case ARCHER:   return "20% chance: attack again (can crit)";
        default:       return "Unknown";
    }
}

// =====================================================================
// Constructor / Destructor
// =====================================================================
Game::Game(Difficulty difficulty)
    : ai_(difficulty), running_(true), skipCombat_(false),
      currentEvent_(EVENT_NONE) {}

Game::~Game() {}

// =====================================================================
// showIntro - Story introduction for new games
// =====================================================================
void Game::showIntro() const {
    const int W = 60;
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '=') << "+" << std::endl;
    printBoxTitle("THE BATTLE FOR AETHORIA", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxLine("", W);
    printBoxLine("  In the war-torn realm of Aethoria, the Dark Lord", W);
    printBoxLine("  Malachar has raised an army of darkness. As the", W);
    printBoxLine("  last surviving commander of the Allied Forces,", W);
    printBoxLine("  you must recruit heroes, form your formation,", W);
    printBoxLine("  and fight wave after wave of enemies to protect", W);
    printBoxLine("  what remains of civilization.", W);
    printBoxLine("", W);
    printBoxLine("  Each round, Malachar's forces grow stronger.", W);
    printBoxLine("  Your task: survive as long as you can, and", W);
    printBoxLine("  perhaps... turn the tide of war.", W);
    printBoxLine("", W);
    std::cout << "  +" << std::string(W, '=') << "+" << std::endl;
    std::cout << "\n  [Press Enter to begin your campaign...]";
    std::string dummy;
    std::getline(std::cin, dummy);
}

// =====================================================================
// run - Master game loop
// =====================================================================
int Game::run(bool show_intro) {
    std::cout << std::endl;
    std::cout << "  +======================================+" << std::endl;
    std::cout << "  |       AUTO-BATTLER ARENA             |" << std::endl;
    std::cout << "  |       Difficulty: " << std::left << std::setw(19)
              << ai_.getDifficultyString() << "|" << std::endl;
    std::cout << "  +======================================+" << std::endl;
    std::cout << "\n  Type 'help' for commands. 'save' to save game.\n" << std::endl;

    // Show story intro for new games only
    if (show_intro) {
        showIntro();
        printHelp();  // Show full command list after intro
    } else {
        printCommandTips();  // Show quick commands if loading saved game
    }

    while (running_ && player_.isAlive()) {
        player_.startNewRound();

        // --- Round flavor text ---
        {
            int round = player_.getRoundsPlayed();
            std::cout << std::endl;
            if (round == 1) {
                std::cout << "  ~ The first wave approaches... ~" << std::endl;
            } else if (round == 5) {
                std::cout << "  ~ The enemy grows restless... ~" << std::endl;
            } else if (round == 10) {
                std::cout << "  ~ A formidable army approaches! ~" << std::endl;
            } else if (round == 15) {
                std::cout << "  ~ The Dark Lord himself sends his elite guard! ~" << std::endl;
            } else if (round >= 20) {
                std::cout << "  ~ The final battle rages on... ~" << std::endl;
            } else {
                const std::string flavor[] = {
                    "  ~ The winds of war howl across the battlefield. ~",
                    "  ~ Malachar's minions march ever onward. ~",
                    "  ~ The scent of battle fills the air. ~",
                    "  ~ Your soldiers steady themselves for the coming fight. ~"
                };
                std::cout << flavor[rand() % 4] << std::endl;
            }
            std::cout << std::endl;
        }

        // --- Random event ---
        currentEvent_ = Event::rollEvent(player_.getRoundsPlayed());
        handleEvent();

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
            const std::string victoryMessages[] = {
                "  >> The enemy retreats! Victory is yours! <<",
                "  >> Your forces prevail! The Dark Army scatters! <<",
                "  >> A glorious victory! The Allied Forces stand strong! <<"
            };
            std::cout << "\n" << victoryMessages[rand() % 3] << std::endl;
        } else {
            player_.recordLoss();
            int damage = LOSS_DAMAGE_BASE
                + (ai_.getArmySize() * LOSS_DAMAGE_PER_SURVIVING);
            player_.takeDamage(damage);
            const std::string defeatMessages[] = {
                "  >> Your forces fall... the Dark Army advances. <<",
                "  >> Defeat! Your soldiers couldn't hold the line. <<"
            };
            std::cout << "\n" << defeatMessages[rand() % 2]
                      << " You take " << damage << " damage." << std::endl;
        }

        board_.clear();
        currentEvent_ = EVENT_NONE;

        if (!player_.isAlive()) {
            const int GW = 50;
            int rounds = player_.getRoundsPlayed();
            int maxHp = STARTING_HP;  // From player.h
            
            std::cout << std::endl;
            std::cout << "  +" << std::string(GW, '=') << "+" << std::endl;
            printBoxTitle("GAME OVER", GW);
            std::string t2 = "You survived " + std::to_string(rounds) + " rounds!";
            printBoxTitle(t2, GW);
            std::cout << "  +" << std::string(GW, '-') << "+" << std::endl;

            // Narrative text based on rounds survived
            std::string narrative;
            if (rounds < 5) {
                narrative = "The Dark Lord's forces overwhelmed you...";
            } else if (rounds < 10) {
                narrative = "You fought bravely, but Malachar's army was too strong.";
            } else if (rounds < 15) {
                narrative = "A valiant effort! Your name will be remembered.";
            } else if (rounds < 20) {
                narrative = "Legendary commander! You held the line longer than anyone.";
            } else {
                narrative = "IMMORTAL! The bards will sing of your deeds forever!";
            }
            printBoxLine("  " + narrative, GW);
            std::cout << "  +" << std::string(GW, '-') << "+" << std::endl;
            
            // Final HP display with blood bar
            std::ostringstream hpLine;
            hpLine << "  Final HP: 0/" << maxHp << " [";
            hpLine << std::string(20, ' ');  // Empty bar since HP is 0
            hpLine << "]";
            printBoxLine(hpLine.str(), GW);
            
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
        std::cout << "\n  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << "  !  EVENT: " << desc << std::endl;
        std::cout << "  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
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
// shopPhase - Buy, sell, place, merge, then ready
// =====================================================================
void Game::shopPhase() {
    std::string line;
    bool ready = false;
    bool shouldShow = true;  // Flag to control when to clear and show full screen

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
        // Only clear and show full screen when needed
        if (shouldShow) {
            // Clear screen for clean pagination
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            
            // Display status bar
            printStatusBar();
            std::cout << std::endl;
            
            // Display shop
            shop_.setPlayerGold(player_.getGold());
            shop_.display();
            std::cout << std::endl;
            
            // Always show commands before bench
            printCommandTips();
            std::cout << std::endl;
            
            // Display bench
            player_.displayBench();
            std::cout << std::endl;
            
            shouldShow = false;  // Don't re-show until a successful action
        }

        std::cout << "  Command > ";
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
                std::cout << "  Usage: buy 1  (slot number 1-5)" << std::endl;
                continue;
            }
            slot--;
            Unit* u = shop_.getUnit(slot);
            if (u == nullptr) {
                std::cout << "  That slot is empty." << std::endl;
                continue;
            }
            int cost = u->getCost();
            // Apply shop discount event
            if (currentEvent_ == EVENT_SHOP_DISCOUNT) {
                cost = (cost > 1) ? cost - 1 : 1;
            }
            if (!player_.spendGold(cost)) {
                std::cout << "  Not enough gold! (need $" << cost
                          << ", have $" << player_.getGold() << ")" << std::endl;
                continue;
            }
            Unit* bought = shop_.buyUnit(slot);
            if (!player_.addToBench(bought)) {
                std::cout << "  Bench is full! Refunding gold." << std::endl;
                player_.addGold(cost);
                delete bought;
                continue;
            }
            std::cout << "  + Purchased " << bought->getName() << "!" << std::endl;
            // Check for 3-merge after each purchase
            checkAndMerge();
            player_.displayStatus();
            shouldShow = true;  // Refresh display after successful buy

        } else if (cmd == "sell") {
            int idx;
            if (!(iss >> idx)) {
                std::cout << "  Usage: sell 1  (bench index)" << std::endl;
                continue;
            }
            idx--;
            if (!player_.sellUnit(idx)) {
                std::cout << "  Invalid bench index." << std::endl;
                continue;
            }
            player_.displayStatus();
            shouldShow = true;  // Refresh display after successful sell

        } else if (cmd == "place") {
            int idx, row, col;
            if (!(iss >> idx >> row >> col)) {
                std::cout << "  Usage: place 1 2 3  (bench_index row col)" << std::endl;
                continue;
            }
            idx--;
            if (col < 0 || col > PLAYER_MAX_COL || row < 0 || row >= BOARD_ROWS) {
                std::cout << "  Position out of range. Row:0-4, Col:0-3" << std::endl;
                continue;
            }
            if (!board_.isEmpty(row, col)) {
                std::cout << "  That cell is occupied!" << std::endl;
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
            shouldShow = true;  // Refresh display after successful place

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
            if (unit == nullptr || !unit->isPlayerUnit()) {
                std::cout << "  No unit of yours at that position." << std::endl;
                continue;
            }
            board_.removeUnit(row, col);
            player_.addToBench(unit);
            std::cout << "  Removed " << unit->getName() << " back to bench." << std::endl;
            shouldShow = true;  // Refresh display after successful remove

        } else if (cmd == "formation" || cmd == "form" || cmd == "f") {
            board_.displayPlayerSide();

        } else if (cmd == "auto") {
            int benchSize = player_.getBenchSize();
            int placed = 0;
            for (int i = 0; i < benchSize; ++i) {
                Unit* unit = player_.removeFromBench(0);
                if (unit == nullptr) continue;
                if (smartPlaceUnit(board_, unit)) {
                    placed++;
                } else {
                    player_.addToBench(unit);
                    break;
                }
            }
            std::cout << "  Smart-placed " << placed << " units." << std::endl;
            board_.displayPlayerSide();
            shouldShow = true;  // Refresh display after successful auto

        } else if (cmd == "refresh") {
            if (!player_.spendGold(shop_.getRefreshCost())) {
                std::cout << "  Not enough gold! (need $" << shop_.getRefreshCost() << ")" << std::endl;
                continue;
            }
            shop_.refresh();
            std::cout << "  Shop refreshed!" << std::endl;
            player_.displayStatus();
            shouldShow = true;  // Refresh display after successful refresh

        } else if (cmd == "save") {
            saveGame();
            std::cout << "  Game saved!" << std::endl;

        } else if (cmd == "ready") {
            // Smart-place bench leftovers
            int benchSize = player_.getBenchSize();
            for (int i = 0; i < benchSize; ++i) {
                Unit* unit = player_.removeFromBench(0);
                if (unit == nullptr) continue;
                if (!smartPlaceUnit(board_, unit)) {
                    player_.addToBench(unit);
                    break;
                }
            }
            if (board_.getPlayerUnits().empty()) {
                std::cout << "  No units! Buy some first." << std::endl;
                continue;
            }
            ready = true;

        } else if (cmd == "info") {
            std::string target;
            if (!(iss >> target)) {
                std::cout << "  Usage: info <bench#>  or  info shop <slot#>" << std::endl;
                continue;
            }
            Unit* infoUnit = nullptr;
            if (target == "shop") {
                int slot;
                if (!(iss >> slot)) {
                    std::cout << "  Usage: info shop <1-5>" << std::endl;
                    continue;
                }
                slot--;
                infoUnit = shop_.getUnit(slot);
                if (infoUnit == nullptr) {
                    std::cout << "  That shop slot is empty." << std::endl;
                    continue;
                }
            } else {
                // Try parsing as bench index
                int idx = 0;
                std::istringstream ts(target);
                if (!(ts >> idx)) {
                    std::cout << "  Usage: info <bench#>  or  info shop <slot#>" << std::endl;
                    continue;
                }
                idx--;
                infoUnit = player_.getBenchUnit(idx);
                if (infoUnit == nullptr) {
                    std::cout << "  Invalid bench index." << std::endl;
                    continue;
                }
            }
            // Display detailed unit info
            const int IW = 45;
            std::cout << std::endl;
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;
            printBoxTitle("UNIT INFO", IW);
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;
            printBoxLine("  Name:   " + infoUnit->getDisplayName(), IW);
            printBoxLine("  Class:  " + infoUnit->getClassString(), IW);
            printBoxLine("  Star:   " + infoUnit->getStarString(), IW);
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;
            printBoxLine("  HP:     " + std::to_string(infoUnit->getMaxHp()), IW);
            printBoxLine("  ATK:    " + std::to_string(infoUnit->getAtk()), IW);
            printBoxLine("  Crit:   " + std::to_string(BASE_CRIT_CHANCE + infoUnit->getCritBonus()) + "%", IW);
            printBoxLine("  Range:  " + std::to_string(infoUnit->getAttackRange()), IW);
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;
            printBoxLine("  Ability: " + abilityName(infoUnit->getClass()), IW);
            printBoxLine("  " + abilityDesc(infoUnit->getClass()), IW);
            printBoxLine("  Lore: " + Unit::getClassDescription(infoUnit->getClass()), IW);
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;
            printBoxLine("  Cost: $" + std::to_string(infoUnit->getCost())
                         + "  |  Sell: $" + std::to_string(infoUnit->getSellPrice()), IW);
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;

        } else if (cmd == "help") {
            printHelp();

        } else if (cmd == "menu") {
            // Return to main menu without asking
            running_ = false;

        } else if (cmd == "quit" || cmd == "exit") {
            std::cout << "  Save before quitting? (y/n) > ";
            std::string ans;
            std::getline(std::cin, ans);
            if (ans == "y" || ans == "Y" || ans == "yes") {
                saveGame();
                std::cout << "  Game saved!" << std::endl;
            }
            running_ = false;

        } else {
            std::cout << "  Unknown command. Type 'help'." << std::endl;
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
    std::cout << "  ALLIED FORCES (" << pUnits.size() << "): ";
    for (size_t i = 0; i < pUnits.size(); ++i) {
        std::cout << pUnits[i]->getName();
        if (i + 1 < pUnits.size()) std::cout << ", ";
    }
    std::cout << std::endl;
    std::cout << "  Dark Army (" << aUnits.size() << "):      ";
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

    // Clear rage flags for all units at combat start
    std::vector<Unit*> allUnitsInit = board_.getAllUnits();
    for (size_t i = 0; i < allUnitsInit.size(); ++i) {
        allUnitsInit[i]->clearRage();
    }

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
            std::cout << "  ALLIED: ";
            for (size_t i = 0; i < p.size(); ++i) {
                std::cout << p[i]->getSymbolString()
                          << "[" << hpBar(p[i]->getHp(), p[i]->getMaxHp(), 10) << "]";
                if (i + 1 < p.size()) std::cout << " ";
            }
            std::cout << "  vs  DARK ARMY: ";
            for (size_t i = 0; i < a.size(); ++i) {
                std::cout << a[i]->getSymbolString()
                          << "[" << hpBar(a[i]->getHp(), a[i]->getMaxHp(), 10) << "]";
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
            // Rage: below 50% HP, permanently gain +30% ATK (once per combat)
            if (!attacker->isRaged() && attacker->getHp() < attacker->getMaxHp() / 2) {
                int rageBonus = attacker->getAtk() * 30 / 100;
                attacker->applyAtkBonus(rageBonus);
                attacker->setRaged();
                if (!skipCombat_) {
                    std::cout << "  [RAGE] " << attacker->getName()
                              << " enters a berserker rage! +" << rageBonus << " ATK!" << std::endl;
                }
            }
            break;
        }
        case ARCHER: {
            // Double Shot: 20% chance to attack again (can crit)
            if (rand() % 100 < 20 && defender->isAlive()) {
                int dmg2 = attacker->getAtk();
                bool crit2 = false;
                int critChance = BASE_CRIT_CHANCE + attacker->getCritBonus();
                if (rand() % 100 < critChance) {
                    dmg2 = (int)(dmg2 * CRIT_MULTIPLIER);
                    crit2 = true;
                }
                // Tank block for double shot too
                if (defender->getClass() == TANK && rand() % 100 < 25) {
                    int blocked = dmg2 * 40 / 100;
                    dmg2 -= blocked;
                    if (!skipCombat_) {
                        std::cout << "  [BLOCK] " << defender->getName()
                                  << " blocks " << blocked << " dmg from double shot!" << std::endl;
                    }
                }
                defender->takeDamage(dmg2);
                if (!skipCombat_) {
                    if (crit2) {
                        std::cout << "  [DOUBLE SHOT] " << attacker->getName()
                                  << " fires a CRIT for " << dmg2 << " dmg!" << std::endl;
                    } else {
                        std::cout << "  [DOUBLE SHOT] " << attacker->getName()
                                  << " fires again for " << dmg2 << " dmg!" << std::endl;
                    }
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
        // Create unit with saved post-upgrade stats directly.
        // Do NOT call upgrade() again — the stats are already scaled.
        Unit* u = new Unit(name, (UnitClass)cls, maxHp, atk, cost, critBonus, range);
        // Manually set star level without re-scaling stats
        for (int s = 1; s < starLevel; ++s) {
            u->forceSetStarLevel(s + 1);
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
        std::cout << "\n  No records found. Play a game first!" << std::endl;
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
    const int W = 55;
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxTitle("COMMANDS", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxLine("  SHOPPING:", W);
    printBoxLine("    buy 1-5        Buy unit from shop slot", W);
    printBoxLine("    sell 1-N       Sell unit from bench", W);
    printBoxLine("    refresh        Re-roll shop ($" + std::to_string(shop_.getRefreshCost()) + ")", W);
    printBoxLine("    info N         Show bench unit details", W);
    printBoxLine("    info shop N    Show shop unit details", W);
    printBoxLine("  FORMATION:", W);
    printBoxLine("    place 1 2 3    Bench# -> row col", W);
    printBoxLine("    remove 2 3     Pick up unit at row col", W);
    printBoxLine("    auto           Smart-place (by class)", W);
    printBoxLine("    formation      Show current formation", W);
    printBoxLine("  GAME:", W);
    printBoxLine("    ready          Start the battle!", W);
    printBoxLine("    save           Save game to file", W);
    printBoxLine("    menu           Return to main menu", W);
    printBoxLine("    quit           Exit game completely", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxTitle("CLASS ABILITIES", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxLine("  Warrior [Rage]    : Below 50% HP: +30% ATK", W);
    printBoxLine("  Mage    [AOE]     : 30% splash to nearby enemies", W);
    printBoxLine("  Tank    [Block]   : 25% chance: block 40% dmg", W);
    printBoxLine("  Assassin [Crit]   : 25% extra damage (backstab)", W);
    printBoxLine("  Archer  [DblShot] : 20% chance: attack twice", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxTitle("TIPS", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxLine("  SYNERGIES: 2+ same class = bonus. 3+ = stronger!", W);
    printBoxLine("  MERGE: Buy 3 of same unit -> auto-upgrade star!", W);
    printBoxLine("  INTEREST: Earn 1 gold per 10 saved (max +5/round)", W);
    printBoxLine("  FORMATION: Tanks front (col3), Mages back (col0)", W);
    printBoxLine("  Assassins in middle (col1) for balanced engage", W);
    printBoxLine("  Saving gold earns interest - balance spending!", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
}

// =====================================================================
// printCommandTips - Compact command menu for reference during gameplay
// =====================================================================
void Game::printCommandTips() const {
    const int W = 55;
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxTitle("QUICK COMMANDS", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxLine("  buy <1-5>  sell <idx>  refresh  info <idx>", W);
    printBoxLine("  place <idx> <row> <col>  remove <row> <col>", W);
    printBoxLine("  auto    ready    formation    help    menu", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
}

// =====================================================================
// printStatusBar - Compact player status (Gold + HP bar)
// =====================================================================
void Game::printStatusBar() const {
    const int W = 62;
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    
    // Line 1: Gold
    std::ostringstream line1;
    line1 << "  Gold: " << player_.getGold();
    std::string s1 = line1.str();
    if ((int)s1.size() < W) s1 += std::string(W - s1.size(), ' ');
    std::cout << "  |" << s1 << "|" << std::endl;
    
    // Line 2: HP with bar
    const int HP_BAR_WIDTH = 25;
    int hp = player_.getHp();
    int maxHp = STARTING_HP;
    int filled = (hp * HP_BAR_WIDTH) / maxHp;
    if (filled < 0) filled = 0;
    if (filled > HP_BAR_WIDTH) filled = HP_BAR_WIDTH;
    
    std::string hpBar = "[" + std::string(filled, '=') + std::string(HP_BAR_WIDTH - filled, ' ') + "]";
    std::ostringstream line2;
    line2 << "  HP: " << std::left << std::setw(3) << hp 
          << "/" << maxHp << " " << hpBar;
    std::string s2 = line2.str();
    if ((int)s2.size() < W) s2 += std::string(W - s2.size(), ' ');
    std::cout << "  |" << s2 << "|" << std::endl;
    
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
}
