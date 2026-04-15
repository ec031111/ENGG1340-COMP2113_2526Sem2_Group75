#include "synergy.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// -----------------------------------------------------------------
// calculateSynergies
// What it does : counts each class, determines tier, applies bonuses.
// Inputs : units - all player units on the board
// Output : vector of active synergy bonuses
// -----------------------------------------------------------------
std::vector<SynergyBonus> Synergy::calculateSynergies(std::vector<Unit*>& units) {
    std::vector<SynergyBonus> active;

    // Count units per class
    int classCount[NUM_CLASSES] = {0};
    for (size_t i = 0; i < units.size(); ++i) {
        classCount[units[i]->getClass()]++;
    }

    // --- WARRIOR synergy: team ATK boost ---
    if (classCount[WARRIOR] >= 2) {
        SynergyBonus b;
        b.unitClass = WARRIOR;
        b.count = classCount[WARRIOR];
        if (classCount[WARRIOR] >= 3) {
            b.tier = 2;
            b.description = "Warrior(3): All units +10 ATK";
            for (size_t i = 0; i < units.size(); ++i)
                units[i]->applyAtkBonus(10);
        } else {
            b.tier = 1;
            b.description = "Warrior(2): All units +5 ATK";
            for (size_t i = 0; i < units.size(); ++i)
                units[i]->applyAtkBonus(5);
        }
        active.push_back(b);
    }

    // --- MAGE synergy: Mage crit boost ---
    if (classCount[MAGE] >= 2) {
        SynergyBonus b;
        b.unitClass = MAGE;
        b.count = classCount[MAGE];
        if (classCount[MAGE] >= 3) {
            b.tier = 2;
            b.description = "Mage(3): All Mages +30% crit";
            for (size_t i = 0; i < units.size(); ++i)
                if (units[i]->getClass() == MAGE)
                    units[i]->applyCritBonus(30);
        } else {
            b.tier = 1;
            b.description = "Mage(2): All Mages +15% crit";
            for (size_t i = 0; i < units.size(); ++i)
                if (units[i]->getClass() == MAGE)
                    units[i]->applyCritBonus(15);
        }
        active.push_back(b);
    }

    // --- TANK synergy: durability boost ---
    if (classCount[TANK] >= 2) {
        SynergyBonus b;
        b.unitClass = TANK;
        b.count = classCount[TANK];
        if (classCount[TANK] >= 3) {
            b.tier = 2;
            b.description = "Tank(3): All units heal +15% HP";
            for (size_t i = 0; i < units.size(); ++i)
                units[i]->heal(units[i]->getMaxHp() * 15 / 100);
        } else {
            b.tier = 1;
            b.description = "Tank(2): All Tanks heal +20% HP";
            for (size_t i = 0; i < units.size(); ++i)
                if (units[i]->getClass() == TANK)
                    units[i]->heal(units[i]->getMaxHp() * 20 / 100);
        }
        active.push_back(b);
    }

    // --- ASSASSIN synergy: burst damage ---
    if (classCount[ASSASSIN] >= 2) {
        SynergyBonus b;
        b.unitClass = ASSASSIN;
        b.count = classCount[ASSASSIN];
        if (classCount[ASSASSIN] >= 3) {
            b.tier = 2;
            b.description = "Assassin(3): Assassins +20 ATK, +10% crit";
            for (size_t i = 0; i < units.size(); ++i)
                if (units[i]->getClass() == ASSASSIN) {
                    units[i]->applyAtkBonus(20);
                    units[i]->applyCritBonus(10);
                }
        } else {
            b.tier = 1;
            b.description = "Assassin(2): Assassins +10 ATK";
            for (size_t i = 0; i < units.size(); ++i)
                if (units[i]->getClass() == ASSASSIN)
                    units[i]->applyAtkBonus(10);
        }
        active.push_back(b);
    }

    // --- ARCHER synergy: range and damage ---
    if (classCount[ARCHER] >= 2) {
        SynergyBonus b;
        b.unitClass = ARCHER;
        b.count = classCount[ARCHER];
        if (classCount[ARCHER] >= 3) {
            b.tier = 2;
            b.description = "Archer(3): Archers +5 ATK";
            for (size_t i = 0; i < units.size(); ++i)
                if (units[i]->getClass() == ARCHER)
                    units[i]->applyAtkBonus(5);
        } else {
            b.tier = 1;
            b.description = "Archer(2): Archers +3 ATK";
            for (size_t i = 0; i < units.size(); ++i)
                if (units[i]->getClass() == ARCHER)
                    units[i]->applyAtkBonus(3);
        }
        active.push_back(b);
    }

    return active;
}

// -----------------------------------------------------------------
// clearSynergies - Remove all temporary synergy bonuses from unit roster
// Description: Resets each unit's stat bonuses (ATK, crit) to base.
//              Called before recalculating synergies each round.
// Parameters: units - Vector of units to reset
// Purpose: Clean slate for synergy recalculation each round
// -----------------------------------------------------------------
void Synergy::clearSynergies(std::vector<Unit*>& units) {
    for (size_t i = 0; i < units.size(); ++i) {
        units[i]->resetBonuses();
    }
}

// -----------------------------------------------------------------
// displaySynergies - Print active synergies to console for player feedback
// Description: Lists all currently active synergy bonuses with descriptions.
//              Provides visual confirmation of team composition benefits.
// Parameters: bonuses - Vector of active SynergyBonus objects to display
// Purpose: Inform player of active team synergies and their effects
// -----------------------------------------------------------------
void Synergy::displaySynergies(const std::vector<SynergyBonus>& bonuses) {
    if (bonuses.empty()) return;
    std::cout << "\n  Active Synergies:" << std::endl;
    for (size_t i = 0; i < bonuses.size(); ++i) {
        std::cout << "    >> " << bonuses[i].description << std::endl;
    }
    std::cout << std::endl;
}
