#include "unit.h"
#include <cmath>

// -----------------------------------------------------------------
// Constructor
// Inputs : name, unitClass, maxHp, atk, cost, critBonus, attackRange
// Output : a fully initialised Unit placed at (-1,-1) (not on board)
// -----------------------------------------------------------------
Unit::Unit(const std::string& name, UnitClass unitClass, int maxHp,
           int atk, int cost, int critBonus, int attackRange)
    : name_(name), unitClass_(unitClass), hp_(maxHp), maxHp_(maxHp),
      atk_(atk), cost_(cost), row_(-1), col_(-1),
      critBonus_(critBonus), attackRange_(attackRange), playerOwned_(true) {}

// -----------------------------------------------------------------
// Copy constructor
// Input  : another Unit
// Output : deep copy with identical stats and position
// -----------------------------------------------------------------
Unit::Unit(const Unit& other)
    : name_(other.name_), unitClass_(other.unitClass_),
      hp_(other.hp_), maxHp_(other.maxHp_), atk_(other.atk_),
      cost_(other.cost_), row_(other.row_), col_(other.col_),
      critBonus_(other.critBonus_), attackRange_(other.attackRange_),
      playerOwned_(other.playerOwned_) {}

// --- Getters ---
std::string Unit::getName() const       { return name_; }
UnitClass   Unit::getClass() const      { return unitClass_; }
int  Unit::getHp() const                { return hp_; }
int  Unit::getMaxHp() const             { return maxHp_; }
int  Unit::getAtk() const               { return atk_; }
int  Unit::getCost() const              { return cost_; }
int  Unit::getRow() const               { return row_; }
int  Unit::getCol() const               { return col_; }
int  Unit::getCritBonus() const         { return critBonus_; }
int  Unit::getAttackRange() const       { return attackRange_; }
bool Unit::isAlive() const              { return hp_ > 0; }
bool Unit::isPlayerUnit() const         { return playerOwned_; }

// -----------------------------------------------------------------
// getSymbol
// What it does : returns the first character of the unit's name,
//                used to display the unit on the ASCII board.
// Input  : none
// Output : a single char (e.g. 'W' for Warrior)
// -----------------------------------------------------------------
char Unit::getSymbol() const {
    return name_[0];
}

// --- Setters ---
void Unit::setPosition(int row, int col) {
    row_ = row;
    col_ = col;
}

void Unit::setPlayerUnit(bool isPlayer) {
    playerOwned_ = isPlayer;
}

// -----------------------------------------------------------------
// takeDamage
// What it does : subtracts damage from HP; floors at 0.
// Input  : damage – positive integer
// Output : none (modifies hp_)
// -----------------------------------------------------------------
void Unit::takeDamage(int damage) {
    hp_ -= damage;
    if (hp_ < 0) hp_ = 0;
}

// -----------------------------------------------------------------
// heal
// What it does : adds amount to HP, capped at maxHp_.
// Input  : amount – positive integer
// Output : none
// -----------------------------------------------------------------
void Unit::heal(int amount) {
    hp_ += amount;
    if (hp_ > maxHp_) hp_ = maxHp_;
}

// -----------------------------------------------------------------
// getSellPrice
// What it does : returns half the purchase cost (minimum 1 gold).
// Input  : none
// Output : integer sell price
// -----------------------------------------------------------------
int Unit::getSellPrice() const {
    int price = cost_ / 2;
    return (price < 1) ? 1 : price;
}

// -----------------------------------------------------------------
// getDistanceTo
// What it does : computes Manhattan distance |r1-r2| + |c1-c2|.
// Input  : other – pointer to another Unit
// Output : integer distance
// -----------------------------------------------------------------
int Unit::getDistanceTo(const Unit* other) const {
    return std::abs(row_ - other->getRow()) + std::abs(col_ - other->getCol());
}

// -----------------------------------------------------------------
// getClassString
// What it does : converts the UnitClass enum to a readable string.
// Input  : none
// Output : string representation of the class
// -----------------------------------------------------------------
std::string Unit::getClassString() const {
    switch (unitClass_) {
        case WARRIOR:  return "Warrior";
        case MAGE:     return "Mage";
        case TANK:     return "Tank";
        case ASSASSIN: return "Assassin";
        case ARCHER:   return "Archer";
        default:       return "Unknown";
    }
}
