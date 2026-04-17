// =====================================================================
// Project: ENGG1340/COMP2113 Auto-Battler Arena
// Group: Group 75
// File Type: Implementation File
// File Name: shop.cpp
// Description: Shop system implementation for unit purchasing and management
//              Implements shop mechanics including unit refresh, purchase,
//              selling, and unit template pool initialization.
// =====================================================================

#include "shop.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <utility>

static const std::string ANSI_RESET = "\033[0m";
static const std::string ANSI_RED = "\033[31m";
static const std::string ANSI_GREEN = "\033[32m";
static const std::string ANSI_YELLOW = "\033[33m";
static const std::string ANSI_BLUE = "\033[34m";
static const std::string ANSI_MAGENTA = "\033[35m";
static const std::string ANSI_CYAN = "\033[36m";
static const std::string ANSI_BOLD = "\033[1m";
static const std::string ANSI_BRIGHT_YELLOW = "\033[93m";

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
        case WARRIOR: return {ANSI_RED, "⚔️"};
        case MAGE: return {ANSI_BLUE, "🔮"};
        case TANK: return {ANSI_GREEN, "🛡️"};
        case ASSASSIN: return {ANSI_MAGENTA, "🗡️"};
        case ARCHER: return {ANSI_YELLOW, "🏹"};
        default: return {ANSI_CYAN, "•"};
    }
}

// -----------------------------------------------------------------
// Constructor
// What it does : initialises the template pool.
// Input  : none
// Output : a Shop ready to refresh()
// -----------------------------------------------------------------
Shop::Shop() {
    initTemplatePool();
    playerGoldRef_ = 0;
}

// -----------------------------------------------------------------
// Destructor
// What it does : frees any dynamically allocated units still in slots.
// Input  : none
// Output : none
// -----------------------------------------------------------------
Shop::~Shop() {
    clearSlots();
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
// initTemplatePool - Load all available unit templates and their base stats
// Description: Populates the template pool with 15 pre-defined unit templates.
//              Each template specifies: name, class, HP, ATK, cost, crit, range
//              Templates are used as blueprints for random shop generation.
// Purpose: Initialize the master list of recruitable unit types
// -----------------------------------------------------------------
void Shop::initTemplatePool() {
    //                         name        class     HP  ATK cost crit range
    templatePool_.push_back({"Warrior",   WARRIOR,  120,  18,  3,  0,  1});
    templatePool_.push_back({"Knight",    WARRIOR,  140,  16,  4,  0,  1});
    templatePool_.push_back({"Berserker", WARRIOR,  100,  22,  4,  5,  1});
    templatePool_.push_back({"Mage",      MAGE,      70,  28,  4,  0,  2});
    templatePool_.push_back({"Sorcerer",  MAGE,      60,  32,  5,  0,  2});
    templatePool_.push_back({"Wizard",    MAGE,      80,  25,  4,  0,  3});
    templatePool_.push_back({"Tank",      TANK,     180,  10,  3,  0,  1});
    templatePool_.push_back({"Guardian",  TANK,     200,  12,  5,  0,  1});
    templatePool_.push_back({"Paladin",   TANK,     160,  15,  4,  0,  1});
    templatePool_.push_back({"Assassin",  ASSASSIN,  85,  26,  4, 20,  1});
    templatePool_.push_back({"Ninja",     ASSASSIN,  75,  30,  5, 25,  1});
    templatePool_.push_back({"Rogue",     ASSASSIN,  90,  22,  3, 15,  1});
    templatePool_.push_back({"Archer",    ARCHER,    90,  20,  3,  5,  3});
    templatePool_.push_back({"Ranger",    ARCHER,    80,  24,  4, 10,  3});
    templatePool_.push_back({"Sniper",    ARCHER,    65,  30,  5, 10,  4});
}

// -----------------------------------------------------------------
// clearSlots - Delete all unsold units in shop and reset inventory
// Description: Safely deletes all Unit pointers in slots and clears vector.
//              Prevents memory leaks before regenerating shop inventory.
// Purpose: Clean up shop inventory and free memory before refresh
// -----------------------------------------------------------------
void Shop::clearSlots() {
    for (size_t i = 0; i < slots_.size(); ++i) {
        if (slots_[i] != nullptr) {
            delete slots_[i];
            slots_[i] = nullptr;
        }
    }
    slots_.clear();
}

// -----------------------------------------------------------------
// refresh - Generate new random shop inventory from template pool
// Description: Clears old slots and populates 5 new random units.
//              Each unit is randomly selected from template pool and
//              instantiated with stats unchanged from template.
// Process: 1. Clear current slots (free memory)
//          2. For each slot, pick random template and create Unit
// Purpose: Provide fresh unit choices each round
// -----------------------------------------------------------------
void Shop::refresh() {
    clearSlots();
    for (int i = 0; i < SHOP_SIZE; ++i) {
        int idx = rand() % templatePool_.size();
        const UnitTemplate& t = templatePool_[idx];
        Unit* u = new Unit(t.name, t.unitClass, t.maxHp, t.atk,
                           t.cost, t.critBonus, t.attackRange);
        slots_.push_back(u);
    }
}

// -----------------------------------------------------------------
// buyUnit
// What it does : removes the unit from the given slot and returns it.
//                The caller takes ownership of the pointer.
// Input  : slotIndex (0-based)
// Output : Unit* on success, nullptr if slot is empty or invalid
// -----------------------------------------------------------------
Unit* Shop::buyUnit(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= (int)slots_.size()) return nullptr;
    if (slots_[slotIndex] == nullptr) return nullptr;

    Unit* unit = slots_[slotIndex];
    slots_[slotIndex] = nullptr;
    return unit;
}

// -----------------------------------------------------------------
// display - Print formatted shop table with unit stats and affordability
// Description: Shows all 5 shop slots with unit details and pricing.
//              Marks units player cannot afford with "X" indicator.
//              Shows sold-out slots as "-- SOLD --".
// Purpose: Visual shop interface for player browsing and purchasing
// -----------------------------------------------------------------
void Shop::display() const {
    const int W = 68;
    std::cout << std::endl;
    std::cout << ANSI_BOLD << ANSI_CYAN << "  +" << std::string(W, '-') << "+" << ANSI_RESET << std::endl;

    std::string title = ANSI_BOLD + ANSI_BRIGHT_YELLOW + "🏪 SHOP  (refresh: $" + std::to_string(REFRESH_COST) + ") 🏪" + ANSI_RESET;
    int pad = (W - visibleWidth(title)) / 2;
    if (pad < 0) pad = 0;
    std::cout << "  |" << std::string(pad, ' ') << title
              << std::string(std::max(0, W - pad - visibleWidth(title)), ' ') << "|" << std::endl;

    std::cout << ANSI_BOLD << ANSI_CYAN << "  +" << std::string(W, '-') << "+" << ANSI_RESET << std::endl;

    for (int i = 0; i < SHOP_SIZE; ++i) {
        std::ostringstream line;
        if (slots_[i] != nullptr) {
            bool canAfford = (slots_[i]->getCost() <= playerGoldRef_);
            auto [color, emoji] = getClassColorEmoji(slots_[i]->getClass());
            line << (canAfford ? "  " + ANSI_GREEN + "✓ " + ANSI_RESET : "  " + ANSI_RED + "✗ " + ANSI_RESET);
            line << "[" << (i + 1) << "] "
                 << color << emoji << " " << std::left << std::setw(10) << slots_[i]->getName() << ANSI_RESET
                 << std::setw(9) << slots_[i]->getClassString()
                 << "HP:" << std::setw(4) << slots_[i]->getMaxHp()
                 << "ATK:" << std::setw(3) << slots_[i]->getAtk()
                 << "R:" << slots_[i]->getAttackRange()
                 << " [" << slots_[i]->getAbilityTag() << "]"
                 << " " << (canAfford ? ANSI_GREEN : ANSI_RED) << "$" << slots_[i]->getCost() << ANSI_RESET;
        } else {
            line << "    [" << (i + 1) << "] " << ANSI_YELLOW << "-- SOLD --" << ANSI_RESET;
        }
        std::string s = padToWidth(line.str(), W);
        std::cout << "  |" << s << "|" << std::endl;
    }

    std::cout << ANSI_BOLD << ANSI_CYAN << "  +" << std::string(W, '-') << "+" << ANSI_RESET << std::endl;
}

// -----------------------------------------------------------------
// getUnit
// What it does : peeks at a shop slot without buying.
// Input  : slotIndex
// Output : Unit* or nullptr
// -----------------------------------------------------------------
Unit* Shop::getUnit(int slotIndex) const {
    if (slotIndex < 0 || slotIndex >= (int)slots_.size()) return nullptr;
    return slots_[slotIndex];
}

// -----------------------------------------------------------------
// getRefreshCost - Return cost to reroll shop inventory
// Description: Returns the fixed gold cost for shop refresh action.
// Returns: Integer gold amount (REFRESH_COST constant)
// Purpose: Allow callers to check refresh affordability
// -----------------------------------------------------------------
int Shop::getRefreshCost() const {
    return REFRESH_COST;
}

// -----------------------------------------------------------------
// setPlayerGold - Update player's gold reference for shop affordability display
// Description: Stores player's current gold amount for use in display().
//              Allows marking unaffordable units with "X" in shop view.
// Parameters: gold - Current player gold amount
// Purpose: Enable dynamic shop UI based on player resources
// -----------------------------------------------------------------
void Shop::setPlayerGold(int gold) {
    playerGoldRef_ = gold;
}
