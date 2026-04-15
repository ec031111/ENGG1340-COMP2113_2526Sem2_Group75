//=====================================================================
//Project Name: Auto-Battler Arena
//Group Number: ENGG1340-COMP2113_2526Sem2_Group75
//File Type: Implementation File (.cpp)
//File Description:
//  Implementation of the Player class. Provides methods for:
//  - Managing player resources (HP, gold, units)
//  - Recording battle results and updating win/loss streaks
//  - Calculating income with streak bonuses and interest
//  - Unit acquisition, selling, and bench management
//  - Display and tracking of player statistics
// =====================================================================

#include "player.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// Color codes for terminal output
#define BOLD        "\033[1m"
#define RESET       "\033[0m"
#define CYAN        "\033[36m"
#define YELLOW      "\033[33m"
#define BR_YELLOW   "\033[93m"
#define BR_CYAN     "\033[96m"
#define BR_GREEN    "\033[92m"
#define BR_RED      "\033[91m"

// -----------------------------------------------------------------
// Constructor
// What it does : sets up the player with starting HP, gold, and
//                empty bench / streak counters.
// Input  : name
// Output : initialised Player
// -----------------------------------------------------------------
Player::Player(const std::string& name)
    : name_(name), hp_(STARTING_HP), gold_(STARTING_GOLD),
      winStreak_(0), lossStreak_(0), roundsPlayed_(0) {}

// -----------------------------------------------------------------
// Destructor
// What it does : deletes all units remaining on the bench.
// Input  : none
// Output : none (frees dynamic memory)
// -----------------------------------------------------------------
Player::~Player() {
    for (size_t i = 0; i < bench_.size(); ++i) {
        delete bench_[i];
    }
    bench_.clear();
}

// --- Getters ---
std::string Player::getName() const    { return name_; }
int  Player::getHp() const            { return hp_; }
int  Player::getGold() const          { return gold_; }
int  Player::getBenchSize() const     { return (int)bench_.size(); }
int  Player::getWinStreak() const     { return winStreak_; }
int  Player::getLossStreak() const    { return lossStreak_; }
int  Player::getRoundsPlayed() const  { return roundsPlayed_; }
bool Player::isAlive() const          { return hp_ > 0; }

// -----------------------------------------------------------------
// addGold
// What it does : increases the player's gold.
// Input  : amount (positive integer)
// Output : none
// -----------------------------------------------------------------
void Player::addGold(int amount) {
    gold_ += amount;
}

// -----------------------------------------------------------------
// spendGold
// What it does : deducts gold if the player can afford it.
// Input  : amount
// Output : true if transaction succeeded, false if insufficient gold
// -----------------------------------------------------------------
bool Player::spendGold(int amount) {
    if (gold_ < amount) return false;
    gold_ -= amount;
    return true;
}

// -----------------------------------------------------------------
// takeDamage
// What it does : reduces HP, floored at 0.
// Input  : damage
// Output : none
// -----------------------------------------------------------------
void Player::takeDamage(int damage) {
    hp_ -= damage;
    if (hp_ < 0) hp_ = 0;
}

// -----------------------------------------------------------------
// addToBench
// What it does : appends a unit to the bench if space is available.
// Input  : unit (caller transfers ownership)
// Output : true on success, false if bench is full
// -----------------------------------------------------------------
bool Player::addToBench(Unit* unit) {
    if ((int)bench_.size() >= MAX_BENCH_SIZE) return false;
    bench_.push_back(unit);
    return true;
}

// -----------------------------------------------------------------
// removeFromBench
// What it does : removes and returns the unit at the given index.
// Input  : index (0-based)
// Output : Unit* (caller takes ownership), nullptr if invalid
// -----------------------------------------------------------------
Unit* Player::removeFromBench(int index) {
    if (index < 0 || index >= (int)bench_.size()) return nullptr;
    Unit* unit = bench_[index];
    bench_.erase(bench_.begin() + index);
    return unit;
}

// -----------------------------------------------------------------
// sellUnit
// What it does : sells a bench unit — adds gold refund and deletes it.
// Input  : index
// Output : true on success
// -----------------------------------------------------------------
bool Player::sellUnit(int index) {
    if (index < 0 || index >= (int)bench_.size()) return false;
    Unit* unit = bench_[index];
    gold_ += unit->getSellPrice();
    std::cout << "  Sold " << unit->getName() << " for "
              << unit->getSellPrice() << " gold." << std::endl;
    delete unit;
    bench_.erase(bench_.begin() + index);
    return true;
}

// -----------------------------------------------------------------
// getBenchUnit
// What it does : peeks at a bench unit without removing it.
// Input  : index
// Output : Unit* or nullptr
// -----------------------------------------------------------------
Unit* Player::getBenchUnit(int index) const {
    if (index < 0 || index >= (int)bench_.size()) return nullptr;
    return bench_[index];
}

// -----------------------------------------------------------------
// displayBench
// What it does : prints a formatted list of bench units.
// Input  : none
// Output : none (stdout)
// -----------------------------------------------------------------
void Player::displayBench() const {
    std::cout << "\n  --- Your Bench (" << bench_.size() << "/"
              << MAX_BENCH_SIZE << ") ---" << std::endl;
    if (bench_.empty()) {
        std::cout << "  (empty)" << std::endl;
        return;
    }
    for (size_t i = 0; i < bench_.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] "
                  << std::left << std::setw(10) << bench_[i]->getName()
                  << " " << std::setw(9) << bench_[i]->getClassString()
                  << "HP:" << std::setw(4) << bench_[i]->getMaxHp()
                  << "ATK:" << std::setw(4) << bench_[i]->getAtk()
                  << "Sell: $" << bench_[i]->getSellPrice()
                  << std::endl;
    }
}

// -----------------------------------------------------------------
// displayStatus
// What it does : prints the player's HP, gold, and streak info with colored output.
// Input  : none
// Output : none (stdout)
// -----------------------------------------------------------------
void Player::displayStatus() const {
    const int W = 55;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;

    // Line 1: Player name and gold
    std::ostringstream line1;
    line1 << BOLD << BR_YELLOW << "  " << std::left << std::setw(15) << name_
          << "Gold: " << std::left << std::setw(6) << gold_ << RESET;
    std::string s1 = line1.str();
    if ((int)s1.size() < W) s1 += std::string(W - s1.size(), ' ');
    std::cout << "  |" << s1 << "|" << std::endl;

    // Line 2: HP with visual bar
    const int HP_BAR_WIDTH = 20;
    int filled = (hp_ * HP_BAR_WIDTH) / STARTING_HP;
    if (filled < 0) filled = 0;
    if (filled > HP_BAR_WIDTH) filled = HP_BAR_WIDTH;
    
    std::string hpBar;
    if (hp_ > STARTING_HP * 0.5) {
        hpBar = "[" + std::string(BOLD) + std::string(BR_GREEN) 
              + std::string(filled, '=') 
              + std::string(RESET) 
              + std::string(HP_BAR_WIDTH - filled, ' ') 
              + "]";
    } else {
        hpBar = "[" + std::string(BOLD) + std::string(BR_RED) 
              + std::string(filled, '=') 
              + std::string(RESET) 
              + std::string(HP_BAR_WIDTH - filled, ' ') 
              + "]";
    }
    
    std::ostringstream line2;
    line2 << BOLD << BR_CYAN << "  HP: " << RESET
          << std::left << std::setw(3) << hp_ 
          << "/" << STARTING_HP << " " << hpBar;
    std::string s2 = line2.str();
    if ((int)s2.size() < W) s2 += std::string(W - s2.size(), ' ');
    std::cout << "  |" << s2 << "|" << std::endl;

    // Line 3: Round and streaks
    std::ostringstream line3;
    line3 << BOLD << BR_YELLOW << "  Round: " << std::left << std::setw(5) << roundsPlayed_
          << "Win: " << winStreak_ << "  Loss: " << lossStreak_ << RESET;
    std::string s3 = line3.str();
    if ((int)s3.size() < W) s3 += std::string(W - s3.size(), ' ');
    std::cout << "  |" << s3 << "|" << std::endl;

    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
}

// -----------------------------------------------------------------
// startNewRound
// What it does : increments round counter and awards income.
//                Income = base gold + 1 per win streak level.
// Input  : none
// Output : none
// -----------------------------------------------------------------
void Player::startNewRound() {
    roundsPlayed_++;
    int baseIncome = GOLD_PER_ROUND;
    // Win streak bonus: 3 gold per win, only if streak >= 3
    int winStreakGold = (winStreak_ >= 3) ? WIN_STREAK_BONUS : 0;
    // Loss streak bonus: 2 gold per loss, only if streak >= 3
    int lossStreakGold = (lossStreak_ >= 3) ? LOSS_STREAK_BONUS : 0;
    // Interest: 1 gold per 10 saved, max 5 gold (25 gold earned, only at start of round)
    int interest = std::min(gold_ / 10, 5) * INTEREST_PER_10;
    int totalIncome = baseIncome + winStreakGold + lossStreakGold + interest;
    gold_ += totalIncome;
    // Clear screen before starting new round
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    
    std::cout << "\n  === Round " << roundsPlayed_ << " ===" << std::endl;
    std::cout << "  Income: " << totalIncome << " gold"
              << " (base:" << baseIncome;
    if (winStreak_ >= 3) {
        std::cout << " +win streak(" << winStreak_ << "):" << winStreakGold;
    }
    if (lossStreak_ >= 3) {
        std::cout << " +loss streak(" << lossStreak_ << "):" << lossStreakGold;
    }
    if (interest > 0) {
        std::cout << " +interest:" << interest;
    }
    std::cout << "). Total gold: " << gold_ << std::endl;
}

// -----------------------------------------------------------------
// recordWin / recordLoss
// What it does : updates streak counters after a battle.
// Input  : none
// Output : none
// -----------------------------------------------------------------
void Player::recordWin() {
    winStreak_++;
    lossStreak_ = 0;
}

void Player::recordLoss() {
    lossStreak_++;
    winStreak_ = 0;
}

const std::vector<Unit*>& Player::getBench() const {
    return bench_;
}

// -----------------------------------------------------------------
// loadState
// What it does : restores player state from saved values.
// Inputs : hp, gold, rounds, winStreak, lossStreak
// Output : none
// -----------------------------------------------------------------
void Player::loadState(int hp, int gold, int rounds, int winStreak, int lossStreak) {
    hp_ = hp;
    gold_ = gold;
    roundsPlayed_ = rounds;
    winStreak_ = winStreak;
    lossStreak_ = lossStreak;
}
