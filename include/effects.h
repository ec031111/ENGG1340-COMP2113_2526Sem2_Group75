//================================================================================
//Project:        ENGG1340/COMP2113 - Auto-Battler Arena Game
//Group:          Group 75
//File Type:      Header File (.h)
//File Name:      effects.h
//Description:    Declares battle visual effects functions including skill
//                animations, round enter/exit animations, and combat tick
//                annotations. Supports color and animation toggle settings.
//================================================================================

#ifndef EFFECTS_H
#define EFFECTS_H

#include "unit.h"

//----------------------------------------------------------------------------------
// ActionType - Enumeration of combat action types for tick annotations
//----------------------------------------------------------------------------------
enum ActionType {
    MOVE = 0,    // Unit movement action
    ATTACK = 1,  // Standard attack action
    CRIT = 2,    // Critical hit action
    ABILITY = 3, // Class ability activation
    EVADE = 4,   // Evasion/dodge action
    KILL = 5,    // Kill action
    HEAL = 6,    // Healing action
    SHIELD = 7   // Shield/block action
};

//----------------------------------------------------------------------------------
// playEnterAnimation - Play round enter animation with round number display
//----------------------------------------------------------------------------------
// Input: round (int) - current round number
//        colorEnabled (bool) - whether to use ANSI color codes
//        animEnabled (bool) - whether to animate with delays
// Output: none (prints animation to stdout)
//----------------------------------------------------------------------------------
void playEnterAnimation(int round, bool colorEnabled, bool animEnabled);

//----------------------------------------------------------------------------------
// playExitAnimation - Play round exit animation with victory/defeat display
//----------------------------------------------------------------------------------
// Input: playerWon (bool) - true for victory, false for defeat
//        colorEnabled (bool) - whether to use ANSI color codes
//        animEnabled (bool) - whether to animate with delays
//        isLastKill (bool) - true if this is the final enemy kill
// Output: none (prints animation to stdout)
//----------------------------------------------------------------------------------
void playExitAnimation(bool playerWon, bool colorEnabled, bool animEnabled, bool isLastKill);

//----------------------------------------------------------------------------------
// playSkillEffect - Play class-specific skill activation animation
//----------------------------------------------------------------------------------
// Input: cls (UnitClass) - unit class determining animation style
//        colorEnabled (bool) - whether to use ANSI color codes
//        animEnabled (bool) - whether to animate with delays
// Output: none (prints animation to stdout)
// Note: Each class has unique symbol pattern and frame count
//----------------------------------------------------------------------------------
void playSkillEffect(UnitClass cls, bool colorEnabled, bool animEnabled);

//----------------------------------------------------------------------------------
// printTickAction - Print colored combat action annotation line
//----------------------------------------------------------------------------------
// Input: action (ActionType) - type of action being performed
//        unit (Unit*) - unit performing the action
//        value (int) - action value (damage, steps, heal amount, etc.)
//        colorEnabled (bool) - whether to use ANSI color codes
// Output: none (prints action line to stdout)
//----------------------------------------------------------------------------------
void printTickAction(ActionType action, Unit* unit, int value, bool colorEnabled);

#endif // EFFECTS_H
