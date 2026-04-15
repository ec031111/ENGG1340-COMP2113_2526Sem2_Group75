// =====================================================================
// PROJECT: ENGG1340/COMP2113 Auto-Battler Arena
// GROUP: Group 75
// FILE TYPE: Header File
// DESCRIPTION: Shop system for purchasing and managing units
//              Maintains a pool of available units for each round,
//              handles unit purchase logic, shop refreshing, and
//              template management for unit generation.
// =====================================================================

#ifndef SHOP_H
#define SHOP_H

#include "unit.h"
#include <vector>

// Number of unit slots shown in the shop each round.
const int SHOP_SIZE = 5;

// Cost to manually refresh the shop.
const int REFRESH_COST = 2;

// ---------------------------------------------------------------------
// UnitTemplate
// ---------------------------------------------------------------------
// A lightweight struct holding the blueprint for creating a Unit.
// The shop picks randomly from a pool of these templates.
// ---------------------------------------------------------------------
struct UnitTemplate {
    std::string name;
    UnitClass   unitClass;
    int maxHp;
    int atk;
    int cost;
    int critBonus;
    int attackRange;
};

// ---------------------------------------------------------------------
// Shop
// ---------------------------------------------------------------------
// Manages the random unit shop. Each round, 5 units are drawn from a
// template pool. The player can buy units, refresh the shop, or skip.
// ---------------------------------------------------------------------
class Shop {
public:
    // -----------------------------------------------------------------
    // Constructor
    // What it does : builds the internal unit template pool.
    // Input  : none
    // Output : a Shop with an initialised template pool
    // -----------------------------------------------------------------
    Shop();

    // -----------------------------------------------------------------
    // Destructor
    // What it does : deletes any unsold units still in the shop slots.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    ~Shop();

    // -----------------------------------------------------------------
    // refresh
    // What it does : clears current shop slots (deleting unsold units)
    //                and generates SHOP_SIZE new random units using `new`.
    // Input  : none
    // Output : none (populates slots_)
    // -----------------------------------------------------------------
    void refresh();

    // -----------------------------------------------------------------
    // buyUnit
    // What it does : removes the unit at the given slot from the shop
    //                and returns ownership to the caller. The caller is
    //                now responsible for deleting it.
    // Input  : slotIndex – 0-based shop slot
    // Output : Unit* if slot is valid and occupied, nullptr otherwise
    // -----------------------------------------------------------------
    Unit* buyUnit(int slotIndex);

    // -----------------------------------------------------------------
    // display
    // What it does : prints the current shop contents to stdout.
    // Input  : none
    // Output : none (prints to stdout)
    // -----------------------------------------------------------------
    void display() const;

    // -----------------------------------------------------------------
    // getUnit
    // What it does : returns a pointer to a unit in a slot (no removal).
    // Input  : slotIndex
    // Output : Unit* or nullptr
    // -----------------------------------------------------------------
    Unit* getUnit(int slotIndex) const;

    // -----------------------------------------------------------------
    // getRefreshCost
    // What it does : returns the gold cost to refresh the shop.
    // Input  : none
    // Output : integer cost
    // -----------------------------------------------------------------
    int getRefreshCost() const;

    // -----------------------------------------------------------------
    // setPlayerGold
    // What it does : updates the gold reference for affordability display.
    // Input  : gold – current player gold
    // Output : none
    // -----------------------------------------------------------------
    void setPlayerGold(int gold);

private:
    std::vector<UnitTemplate> templatePool_;  // all possible unit blueprints
    std::vector<Unit*>        slots_;         // current SHOP_SIZE offerings
    int playerGoldRef_;                       // for affordability display

    // -----------------------------------------------------------------
    // initTemplatePool
    // What it does : populates templatePool_ with all unit blueprints.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void initTemplatePool();

    // -----------------------------------------------------------------
    // clearSlots
    // What it does : deletes all units still in slots_ and clears the vector.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
    void clearSlots();
};

#endif // SHOP_H
