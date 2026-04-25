// ============================================================================
// Project: ENGG1340/COMP2113 - Auto-Battler Arena
// Group: 75
// File Type: Implementation File (.cpp)
// File Description:
//   Implements the Unit class for individual game pieces.
//   Provides constructors, getters/setters for unit attributes,
//   combat mechanics (damage calculation, critical hits, special abilities),
//   unit merging/leveling system, and position management on the game board.
// ============================================================================

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

// Purpose: Return display character for board representation
// Input: None
// Output: char - first character of unit's name
char Unit::getSymbol() const {
    return baseName_[0];
}

// Purpose: Return 2-character display string including star level indicator
// Input: None
// Output: string - "X " (star 1), "X2" (star 2), "X3" (star 3)
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
// getAbilityTag - Return unit's special ability name based on class
// Description: Maps each unit class to its unique combat ability.
//              Used for displaying ability info in UI and tooltips.
// Returns: String like "Rage", "AOE", "Block", "Crit", "DblShot"
// Purpose: Allows dynamic UI display of unit abilities
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

// Purpose: Apply damage to unit, reducing HP (minimum 0)
// Input: damage (int) - damage amount to apply
// Output: None (modifies hp_ state)
void Unit::takeDamage(int damage) {
    hp_ -= damage;
    if (hp_ < 0) hp_ = 0;
}

// Purpose: Restore unit health by amount (capped at maxHp)
// Input: amount (int) - health to restore
// Output: None (modifies hp_ state)
void Unit::heal(int amount) {
    hp_ += amount;
    if (hp_ > maxHp_) hp_ = maxHp_;
}

// Purpose: Instantly restore unit to full health
// Input: None
// Output: None (sets hp_ to maxHp_)
void Unit::healToFull() {
    hp_ = maxHp_;
}

// Purpose: Calculate unit's sell value based on cost and star level
// Input: None
// Output: int - sell price (cost/2 * starLevel, minimum 1)
int Unit::getSellPrice() const {
    int price = (cost_ / 2) * starLevel_;
    return (price < 1) ? 1 : price;
}

// Purpose: Calculate Manhattan distance to another unit
// Input: other (const Unit*) - target unit for distance calculation
// Output: int - distance value (always >= 0)
int Unit::getDistanceTo(const Unit* other) const {
    return std::abs(row_ - other->getRow()) + std::abs(col_ - other->getCol());
}

// -----------------------------------------------------------------
// getClassString - Return human-readable unit class name
// Description: Maps UnitClass enum to user-facing class name strings.
//              Used throughout UI for player communication.
// Returns: Class name like "Warrior", "Mage", "Tank", etc.
// Purpose: Display unit type information in menus and tooltips
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
// getStarString - Return star level as asterisk string representation
// Description: Converts star level (1-3) into visual string format.
//              Star 1: "", Star 2: "**", Star 3: "***"
// Returns: String of asterisks matching star level (empty for star 1)
// Purpose: Visual indicator of unit upgrade status for display
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
    return baseName_ + getStarString();
}

// -----------------------------------------------------------------
// upgrade
// What it does : bumps star level and scales base stats.
//   Star 2: HP * 1.8, ATK * 1.5
//   Star 3: HP * 3.0, ATK * 2.5 (from original base)
// -----------------------------------------------------------------
// Purpose: Upgrade unit to next star level with enhanced stats
// Input: None
// Output: None (increases starLevel_, maxHp_, atk_)
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

// Purpose: Manually set unit's star level for testing and save loading
// Input: level (int) - target star level (must be 1-3)
// Output: None (sets starLevel_ if valid, ignores otherwise)
void Unit::forceSetStarLevel(int level) {
    if (level < 1 || level > MAX_STAR_LEVEL) return;
    starLevel_ = level;
}

// -----------------------------------------------------------------
// applyAtkBonus / applyCritBonus - Apply temporary stat bonuses from synergies
// Description: Accumulate bonus attack or crit chance from synergy effects.
//              Bonuses persist until resetBonuses() called (each round).
// Parameters: bonus - Amount to add to current bonus pool
// Purpose: Implement synergy system for team composition rewards
// -----------------------------------------------------------------
void Unit::applyAtkBonus(int bonus)  { atkBonus_ += bonus; }
void Unit::applyCritBonus(int bonus) { critBonusExtra_ += bonus; }

// Purpose: Clear all temporary stat bonuses from synergies
// Input: None
// Output: None (resets atkBonus_ and critBonusExtra_ to 0)
void Unit::resetBonuses() {
    atkBonus_ = 0;
    critBonusExtra_ = 0;
}

// Purpose: Reset unit's rage status flag
// Input: None
// Output: None (sets raged_ to false)
void Unit::clearRage() {
    raged_ = false;
}

// Purpose: Mark unit as currently in rage mode for special ability
// Input: None
// Output: None (sets raged_ to true)
void Unit::setRaged() {
    raged_ = true;
}
