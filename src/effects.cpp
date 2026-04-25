//================================================================================
//Project:        ENGG1340/COMP2113 - Auto-Battler Arena Game
//Group:          Group 75
//File Type:      Implementation File (.cpp)
//File Name:      effects.cpp
//Description:    Implements battle visual effects including skill animations,
//                round enter/exit animations, and combat tick annotations.
//                Supports color and animation toggle settings.
//================================================================================

#include "effects.h"
#include "game.h"
#include <iostream>
#include <unistd.h>
#include <string>

// Color string constants (used when colorEnabled is true)
#define C_RED     "\033[31m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_BLUE    "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_CYAN    "\033[36m"
#define C_RESET   "\033[0m"

//----------------------------------------------------------------------------------
// Helper macro to get color code string based on enabled flag
//----------------------------------------------------------------------------------
#define COLOR_CODE(colorStr) (colorEnabled ? colorStr : "")

//----------------------------------------------------------------------------------
// Helper macro for animation delay
//----------------------------------------------------------------------------------
#define ANIM_DELAY(us) if (animEnabled) usleep(us)

//----------------------------------------------------------------------------------
// playEnterAnimation - Round enter animation with border and flash effect
//----------------------------------------------------------------------------------
void playEnterAnimation(int round, bool colorEnabled, bool animEnabled) {
    std::string green = COLOR_CODE(C_GREEN);
    std::string yellow = COLOR_CODE(C_YELLOW);
    std::string reset = COLOR_CODE(C_RESET);

    // Frame 1: Draw border box
    std::cout << "\n";
    std::cout << green << "╔══════════════════════════════════════╗" << reset << "\n";
    std::cout << green << "║          ROUND " << round << " BEGINS!            ║" << reset << "\n";
    std::cout << green << "║                                      ║" << reset << "\n";
    std::cout << green << "║    ⚔️  Prepare your units!  ⚔️        ║" << reset << "\n";
    std::cout << green << "╚══════════════════════════════════════╝" << reset << "\n";
    std::cout << "\n";

    ANIM_DELAY(500000);

    // Frame 2: Flash highlight
    std::cout << yellow << ">>> Units are entering the battlefield! <<<" << reset << "\n";
    std::cout << "\n";

    ANIM_DELAY(300000);
}

//----------------------------------------------------------------------------------
// playExitAnimation - Victory or defeat animation with optional last-kill celebration
//----------------------------------------------------------------------------------
void playExitAnimation(bool playerWon, bool colorEnabled, bool animEnabled, bool isLastKill) {
    std::string color = playerWon ? COLOR_CODE(C_GREEN) : COLOR_CODE(C_RED);
    std::string yellow = COLOR_CODE(C_YELLOW);
    std::string reset = COLOR_CODE(C_RESET);

    // Special last-kill congratulations animation (drawn with ★ border)
    if (isLastKill) {
        std::cout << "\n";
        std::cout << yellow;
        std::cout << "★───────────────────────────────────────★" << "\n";
        std::cout << "★                                       ★" << "\n";
        std::cout << "★    🎉 CONGRATULATIONS! 🎉              ★" << "\n";
        std::cout << "★                                       ★" << "\n";
        std::cout << "★     The enemy army has been wiped out! ★" << "\n";
        std::cout << "★                                       ★" << "\n";
        std::cout << "★───────────────────────────────────────★" << reset << "\n";
        std::cout << "\n";
        ANIM_DELAY(500000);
    }

    if (playerWon) {
        // Victory animation
        std::cout << "\n";
        std::cout << color << "╔══════════════════════════════════════╗" << reset << "\n";
        std::cout << color << "║          VICTORY!                     ║" << reset << "\n";
        std::cout << color << "║                                      ║" << reset << "\n";
        std::cout << color << "║     ✓ Enemies eliminated!           ║" << reset << "\n";
        std::cout << color << "║                                      ║" << reset << "\n";
        std::cout << color << "╚══════════════════════════════════════╝" << reset << "\n";
        std::cout << "\n";
    } else {
        // Defeat animation
        std::cout << "\n";
        std::cout << color << "╔══════════════════════════════════════╗" << reset << "\n";
        std::cout << color << "║          DEFEAT!                      ║" << reset << "\n";
        std::cout << color << "║                                      ║" << reset << "\n";
        std::cout << color << "║     ✗ Units fell in battle...        ║" << reset << "\n";
        std::cout << color << "║                                      ║" << reset << "\n";
        std::cout << color << "╚══════════════════════════════════════╝" << reset << "\n";
        std::cout << "\n";
    }

    ANIM_DELAY(300000);
}

//----------------------------------------------------------------------------------
// playSkillEffect - Class-specific skill animation with unique symbol patterns
//----------------------------------------------------------------------------------
void playSkillEffect(UnitClass cls, bool colorEnabled, bool animEnabled) {
    std::string red = COLOR_CODE(C_RED);
    std::string blue = COLOR_CODE(C_BLUE);
    std::string cyan = COLOR_CODE(C_CYAN);
    std::string magenta = COLOR_CODE(C_MAGENTA);
    std::string yellow = COLOR_CODE(C_YELLOW);
    std::string reset = COLOR_CODE(C_RESET);

    switch (cls) {
        case WARRIOR: {
            // Red '*' symbols in fan spread pattern, 3 frames
            std::cout << red;
            std::cout << "    *         " << "\n";
            std::cout << "   ***        " << "\n";
            std::cout << "  *****       " << "\n";
            std::cout << " *******      " << reset << "\n";
            ANIM_DELAY(500000);

            std::cout << red;
            std::cout << "      *       " << "\n";
            std::cout << "     ***      " << "\n";
            std::cout << "    *****     " << "\n";
            std::cout << "   *******    " << reset << "\n";
            ANIM_DELAY(500000);

            std::cout << red;
            std::cout << "          *   " << "\n";
            std::cout << "         ***  " << "\n";
            std::cout << "        ***** " << "\n";
            std::cout << "       *******" << reset << "\n";
            ANIM_DELAY(500000);
            break;
        }

        case MAGE: {
            // Blue '@' symbols in circular ripple, 4 frames
            std::cout << blue;
            std::cout << "      @       " << "\n";
            std::cout << reset;
            ANIM_DELAY(500000);

            std::cout << blue;
            std::cout << "    @   @     " << "\n";
            std::cout << "   @     @    " << "\n";
            std::cout << "    @   @     " << reset << "\n";
            ANIM_DELAY(500000);

            std::cout << blue;
            std::cout << "  @       @   " << "\n";
            std::cout << "   @     @    " << "\n";
            std::cout << "  @       @   " << "\n";
            std::cout << "   @     @    " << "\n";
            std::cout << "  @       @   " << reset << "\n";
            ANIM_DELAY(500000);

            std::cout << blue;
            std::cout << " @         @  " << "\n";
            for (int i = 0; i < 5; i++) {
                std::cout << "  @       @   " << "\n";
            }
            std::cout << " @         @  " << reset << "\n";
            ANIM_DELAY(500000);
            break;
        }

        case TANK: {
            // Cyan '#' symbols blinking 3 times around shield shape
            std::cout << cyan;
            std::cout << "    #####     " << "\n";
            std::cout << "   ##   ##    " << "\n";
            std::cout << "  ##     ##   " << "\n";
            std::cout << " ##       ##  " << reset << "\n";
            ANIM_DELAY(500000);

            std::cout << cyan;
            std::cout << "    #####     " << "\n";
            std::cout << "   #     #    " << "\n";
            std::cout << "  #       #   " << "\n";
            std::cout << " #         #  " << reset << "\n";
            ANIM_DELAY(500000);

            std::cout << cyan;
            std::cout << "    #####     " << "\n";
            std::cout << "   ##   ##    " << "\n";
            std::cout << "  ##     ##   " << "\n";
            std::cout << " ##       ##  " << reset << "\n";
            ANIM_DELAY(500000);
            break;
        }

        case ASSASSIN: {
            // Magenta '>' symbols flashing quickly, 2 frames
            std::cout << magenta;
            std::cout << "   >>         " << "\n";
            std::cout << "    >>        " << "\n";
            std::cout << "     >>       " << "\n";
            std::cout << "      >>      " << reset << "\n";
            ANIM_DELAY(500000);

            std::cout << magenta;
            std::cout << "         >>   " << "\n";
            std::cout << "        >>    " << "\n";
            std::cout << "       >>     " << "\n";
            std::cout << "      >>      " << reset << "\n";
            ANIM_DELAY(500000);
            break;
        }

        case ARCHER: {
            // Yellow '|' arrow symbols shooting out, 2 frames
            std::cout << yellow;
            std::cout << "   |          " << "\n";
            std::cout << "   |          " << "\n";
            std::cout << "   |          " << "\n";
            std::cout << "   |          " << reset << "\n";
            ANIM_DELAY(500000);

            std::cout << yellow;
            std::cout << "        |    " << "\n";
            std::cout << "        |    " << "\n";
            std::cout << "        |    " << "\n";
            std::cout << "        |    " << reset << "\n";
            ANIM_DELAY(500000);
            break;
        }

        default:
            break;
    }
}

//----------------------------------------------------------------------------------
// printTickAction - Print colored combat action annotation with emoji symbols
//----------------------------------------------------------------------------------
void printTickAction(ActionType action, Unit* unit, int value, bool colorEnabled) {
    if (!unit) return;

    std::string red = COLOR_CODE(C_RED);
    std::string green = COLOR_CODE(C_GREEN);
    std::string yellow = COLOR_CODE(C_YELLOW);
    std::string blue = COLOR_CODE(C_BLUE);
    std::string magenta = COLOR_CODE(C_MAGENTA);
    std::string cyan = COLOR_CODE(C_CYAN);
    std::string reset = COLOR_CODE(C_RESET);

    std::string name = unit->getName();

    switch (action) {
        case MOVE:
            std::cout << cyan << "[" << "🏃" << "]" << reset << " " << name
                      << " moves toward enemy (" << value << " steps)";
            break;

        case ATTACK:
            std::cout << red << "[" << "⚔️" << "]" << reset << " " << name
                      << " ⚔️ " << value << " damage dealt!";
            break;

        case CRIT:
            std::cout << yellow << "[" << "💥" << "]" << reset << " " << name
                      << " 💥 Backstab crit! " << value << " damage!";
            break;

        case ABILITY:
            std::cout << blue << "[" << "✨" << "]" << reset << " " << name
                      << " ✨ casts AOE! Hits " << value << " enemies for 10 each!";
            break;

        case EVADE:
            std::cout << cyan << "[" << "👻" << "]" << reset << " " << name
                      << " 👻 evades the attack!";
            break;

        case KILL:
            std::cout << magenta << "[" << "☠️" << "]" << reset << " " << name
                      << " ☠️ " << value << " slain!";
            break;

        case HEAL:
            std::cout << green << "[" << "💚" << "]" << reset << " " << name
                      << " 💚 heals for " << value << " HP!";
            break;

        case SHIELD:
            std::cout << cyan << "[" << "🛡️" << "]" << reset << " " << name
                      << " 🛡️ Block absorbs " << value << " damage!";
            break;

        default:
            break;
    }

    std::cout << reset << "\n";
}
