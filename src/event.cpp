#include "event.h"
#include "shop.h"
#include <iostream>
#include <cstdlib>

// -----------------------------------------------------------------
// rollEvent
// What it does : 40% chance an event occurs each round (after round 1).
//                Returns a random event type.
// Input  : round number
// Output : EventType
// -----------------------------------------------------------------
EventType Event::rollEvent(int round) {
    if (round <= 1) return EVENT_NONE;  // no events on round 1
    if (rand() % 100 >= 40) return EVENT_NONE;  // 60% chance nothing happens

    // Pick a random event (skip EVENT_NONE = index 0)
    int roll = (rand() % (NUM_EVENT_TYPES - 1)) + 1;
    return (EventType)roll;
}

// -----------------------------------------------------------------
// applyEvent - Execute event effect and return description to player
// Description: Processes random event occurrence (gold bonus, heal, etc).
//              Modifies player state (gold, units) based on event type.
//              Returns user-facing description of what happened.
// Parameters: event - EventType enum (GOLD_BONUS, HEAL_ALL, SHOP_DISCOUNT, etc)
//             player - Player object to modify
// Returns: String describing event result for display
// Purpose: Implement random event system for game variety and surprises
// -----------------------------------------------------------------
std::string Event::applyEvent(EventType event, Player& player) {
    switch (event) {
        case EVENT_SHOP_DISCOUNT:
            // Discount is handled in shopPhase by checking a flag
            return "SHOP SALE! All units cost $1 less this round!";

        case EVENT_GOLD_BONUS: {
            int bonus = 3 + rand() % 4;  // 3-6 gold
            player.addGold(bonus);
            return "GOLD RUSH! You found " + std::to_string(bonus) + " bonus gold!";
        }

        case EVENT_HEAL_ALL: {
            const std::vector<Unit*>& bench = player.getBench();
            for (size_t i = 0; i < bench.size(); ++i) {
                bench[i]->heal(bench[i]->getMaxHp());
            }
            return "DIVINE BLESSING! All your units are fully healed!";
        }

        case EVENT_ENEMY_BUFF:
            return "WARNING: The enemy grows stronger this round! (+3 gold to AI)";

        case EVENT_RANDOM_FREE_UNIT:
            // The actual unit creation is handled in shopPhase
            return "LUCKY FIND! A free unit has been added to your bench!";

        case EVENT_GOLD_TAX: {
            int tax = 2 + rand() % 3;  // 2-4 gold
            if (player.getGold() >= tax) {
                player.spendGold(tax);
                return "TAX COLLECTOR! You lose " + std::to_string(tax) + " gold.";
            }
            return "TAX COLLECTOR arrived, but you have no gold to take!";
        }

        default:
            return "";
    }
}

// -----------------------------------------------------------------
// getEventName - Return human-readable name for event type
// Description: Converts EventType enum to user-friendly display name.
//              Used in UI, logs, and player notifications.
// Parameters: event - EventType enum value
// Returns: String name like "Gold Rush", "Divine Blessing", etc
// Purpose: Display event information in human-readable format
// -----------------------------------------------------------------
std::string Event::getEventName(EventType event) {
    switch (event) {
        case EVENT_SHOP_DISCOUNT:    return "Shop Sale";
        case EVENT_GOLD_BONUS:       return "Gold Rush";
        case EVENT_HEAL_ALL:         return "Divine Blessing";
        case EVENT_ENEMY_BUFF:       return "Enemy Buff";
        case EVENT_RANDOM_FREE_UNIT: return "Lucky Find";
        case EVENT_GOLD_TAX:         return "Tax Collector";
        default:                     return "None";
    }
}
