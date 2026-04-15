/*
================================================================================
Project:        ENGG1340/COMP2113 - Auto-Battler Arena Game
Group:          Group 75
File Type:      Implementation File (.cpp)
File Name:      event.cpp
Description:    Implements the Event class for managing random in-game events.
                Handles event probability calculations, event application,
                and impacts on player gold, units, and game state between
                rounds.
================================================================================
*/

#include "event.h"
#include "shop.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>

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

// ===== ANIMATION SYSTEM =====

// Static member variable initialization
int Event::lastAnimationUsed_ = -1;
int Event::secondLastAnimation_ = -1;
int Event::thirdLastAnimation_ = -1;

// -----------------------------------------------------------------
// animationLoadingBar - Display a filling progress bar animation
// Shows bar filling from 0% to 100% with yellow color
// Parameters: text - Event name/description to display above bar
// Returns: void (displays animation)
// -----------------------------------------------------------------
void Event::animationLoadingBar(const std::string& text) {
    std::cout << "\n" << "\033[33m"; // YELLOW
    std::cout << "  ⚡ " << text << "\n\n";
    
    for (int i = 0; i <= 100; i += 5) {
        std::cout << "  [";
        for (int j = 0; j < 20; j++) {
            if (j < (i / 5)) std::cout << "█";
            else std::cout << "░";
        }
        std::cout << "] " << i << "%\r";
        std::cout.flush();
        usleep(40000); // 40ms delay
    }
    std::cout << "\n  EVENT TRIGGERED!" << "\033[0m" << std::endl;
    usleep(200000); // 200ms delay before continuing
}

// -----------------------------------------------------------------
// animationSpinningWheel - Display a spinning wheel animation
// Shows rotating symbols: /, -, \, | in cyan color
// Parameters: text - Event name/description to display
// Returns: void (displays animation)
// -----------------------------------------------------------------
void Event::animationSpinningWheel(const std::string& text) {
    std::cout << "\n" << "\033[36m"; // CYAN
    std::cout << "  ✨ " << text << "\n\n";
    
    const char spinners[] = {'/', '-', '\\', '|'};
    for (int i = 0; i < 12; i++) {
        std::cout << "  " << spinners[i % 4] << " Processing...\r";
        std::cout.flush();
        usleep(80000); // 80ms delay
    }
    std::cout << "  ✓ EVENT ACTIVATED!" << "\033[0m" << std::endl;
    usleep(200000); // 200ms delay
}

// -----------------------------------------------------------------
// animationBlinkingText - Display blinking event text
// Text blinks on and off in green color for dramatic effect
// Parameters: text - Event name/description to blink
// Returns: void (displays animation)
// -----------------------------------------------------------------
void Event::animationBlinkingText(const std::string& text) {
    std::cout << "\n" << "\033[32m"; // GREEN
    
    for (int i = 0; i < 6; i++) {
        if (i % 2 == 0) {
            std::cout << "  ► " << text << " ◄\n";
        } else {
            std::cout << "                                        \r";
        }
        std::cout.flush();
        usleep(150000); // 150ms delay
    }
    std::cout << "  ▶ " << text << " ◀" << "\033[0m" << std::endl;
    usleep(200000); // 200ms delay
}

// -----------------------------------------------------------------
// animationSlidingTitle - Display text sliding across screen
// Title slides from right to left in bright red
// Parameters: text - Event name/description to slide
// Returns: void (displays animation)
// -----------------------------------------------------------------
void Event::animationSlidingTitle(const std::string& text) {
    std::cout << "\n" << "\033[91m"; // BRIGHT RED
    
    int screenWidth = 60;
    for (int pos = screenWidth; pos >= 0; pos--) {
        std::cout << "  " << std::string(pos, ' ') << ">>> " << text << " <<<\r";
        std::cout.flush();
        usleep(30000); // 30ms delay
    }
    std::cout << "\n  >>> EVENT TRIGGERED! <<<" << "\033[0m" << std::endl;
    usleep(200000); // 200ms delay
}

// -----------------------------------------------------------------
// randomEventAnimation - Play random animation (no immediate repeats)
// Selects from 4 different animation styles
// Ensures the same animation doesn't play 3 times in a row
// Parameters: eventName - Name of the event to announce
// Returns: void (displays random animation)
// -----------------------------------------------------------------
void Event::randomEventAnimation(const std::string& eventName) {
    // Pick a random animation (0-3)
    int selectedAnimation = rand() % 4;
    
    // Keep picking if we got the same animation too recently
    int attempts = 0;
    while ((selectedAnimation == lastAnimationUsed_ || 
            selectedAnimation == secondLastAnimation_ ||
            selectedAnimation == thirdLastAnimation_) && attempts < 10) {
        selectedAnimation = rand() % 4;
        attempts++;
    }
    
    // Update animation history
    thirdLastAnimation_ = secondLastAnimation_;
    secondLastAnimation_ = lastAnimationUsed_;
    lastAnimationUsed_ = selectedAnimation;
    
    // Call the appropriate animation
    switch (selectedAnimation) {
        case 0:
            animationLoadingBar(eventName);
            break;
        case 1:
            animationSpinningWheel(eventName);
            break;
        case 2:
            animationBlinkingText(eventName);
            break;
        case 3:
            animationSlidingTitle(eventName);
            break;
    }
}
