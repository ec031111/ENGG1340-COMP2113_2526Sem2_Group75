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
    // -----------------------------------------------------------------
    // rollEvent - Randomly determine if an event occurs and which one
    // Probability: 40% chance of event, then random type selection
    // Parameters: round - Current round number (affects event types)
    // Returns: EventType (EVENT_NONE if no event, otherwise specific type)
    // Purpose: Generate random events for game variety and unpredictability
    // -----------------------------------------------------------------
    static EventType rollEvent(int round);

    // -----------------------------------------------------------------
    // applyEvent - Execute event effect and return description
    // Processes random event occurrence (GOLD_BONUS, HEAL_ALL, etc.)
    // Modifies player state (gold, units) based on event type
    // Parameters: event - EventType enum (GOLD_BONUS, HEAL_ALL, etc.)
    //             player - Player object to modify with event effect
    // Returns: String describing event result for display to player
    // Purpose: Implement random event system for game variety
    // -----------------------------------------------------------------
    static std::string applyEvent(EventType event, Player& player);

    // -----------------------------------------------------------------
    // getEventName - Return human-readable name for event type
    // Converts EventType enum to user-friendly display name
    // Used in UI, logs, and player notifications
    // Parameters: event - EventType enum value
    // Returns: String name like "Gold Rush", "Divine Blessing", etc.
    // Purpose: Display event information in human-readable format
    // -----------------------------------------------------------------
    static std::string getEventName(EventType event);

    // ===== ANIMATION FUNCTIONS FOR EVENT TRIGGERS =====

    // -----------------------------------------------------------------
    // randomEventAnimation - Play random flashy animation for event trigger
    // Automatically selects one of 4 different animation styles
    // Ensures no same animation repeats within 3 consecutive calls
    // Animations: Loading bar, spinning wheel, blinking text, sliding title
    // Parameters: eventName - Name/description of the event to announce
    // Returns: void (displays animation to terminal)
    // Purpose: Create exciting visual feedback when random events trigger
    // -----------------------------------------------------------------
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
