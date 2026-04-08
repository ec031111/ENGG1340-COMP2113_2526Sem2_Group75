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
    // -----------------------------------------------------------------
    // Constructor
    // Inputs : name, unitClass, maxHp, atk, cost, critBonus, attackRange
    // Output : a fully initialised star-1 Unit object
    // -----------------------------------------------------------------
    Unit(const std::string& name, UnitClass unitClass, int maxHp,
         int atk, int cost, int critBonus = 0, int attackRange = 1);

    // -----------------------------------------------------------------
    // Copy constructor (deep copy)
    // Input  : another Unit object
    // Output : a new Unit with identical stats
    // -----------------------------------------------------------------
    Unit(const Unit& other);

    // Getters ----------------------------------------------------------
    std::string getName() const;
    std::string getBaseName() const;  // name without star suffix
    UnitClass   getClass() const;
    int  getHp() const;
    int  getMaxHp() const;
    int  getAtk() const;
    int  getCost() const;
    int  getRow() const;
    int  getCol() const;
    int  getCritBonus() const;
    int  getAttackRange() const;
    int  getStarLevel() const;
    bool isAlive() const;
    bool isPlayerUnit() const;
    bool isRaged() const;
    char getSymbol() const;
    std::string getSymbolString() const;
    std::string getAbilityTag() const;

    // Setters ----------------------------------------------------------
    void setPosition(int row, int col);
    void setPlayerUnit(bool isPlayer);

    // -----------------------------------------------------------------
    // takeDamage
    // What it does : reduces the unit's current HP by `damage`.
    //                HP will not drop below 0.
    // Input  : damage - the amount of damage to take
    // Output : none (modifies internal HP)
    // -----------------------------------------------------------------
    void takeDamage(int damage);

    // -----------------------------------------------------------------
    // heal
    // What it does : restores HP by `amount`, capped at maxHp.
    // Input  : amount - HP to restore
    // Output : none
    // -----------------------------------------------------------------
    void heal(int amount);

    // -----------------------------------------------------------------
    // getSellPrice
    // What it does : returns the gold refund (half cost * star level).
    // Input  : none
    // Output : integer gold value
    // -----------------------------------------------------------------
    int getSellPrice() const;

    // -----------------------------------------------------------------
    // getDistanceTo
    // What it does : calculates Manhattan distance to another unit.
    // Inputs : other - pointer to the target unit
    // Output : integer Manhattan distance
    // -----------------------------------------------------------------
    int getDistanceTo(const Unit* other) const;

    // -----------------------------------------------------------------
    // getClassString
    // What it does : returns a human-readable string for the unit class.
    // Input  : none
    // Output : string like "Warrior", "Mage", etc.
    // -----------------------------------------------------------------
    std::string getClassString() const;

    // -----------------------------------------------------------------
    // getClassDescription
    // What it does : returns a one-line lore description for a unit class.
    // Input  : cls - the UnitClass to describe
    // Output : string with flavor text for that class
    // -----------------------------------------------------------------
    static std::string getClassDescription(UnitClass cls);

    // -----------------------------------------------------------------
    // getStarString
    // What it does : returns star display like "*", "**", "***"
    // Input  : none
    // Output : string of stars
    // -----------------------------------------------------------------
    std::string getStarString() const;

    // -----------------------------------------------------------------
    // upgrade
    // What it does : increases star level and scales stats.
    //                Star 2 = 1.8x HP, 1.5x ATK.
    //                Star 3 = 3.0x HP, 2.5x ATK (from base).
    // Input  : none
    // Output : none (modifies stats)
    // -----------------------------------------------------------------
    void upgrade();

    // -----------------------------------------------------------------
    // forceSetStarLevel
    // What it does : sets the star level directly WITHOUT rescaling stats.
    //                Used by the load system when stats are already saved
    //                post-upgrade.
    // Input  : level – the target star level (1-3)
    // Output : none
    // -----------------------------------------------------------------
    void forceSetStarLevel(int level);

    // -----------------------------------------------------------------
    // applyAtkBonus / applyCritBonus
    // What it does : adds a flat bonus to ATK or crit chance.
    //                Used by the synergy system.
    // Input  : bonus amount
    // Output : none
    // -----------------------------------------------------------------
    void applyAtkBonus(int bonus);
    void applyCritBonus(int bonus);

    // -----------------------------------------------------------------
    // resetBonuses
    // What it does : removes synergy bonuses (resets to base stats).
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void resetBonuses();

    // -----------------------------------------------------------------
    // clearRage
    // What it does : resets the rage flag. Called at start of each combat.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void clearRage();

    // -----------------------------------------------------------------
    // setRaged
    // What it does : marks the unit as having entered rage mode.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void setRaged();

    // -----------------------------------------------------------------
    // getDisplayName
    // What it does : returns name with star indicator, e.g. "Warrior**"
    // Input  : none
    // Output : string
    // -----------------------------------------------------------------
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
