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
const int LOSS_DAMAGE_BASE = 5;
const int LOSS_DAMAGE_PER_SURVIVING = 2;

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

// Round type enum - determines what type of round this is
enum RoundType {
    ROUND_TYPE_PVE,      // Round 1,2,3,5 - Fight weak monsters
    ROUND_TYPE_EVENT,    // Round 7 - Special event with rewards, no combat
    ROUND_TYPE_PVP       // Normal rounds - Fight AI opponent
};

// PVE Round constants
const int PVE_ROUNDS[] = {1, 2, 3, 5};
const int EVENT_ROUNDS[] = {7};
const int PVE_DAMAGE_ON_LOSS = 3;      // Damage taken if lose PVE round
const int PVE_GOLD_REWARD = 20;        // Gold gained if win PVE round
const int PVE_EQUIPMENT_REWARD = 1;    // Number of random units given as reward

// ---------------------------------------------------------------------
// Game
// ---------------------------------------------------------------------
// The master class that orchestrates all game systems: shop, combat,
// synergies, events, unit merging, class abilities, and game flow.
// Note: File I/O operations are handled by the Record class.
// ---------------------------------------------------------------------
class Game {
public:
    // -----------------------------------------------------------------
    // Constructor - Initialize game with specified difficulty
    // Parameters: difficulty - EASY or HARD (affects AI strategy)
    // -----------------------------------------------------------------
    Game(Difficulty difficulty);

    // -----------------------------------------------------------------
    // Destructor - Clean up game resources and dynamically allocated memory
    // -----------------------------------------------------------------
    ~Game();

    // -----------------------------------------------------------------
    // run - Main game loop orchestrating all phases until player loses
    // Game Flow: SHOP -> EVENT -> BATTLE -> RESULT -> REPEAT until HP=0
    // Description: Coordinates all gameplay phases in sequence:
    //              1. SHOP: Player buys/sells/places units
    //              2. EVENT: Random event effects (gold, healing, etc.)
    //              3. BATTLE: Auto-combat between armies
    //              4. RESULT: Process outcome, damage, rewards, merging
    // Parameters: showIntro - Display story intro (true for new games)
    // Returns: Final score = number of rounds survived before defeat
    // Purpose: Core game engine loop managing entire game progression
    // -----------------------------------------------------------------
    int run(bool showIntro = true);

    // -----------------------------------------------------------------
    // File I/O methods
    // -----------------------------------------------------------------
    // saveRecord - Save final score and game stats to leaderboard
    // -----------------------------------------------------------------
    void saveRecord() const;

    // -----------------------------------------------------------------
    // saveGame - Save complete game state for later resumption
    // Saves: player HP/gold/round, bench units, board state
    // -----------------------------------------------------------------
    void saveGame() const;

    // -----------------------------------------------------------------
    // loadGame - Load previously saved game state
    // Returns: true if save file exists and loads successfully
    // -----------------------------------------------------------------
    bool loadGame();

    // -----------------------------------------------------------------
    // displayLeaderboard - Static method to show all saved game records
    // Displays ranked list of scores and player performance
    // -----------------------------------------------------------------
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

private:
    Board   board_;
    Shop    shop_;
    Player  player_;
    AI      ai_;
    bool    running_;
    bool    skipCombat_;
    EventType currentEvent_;  // event active this round
    int     combatPace_;      // battle display pace (0-3)
    GamePhase currentPhase_;  // current phase in the round
    bool shouldResumeShopPhase_;  // flag to resume shop phase from saved game

    // --- Phase handlers ---
    // -----------------------------------------------------------------
    // shopPhase - Player shopping and formation setup phase
    // Main loop where players buy/sell units, arrange formation,
    // manage resources, and prepare for battles
    // -----------------------------------------------------------------
    void shopPhase();

    // -----------------------------------------------------------------
    // battlePhase - Initialize combat between player and AI armies
    // Sets up board, positions units, and initiates combat resolution
    // Returns: false if combat should be skipped
    // -----------------------------------------------------------------
    bool battlePhase();

    // -----------------------------------------------------------------
    // displayMilestoneAnimation - Show celebratory animation at rounds 5/10/15/20
    // Shows milestone achievement with visual effects and messages
    // -----------------------------------------------------------------
    void displayMilestoneAnimation(int round) const;

    // --- Combat ---
    // -----------------------------------------------------------------
    // resolveCombat - Execute tick-by-tick combat simulation
    // Processes attack rounds until one army is eliminated
    // Parameters: deadUnits - Vector to collect defeated units
    // Returns: true if player won this round
    // -----------------------------------------------------------------
    bool resolveCombat(std::vector<Unit*>& deadUnits);

    // -----------------------------------------------------------------
    // performAttack - Execute single unit attack with damage calculation
    // Includes damage roll, critical hit chance, and damage application
    // Returns: Actual damage dealt to defender
    // -----------------------------------------------------------------
    int  performAttack(Unit* attacker, Unit* defender);

    // -----------------------------------------------------------------
    // performAbility - Trigger unit-class special ability
    // Warrior: Rage mode (double turn), Mage: AOE, Tank: Block, etc.
    // Parameters: allUnits - All units on board for AOE/multi-target effects
    // -----------------------------------------------------------------
    void performAbility(Unit* attacker, Unit* defender, std::vector<Unit*>& allUnits);

    // -----------------------------------------------------------------
    // cleanupDeadUnits - Remove defeated units from board and track them
    // Collects killed units in deadUnits vector for later processing
    // -----------------------------------------------------------------
    void cleanupDeadUnits(std::vector<Unit*>& deadUnits);

    // --- Unit merging (3 -> 1 upgrade) ---
    // -----------------------------------------------------------------
    // checkAndMerge - Auto-merge: 3 identical star-N units → 1 star-(N+1)
    // Automatically upgrades units when merge conditions are met
    // -----------------------------------------------------------------
    void checkAndMerge();

    // --- Events ---
    // -----------------------------------------------------------------
    // handleEvent - Process and apply event effects for current round
    // Applies random event (gold bonus, healing, etc.) and displays result
    // -----------------------------------------------------------------
    void handleEvent();

    // --- Free unit for EVENT_RANDOM_FREE_UNIT ---
    // -----------------------------------------------------------------
    // giveRandomFreeUnit - Grant player free unit from random event
    // Creates temporary shop, picks random unit, adds to player bench
    // -----------------------------------------------------------------
    void giveRandomFreeUnit();

    // --- UI ---
    // -----------------------------------------------------------------
    // printHelp - Display command reference and game help information
    // Shows all available commands and their descriptions
    // -----------------------------------------------------------------
    void printHelp() const;

    // -----------------------------------------------------------------
    // printCommandTips - Display quick command tips during gameplay
    // Short reference for common commands (buy, sell, place, etc.)
    // -----------------------------------------------------------------
    void printCommandTips() const;

    // -----------------------------------------------------------------
    // printStatusBar - Show compact player stats (HP, Gold, Round, Streaks)
    // One-line status summary for during gameplay
    // -----------------------------------------------------------------
    void printStatusBar() const;

    // -----------------------------------------------------------------
    // printDeployLimit - Show current/max unit deployment count
    // Displays "Units deployed: X/Y" for current round
    // -----------------------------------------------------------------
    void printDeployLimit() const;

    // -----------------------------------------------------------------
    // printFormation - Display player unit formation with HP bars
    // Visual confirmation of army composition before battle
    // -----------------------------------------------------------------
    void printFormation() const;

    // -----------------------------------------------------------------
    // showIntro - Display narrative story introduction for new game sessions
    // Shows opening story about realm, characters, and player mission
    // -----------------------------------------------------------------
    void showIntro() const;

    // -----------------------------------------------------------------
    // setCombatPace - Set battle display pace (0-3 levels)
    // Levels: 0=SLOW (extra delay), 1=NORMAL (wait input), 2=FAST (600ms), 3=FASTEST (instant)
    // -----------------------------------------------------------------
    void setCombatPace(int pace);

    // -----------------------------------------------------------------
    // getCombatPace - Get current battle display pace setting
    // Returns: Current pace level (0-3)
    // -----------------------------------------------------------------
    int getCombatPace() const;

    // -----------------------------------------------------------------
    // getMaxDeployUnits - Get deployment limit for current round
    // Scaling: Round 1-3: 3-5 units, Round 4+: 6 units (capped at round 20)
    // Returns: Maximum units allowed this round
    // -----------------------------------------------------------------
    int getMaxDeployUnits() const;

    // -----------------------------------------------------------------
    // showBattleRewards - Display colored rewards information after battle
    // Shows win/loss streak bonuses, interest earned, and total new gold
    // Parameters: playerWon - whether player won the battle
    // -----------------------------------------------------------------
    void showBattleRewards(bool playerWon);

    // -----------------------------------------------------------------
    // getRoundType - Determine the type of round (PVE/EVENT/PVP)
    // Parameters: round - the round number (1-indexed)
    // Returns: RoundType enum indicating the round type
    // -----------------------------------------------------------------
    RoundType getRoundType(int round) const;

    // -----------------------------------------------------------------
    // pvePhase - Handle PVE battle against weak monsters
    // Creates simple monsters based on round difficulty
    // Returns: true if player won the PVE battle
    // -----------------------------------------------------------------
    bool pvePhase();

    // -----------------------------------------------------------------
    // eventPhase - Handle special event round with direct rewards
    // Gives players immediate rewards without combat
    // -----------------------------------------------------------------
    void eventPhase();

    // -----------------------------------------------------------------
    // createPveMonsters - Create array of weak monsters for PVE round
    // Parameters: round - the current round number (affects scale)
    // Returns: Vector of monster units to fight
    // -----------------------------------------------------------------
    std::vector<Unit*> createPveMonsters(int round);

private:
    RoundType currentRoundType_;  // Type of current round
};

#endif // GAME_H
