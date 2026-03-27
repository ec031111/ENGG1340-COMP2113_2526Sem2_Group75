#include "shop.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>

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
// initTemplatePool
// What it does : defines all available unit types and their stats.
//                These templates are used by refresh() to randomly
//                generate purchasable units.
// Input  : none
// Output : none (fills templatePool_)
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
// clearSlots
// What it does : deletes every unit still sitting in the shop and
//                empties the slots vector.
// Input  : none
// Output : none (frees dynamic memory)
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
// refresh
// What it does : clears current slots and randomly picks SHOP_SIZE
//                units from the template pool, creating each with `new`.
// Input  : none
// Output : none (fills slots_ with new Unit objects)
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
// display
// What it does : prints a formatted table of the current shop offerings.
//                Units the player cannot afford are dimmed with (X).
// Input  : none
// Output : none (stdout)
// -----------------------------------------------------------------
void Shop::display() const {
    const int W = 50;  // inner width between the two '|'
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;

    // Title centered
    std::string title = "SHOP  (refresh: $" + std::to_string(REFRESH_COST) + ")";
    int pad = (W - (int)title.size()) / 2;
    std::cout << "  |" << std::string(pad, ' ') << title
              << std::string(W - pad - (int)title.size(), ' ') << "|" << std::endl;

    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;

    for (int i = 0; i < SHOP_SIZE; ++i) {
        std::ostringstream line;
        if (slots_[i] != nullptr) {
            bool canAfford = (slots_[i]->getCost() <= playerGoldRef_);
            if (!canAfford) line << "  X ";
            else            line << "    ";
            line << "[" << (i + 1) << "] "
                 << std::left << std::setw(10) << slots_[i]->getName()
                 << std::setw(9) << slots_[i]->getClassString()
                 << "HP:" << std::setw(5) << slots_[i]->getMaxHp()
                 << "ATK:" << std::setw(4) << slots_[i]->getAtk()
                 << "$" << slots_[i]->getCost();
        } else {
            line << "    [" << (i + 1) << "] -- SOLD --";
        }
        std::string s = line.str();
        // Pad or trim to exactly W chars
        if ((int)s.size() < W) s += std::string(W - s.size(), ' ');
        else if ((int)s.size() > W) s = s.substr(0, W);
        std::cout << "  |" << s << "|" << std::endl;
    }

    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
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
// getRefreshCost
// What it does : returns the gold needed to re-roll the shop.
// Input  : none
// Output : REFRESH_COST constant
// -----------------------------------------------------------------
int Shop::getRefreshCost() const {
    return REFRESH_COST;
}

// -----------------------------------------------------------------
// setPlayerGold
// What it does : updates the gold reference so display() can show
//                affordability.
// Input  : gold amount
// Output : none
// -----------------------------------------------------------------
void Shop::setPlayerGold(int gold) {
    playerGoldRef_ = gold;
}
