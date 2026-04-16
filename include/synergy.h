 //=====================================================================
 // Project Name: AUTO-BATTLER ARENA
 // ENGG1340 / COMP2113 - 2025-2026 Semester 2
 // Group Number: Group 75
 // 
 // File Type: Header File
 // File Name: synergy.h
 // Description:
 //Synergy system header file that defines the synergy bonus structure
 //  and the Synergy class. Handles unit class grouping, tier calculation,
 //  and attribute bonuses when units of the same class are deployed together.
 //  Synergies provide strategic depth by rewarding players for building
 //  themed armies with matching unit classes.
 //=====================================================================


#ifndef SYNERGY_H
#define SYNERGY_H

#include "unit.h"
#include <vector>
#include <string>

// ---------------------------------------------------------------------
// Synergy thresholds and bonuses
// ---------------------------------------------------------------------
// 2 of same class = Tier 1 bonus
// 3 of same class = Tier 2 bonus (replaces Tier 1)
// 4+ of same class = Tier 3 bonus (replaces Tier 2)

struct SynergyBonus {
    UnitClass unitClass;
    int count;       // how many of this class are on the field
    int tier;        // 0 = none, 1/2/3
    std::string description;
};

// ---------------------------------------------------------------------
// Synergy - Unit class synergy bonus system
// ---------------------------------------------------------------------
// Calculates and applies synergy bonuses based on unit composition.
// When multiple units of the same class are deployed together, they
// receive stat bonuses. The more units of a class present, the higher
// the tier of bonus (Tier 1/2/3).
// Tier 1: 2+ units of same class
// Tier 2: 3+ units of same class (replaces Tier 1)
// Tier 3: 4+ units of same class (replaces Tier 2)
// Call calculateSynergies() before each battle, and clearSynergies()
// after combat ends to remove bonuses.
// Purpose: Strategic depth through team composition rewards.
// -----------------------------------------------------------------
class Synergy {
public:
    // Purpose: Count units by class and apply stat bonuses (Tier 1/2/3)
    // Input: units (vector of Unit* references) - units on board
    // Output: vector of SynergyBonus - active synergies for display
    static std::vector<SynergyBonus> calculateSynergies(std::vector<Unit*>& units);

    // Purpose: Reset all synergy bonuses on given units
    // Input: units (vector of Unit*) - units to reset
    // Output: none
    static void clearSynergies(std::vector<Unit*>& units);

    // Purpose: Display active synergy bonuses
    // Input: bonuses (const vector of SynergyBonus) - active synergies
    // Output: none (prints to stdout)
    static void displaySynergies(const std::vector<SynergyBonus>& bonuses);
};

#endif // SYNERGY_H
