#ifndef UNIT_H
#define UNIT_H

#include <string>

// Enum representing the different unit classes available in the game.
enum UnitClass {
    WARRIOR,   // High HP, medium ATK — front-line fighter
    MAGE,      // Low HP, high ATK — glass cannon
    TANK,      // Very high HP, low ATK — damage sponge
    ASSASSIN,  // Medium HP, high ATK, high crit bonus
    ARCHER,    // Medium HP, medium ATK, attacks from range
    NUM_CLASSES
};

// ---------------------------------------------------------------------
// Unit
// ---------------------------------------------------------------------
// Represents a single chess piece (unit) on the battlefield or bench.
// Each unit has stats, a position on the grid, and combat methods.
// Units are dynamically allocated with `new` when purchased and freed
// with `delete` when killed or sold.
// ---------------------------------------------------------------------
class Unit {
public:
    // -----------------------------------------------------------------
    // Constructor
    // Inputs : name        – display name (e.g. "Warrior")
    //          unitClass   – enum value for the class
    //          maxHp       – starting / maximum hit points
    //          atk         – base attack damage
    //          cost        – gold cost in the shop
    //          critBonus   – extra crit chance (0-100), added on top of base 20%
    //          attackRange – how many grid cells the unit can reach
    // Output : a fully initialised Unit object
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
    UnitClass   getClass() const;
    int  getHp() const;
    int  getMaxHp() const;
    int  getAtk() const;
    int  getCost() const;
    int  getRow() const;
    int  getCol() const;
    int  getCritBonus() const;
    int  getAttackRange() const;
    bool isAlive() const;
    bool isPlayerUnit() const;
    char getSymbol() const;

    // Setters ----------------------------------------------------------
    void setPosition(int row, int col);
    void setPlayerUnit(bool isPlayer);

    // -----------------------------------------------------------------
    // takeDamage
    // What it does : reduces the unit's current HP by `damage`.
    //                HP will not drop below 0.
    // Input  : damage – the amount of damage to take
    // Output : none (modifies internal HP)
    // -----------------------------------------------------------------
    void takeDamage(int damage);

    // -----------------------------------------------------------------
    // heal
    // What it does : restores HP by `amount`, capped at maxHp.
    // Input  : amount – HP to restore
    // Output : none
    // -----------------------------------------------------------------
    void heal(int amount);

    // -----------------------------------------------------------------
    // getSellPrice
    // What it does : returns the gold refund when selling this unit.
    // Input  : none
    // Output : integer gold value (half of original cost, minimum 1)
    // -----------------------------------------------------------------
    int getSellPrice() const;

    // -----------------------------------------------------------------
    // getDistanceTo
    // What it does : calculates Manhattan distance to another unit.
    // Inputs : other – pointer to the target unit
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

private:
    std::string name_;
    UnitClass   unitClass_;
    int hp_;
    int maxHp_;
    int atk_;
    int cost_;
    int row_;
    int col_;
    int critBonus_;
    int attackRange_;
    bool playerOwned_;
};

#endif // UNIT_H
