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
    // -----------------------------------------------------------------
    // calculateSynergies
    // What it does : counts units of each class and applies bonuses.
    //   Warrior 2+: all units get +5 ATK. 3+: +10 ATK.
    //   Mage 2+: all Mages get +15% crit. 3+: +30% crit.
    //   Tank 2+: all Tanks get +20% HP (via heal). 3+: all units +10% HP.
    //   Assassin 2+: all Assassins get +10 ATK. 3+: +20 ATK, +10% crit.
    //   Archer 2+: all Archers get +1 range. 3+: +1 range, +5 ATK.
    // Inputs : units - vector of unit pointers on the board
    // Output : vector of active SynergyBonus for display
    // -----------------------------------------------------------------
    static std::vector<SynergyBonus> calculateSynergies(std::vector<Unit*>& units);

    // -----------------------------------------------------------------
    // clearSynergies
    // What it does : resets all synergy bonuses on the given units.
    // Input  : units
    // Output : none
    // -----------------------------------------------------------------
    static void clearSynergies(std::vector<Unit*>& units);

    // -----------------------------------------------------------------
    // displaySynergies
    // What it does : prints active synergy bonuses to stdout.
    // Input  : bonuses - vector from calculateSynergies
    // Output : none (stdout)
    // -----------------------------------------------------------------
    static void displaySynergies(const std::vector<SynergyBonus>& bonuses);
};

#endif // SYNERGY_H
