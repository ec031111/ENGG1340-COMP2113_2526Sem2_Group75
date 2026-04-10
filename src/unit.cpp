#include "unit.h"
#include <cmath>

// -----------------------------------------------------------------
// Constructor
// Inputs : name, unitClass, maxHp, atk, cost, critBonus, attackRange
// Output : a star-1 Unit at position (-1,-1)
// -----------------------------------------------------------------
Unit::Unit(const std::string& name, UnitClass unitClass, int maxHp,
           int atk, int cost, int critBonus, int attackRange)
    : baseName_(name), unitClass_(unitClass), hp_(maxHp), maxHp_(maxHp),
      baseMaxHp_(maxHp), atk_(atk), baseAtk_(atk), cost_(cost),
      row_(-1), col_(-1), critBonus_(critBonus), baseCritBonus_(critBonus),
      attackRange_(attackRange), starLevel_(1), raged_(false),
      atkBonus_(0), critBonusExtra_(0), playerOwned_(true) {}

// -----------------------------------------------------------------
// Copy constructor
// -----------------------------------------------------------------
Unit::Unit(const Unit& other)
    : baseName_(other.baseName_), unitClass_(other.unitClass_),
      hp_(other.hp_), maxHp_(other.maxHp_), baseMaxHp_(other.baseMaxHp_),
      atk_(other.atk_), baseAtk_(other.baseAtk_), cost_(other.cost_),
      row_(other.row_), col_(other.col_),
      critBonus_(other.critBonus_), baseCritBonus_(other.baseCritBonus_),
      attackRange_(other.attackRange_), starLevel_(other.starLevel_), raged_(other.raged_),
      atkBonus_(other.atkBonus_), critBonusExtra_(other.critBonusExtra_),
      playerOwned_(other.playerOwned_) {}

// --- Getters ---
std::string Unit::getName() const       { return getDisplayName(); }
std::string Unit::getBaseName() const   { return baseName_; }
UnitClass   Unit::getClass() const      { return unitClass_; }
int  Unit::getHp() const                { return hp_; }
int  Unit::getMaxHp() const             { return maxHp_; }
int  Unit::getAtk() const               { return atk_ + atkBonus_; }
int  Unit::getCost() const              { return cost_; }
int  Unit::getRow() const               { return row_; }
int  Unit::getCol() const               { return col_; }
int  Unit::getCritBonus() const         { return critBonus_ + critBonusExtra_; }
int  Unit::getAttackRange() const       { return attackRange_; }
int  Unit::getStarLevel() const         { return starLevel_; }
bool Unit::isAlive() const              { return hp_ > 0; }
bool Unit::isPlayerUnit() const         { return playerOwned_; }
bool Unit::isRaged() const              { return raged_; }

// -----------------------------------------------------------------
// getSymbol
// What it does : returns display symbol for the board.
//                Star 1: first char uppercase (e.g. 'W')
//                Star 2: first char + '2' indicator
//                Star 3: first char + '3' indicator
// Input  : none
// Output : char (first character of name)
// -----------------------------------------------------------------
char Unit::getSymbol() const {
    return baseName_[0];
}

// -----------------------------------------------------------------
// getSymbolString
// What it does : returns a 2-char display string for the board.
// Input  : none
// Output : string like "W ", "W2", "W3"
// -----------------------------------------------------------------
std::string Unit::getSymbolString() const {
    std::string s;
    s += baseName_[0];
    if (starLevel_ >= 2) {
        s += ('0' + starLevel_);
    } else {
        s += ' ';
    }
    return s;
}

// -----------------------------------------------------------------
// getAbilityTag
// -----------------------------------------------------------------
std::string Unit::getAbilityTag() const {
    switch (unitClass_) {
        case WARRIOR:  return "Rage";
        case MAGE:     return "AOE";
        case TANK:     return "Block";
        case ASSASSIN: return "Crit";
        case ARCHER:   return "DblShot";
        default:       return "?";
    }
}

// --- Setters ---
void Unit::setPosition(int row, int col) { row_ = row; col_ = col; }
void Unit::setPlayerUnit(bool isPlayer)  { playerOwned_ = isPlayer; }

// -----------------------------------------------------------------
// takeDamage
// -----------------------------------------------------------------
void Unit::takeDamage(int damage) {
    hp_ -= damage;
    if (hp_ < 0) hp_ = 0;
}

// -----------------------------------------------------------------
// heal
// -----------------------------------------------------------------
void Unit::heal(int amount) {
    hp_ += amount;
    if (hp_ > maxHp_) hp_ = maxHp_;
}

// -----------------------------------------------------------------
// healToFull
// -----------------------------------------------------------------
void Unit::healToFull() {
    hp_ = maxHp_;
}

// -----------------------------------------------------------------
// getSellPrice
// What it does : returns cost/2 * starLevel, minimum 1.
// -----------------------------------------------------------------
int Unit::getSellPrice() const {
    int price = (cost_ / 2) * starLevel_;
    return (price < 1) ? 1 : price;
}

// -----------------------------------------------------------------
// getDistanceTo
// -----------------------------------------------------------------
int Unit::getDistanceTo(const Unit* other) const {
    return std::abs(row_ - other->getRow()) + std::abs(col_ - other->getCol());
}

// -----------------------------------------------------------------
// getClassString
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

// -----------------------------------------------------------------
// getClassDescription
// Returns a one-line lore description for each unit class.
// -----------------------------------------------------------------
std::string Unit::getClassDescription(UnitClass cls) {
    switch (cls) {
        case WARRIOR:  return "Fearless fighters who grow stronger when wounded.";
        case MAGE:     return "Glass cannons wielding devastating area magic.";
        case TANK:     return "Iron-walled defenders who shrug off blows.";
        case ASSASSIN: return "Shadowy killers with lethal critical strikes.";
        case ARCHER:   return "Precise marksmen who can fire twice in succession.";
        default:       return "Unknown class.";
    }
}

// -----------------------------------------------------------------
// getStarString
// -----------------------------------------------------------------
std::string Unit::getStarString() const {
    std::string s = "";
    for (int i = 0; i < starLevel_; ++i) s += "*";
    return s;
}

// -----------------------------------------------------------------
// getDisplayName
// What it does : returns "Name*" / "Name**" / "Name***"
// -----------------------------------------------------------------
std::string Unit::getDisplayName() const {
    if (starLevel_ <= 1) return baseName_;
    return baseName_ + getStarString();
}

// -----------------------------------------------------------------
// upgrade
// What it does : bumps star level and scales base stats.
//   Star 2: HP * 1.8, ATK * 1.5
//   Star 3: HP * 3.0, ATK * 2.5 (from original base)
// -----------------------------------------------------------------
void Unit::upgrade() {
    if (starLevel_ >= MAX_STAR_LEVEL) return;
    starLevel_++;

    if (starLevel_ == 2) {
        maxHp_ = (int)(baseMaxHp_ * 1.8);
        atk_   = (int)(baseAtk_ * 1.5);
    } else if (starLevel_ == 3) {
        maxHp_ = (int)(baseMaxHp_ * 3.0);
        atk_   = (int)(baseAtk_ * 2.5);
    }
    hp_ = maxHp_;  // full heal on upgrade
}

// -----------------------------------------------------------------
// forceSetStarLevel
// -----------------------------------------------------------------
void Unit::forceSetStarLevel(int level) {
    if (level < 1 || level > MAX_STAR_LEVEL) return;
    starLevel_ = level;
}

// -----------------------------------------------------------------
// applyAtkBonus / applyCritBonus
// -----------------------------------------------------------------
void Unit::applyAtkBonus(int bonus)  { atkBonus_ += bonus; }
void Unit::applyCritBonus(int bonus) { critBonusExtra_ += bonus; }

// -----------------------------------------------------------------
// resetBonuses
// -----------------------------------------------------------------
void Unit::resetBonuses() {
    atkBonus_ = 0;
    critBonusExtra_ = 0;
}

// -----------------------------------------------------------------
// clearRage
// -----------------------------------------------------------------
void Unit::clearRage() {
    raged_ = false;
}

// -----------------------------------------------------------------
// setRaged
// -----------------------------------------------------------------
void Unit::setRaged() {
    raged_ = true;
}
