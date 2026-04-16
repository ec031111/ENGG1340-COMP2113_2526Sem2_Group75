// =====================================================================
// PROJECT: ENGG1340/COMP2113 Auto-Battler Arena
// GROUP: Group 75
// FILE TYPE: Header File
// FILE NAME: shop.h
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
    // Purpose: Initialize shop with unit template pool
    // Input: none
    // Output: Shop object with initialized template pool
    Shop();

    // Purpose: Delete unsold units still in shop slots
    // Input: none
    // Output: none
    ~Shop();

    // Purpose: Clear current shop and generate SHOP_SIZE new random units
    // Input: none
    // Output: none
    void refresh();

    // Purpose: Remove and return unit from shop slot to caller
    // Input: slotIndex (int) - 0-based shop slot
    // Output: Unit* - pointer to removed unit or nullptr if invalid
    Unit* buyUnit(int slotIndex);

    // Purpose: Display current shop contents
    // Input: none
    // Output: none (prints to stdout)
    void display() const;

    // Purpose: Get pointer to unit in slot without removing
    // Input: slotIndex (int) - shop slot
    // Output: Unit* - pointer to unit or nullptr if invalid
    Unit* getUnit(int slotIndex) const;

    // Purpose: Get gold cost to refresh shop
    // Input: none
    // Output: int - refresh cost
    int getRefreshCost() const;

    // Purpose: Update stored player gold for affordability display
    // Input: gold (int) - current player gold
    // Output: none
    void setPlayerGold(int gold);

private:
    std::vector<UnitTemplate> templatePool_;  // all possible unit blueprints
    std::vector<Unit*>        slots_;         // current SHOP_SIZE offerings
    int playerGoldRef_;                       // for affordability display

    // Purpose: Populate template pool with all available unit blueprints
    // Input: none
    // Output: none
    void initTemplatePool();

    // Purpose: Delete all units in slots and clear the vector
    // Input: none
    // Output: none
    void clearSlots();
};

#endif // SHOP_H
