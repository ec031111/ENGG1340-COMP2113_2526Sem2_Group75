// ====================================================================
// Project: Auto-Battler Arena
// Group: Group 75
// File Type: game.h
// Description:
//   Main game controller class that manages the overall game flow,
//   including turn phases (shop, deploy, battle), player progression,
//   game state tracking, and battle logging. Orchestrates interactions
//   between Board, Shop, Player, AI, and other game systems.
// ====================================================================

#pragma once
#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "shop.h"
#include "player.h"
#include "ai.h"
#include "synergy.h"
#include "event.h"
#include <string>
#include <vector>
//---- word type ----
#define BOLD        "\033[1m"
#define ITALIC      "\033[3m"    
#define UNDERLINE   "\033[4m"    
//--- color define ---
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define GRAY        "\033[90m"
#define BR_RED      "\033[91m"
#define BR_GREEN    "\033[92m"
#define BR_YELLOW   "\033[93m"
#define BR_BLUE     "\033[94m"
#define BR_PURPLE   "\033[95m"
#define BR_CYAN     "\033[96m"
#define BR_WHITE    "\033[97m"

// Damage dealt to the losing player after each battle round.
const int LOSS_DAMAGE_BASE = 10;
const int LOSS_DAMAGE_PER_SURVIVING = 4;

// Maximum units player can deploy per round
// Round 1: 3, Round 2: 4, Round 3: 5, Round 4+: 6
const int MAX_DEPLOY_UNITS[] = {3, 4, 5, 6, 6, 6, 6, 6, 6, 6,  // rounds 1-10
                                 6, 6, 6, 6, 6, 6, 6, 6, 6, 6}; // rounds 11-20

// Base critical hit chance (percentage, 0-100).
const int BASE_CRIT_CHANCE = 20;
const double CRIT_MULTIPLIER = 1.5;

// Maximum combat iterations to prevent infinite loops.
const int MAX_COMBAT_TICKS = 200;

// Game phase enum to track where in the round we saved
enum GamePhase {
    PHASE_ROUND_START,
    PHASE_SHOP,
    PHASE_BATTLE
};

// Maximum rounds in the game (victory condition)
const int MAX_ROUNDS = 20;

// =====================================================================
// Settings - Structure to manage game display and audio preferences
// =====================================================================
struct Settings {
    bool animationsEnabled;  // Toggle visual animations on/off
    bool colorEnabled;       // Toggle ANSI color codes on/off
    bool soundEnabled;       // Toggle sound feedback display on/off (text-only)
    
    // Constructor: Initialize all settings to true (enabled)
    Settings() : animationsEnabled(true), colorEnabled(true), soundEnabled(true) {}
};

// ---------------------------------------------------------------------
// Game
// ---------------------------------------------------------------------
// The master class that orchestrates all game systems: shop, combat,
// synergies, events, unit merging, class abilities, and game flow.
// Note: File I/O operations are handled by the Record class.
// ---------------------------------------------------------------------
class Game {
public:
    // Purpose: Initialize game with specified difficulty and player name
    // Input: difficulty (enum) - EASY or HARD
    //        playerName (string) - name of the player
    // Output: Initialized Game object
    Game(Difficulty difficulty, const std::string& playerName = "Player");

    // Purpose: Clean up game resources and dynamically allocated memory
    // Input: none
    // Output: none
    ~Game();

    // Purpose: Main game loop orchestrating all phases until player loses
    // Input: showIntro (bool) - whether to display story intro
    // Output: int - rounds survived
    int run(bool showIntro = true);

    // Purpose: Save complete game state for later resumption
    // Input: none
    // Output: none
    void saveGame() const;

    // Purpose: Load previously saved game state
    // Input: none
    // Output: bool - true if save file exists and loads successfully
    bool loadGame();

    // Purpose: Display ranked list of all saved game records
    // Input: none
    // Output: none (prints to stdout)
    static void displayLeaderboard();

    // Accessors for persistence layer
    Board& getBoard() { return board_; }
    Shop& getShop() { return shop_; }
    Player& getPlayer() { return player_; }
    const Player& getPlayer() const { return player_; }
    const AI& getAI() const { return ai_; }
    GamePhase getCurrentPhase() const { return currentPhase_; }
    void setCurrentPhase(GamePhase phase) { currentPhase_ = phase; }
    EventType getCurrentEvent() const { return currentEvent_; }
    void setCurrentEvent(EventType event) { currentEvent_ = event; }
    bool shouldResumeShop() const { return shouldResumeShopPhase_; }
    void setShouldResumeShop(bool resume) { shouldResumeShopPhase_ = resume; }
    void setDifficulty(Difficulty difficulty) { ai_.setDifficulty(difficulty); }

private:
    Board   board_;                      // 5x8 battlefield grid
    Shop    shop_;                       // Unit shop system
    Player  player_;                     // Player 1 (human controlled)
    AI      ai_;                         // AI opponent
    bool    running_;                    // Game loop flag
    bool    skipCombat_;                 // Skip combat debug flag
    EventType currentEvent_;             // Event active this round
    int     combatPace_;                 // Battle display pace (0-3)
    GamePhase currentPhase_;             // Current phase in the round
    bool shouldResumeShopPhase_;         // Flag to resume shop phase from saved game
    int lastBattleAISurvivors_;          // Number of AI units surviving last battle (for damage calc)
    bool lastBattleWasDraw_;            // Whether last battle ended in a draw (time limit)

    // --- Phase handlers ---
    // Purpose: Handle player shopping and formation setup phase
    // Input: none
    // Output: none
    void shopPhase();

    // Purpose: Initialize combat between player and AI armies
    // Input: none
    // Output: bool - false if combat should be skipped
    bool battlePhase();

    // Purpose: Show celebratory animation at milestone rounds (5/10/15/20)
    // Input: round (int) - current round number
    // Output: none (displays animation)
    void displayMilestoneAnimation(int round) const;

    // --- Combat ---
    // Purpose: Execute tick-by-tick combat simulation until one army eliminated
    // Input: deadUnits (vector reference) - collect defeated units
    // Output: bool - true if player won this round
    bool resolveCombat(std::vector<Unit*>& deadUnits);

    // Purpose: Execute single unit attack with damage calculation and crit chance
    // Input: attacker, defender (Unit* pointers)
    // Output: int - actual damage dealt
    int  performAttack(Unit* attacker, Unit* defender);

    // Purpose: Trigger unit class special ability (Warrior/Mage/Tank/etc.)
    // Input: attacker (Unit*), defender (Unit*), allUnits (vector reference)
    // Output: none
    void performAbility(Unit* attacker, Unit* defender, std::vector<Unit*>& allUnits);

    // Purpose: Remove defeated units from board and track them
    // Input: deadUnits (vector reference) - collect killed units
    // Output: none
    void cleanupDeadUnits(std::vector<Unit*>& deadUnits);

    // --- Unit merging (3 -> 1 upgrade) ---
    // Purpose: Auto-merge 3 identical star-N units into 1 star-(N+1) unit
    // Input: none
    // Output: none
    void checkAndMerge();

    // --- Events ---
    // Purpose: Process and apply event effects for current round
    // Input: none
    // Output: none
    void handleEvent();

    // --- Free unit for EVENT_RANDOM_FREE_UNIT ---
    // Purpose: Grant player free unit from random event
    // Input: none
    // Output: none
    void giveRandomFreeUnit();

    // --- UI ---
    // Purpose: Display command reference and game help information
    // Input: none
    // Output: none (prints to stdout)
    void printHelp() const;

    // Purpose: Display quick command tips during gameplay
    // Input: none
    // Output: none (prints to stdout)
    void printCommandTips() const;

    // Purpose: Show compact player stats (HP, Gold, Round, Streaks)
    // Input: none
    // Output: none (prints to stdout)
    void printStatusBar() const;

    // Purpose: Show current vs max unit deployment count
    // Input: none
    // Output: none (prints to stdout)
    void printDeployLimit() const;

    // Purpose: Display player unit formation with HP bars
    // Input: none
    // Output: none (prints to stdout)
    void printFormation() const;

    // Purpose: Display narrative story introduction for new game sessions
    // Input: none
    // Output: none (prints to stdout)
    void showIntro() const;

    // Purpose: Display celebratory victory animation when player survives all rounds
    // Input: none
    // Output: none (prints animated victory screen to stdout)
    void showVictoryScreen() const;

    // Purpose: Display animated game over screen when player HP reaches 0
    // Input: none
    // Output: none (prints animated defeat screen to stdout)
    void showDefeatScreen() const;

    // Purpose: Set battle display pace level
    // Input: pace (int) - 0=SLOW, 1=NORMAL, 2=FAST, 3=FASTEST
    // Output: none
    void setCombatPace(int pace);

    // Purpose: Get current battle display pace setting
    // Input: none
    // Output: int - pace level (0-3)
    int getCombatPace() const;

    // Purpose: Get deployment limit for current round
    // Input: none
    // Output: int - maximum units allowed this round
    int getMaxDeployUnits() const;

    // Purpose: Automatically save game to slot 1 after each round
    // Input: none
    // Output: none (silent operation)
    void performAutosave();

    // Purpose: Display settings menu and allow user to toggle preferences
    // Input: none
    // Output: none
    void settingsMenu();

    // Accessor for color setting
    // Purpose: Check if colors are enabled for display
    // Input: none
    // Output: bool - true if colors enabled
    bool isColorEnabled() const { return settings_.colorEnabled; }

private:
    Settings settings_;                  // Game display and audio preferences
};

#endif // GAME_H
