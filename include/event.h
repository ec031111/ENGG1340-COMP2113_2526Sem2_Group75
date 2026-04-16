/*
================================================================================
Project:        ENGG1340/COMP2113 - Auto-Battler Arena Game
Group:          Group 75
File Type:      Header File (.h)
File Name:      event.h
Description:    Declares the Event class for managing random in-game events
                that occur between rounds. Events add unpredictability and
                strategic variance, affecting gold, units, healing, and enemy
                buffs throughout gameplay.
================================================================================
*/

#ifndef EVENT_H
#define EVENT_H

#include "player.h"
#include "board.h"
#include <string>

// ---------------------------------------------------------------------
// Event types
// ---------------------------------------------------------------------
enum EventType {
    EVENT_NONE,
    EVENT_SHOP_DISCOUNT,     // next shop costs 1 less per unit
    EVENT_GOLD_BONUS,        // bonus gold this round
    EVENT_HEAL_ALL,          // all bench units heal to full
    EVENT_ENEMY_BUFF,        // AI gets extra gold this round
    EVENT_RANDOM_FREE_UNIT,  // get a free random unit
    EVENT_GOLD_TAX,          // lose some gold
    NUM_EVENT_TYPES
};

// ---------------------------------------------------------------------
// Event
// ---------------------------------------------------------------------
// Generates and applies random events between rounds.
// Events add unpredictability and strategic variance to the game.
// ---------------------------------------------------------------------
class Event {
public:
    // Purpose: Randomly determine if event occurs and which type
    // Input: round (int) - current round number
    // Output: EventType - event type or EVENT_NONE (40% chance of event)
    static EventType rollEvent(int round);

    // Purpose: Execute event effect and modify player state
    // Input: event (EventType), player (Player reference to modify)
    // Output: string - description of event result
    static std::string applyEvent(EventType event, Player& player);

    // Purpose: Get human-readable name for event type
    // Input: event (EventType) - event enum value
    // Output: string - display name (e.g., "Gold Rush", "Divine Blessing")
    static std::string getEventName(EventType event);

    // ===== ANIMATION FUNCTIONS FOR EVENT TRIGGERS =====

    // Purpose: Display random flashy animation for event trigger
    // Input: eventName (const string reference) - event description
    // Output: none (displays animation to terminal)
    static void randomEventAnimation(const std::string& eventName);

private:
    // Track last used animations to prevent immediate repeats
    static int lastAnimationUsed_;
    static int secondLastAnimation_;
    static int thirdLastAnimation_;

    // Animation helper functions (called by randomEventAnimation)
    static void animationLoadingBar(const std::string& text);
    static void animationSpinningWheel(const std::string& text);
    static void animationBlinkingText(const std::string& text);
    static void animationSlidingTitle(const std::string& text);
};

#endif // EVENT_H
