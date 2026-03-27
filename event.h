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
    // rollEvent
    // What it does : randomly decides if an event occurs (40% chance)
    //                and which event it is.
    // Input  : round - current round number
    // Output : EventType enum value
    // -----------------------------------------------------------------
    static EventType rollEvent(int round);

    // -----------------------------------------------------------------
    // applyEvent
    // What it does : applies the given event to the player/game state
    //                and prints a description.
    // Inputs : event, player, board
    // Output : string description of what happened
    // -----------------------------------------------------------------
    static std::string applyEvent(EventType event, Player& player);

    // -----------------------------------------------------------------
    // getEventName
    // What it does : returns a short display name for an event.
    // Input  : event type
    // Output : string name
    // -----------------------------------------------------------------
    static std::string getEventName(EventType event);
};

#endif // EVENT_H
