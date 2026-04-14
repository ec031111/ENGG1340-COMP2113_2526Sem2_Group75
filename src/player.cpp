#include "player.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// ============== ANSI COLOR CODES ==============
const std::string ANSI_RED     = "\033[31m";
const std::string ANSI_GREEN   = "\033[32m";
const std::string ANSI_YELLOW  = "\033[33m";
const std::string ANSI_BLUE    = "\033[34m";
const std::string ANSI_MAGENTA = "\033[35m";
const std::string ANSI_BRIGHT_RED = "\033[91m";
const std::string ANSI_BRIGHT_GREEN = "\033[92m";
const std::string ANSI_RESET   = "\033[0m";

// Get color + emoji based on unit class
static std::pair<std::string, std::string> getClassColorEmoji(UnitClass cls) {
    switch (cls) {
        case WARRIOR:  return {ANSI_RED,     "⚔️"};
        case MAGE:     return {ANSI_BLUE,    "🔮"};
        case TANK:     return {ANSI_GREEN,   "🛡️"};
        case ASSASSIN: return {ANSI_MAGENTA, "🗡️"};
        case ARCHER:   return {ANSI_YELLOW,  "🏹"};
        default:       return {ANSI_RESET,   "?"};
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
    std::cout << "\n  " << ANSI_BLUE << "📦 Your Bench (" << bench_.size() << "/"
              << MAX_BENCH_SIZE << ")" << ANSI_RESET << std::endl;
    if (bench_.empty()) {
        std::cout << "  (empty)" << std::endl;
        return;
    }
    for (size_t i = 0; i < bench_.size(); ++i) {
        auto [color, emoji] = getClassColorEmoji(bench_[i]->getClass());
        std::string stars = "";
        for (int s = 1; s < bench_[i]->getStarLevel(); ++s) stars += "⭐";
        
        std::cout << "  [" << (i + 1) << "] "
                  << color << emoji << ANSI_RESET << " "
                  << std::left << std::setw(12) << bench_[i]->getName()
                  << " HP:" << std::setw(3) << bench_[i]->getMaxHp()
                  << " ATK:" << std::setw(3) << bench_[i]->getAtk()
                  << " Sell: " << ANSI_YELLOW << "$" << bench_[i]->getSellPrice() 
                  << ANSI_RESET << " " << stars
                  << std::endl;
    }
}

// -----------------------------------------------------------------
// displayStatus
// What it does : prints the player's HP, gold, and streak info.
// Input  : none
// Output : none (stdout)
// -----------------------------------------------------------------
void Player::displayStatus() const {
    const int W = 60;
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '=') << "+" << std::endl;

    // Line 1: Player name and gold
    std::ostringstream line1;
    line1 << "  👤 " << std::left << std::setw(12) << name_
          << "💰 Gold: " << ANSI_YELLOW << std::left << std::setw(6) << gold_ 
          << ANSI_RESET;
    std::string s1 = line1.str();
    if ((int)s1.size() < W) s1 += std::string(W - s1.size(), ' ');
    std::cout << "  |" << s1 << "|" << std::endl;

    // Line 2: HP with visual bar
    const int HP_BAR_WIDTH = 25;
    int filled = (hp_ * HP_BAR_WIDTH) / STARTING_HP;
    if (filled < 0) filled = 0;
    if (filled > HP_BAR_WIDTH) filled = HP_BAR_WIDTH;
    
    std::string hpColor = (hp_ > STARTING_HP * 0.75) ? ANSI_GREEN : 
                          (hp_ > STARTING_HP * 0.25) ? ANSI_YELLOW : ANSI_RED;
    std::string hpBar = "[" + hpColor + std::string(filled, '=') + ANSI_RESET + 
                        std::string(HP_BAR_WIDTH - filled, '-') + "]";
    std::ostringstream line2;
    line2 << "  ❤️ " << std::left << std::setw(3) << hp_ 
          << "/" << STARTING_HP << " " << hpBar;
    std::string s2 = line2.str();
    if ((int)s2.size() < W) s2 += std::string(W - s2.size(), ' ');
    std::cout << "  |" << s2 << "|" << std::endl;

    // Line 3: Round and streaks
    std::string winColor = (winStreak_ > 0) ? ANSI_GREEN : ANSI_RESET;
    std::string lossColor = (lossStreak_ > 0) ? ANSI_RED : ANSI_RESET;
    
    std::ostringstream line3;
    line3 << "  🎯 Round: " << std::left << std::setw(3) << roundsPlayed_
          << "  " << winColor << "✓ Win: " << winStreak_ << ANSI_RESET 
          << "  " << lossColor << "✗ Loss: " << lossStreak_ << ANSI_RESET;
    std::string s3 = line3.str();
    if ((int)s3.size() < W) s3 += std::string(W - s3.size(), ' ');
    std::cout << "  |" << s3 << "|" << std::endl;

    std::cout << "  +" << std::string(W, '=') << "+" << std::endl;
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
    int winStreakGold = winStreak_ * WIN_STREAK_BONUS;
    int lossStreakGold = lossStreak_ * LOSS_STREAK_BONUS;
    int interest = std::min(gold_ / 10, 5) * INTEREST_PER_10;
    int totalIncome = baseIncome + winStreakGold + lossStreakGold + interest;
    gold_ += totalIncome;
    std::cout << "\n  === Round " << roundsPlayed_ << " ===" << std::endl;
    std::cout << "  Income: " << totalIncome << " gold"
              << " (base:" << baseIncome;
    if (winStreak_ > 0) {
        std::cout << " +win streak:" << winStreakGold;
    }
    if (lossStreak_ > 0) {
        std::cout << " +loss streak:" << lossStreakGold;
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
