// ============================================================================
// Project: ENGG1340/COMP2113 - Auto-Battler Arena
// Group: 75
// File Type: Header File (.h)
// File Description:
//   Defines the Unit class representing individual game pieces (characters).
//   Each unit has attributes including class type (Warrior, Mage, Tank, etc.),
//   health, attack power, star level, and special abilities.
//   Supports unit merging, position tracking on board, stat modifications,
//   and combat mechanics including critical strikes and special abilities.
// ============================================================================


#ifndef UNIT_H
#define UNIT_H

#include <string>

// Enum representing the different unit classes available in the game.
enum UnitClass {
    WARRIOR,   // High HP, medium ATK - front-line fighter. Ability: Rage
    MAGE,      // Low HP, high ATK - glass cannon. Ability: AOE splash
    TANK,      // Very high HP, low ATK - damage sponge. Ability: Block
    ASSASSIN,  // Medium HP, high ATK, high crit. Ability: Backstab
    ARCHER,    // Medium HP, medium ATK, ranged. Ability: Double Shot
    NUM_CLASSES
};

// Star level constants
const int MAX_STAR_LEVEL = 3;
const int MERGE_COUNT = 3;  // units needed to merge into next star

// ---------------------------------------------------------------------
// Unit
// ---------------------------------------------------------------------
// Represents a single chess piece on the battlefield or bench.
// Each unit has stats, a position, star level, and combat abilities.
// Units are dynamically allocated with `new` when purchased and freed
// with `delete` when killed or sold.
// ---------------------------------------------------------------------
class Unit {
public:
    // Purpose: Initialize a new unit with base stats at star level 1
    // Input: name (string), unitClass (enum), maxHp, atk, cost (int values), critBonus (optional), attackRange (optional)
    // Output: Fully initialized Unit object
    Unit(const std::string& name, UnitClass unitClass, int maxHp,
         int atk, int cost, int critBonus = 0, int attackRange = 1);

    // Purpose: Create a deep copy of a unit
    // Input: other (const Unit reference) - unit to copy
    // Output: New Unit with identical stats
    Unit(const Unit& other);

    // Purpose: Get unit's display name
    // Input: none
    // Output: const string reference - unit name
    std::string getName() const;

    // Purpose: Get unit's base name without star suffix
    // Input: none
    // Output: const string reference - base name
    std::string getBaseName() const;

    // Purpose: Get unit's class
    // Input: none
    // Output: UnitClass enum value
    UnitClass   getClass() const;

    // Purpose: Get unit's current HP
    // Input: none
    // Output: int - current health points
    int  getHp() const;

    // Purpose: Get unit's maximum HP
    // Input: none
    // Output: int - max health points
    int  getMaxHp() const;

    // Purpose: Get unit's attack power
    // Input: none
    // Output: int - attack damage value
    int  getAtk() const;

    // Purpose: Get unit's purchase cost
    // Input: none
    // Output: int - gold cost
    int  getCost() const;

    // Purpose: Get unit's row position on board
    // Input: none
    // Output: int - row coordinate
    int  getRow() const;

    // Purpose: Get unit's column position on board
    // Input: none
    // Output: int - column coordinate
    int  getCol() const;

    // Purpose: Get unit's critical strike bonus
    // Input: none
    // Output: int - crit bonus percentage
    int  getCritBonus() const;

    // Purpose: Get unit's attack range
    // Input: none
    // Output: int - attack range in squares
    int  getAttackRange() const;

    // Purpose: Get unit's star level
    // Input: none
    // Output: int - star level (1-3)
    int  getStarLevel() const;

    // Purpose: Check if unit is still alive
    // Input: none
    // Output: bool - true if HP > 0
    bool isAlive() const;

    // Purpose: Check if unit is owned by player
    // Input: none
    // Output: bool - true if player unit
    bool isPlayerUnit() const;

    // Purpose: Check if unit is in rage mode
    // Input: none
    // Output: bool - true if raging
    bool isRaged() const;

    // Purpose: Get unit's single-character symbol
    // Input: none
    // Output: char - character representation
    char getSymbol() const;

    // Purpose: Get unit's string representation
    // Input: none
    // Output: string - symbol and star display
    std::string getSymbolString() const;

    // Purpose: Get unit's ability name tag
    // Input: none
    // Output: string - ability identifier
    std::string getAbilityTag() const;

    // Purpose: Set unit's position on board
    // Input: row, col (int) - board coordinates
    // Output: none
    void setPosition(int row, int col);

    // Purpose: Set whether unit is owned by player
    // Input: isPlayer (bool) - ownership flag
    // Output: none
    void setPlayerUnit(bool isPlayer);

    // Purpose: Reduce unit's HP by damage amount (minimum 0)
    // Input: damage (int) - damage to apply
    // Output: none
    void takeDamage(int damage);

    // Purpose: Restore HP by amount, capped at maxHp
    // Input: amount (int) - HP to restore
    // Output: none
    void heal(int amount);

    // Purpose: Restore HP to maximum value
    // Input: none
    // Output: none
    void healToFull();

    // Purpose: Calculate gold refund for selling (half cost * star level)
    // Input: none
    // Output: int - gold value
    int getSellPrice() const;

    // Purpose: Calculate Manhattan distance to another unit
    // Input: other (const Unit*) - target unit
    // Output: int - Manhattan distance
    int getDistanceTo(const Unit* other) const;

    // Purpose: Get human-readable class name
    // Input: none
    // Output: string - class name ("Warrior", "Mage", etc.)
    std::string getClassString() const;

    // Purpose: Get flavor text description for unit class
    // Input: cls (UnitClass) - class to describe
    // Output: string - lore description for class
    static std::string getClassDescription(UnitClass cls);

    // Purpose: Get star level display string
    // Input: none
    // Output: string - "*", "**", or "***"
    std::string getStarString() const;

    // Purpose: Increase star level and scale stats (1.8x/1.5x for star 2, 3.0x/2.5x for star 3)
    // Input: none
    // Output: none
    void upgrade();

    // Purpose: Set star level directly without rescaling stats (for loading saved units)
    // Input: level (int) - target star level 1-3
    // Output: none
    void forceSetStarLevel(int level);

    // Purpose: Add flat bonus to attack power (synergy bonus)
    // Input: bonus (int) - ATK bonus amount
    // Output: none
    void applyAtkBonus(int bonus);

    // Purpose: Add flat bonus to critical strike chance (synergy bonus)
    // Input: bonus (int) - crit bonus amount
    // Output: none
    void applyCritBonus(int bonus);

    // Purpose: Remove all synergy bonuses and reset to base stats
    // Input: none
    // Output: none
    void resetBonuses();

    // Purpose: Reset rage flag at start of combat
    // Input: none
    // Output: none
    void clearRage();

    // Purpose: Mark unit as entered rage mode
    // Input: none
    // Output: none
    void setRaged();

    // Purpose: Get display name with star indicator (e.g. "Warrior**")
    // Input: none
    // Output: string - formatted name with stars
    std::string getDisplayName() const;

private:
    std::string baseName_;  // original name (e.g. "Warrior")
    UnitClass   unitClass_;
    int hp_;
    int maxHp_;
    int baseMaxHp_;   // original max HP before star upgrades
    int atk_;
    int baseAtk_;     // original ATK before star upgrades
    int cost_;
    int row_;
    int col_;
    int critBonus_;
    int baseCritBonus_;
    int attackRange_;
    int starLevel_;
    bool raged_;       // Warrior Rage flag (applied once per combat)
    int atkBonus_;    // synergy bonus
    int critBonusExtra_; // synergy bonus
    bool playerOwned_;
};

#endif // UNIT_H
