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
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define RED         "\033[31m"
#define BR_YELLOW   "\033[93m"
#define BR_BLUE     "\033[94m"
#define BR_PURPLE   "\033[95m"
#define BR_CYAN     "\033[96m"
#define BR_GREEN    "\033[92m"
#define BR_RED      "\033[91m"

static int visibleWidth(const std::string& text) {
    int width = 0;
    bool inEscape = false;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\033') {
            inEscape = true;
            continue;
        }
        if (inEscape) {
            if (text[i] == 'm') inEscape = false;
            continue;
        }

        unsigned char ch = static_cast<unsigned char>(text[i]);
        if ((ch & 0x80) == 0) {
            ++width;
        } else if ((ch & 0xE0) == 0xC0) {
            width += 2;
            ++i;
        } else if ((ch & 0xF0) == 0xE0) {
            width += 2;
            i += 2;
        } else if ((ch & 0xF8) == 0xF0) {
            width += 2;
            i += 3;
        }
    }
    return width;
}

static std::string padToWidth(std::string text, int width) {
    int display = visibleWidth(text);
    if (display < width) text += std::string(width - display, ' ');
    return text;
}

static std::pair<std::string, std::string> getClassColorEmoji(UnitClass cls) {
    switch (cls) {
        case WARRIOR: return {RED, "⚔️"};
        case MAGE: return {BR_BLUE, "🔮"};
        case TANK: return {BR_GREEN, "🛡️"};
        case ASSASSIN: return {BR_PURPLE, "🗡️"};
        case ARCHER: return {BR_YELLOW, "🏹"};
        default: return {CYAN, "•"};
    }
}

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
    std::cout << "\n  " << BOLD << BR_BLUE << "📦 Your Bench (" << bench_.size() << "/"
              << MAX_BENCH_SIZE << ")" << RESET << std::endl;
    if (bench_.empty()) {
        std::cout << "  " << YELLOW << "(empty)" << RESET << std::endl;
        return;
    }
    for (size_t i = 0; i < bench_.size(); ++i) {
        auto [color, emoji] = getClassColorEmoji(bench_[i]->getClass());
        std::string starDisplay;
        for (int level = 0; level < bench_[i]->getStarLevel(); ++level) {
            starDisplay += "*";
        }
        std::cout << "  [" << (i + 1) << "] "
                  << color << emoji << " " << RESET
                  << std::left << std::setw(10) << bench_[i]->getName()
                  << " " << std::setw(9) << bench_[i]->getClassString()
                  << "HP:" << std::setw(4) << bench_[i]->getMaxHp()
                  << "ATK:" << std::setw(4) << bench_[i]->getAtk()
                  << "Lv:" << BR_YELLOW << starDisplay << RESET
                  << " Sell: " << BR_YELLOW << "$" << bench_[i]->getSellPrice() << RESET
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
    const int W = 62;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;

    std::ostringstream line1;
    line1 << "  " << BOLD << BR_CYAN << "👤 " << RESET << std::left << std::setw(15) << name_
        << BOLD << BR_YELLOW << "💰 Gold: " << RESET << std::left << std::setw(6) << gold_;
    std::string s1 = padToWidth(line1.str(), W);
    std::cout << "  |" << s1 << "|" << std::endl;

    const int HP_BAR_WIDTH = 20;
    int filled = (hp_ * HP_BAR_WIDTH) / STARTING_HP;
    if (filled < 0) filled = 0;
    if (filled > HP_BAR_WIDTH) filled = HP_BAR_WIDTH;
    std::string hpColor = (hp_ > STARTING_HP * 0.75) ? BR_GREEN : ((hp_ > STARTING_HP * 0.25) ? BR_YELLOW : BR_RED);
    std::string hpBar = "[" + std::string(BOLD) + hpColor + std::string(filled, '=')
                + std::string(HP_BAR_WIDTH - filled, '-') + RESET + "]";
    
    std::ostringstream line2;
    line2 << "  " << BOLD << BR_RED << "❤️ HP: " << RESET
          << std::left << std::setw(3) << hp_ 
          << "/" << STARTING_HP << " " << hpBar;
    std::string s2 = padToWidth(line2.str(), W);
    std::cout << "  |" << s2 << "|" << std::endl;

    std::ostringstream line3;
    line3 << "  " << BOLD << BR_CYAN << "🎯 Round: " << RESET << std::left << std::setw(5) << roundsPlayed_
        << BR_GREEN << "✓ Win: " << winStreak_ << RESET
        << "  " << BR_RED << "✗ Loss: " << lossStreak_ << RESET;
    std::string s3 = padToWidth(line3.str(), W);
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
