// ====================================================================
// Project: Auto-Battler Arena
// Group: Group 75
// File Type: game.cpp (Implementation File)
// Description:
//   Implementation of the Game class. Handles game initialization,
//   phase management (shop, deploy, battle), player interactions,
//   round progression, battle resolution with colored HP bars,
//   income/reward system, streak bonuses, battle logging, and
//   special round types (PVE, events, and PVP).
// ====================================================================

#include "game.h"
#include "record.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <unistd.h>
#include <thread>
#include <chrono>

static int visibleWidth(const std::string& text) {
    int width = 0;
    bool inEscape = false;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\033') {
            inEscape = true;
            continue;
        }
        if (inEscape) {
            if (text[i] == 'm') inEscape = false;
            continue;
        }

        unsigned char ch = static_cast<unsigned char>(text[i]);
        if ((ch & 0x80) == 0) {
            ++width;
        } else if ((ch & 0xE0) == 0xC0) {
            width += 2;
            ++i;
        } else if ((ch & 0xF0) == 0xE0) {
            width += 2;
            i += 2;
        } else if ((ch & 0xF8) == 0xF0) {
            width += 2;
            i += 3;
        }
    }
    return width;
}

// -----------------------------------------------------------------
// trim - Remove leading and trailing whitespace from string
// What it does : strips spaces, tabs, newlines from string edges
// Input  : s - string to trim
// Output : trimmed string without leading/trailing whitespace
// -----------------------------------------------------------------
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

// -----------------------------------------------------------------
// toLower - Convert string to lowercase
// What it does : converts all uppercase letters to lowercase
// Input  : s - string to convert
// Output : string with all letters in lowercase
// -----------------------------------------------------------------
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}


// Helper: print a line padded to width W inside |...|
static void printBoxLine(const std::string& text, int W) {
    std::string s = text;
    int width = visibleWidth(s);
    if (width < W) s += std::string(W - width, ' ');
    std::cout << "  |" << s << "|" << std::endl;
}

// Helper: print centered title
static void printBoxTitle(const std::string& title, int W) {
    int titleWidth = visibleWidth(title);
    int pad = (W - titleWidth) / 2;
    if (pad < 0) pad = 0;
    std::cout << "  |" << std::string(pad, ' ') << title
              << std::string(std::max(0, W - pad - titleWidth), ' ') << "|" << std::endl;
}

// Helper: build an HP bar string of given width
// e.g. hpBar(current, max, 10) => "===       "
static std::string hpBar(int hp, int maxHp, int width) {
    if (maxHp <= 0) return std::string(width, ' ');
    int filled = (hp * width) / maxHp;
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;
    return std::string(filled, '=') + std::string(width - filled, ' ');
}

// Helper: build a colored HP bar - Green if > 60%, Red if <= 60%
// e.g. hpBarColored(current, max, 10) => "[GREEN]===       [RESET]" or "[RED]===       [RESET]"
static std::string hpBarColored(int hp, int maxHp, int width) {
    if (maxHp <= 0) return std::string(width, ' ');
    int filled = (hp * width) / maxHp;
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;
    
    // Calculate HP percentage
    int hpPercent = (hp * 100) / maxHp;
    
    // Choose color: Green if > 60%, Red otherwise
    std::string color = (hpPercent > 60) ? GREEN : RED;
    std::string bar = std::string(filled, '=') + std::string(width - filled, ' ');
    
    return BOLD + color + "[" + bar + "]" + RESET;
}

// Helper: preferred column for smart placement by class
static int smartPlaceCol(UnitClass cls) {
    switch (cls) {
        case TANK:     return 3;  // front line
        case WARRIOR:  return 3;
        case ARCHER:   return 0;  // back line
        case MAGE:     return 0;
        case ASSASSIN: return 1;  // middle
        default:       return 2;
    }
}

// Helper: smart-place a unit on the player side
static bool smartPlaceUnit(Board& board, Unit* unit) {
    int pc = smartPlaceCol(unit->getClass());
    for (int off = 0; off <= PLAYER_MAX_COL; ++off) {
        for (int d = 0; d < 2; ++d) {
            int c = (d == 0) ? pc - off : pc + off;
            if (c < 0 || c > PLAYER_MAX_COL) continue;
            if (off == 0 && d == 1) continue;
            for (int r = 0; r < BOARD_ROWS; ++r) {
                if (board.isEmpty(r, c)) {
                    board.placeUnit(unit, r, c);
                    return true;
                }
            }
        }
    }
    return false;
}

// Helper: ability name for shop display
static std::string abilityName(UnitClass cls) {
    switch (cls) {
        case WARRIOR:  return "Rage";
        case MAGE:     return "AOE";
        case TANK:     return "Block";
        case ASSASSIN: return "Crit";
        case ARCHER:   return "DblShot";
        default:       return "?";
    }
}

// Helper: ability description for info command
static std::string abilityDesc(UnitClass cls) {
    switch (cls) {
        case WARRIOR:  return "Below 50% HP: +30% ATK (once per combat)";
        case MAGE:     return "30% chance: 50% splash to nearby enemies";
        case TANK:     return "25% chance: block 40% of incoming damage";
        case ASSASSIN: return "25% chance: extra 50% damage (backstab)";
        case ARCHER:   return "20% chance: attack again (can crit)";
        default:       return "Unknown";
    }
}

// =====================================================================
// Constructor / Destructor
// =====================================================================
Game::Game(Difficulty difficulty)
    : ai_(difficulty), running_(true), skipCombat_(false),
      currentEvent_(EVENT_NONE), combatPace_(1), currentPhase_(PHASE_ROUND_START),
      shouldResumeShopPhase_(false), currentRoundType_(ROUND_TYPE_PVP), logFilename_("") {}

Game::~Game() {}

// =====================================================================
// showIntro - Display narrative story introduction for new game sessions
// Description: Shows the opening story of the war-torn realm of Aethoria,
//              with formatted box display, narrative text, and prompts player
//              to press Enter to begin their campaign.
// Process: 1. Draw decorative box header with game title
//          2. Display narrative text about the Dark Lord Malachar's invasion
//          3. Explain player's mission: recruit heroes and survive waves
//          4. Wait for player input (Enter key) to dismiss intro
// Purpose: Set game atmosphere, explain story context, and confirm player readiness
// =====================================================================
void Game::showIntro() const {
    const int W = 60;
    std::cout << std::endl;
    std::cout << BOLD << BR_CYAN << "  +" << std::string(W, '=') << "+" << RESET << std::endl;
    printBoxTitle(BOLD + std::string(BR_CYAN) + "⚔️  THE BATTLE FOR AETHORIA  🏆" + RESET, W);
    std::cout << BOLD << BR_CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    printBoxLine("", W);
    printBoxLine("  In the war-torn realm of Aethoria, the " + std::string(BR_RED) + "Dark Lord" + RESET, W);
    printBoxLine("  " + std::string(BR_RED) + "Malachar" + RESET + " has raised an army of darkness. As the", W);
    printBoxLine("  last surviving commander of the " + std::string(BR_GREEN) + "Allied Forces" + RESET + ",", W);
    printBoxLine("  you must recruit " + std::string(BR_YELLOW) + "heroes" + RESET + ", form your formation,", W);
    printBoxLine("  and fight wave after wave of enemies to protect", W);
    printBoxLine("  what remains of civilization.", W);
    printBoxLine("", W);
    printBoxLine("  Each round, " + std::string(BR_RED) + "Malachar's forces" + RESET + " grow stronger.", W);
    printBoxLine("  Your task: " + std::string(BR_YELLOW) + "survive as long as you can" + RESET + ", and", W);
    printBoxLine("  perhaps... " + std::string(BR_GREEN) + "turn the tide of war" + RESET + ".", W);
    printBoxLine("", W);
    std::cout << BOLD << BR_CYAN << "  +" << std::string(W, '=') << "+" << RESET << std::endl;
    std::cout << "\n  " << BOLD << BR_YELLOW << "[Press Enter to begin your campaign...]" << RESET;
    std::string dummy;
    std::getline(std::cin, dummy);
}

// =====================================================================
// displayMilestoneAnimation - Show visual milestone celebration at round 5,10,15,20
// Description: Displays animated celebration with pulsing frame, typewriter text,
//              ribbon animation, and achievement message at milestone rounds.
// Parameters: round - Current round number (checked against 5, 10, 15, 20)
// Purpose: Provides visual feedback and encouragement at significant round milestones
// =====================================================================
void Game::displayMilestoneAnimation(int round) const {
    // Determine milestone message
    std::string milestoneTitle, milestoneMsg, frameColor;
    
    if (round == 5) {
        milestoneTitle = "R O U N D  5";
        milestoneMsg = "The tide is turning! Your resistance inspires hope!";
        frameColor = BR_YELLOW;
    } else if (round == 10) {
        milestoneTitle = "R O U N D  1 0";
        milestoneMsg = "Extraordinary endurance! The Dark Lord takes notice!";
        frameColor = BR_CYAN;
    } else if (round == 15) {
        milestoneTitle = "R O U N D  1 5";
        milestoneMsg = "LEGENDARY! Surpassed all expectations!";
        frameColor = BR_GREEN;
    } else if (round == 20) {
        milestoneTitle = "R O U N D  2 0";
        milestoneMsg = "IMMORTAL! You are a FORCE OF NATURE!";
        frameColor = BR_PURPLE;
    } else {
        return;
    }

    // Clear screen
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    // 1. Pulsing frame animation (3 times)
    for (int pulse = 0; pulse < 3; pulse++) {
        std::cout << BOLD << "\033[" << frameColor << "m" 
                  << "  ╔════════════════════════════════════╗" << RESET << std::endl;
        std::cout << BOLD << "\033[" << frameColor << "m"
                  << "  ║      🎉 MILESTONE ACHIEVED 🎉    ║" << RESET << std::endl;
        std::cout << BOLD << "\033[" << frameColor << "m"
                  << "  ╚════════════════════════════════════╝" << RESET << std::endl;
        std::cout.flush();
        usleep(100000);  // 0.1 seconds
        
        system("clear");
    }

    std::cout << std::endl;
    
    // 2. Typewriter effect for message
    std::cout << BOLD << BR_CYAN << "  > ";
    for (char c : milestoneMsg) {
        std::cout << c;
        std::cout.flush();
        usleep(20000);  // 0.02 seconds per character
    }
    std::cout << RESET << "\n\n";

    // 3. Ribbon animation effect - ultra cool (0.8 seconds total)
    // Simulate flowing ribbon from left to right
    std::string leftRibbon = "    ✨";
    std::string centerStar = "⭐";
    std::string rightRibbon = "✨";
    
    for (int frame = 0; frame < 16; frame++) {
        // Create wave effect
        int offset = (frame % 4);
        std::string spacing = std::string(offset, ' ');
        
        std::cout << BOLD << BR_YELLOW 
                  << leftRibbon << spacing << centerStar << spacing << rightRibbon 
                  << RESET << "\r";
        std::cout.flush();
        usleep(50000);  // 0.05 seconds per frame
    }
    std::cout << "\n\n";

    // 4. Final announcement box
    std::cout << BOLD << "\033[" << frameColor << "m"
              << "  ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀" << RESET << std::endl;
    std::cout << BOLD << "\033[" << frameColor << "m"
              << "    " << milestoneTitle << "  UNLOCKED  " << RESET << std::endl;
    std::cout << BOLD << "\033[" << frameColor << "m"
              << "  ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄" << RESET << std::endl;
    std::cout << std::endl;

    // Wait for player input to continue
    std::cout << ITALIC << YELLOW << "  [Press Enter to continue...]" << RESET;
    std::string dummy;
    std::getline(std::cin, dummy);
}

// =====================================================================
// run - Main game loop orchestrating all phases until player loses
// Game Flow: SHOP PHASE -> EVENT PHASE -> BATTLE PHASE -> RESULT PHASE -> REPEAT
// Description: Main game loop that coordinates all gameplay phases:
//              1. SHOP: Player buys/sells/places units, arranges formation
//              2. EVENT: Random event effects (gold, discount, blessing, free unit)
//              3. BATTLE: Auto-combat between player and AI armies
//              4. RESULT: Process battle outcome, damage dealt, rewards earned
//              5. REPEAT until player HP drops to 0 (game over)
// Parameters: show_intro - Whether to display story intro (true for new games)
// Returns: Final score = number of rounds survived before defeat
// Purpose: Core game engine that manages game progression and win/lose conditions
// =====================================================================
int Game::run(bool show_intro) {
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +======================================+" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  |    ⚔️  AUTO-BATTLER ARENA  🏆      |" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  |       Difficulty: "
              << (ai_.getDifficultyString() == "HARD" ? BR_RED : BR_GREEN)
              << std::left << std::setw(19) << ai_.getDifficultyString()
              << CYAN << "|" << RESET << std::endl;
    std::cout << BOLD << CYAN << "  +======================================+" << RESET << std::endl;
    std::cout << "\n  " << BR_CYAN << "📖" << RESET << " Type 'help' for commands. "
              << BR_YELLOW << "💾" << RESET << " 'save' to save game.\n" << std::endl;

    // Show story intro for new games only
    if (show_intro) {
        showIntro();
        
        // Clear screen before showing full help
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        // Show full help on first playthrough
        printHelp();  // Show full command list after intro
        std::cout << std::endl << "  [Press Enter to continue...]";
        std::string dummy;
        std::getline(std::cin, dummy);
        
        // Clear screen before starting game
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    } else {
        // Loading saved game
        printCommandTips();  // Show quick commands if loading saved game
        
        // If resuming from shop phase, brief pause then transition
        if (shouldResumeShopPhase_) {
            std::cout << std::endl;
            // 0.5 second delay
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // Clear screen for smooth transition
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
        }
    }

    // Initialize battle log for this game session
    initializeLog();

    while (running_ && player_.isAlive()) {
        // Handle resuming from saved game
        if (!shouldResumeShopPhase_) {
            player_.startNewRound();

            // --- Round flavor text ---
            {
                int round = player_.getRoundsPlayed();
                std::cout << std::endl;
                if (round == 1) {
                    std::cout << "  ~ The first wave approaches... ~" << std::endl;
                } else if (round == 5) {
                    std::cout << "  ~ The enemy grows restless... ~" << std::endl;
                } else if (round == 10) {
                    std::cout << "  ~ A formidable army approaches! ~" << std::endl;
                } else if (round == 15) {
                    std::cout << "  ~ The Dark Lord himself sends his elite guard! ~" << std::endl;
                } else if (round >= 20) {
                    std::cout << "  ~ The final battle rages on... ~" << std::endl;
                } else {
                    const std::string flavor[] = {
                        "  ~ The winds of war howl across the battlefield. ~",
                        "  ~ Malachar's minions march ever onward. ~",
                        "  ~ The scent of battle fills the air. ~",
                        "  ~ Your soldiers steady themselves for the coming fight. ~"
                    };
                    std::cout << flavor[rand() % 4] << std::endl;
                }
                std::cout << std::endl;
            }

            // --- Check for milestones ---
            {
                int round = player_.getRoundsPlayed();
                if (round == 5 || round == 10 || round == 15 || round == 20) {
                    displayMilestoneAnimation(round);
                }
            }

            // --- Random event ---
            currentEvent_ = Event::rollEvent(player_.getRoundsPlayed());
            handleEvent();

            //--- Ask Quit when Gold is insufficient (ROUND 2 ONLY) ---
            
            if (player_.getRoundsPlayed() >= 2) {
                int min_cost = 999;
                for (int i = 0; i < SHOP_SIZE; i++) {
                    Unit* u = shop_.getUnit(i);
                    if (u != nullptr) {
                        int cost = u->getCost();
                        if (cost < min_cost) {
                            min_cost = cost;
                        }
                    }
                }

                int my_gold = player_.getGold();
                if (my_gold < min_cost) {
                    std::cout << BOLD << RED << "\n======================================" << RESET << std::endl;
                    std::cout << BOLD << RED << " Your gold CANNOT buy any hero in shop!" << RESET << std::endl;
                    std::cout << BOLD << YELLOW << " Do you want to quit? (yes/no) > " << RESET;
                    std::string answer;
                    std::getline(std::cin, answer);
                    std::string clean_an = toLower(trim(answer));
                    if (clean_an == "yes" || clean_an == "y") {
                        running_ = false;
                        break;
                    }
                }
            }

        } else {
            // Resuming from saved shop phase
            std::cout << BLUE << "\n  === Resuming Round " << player_.getRoundsPlayed() << " (Shop Phase) ===" << RESET << std::endl;
            shouldResumeShopPhase_ = false;  // Only do this once
            
            // Refresh shop for this round if not already set
            if (currentPhase_ == PHASE_SHOP) {
                // Shop states were saved, don't refresh
                std::cout << CYAN << "  (Shop state restored from save)" << RESET << std::endl;
            }
        }

// ==============================================
        // --- Shop Phase ---
        if (currentPhase_ != PHASE_BATTLE) {
            // Refresh shop - whether starting new round or resuming from save
            shop_.refresh();
            currentPhase_ = PHASE_SHOP;
            shopPhase();
            if (!running_) break;
        }

        // --- Battle Phase ---
        currentPhase_ = PHASE_BATTLE;
        std::cout << BOLD << RED << "\n  ⚔️ ========== BATTLE PHASE ========== ⚔️" << RESET << std::endl;
        skipCombat_ = false;
        bool playerWon = battlePhase();

        if (playerWon) {
            player_.recordWin();
            const std::string victoryMessages[] = {
                BOLD RED "  >> 🌟 The enemy retreats! Victory is yours! 🌟 <<" RESET,
                BOLD CYAN "  >> 💥 Your forces prevail! The Dark Army scatters! 💥 <<" RESET,
                BOLD YELLOW "  >> 🥳 A glorious victory! The Allied Forces stand strong! 🥳 <<" RESET
            };
            std::cout << "\n" << victoryMessages[rand() % 3] << std::endl;
            
        } else {
            player_.recordLoss();
            int damage = LOSS_DAMAGE_BASE
                + (ai_.getArmySize() * LOSS_DAMAGE_PER_SURVIVING);
            player_.takeDamage(damage);
            const std::string defeatMessages[] = {
                BOLD RED "  >> ☠️ Your forces fall... the Dark Army advances. ☠️ <<" RESET,
                BOLD MAGENTA "  >> 💀🪖 Defeat! Your soldiers couldn't hold the line. 🪖💀 <<" RESET
            };
            std::cout << "\n" << defeatMessages[rand() % 2]
                      << " You take " << damage << " damage." << std::endl;
        }

        // Write to log after battle
        writeToLog(player_.getRoundsPlayed(), playerWon, player_.getGold(), 
                   playerWon ? player_.getWinStreak() : player_.getLossStreak(), 
                   currentEvent_ != EVENT_NONE);

        // Note: Unit cleanup is handled in battlePhase() for unit persistence
        currentEvent_ = EVENT_NONE;
        currentPhase_ = PHASE_ROUND_START;  // Ready for next round

        if (!player_.isAlive()) {
            const int GW = 50;
            int rounds = player_.getRoundsPlayed();
            int maxHp = STARTING_HP;  // From player.h
            
            std::cout << std::endl;
            std::cout << "  +" << std::string(GW, '=') << "+" << std::endl;
            printBoxTitle("GAME OVER", GW);
            std::string t2 = "You survived " + std::to_string(rounds) + " rounds!";
            printBoxTitle(t2, GW);
            std::cout << "  +" << std::string(GW, '-') << "+" << std::endl;

            // Narrative text based on rounds survived
            std::string narrative;
            if (rounds < 5) {
                narrative = BOLD BR_RED "The Dark Lord's forces overwhelmed you..." RESET;
            } else if (rounds < 10) {
                narrative = BOLD RED "You fought bravely, but Malachar's army was too strong." RESET;
            } else if (rounds < 15) {
                narrative = BOLD BR_YELLOW "A valiant effort! Your name will be remembered." RESET;
            } else if (rounds < 20) {
                narrative = BOLD BR_GREEN "Legendary commander! You held the line longer than anyone." RESET;
            } else {
                narrative = BOLD BR_CYAN "IMMORTAL! The bards will sing of your deeds forever!" RESET;
            }
            printBoxLine("  " + narrative, GW);
            std::cout << "  +" << std::string(GW, '-') << "+" << std::endl;
            
            // Final HP display with blood bar
            std::ostringstream hpLine;
            hpLine << "  Final HP: 0/" << maxHp << " [";
            hpLine << std::string(20, ' ');  // Empty bar since HP is 0
            hpLine << "]";
            printBoxLine(hpLine.str(), GW);
            
            std::cout << "  +" << std::string(GW, '=') << "+" << std::endl;
        } else {
            // Display log for this round
            displayLogWithBattleReport();
            
            // Autosave to slot 1 after round completes
            performAutosave();
            
            std::cout << "\n  [Press Enter to continue to next round...]";
            std::string dummy;
            std::getline(std::cin, dummy);
        }
    }

    Record::saveRecord(player_, ai_);
    return player_.getRoundsPlayed();
}

// =====================================================================
// handleEvent - Process and apply event effects for current round
// Description: Applies the effects of the current round's event (if any).
//              Event types include: GOLD_RUSH (extra gold), SHOP_DISCOUNT
//              (reduced prices), DIVINE_BLESSING (heal units), and
//              RANDOM_FREE_UNIT (receive a free unit).
// Process: 1. Apply event effect via Event::applyEvent()
//          2. Display event banner to player
//          3. If EVENT_RANDOM_FREE_UNIT, grant free unit via giveRandomFreeUnit()
// Purpose: Provides dynamic random rewards/effects each round for gameplay variety
// =====================================================================
void Game::handleEvent() {
    if (currentEvent_ == EVENT_NONE) return;

    std::string desc = Event::applyEvent(currentEvent_, player_);
    if (!desc.empty()) {
        // Display random flashy animation when event is triggered
        Event::randomEventAnimation(Event::getEventName(currentEvent_));
        
        std::cout << BOLD << BLUE << "\n  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << RESET << std::endl;
        std::cout << BOLD << BLUE << "  !  EVENT 🫢: " << desc << RESET << std::endl;
        std::cout << BOLD << BLUE << "  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << RESET << std::endl;
    }

    // Give free unit if that event was rolled
    if (currentEvent_ == EVENT_RANDOM_FREE_UNIT) {
        giveRandomFreeUnit();
    }
}

// =====================================================================
// giveRandomFreeUnit - Grant player a free random unit from event
// Description: Creates a temporary shop, purchases one random unit,
//              and adds it to player's bench. If bench is full, unit is lost.
// Purpose: Reward random event (EVENT_RANDOM_FREE_UNIT)
// =====================================================================
void Game::giveRandomFreeUnit() {
    // Create a temporary shop and grab one unit for free
    Shop tempShop;
    tempShop.refresh();
    Unit* freeUnit = tempShop.buyUnit(rand() % SHOP_SIZE);
    if (freeUnit != nullptr) {
        if (player_.addToBench(freeUnit)) {
            std::cout << "  You received a free " << freeUnit->getName() << "!" << std::endl;
        } else {
            std::cout << "  Bench is full! The free unit was lost." << std::endl;
            delete freeUnit;
        }
    }
}

// =====================================================================
// shopPhase - Player shopping and formation setup phase
// Description: Main loop where players buy/sell units, arrange formation,
//              manage resources, and prepare for battle.
// Key Commands:
//   - buy/sell: Purchase or sell units
//   - place/remove: Position units on board
//   - refresh: Re-roll shop items
//   - pace: Set battle speed (0-3)
//   - ready: Confirm and start battle
// Purpose: Game phase between battles where player makes strategic decisions
// =====================================================================
void Game::shopPhase() {
    std::string line;
    bool ready = false;
    bool shouldShow = true;  // Flag to control when to clear and show full screen
    bool isFirstDisplay = true;  // Don't clear screen on first display

    // Return board units to bench for repositioning
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c <= PLAYER_MAX_COL; ++c) {
            Unit* u = board_.getUnit(r, c);
            if (u != nullptr) {
                board_.removeUnit(r, c);
                player_.addToBench(u);
            }
        }

    // Note: All units are already healed to full HP when returning from battle

    // Check for auto-merges after returning units
    checkAndMerge();

    while (!ready && running_) {
        // Only clear and show full screen when needed
        if (shouldShow) {
            // Clear screen for clean pagination (but not on first display)
            if (!isFirstDisplay) {
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");
                #endif
            }
            isFirstDisplay = false;

            // Display status bar
            player_.displayStatus();
            printDeployLimit();
            std::cout << std::endl;

            // Display formation before each operation
            board_.displayPlayerSide();
            printFormation();

            // Display shop
            shop_.setPlayerGold(player_.getGold());
            shop_.display();
            std::cout << std::endl;

            // Always show commands before bench
            printCommandTips();
            std::cout << std::endl;

            // Display bench
            player_.displayBench();
            std::cout << std::endl;

            shouldShow = false;  // Don't re-show until a successful action
        }

        std::cout << BLUE << "  Command > " << RESET;
        if (!std::getline(std::cin, line)) {
            running_ = false;
            break;
        }

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "buy") {
            int slot;
            if (!(iss >> slot)) {
                std::cout << ITALIC << YELLOW << "  Usage: buy 1  (slot number 1-5)" << RESET << std::endl;
                continue;
            }
            slot--;
            Unit* u = shop_.getUnit(slot);
            if (u == nullptr) {
                std::cout << ITALIC << RED << "  That slot is empty." << RESET << std::endl;
                continue;
            }
            
            //purchase confirmation
            std::cout << UNDERLINE << BLUE << " Are you sure you want to buy " << WHITE << u->getName() << BLUE << "? (y/n) > " << RESET;
            std::string confirm;
            std::getline(std::cin, confirm);
            std::string c = toLower(trim(confirm));
            if (c != "y") {
                std::cout << YELLOW << " Purchase cancelled." << RESET << '\n';
                continue;
            }
            //=========================================================
            int cost = u->getCost();
            // Apply shop discount event
            if (currentEvent_ == EVENT_SHOP_DISCOUNT) {
                cost = (cost > 1) ? cost - 1 : 1;
            }
            if (!player_.spendGold(cost)) {
                std::cout << RED << "  Not enough gold! (need $" << cost
                          << ", have $" << player_.getGold() << ")" << RESET << std::endl;
                continue;
            }
            Unit* bought = shop_.buyUnit(slot);
            if (!player_.addToBench(bought)) {
                std::cout << RED << "  Bench is full! Refunding gold." << RESET << std::endl;
                player_.addGold(cost);
                delete bought;
                continue;
            }
            std::cout << GREEN << "  ✅ Purchased " << bought->getName() << "!" << RESET << std::endl;
            // Pause to let user see the purchase message
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            // Check for 3-merge after each purchase
            checkAndMerge();
            shouldShow = true;  // Refresh display after successful buy

        } else if (cmd == "sell") {
            int idx;
            if (!(iss >> idx)) {
                std::cout << YELLOW << "  Usage: sell 1  (bench index)" << RESET << std::endl;
                continue;
            }
            idx--;
            Unit* Sold = player_.getBenchUnit(idx);
            if (Sold == nullptr) {
                std::cout << RED << " Invalid bench index." << RESET << std::endl;
                continue;
            }
            //selling confirmation
            std::cout << UNDERLINE << BLUE << " Sure you want to sell " << WHITE << Sold->getName() << BLUE << "? (yes/no) > " << RESET;
            std::string confirm;
            std::getline(std::cin, confirm);
            std::string c = toLower(trim(confirm));
            if (c != "yes" && c != "y") {
                std::cout << YELLOW << " Sell cancelled." << RESET << std::endl;
                continue;
            }
            if (!player_.sellUnit(idx)) {
                std::cout << RED << "  Invalid bench index." << RESET << std::endl;
            } else {
                std::cout << GREEN << " ✅ Sold successfully!" << RESET << std::endl;
            }
            player_.displayStatus();
            shouldShow = true;  // Refresh display after successful sell

        } else if (cmd == "place") {
            int idx, row, col;
            if (!(iss >> idx >> row >> col)) {
                std::cout << YELLOW << "  Usage: place 1 2 3  (bench_index row col)" << RESET << std::endl;
                continue;
            }
            idx--;
            if (col < 0 || col > PLAYER_MAX_COL || row < 0 || row >= BOARD_ROWS) {
                std::cout << RED << "  Position out of range. Row:0-4, Col:0-3" << RESET << std::endl;
                continue;
            }
            if (!board_.isEmpty(row, col)) {
                std::cout << RED << "  That cell is occupied!" << RESET << std::endl;
                continue;
            }
            
            
            // Check deployment limit AND bench capacity
            int maxUnits = getMaxDeployUnits();
            int currentUnits = board_.getPlayerUnits().size();
            if (currentUnits >= maxUnits) {
                std::cout << "  Deployment limit reached! Max " << maxUnits << " units this round." << std::endl;
                std::cout << "  Use 'remove' to take a unit back to bench first." << std::endl;
                continue;
            }
            // Check if bench would overflow after battle (bench - 1 + currentUnits <= MAX_BENCH)
            int benchAfterPlace = player_.getBenchSize() - 1;  // -1 because we're about to move one to board
            if (benchAfterPlace + currentUnits + 1 > MAX_BENCH_SIZE) {
                std::cout << "  Cannot place! Bench would overflow after battle." << std::endl;
                std::cout << "  Total units (bench + board): " << (benchAfterPlace + currentUnits + 1)
                          << " > Max bench: " << MAX_BENCH_SIZE << std::endl;
                std::cout << "  Sell some units first." << std::endl;
                continue;
            }
            Unit* unit = player_.removeFromBench(idx);
            if (unit == nullptr) {
                std::cout << RED << "  Invalid bench index." << RESET << std::endl;
                continue;
            }
            board_.placeUnit(unit, row, col);
            std::cout << GREEN << "  ✅ Placed " << unit->getName() << " at ("
                      << row << "," << col << ")." << RESET << std::endl;
            board_.displayPlayerSide();
            shouldShow = true;  // Refresh display after successful place

        } else if (cmd == "remove") {
            int row, col;
            if (!(iss >> row >> col)) {
                std::cout << YELLOW << "  Usage: remove 2 3  (row col)" << RESET << std::endl;
                continue;
            }
            if (col < 0 || col > PLAYER_MAX_COL || row < 0 || row >= BOARD_ROWS) {
                std::cout << RED << "  Invalid position." << RESET << std::endl;
                continue;
            }
            Unit* unit = board_.getUnit(row, col);
            if (unit == nullptr || !unit->isPlayerUnit()) {
                std::cout << RED << "  No unit of yours at that position." << RESET << std::endl;
                continue;
            }
            board_.removeUnit(row, col);
            player_.addToBench(unit);
            std::cout << GREEN << "  ✅ Removed " << unit->getName() << " back to bench." << RESET << std::endl;
            shouldShow = true;  // Refresh display after successful remove

        } else if (cmd == "formation" || cmd == "form" || cmd == "f") {
            std::cout << MAGENTA << "\n 📊 Your Formation:" << RESET << std::endl;
            board_.displayPlayerSide();

        } else if (cmd == "auto") {
            int benchSize = player_.getBenchSize();
            int maxUnits = getMaxDeployUnits();
            int currentUnits = board_.getPlayerUnits().size();
            // Check bench capacity before auto-placing
            if (currentUnits + benchSize > MAX_BENCH_SIZE) {
                std::cout << "  Cannot auto-place! Total units would exceed bench capacity." << std::endl;
                std::cout << "  Currently deployed: " << currentUnits << ", Bench: " << benchSize
                          << ", Max bench: " << MAX_BENCH_SIZE << std::endl;
                std::cout << "  Sell some units first." << std::endl;
                continue;
            }
            int placed = 0;
            for (int i = 0; i < benchSize; ++i) {
                if (currentUnits + placed >= maxUnits) {
                    // Put remaining units back on bench
                    Unit* unit = player_.removeFromBench(0);
                    if (unit != nullptr) {
                        player_.addToBench(unit);
                    }
                    break;
                }
                Unit* unit = player_.removeFromBench(0);
                if (unit == nullptr) continue;
                if (smartPlaceUnit(board_, unit)) {
                    placed++;
                } else {
                    player_.addToBench(unit);
                    break;
                }
            }
            std::cout << GREEN << "  ✅ Smart-placed " << placed << " units." << RESET << std::endl;
            if (currentUnits + placed >= maxUnits) {
                std::cout << "  Deployment limit (" << maxUnits << ") reached!" << std::endl;
            }
            board_.displayPlayerSide();
            shouldShow = true;  // Refresh display after successful auto

        } else if (cmd == "refresh") {
            if (!player_.spendGold(shop_.getRefreshCost())) {
                std::cout << RED << "  Not enough gold! (need $" << shop_.getRefreshCost() << ")" << RESET << std::endl;
                continue;
            }
            shop_.refresh();
            std::cout << GREEN << "  ✅ Shop refreshed!" << RESET << std::endl;
            player_.displayStatus();
            shouldShow = true;  // Refresh display after successful refresh


        } else if (cmd == "save") {
            // Save game with slot selection
            std::cout << BOLD << BR_CYAN << "\n  Select save slot (1-3): " << RESET;
            std::string slotInput;
            std::getline(std::cin, slotInput);
            slotInput = trim(slotInput);
            
            int slot = 1;
            if (slotInput == "1" || slotInput == "2" || slotInput == "3") {
                slot = std::stoi(slotInput);
            } else {
                std::cout << "  Invalid slot. Using slot 1." << std::endl;
            }
            
            Record::saveGame(player_, board_, shop_, ai_, currentPhase_, currentEvent_, slot);
            std::cout << BOLD << BR_GREEN << "  ✅ Game saved to slot " << slot << "!" << RESET << std::endl;

        } else if (cmd == "ready") {
            // No champion on the board
            if (board_.getPlayerUnits().empty()) {
                std::cout << YELLOW << "\n You haven't placed any soldiers yet!" << RESET << std::endl;
                std::cout << YELLOW << " Do you want to AUTO PLACE units? (yes/no) > " << RESET;
                std::string ans;
                std::getline(std::cin, ans);
                std::string choice = toLower(trim(ans)); 
                if (choice == "yes" || choice == "y") {
            // Auto-place bench leftovers (when no champions on board)
                    int benchSize = player_.getBenchSize();
                    int placed = 0;
                    for (int i = 0; i < benchSize; ++i) {
                       Unit* unit = player_.removeFromBench(0);
                       if (unit == nullptr) break;
                       bool ok = false;
                       for (int c = PLAYER_MAX_COL; c >= 0 && !ok; --c )
                           for (int r = 0; r < BOARD_ROWS && !ok; ++r)
                               if (board_.isEmpty(r, c)) {
                                   board_.placeUnit(unit, r, c);
                                   ok = true;
                                   placed++;
                               }
                           if (!ok) { player_.addToBench(unit); break; }
                    }
                    std::cout << GREEN << " Auto-placed " << placed << " units!" << RESET << std::endl;
                    board_.displayPlayerSide();
                }   else {
                    // if no
                    std::cout << YELLOW << " Please place your units manually, then type 'ready' again." << RESET << std::endl;
                    continue;
                }
            }
            // Auto-place bench leftovers
            int benchSize = player_.getBenchSize();
            int maxUnits = getMaxDeployUnits();
            int currentUnits = board_.getPlayerUnits().size();
            // Check: after battle, units return to bench
            // So we need: currentUnits + benchSize <= MAX_BENCH_SIZE
            if (currentUnits + benchSize > MAX_BENCH_SIZE) {
                std::cout << "  Cannot start battle! Bench would overflow after units return." << std::endl;
                std::cout << "  Currently deployed: " << currentUnits << ", Bench: " << benchSize
                          << ", Max bench: " << MAX_BENCH_SIZE << std::endl;
                std::cout << "  Sell some units or remove some from board first." << std::endl;
                continue;
            }
            for (int i = 0; i < benchSize; ++i) {
                if (currentUnits >= maxUnits) {
                    // Put remaining units back on bench
                    Unit* unit = player_.removeFromBench(0);
                    if (unit != nullptr) {
                        player_.addToBench(unit);
                    }
                    continue;
                }
                Unit* unit = player_.removeFromBench(0);
                if (unit == nullptr) continue;
                if (!smartPlaceUnit(board_, unit)) {
                    player_.addToBench(unit);
                    break;
                }
                currentUnits++;
            }
            if (board_.getPlayerUnits().empty()) {
                std::cout << RED << "  No units deployed! Place at least 1 unit." << RESET << std::endl;
                continue;
            }
            std::cout << BLUE << " 🎯 Ready for battle!" << RESET << std::endl;
            // Pause to let user see the message
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // Clear screen before battle
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            
            ready = true;

        } else if (cmd == "status") {
            player_.displayStatus();
            
        } else if (cmd == "gold") {
            std::cout << GREEN << " 💰 Gold: " << player_.getGold() << RESET << std::endl;
            

        } else if (cmd == "info") {
            std::string target;
            if (!(iss >> target)) {
                std::cout << "  Usage: info <bench#>  or  info shop <slot#>" << std::endl;
                continue;
            }
            Unit* infoUnit = nullptr;
            if (target == "shop") {
                int slot;
                if (!(iss >> slot)) {
                    std::cout << "  Usage: info shop <1-5>" << std::endl;
                    continue;
                }
                slot--;
                infoUnit = shop_.getUnit(slot);
                if (infoUnit == nullptr) {
                    std::cout << "  That shop slot is empty." << std::endl;
                    continue;
                }
            } else {
                // Try parsing as bench index
                int idx = 0;
                std::istringstream ts(target);
                if (!(ts >> idx)) {
                    std::cout << "  Usage: info <bench#>  or  info shop <slot#>" << std::endl;
                    continue;
                }
                idx--;
                infoUnit = player_.getBenchUnit(idx);
                if (infoUnit == nullptr) {
                    std::cout << "  Invalid bench index." << std::endl;
                    continue;
                }
            }
            // Display detailed unit info
            const int IW = 45;
            std::cout << std::endl;
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;
            printBoxTitle("UNIT INFO", IW);
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;
            printBoxLine("  Name:   " + infoUnit->getDisplayName(), IW);
            printBoxLine("  Class:  " + infoUnit->getClassString(), IW);
            printBoxLine("  Star:   " + infoUnit->getStarString(), IW);
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;
            printBoxLine("  HP:     " + std::to_string(infoUnit->getMaxHp()), IW);
            printBoxLine("  ATK:    " + std::to_string(infoUnit->getAtk()), IW);
            printBoxLine("  Crit:   " + std::to_string(BASE_CRIT_CHANCE + infoUnit->getCritBonus()) + "%", IW);
            printBoxLine("  Range:  " + std::to_string(infoUnit->getAttackRange()), IW);
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;
            printBoxLine("  Ability: " + abilityName(infoUnit->getClass()), IW);
            printBoxLine("  " + abilityDesc(infoUnit->getClass()), IW);
            printBoxLine("  Lore: " + Unit::getClassDescription(infoUnit->getClass()), IW);
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;
            printBoxLine("  Cost: $" + std::to_string(infoUnit->getCost())
                         + "  |  Sell: $" + std::to_string(infoUnit->getSellPrice()), IW);
            std::cout << "  +" << std::string(IW, '-') << "+" << std::endl;

        } else if (cmd == "pace") {
            int pace;
            if (!(iss >> pace)) {
                std::cout << YELLOW << "  Usage: pace 0|1|2|3" << RESET << std::endl;
                std::cout << "  0=SLOW (extra delay), 1=NORMAL, 2=FAST (600ms), 3=FASTEST (no display)" << RESET << std::endl;
                continue;
            }
            if (pace < 0 || pace > 3) {
                std::cout << RED << "  Invalid pace! Please input a pace number between 0-3." << RESET << std::endl;
                std::cout << "  0=SLOW, 1=NORMAL, 2=FAST, 3=FASTEST" << RESET << std::endl;
                continue;
            }
            setCombatPace(pace);
            std::cout << GREEN << "  ✅ Combat pace set to " << pace << RESET << std::endl;

        } else if (cmd == "settings") {
            settingsMenu();

        } else if (cmd == "help") {
            printHelp();

        } else if (cmd == "menu") {
            // Return to main menu without asking
            running_ = false;

        } else if (cmd == "quit" || cmd == "exit") {
            std::cout << YELLOW << "  Save before quitting? (y/n) > " << RESET;
            std::string ans;
            std::getline(std::cin, ans);
            std::string c = toLower(trim(ans));
            if (c == "y" || c == "yes") {
                Record::saveGame(player_, board_, shop_, ai_, currentPhase_, currentEvent_);
            }
            std::cout << BLUE << " 👋 Game exiting..." << RESET << std::endl;
            running_ = false;

        } else {
            std::cout << RED << "  Unknown command. Type 'help'." << RESET << std::endl;
        }
    }
}

// =====================================================================
// checkAndMerge - Auto-merge: merge 3 same-unit same-star into 1 star-level-up
// Algorithm: Loops through bench searching for 3 identical units.
//            When found: upgrade first unit, remove other two, recursively check.
// Result: Repeated merges possible if chain reactions occur
// Purpose: Core progression mechanic - units evolve through collection
// =====================================================================
void Game::checkAndMerge() {
    bool merged = true;
    while (merged) {
        merged = false;
        const std::vector<Unit*>& bench = player_.getBench();

        for (size_t i = 0; i < bench.size() && !merged; ++i) {
            std::string name = bench[i]->getBaseName();
            int star = bench[i]->getStarLevel();
            if (star >= MAX_STAR_LEVEL) continue;

            // Find 2 more units with same name and star level
            std::vector<int> matches;
            matches.push_back((int)i);
            for (size_t j = i + 1; j < bench.size(); ++j) {
                if (bench[j]->getBaseName() == name && bench[j]->getStarLevel() == star) {
                    matches.push_back((int)j);
                    if ((int)matches.size() >= MERGE_COUNT) break;
                }
            }

            if ((int)matches.size() >= MERGE_COUNT) {
                // Keep the first, upgrade it, delete the other two
                Unit* keeper = bench[matches[0]];
                keeper->upgrade();

                // Remove from bench in reverse order to preserve indices
                for (int k = MERGE_COUNT - 1; k >= 1; --k) {
                    Unit* removed = player_.removeFromBench(matches[k]);
                    if (removed != nullptr) delete removed;
                }

                std::cout << "  ** MERGE! 3x " << name << " -> "
                          << keeper->getDisplayName() << " (Star "
                          << keeper->getStarLevel() << ") **" << std::endl;
                merged = true;  // check again for chain merges
            }
        }
    }
}

// =====================================================================
// battlePhase - Automated turn-based combat simulation
// Process:
//   1. Clear previous battle board
//   2. Generate and place AI army (difficulty-scaled)
//   3. Apply player synergy bonuses
//   4. Show formations and let player preview (or skip)
//   5. Execute combat ticks until one side eliminated
//   6. Return surviving player units to bench (healed to full HP)
//   7. Permanently delete dead units
// Return: true=player victory, false=player defeat
// Purpose: Core game mechanic - automatic battles with units' abilities
// =====================================================================
bool Game::battlePhase() {
    // Clear AI side
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = AI_MIN_COL; c < BOARD_COLS; ++c)
            board_.removeUnit(r, c);

    // Generate AI army
    ai_.generateArmy(player_.getRoundsPlayed());
    ai_.placeUnits(board_);
    ai_.releaseArmy();

    // Apply synergies to player units
    std::vector<Unit*> pUnits = board_.getPlayerUnits();
    std::vector<SynergyBonus> synergies = Synergy::calculateSynergies(pUnits);

    std::cout << "\n  Initial formation:" << std::endl;
    board_.display();

    // Show armies
    pUnits = board_.getPlayerUnits();
    std::vector<Unit*> aUnits = board_.getAIUnits();
    std::cout << "  ALLIED FORCES (" << pUnits.size() << "): ";
    for (size_t i = 0; i < pUnits.size(); ++i) {
        std::cout << pUnits[i]->getName();
        if (i + 1 < pUnits.size()) std::cout << ", ";
    }
    std::cout << std::endl;
    std::cout << "  Dark Army (" << aUnits.size() << "):      ";
    for (size_t i = 0; i < aUnits.size(); ++i) {
        std::cout << aUnits[i]->getName();
        if (i + 1 < aUnits.size()) std::cout << ", ";
    }
    std::cout << std::endl;

    // Display active synergies
    Synergy::displaySynergies(synergies);

    std::cout << "  Press Enter to begin (or 's' to skip)...";
    std::string input;
    std::getline(std::cin, input);
    if (input == "s" || input == "S") skipCombat_ = true;

    std::vector<Unit*> deadUnits;
    bool playerWon = resolveCombat(deadUnits);

    // Clear synergy bonuses
    pUnits = board_.getPlayerUnits();
    Synergy::clearSynergies(pUnits);

    // UNIT PERSISTENCE: Only SURVIVING player units return to bench
    // Dead units are permanently deleted (no resurrection)
    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            Unit* u = board_.getUnit(r, c);
            if (u != nullptr) {
                board_.removeUnit(r, c);
                if (u->isPlayerUnit()) {
                    if (!u->isAlive()) {
                        // Dead unit - permanently delete it
                        std::cout << "  [PERMANENT LOSS] " << u->getName() << " has fallen in battle!" << std::endl;
                        delete u;
                    } else {
                        // Surviving unit - heal to full HP and return to bench
                        u->healToFull();
                        if (!player_.addToBench(u)) {
                            std::cerr << "  ERROR: Bench full! Unit " << u->getName() << " lost!" << std::endl;
                            delete u;  
                        } else {
                            std::cout << "  [HEALED] " << u->getName() << " returns with full HP ("
                                      << u->getMaxHp() << "/" << u->getMaxHp() << ")!" << std::endl;
                        }
                    }
                } else {
                    // AI unit - always delete
                    delete u;
                }
            }
        }
    }

    // Check for auto-merges after returning units
    checkAndMerge();

    // Delete all dead units collected during combat
    for (size_t i = 0; i < deadUnits.size(); ++i) {
        Unit* u = deadUnits[i];
        delete u;  // All dead units are permanently deleted
    }

    return playerWon;
}

// =====================================================================
// resolveCombat - Execute tick-by-tick combat resolution
// Algorithm:
//   For each tick (max 200):
//   1. Check if either side has units alive
//   2. Each alive unit acts in turn: attack or move closer
//   3. Perform special ability (Warrior rage, Mage AOE, etc.)
//   4. Clean up dead units
//   5. Handle display based on combatPace_ setting
// Parameters: deadUnits - vector to collect permanently dead units
// Return: true=player won, false=player lost
// Purpose: Core combat AI - handles all attack logic, abilities, and battlefield state
// =====================================================================
bool Game::resolveCombat(std::vector<Unit*>& deadUnits) {
    int totalPlayerKills = 0, totalAIKills = 0;
    int totalPlayerDmg = 0, totalAIDmg = 0;
    int lastTick = 0;

    // Clear rage flags for all units at combat start
    std::vector<Unit*> allUnitsInit = board_.getAllUnits();
    for (size_t i = 0; i < allUnitsInit.size(); ++i) {
        allUnitsInit[i]->clearRage();
    }

    for (int tick = 1; tick <= MAX_COMBAT_TICKS; ++tick) {
        std::vector<Unit*> allUnits = board_.getAllUnits();

        bool playerAlive = false, aiAlive = false;
        for (size_t i = 0; i < allUnits.size(); ++i) {
            if (allUnits[i]->isAlive()) {
                if (allUnits[i]->isPlayerUnit()) playerAlive = true;
                else aiAlive = true;
            }
        }
        if (!playerAlive || !aiAlive) break;
        lastTick = tick;

        // Clear screen before each tick (except first) and redisplay battle header
        if (!skipCombat_ && tick > 1) {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            
            // Redisplay Battle Phase title
            std::cout << BOLD << RED << "  ⚔️ ========== BATTLE PHASE ========== ⚔️" << RESET << std::endl;
        }

        if (!skipCombat_) {
            std::cout << "\n  -------- Tick " << tick << " --------" << std::endl;
            board_.display();
            std::vector<Unit*> p = board_.getPlayerUnits();
            std::vector<Unit*> a = board_.getAIUnits();
            std::cout << "  ALLIED: ";
            for (size_t i = 0; i < p.size(); ++i) {
                std::cout << p[i]->getSymbolString()
                          << hpBarColored(p[i]->getHp(), p[i]->getMaxHp(), 10);
                if (i + 1 < p.size()) std::cout << " ";
            }
            std::cout << "  vs  DARK ARMY: ";
            for (size_t i = 0; i < a.size(); ++i) {
                std::cout << a[i]->getSymbolString()
                          << hpBarColored(a[i]->getHp(), a[i]->getMaxHp(), 10);
                if (i + 1 < a.size()) std::cout << " ";
            }
            std::cout << std::endl << std::endl;
        }

        for (size_t i = 0; i < allUnits.size(); ++i) {
            Unit* unit = allUnits[i];
            if (!unit->isAlive()) continue;

            Unit* enemy = board_.findNearestEnemy(unit);
            if (enemy == nullptr) continue;

            int dist = unit->getDistanceTo(enemy);

            if (dist <= unit->getAttackRange()) {
                int dmg = performAttack(unit, enemy);
                // Class ability triggers
                performAbility(unit, enemy, allUnits);

                if (unit->isPlayerUnit()) {
                    totalPlayerDmg += dmg;
                    if (!enemy->isAlive()) totalPlayerKills++;
                } else {
                    totalAIDmg += dmg;
                    if (!enemy->isAlive()) totalAIKills++;
                }
            } else {
                board_.moveToward(unit, enemy->getRow(), enemy->getCol());
                if (!skipCombat_) {
                    std::cout << "  " << unit->getName()
                              << " moves toward " << enemy->getName() << std::endl;
                }
            }
        }

        cleanupDeadUnits(deadUnits);

        if (!skipCombat_) {
            std::cout << "\n  [Enter] next | [s] skip > ";
            std::string in;
            std::getline(std::cin, in);
            if (in == "s" || in == "S") {
                skipCombat_ = true;
                std::cout << "  >> Fast-forwarding..." << std::endl;
            }
        }
    }

    // Battle report
    std::vector<Unit*> pUnits = board_.getPlayerUnits();
    std::vector<Unit*> aUnits = board_.getAIUnits();
    bool playerWon = !pUnits.empty() && aUnits.empty();

    const int BW = 39;
    std::cout << std::endl;
    std::cout << "  +" << std::string(BW, '=') << "+" << std::endl;
    printBoxTitle("BATTLE REPORT", BW);
    std::cout << "  +" << std::string(BW, '=') << "+" << std::endl;
    printBoxLine("  Result: " + std::string(playerWon ? "VICTORY!" : "DEFEAT..."), BW);
    printBoxLine("  Ticks:  " + std::to_string(lastTick), BW);
    std::cout << "  +" << std::string(BW, '-') << "+" << std::endl;
    printBoxLine("  Your damage dealt:  " + std::to_string(totalPlayerDmg), BW);
    printBoxLine("  Your kills:         " + std::to_string(totalPlayerKills), BW);
    printBoxLine("  Enemy damage dealt: " + std::to_string(totalAIDmg), BW);
    printBoxLine("  Enemy kills:        " + std::to_string(totalAIKills), BW);

    if (playerWon && !pUnits.empty()) {
        std::cout << "  +" << std::string(BW, '-') << "+" << std::endl;
        printBoxLine("  Survivors:", BW);
        for (size_t i = 0; i < pUnits.size(); ++i) {
            std::string line = "    " + pUnits[i]->getName() + "  HP: "
                + std::to_string(pUnits[i]->getHp()) + "/"
                + std::to_string(pUnits[i]->getMaxHp());
            printBoxLine(line, BW);
        }
    }
    std::cout << "  +" << std::string(BW, '=') << "+" << std::endl;

    return playerWon;
}

// =====================================================================
// performAttack - Execute single attack with critical hit calculation
// Logic:
//   1. Calculate base damage from attacker's ATK stat
//   2. Roll critical hit (BASE_CRIT_CHANCE + bonuses)
//   3. If crit: damage *= 1.5x
//   4. Check defender's ability (Tank blocks 40% of damage)
//   5. Apply damage to defender
//   6. Display combat message (if not skipping)
// Return: Final damage dealt after all calculations
// Purpose: Fundamental combat mechanic - resolves individual attacks
// =====================================================================
int Game::performAttack(Unit* attacker, Unit* defender) {
    int damage = attacker->getAtk();
    bool crit = false;

    int critChance = BASE_CRIT_CHANCE + attacker->getCritBonus();
    if (rand() % 100 < critChance) {
        damage = (int)(damage * CRIT_MULTIPLIER);
        crit = true;
    }

    // TANK ability: 25% chance to block 40% damage
    if (defender->getClass() == TANK && rand() % 100 < 25) {
        int blocked = damage * 40 / 100;
        damage -= blocked;
        if (!skipCombat_) {
            std::cout << "  [BLOCK] " << defender->getName()
                      << " blocks " << blocked << " dmg!" << std::endl;
        }
    }

    defender->takeDamage(damage);

    if (!skipCombat_) {
        if (crit) {
            std::cout << "  ** " << attacker->getName() << " CRITS "
                      << defender->getName() << " for " << damage << " dmg!";
        } else {
            std::cout << "  " << attacker->getName() << " -> "
                      << defender->getName() << " " << damage << " dmg";
        }
        if (!defender->isAlive()) std::cout << "  << KILLED >>";
        else std::cout << "  [" << defender->getHp() << " HP]";
        std::cout << std::endl;
    }

    return damage;
}

// =====================================================================
// performAbility - Trigger class-specific special abilities
// Abilities by class:
//   - WARRIOR: Rage - +30% ATK when HP < 50% (once per combat)
//   - MAGE: AOE - 30% chance to splash 50% damage to nearby enemies
//   - TANK: Block - 25% chance to reduce damage by 40%
//   - ASSASSIN: Backstab - 25% chance for +50% extra damage
//   - ARCHER: Double Shot - 20% chance to attack twice (can crit both)
// Purpose: Class differentiation - special mechanics that define unit roles
// =====================================================================
void Game::performAbility(Unit* attacker, Unit* defender, std::vector<Unit*>& allUnits) {
    UnitClass cls = attacker->getClass();

    switch (cls) {
        case MAGE: {
            // AOE: 30% chance to deal 50% splash to adjacent enemies
            if (rand() % 100 < 30) {
                int splash = attacker->getAtk() / 2;
                for (size_t i = 0; i < allUnits.size(); ++i) {
                    Unit* u = allUnits[i];
                    if (u == defender || !u->isAlive()) continue;
                    if (u->isPlayerUnit() == attacker->isPlayerUnit()) continue;
                    if (attacker->getDistanceTo(u) <= 2) {
                        u->takeDamage(splash);
                        if (!skipCombat_) {
                            std::cout << "  [AOE] " << attacker->getName()
                                      << " splashes " << u->getName()
                                      << " for " << splash << " dmg!" << std::endl;
                        }
                    }
                }
            }
            break;
        }
        case ASSASSIN: {
            // Backstab: 25% chance to deal extra 50% damage
            if (rand() % 100 < 25 && defender->isAlive()) {
                int bonus = attacker->getAtk() / 2;
                defender->takeDamage(bonus);
                if (!skipCombat_) {
                    std::cout << "  [BACKSTAB] " << attacker->getName()
                              << " strikes again for " << bonus << " dmg!" << std::endl;
                }
            }
            break;
        }
        case WARRIOR: {
            // Rage: below 50% HP, permanently gain +30% ATK (once per combat)
            if (!attacker->isRaged() && attacker->getHp() < attacker->getMaxHp() / 2) {
                int rageBonus = attacker->getAtk() * 30 / 100;
                attacker->applyAtkBonus(rageBonus);
                attacker->setRaged();
                if (!skipCombat_) {
                    std::cout << "  [RAGE] " << attacker->getName()
                              << " enters a berserker rage! +" << rageBonus << " ATK!" << std::endl;
                }
            }
            break;
        }
        case ARCHER: {
            // Double Shot: 20% chance to attack again (can crit)
            if (rand() % 100 < 20 && defender->isAlive()) {
                int dmg2 = attacker->getAtk();
                bool crit2 = false;
                int critChance = BASE_CRIT_CHANCE + attacker->getCritBonus();
                if (rand() % 100 < critChance) {
                    dmg2 = (int)(dmg2 * CRIT_MULTIPLIER);
                    crit2 = true;
                }
                // Tank block for double shot too
                if (defender->getClass() == TANK && rand() % 100 < 25) {
                    int blocked = dmg2 * 40 / 100;
                    dmg2 -= blocked;
                    if (!skipCombat_) {
                        std::cout << "  [BLOCK] " << defender->getName()
                                  << " blocks " << blocked << " dmg from double shot!" << std::endl;
                    }
                }
                defender->takeDamage(dmg2);
                if (!skipCombat_) {
                    if (crit2) {
                        std::cout << "  [DOUBLE SHOT] " << attacker->getName()
                                  << " fires a CRIT for " << dmg2 << " dmg!" << std::endl;
                    } else {
                        std::cout << "  [DOUBLE SHOT] " << attacker->getName()
                                  << " fires again for " << dmg2 << " dmg!" << std::endl;
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}

// =====================================================================
// cleanupDeadUnits - Collect dead units from board for deletion
// Purpose: Remove dead units from board and collect them for final deletion.
//          Prevents double-deletion and memory leaks.
// =====================================================================
void Game::cleanupDeadUnits(std::vector<Unit*>& deadUnits) {
    for (int r = 0; r < BOARD_ROWS; ++r)
        for (int c = 0; c < BOARD_COLS; ++c) {
            Unit* u = board_.getUnit(r, c);
            if (u != nullptr && !u->isAlive()) {
                board_.removeUnit(r, c);
                deadUnits.push_back(u);
            }
        }
}

// =====================================================================
// saveRecord - Append game result to gameplay records
// Format: PlayerName RoundsCompleted FinalGold DifficultyLevel
// File: docs/records.txt (append mode)
// Purpose: Build leaderboard history, track player performance statistics
// =====================================================================
void Game::saveRecord() const {
    std::ofstream file(RECORD_FILE, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "  Warning: could not write to " << RECORD_FILE << std::endl;
        return;
    }
    file << player_.getName() << " "
         << player_.getRoundsPlayed() << " "
         << player_.getGold() << " "
         << ai_.getDifficultyString() << std::endl;
    file.close();
    std::cout << "\n  Record saved." << std::endl;
}

// =====================================================================
// saveGame - Serialize complete game state to binary file
// Saves: Player HP/gold/round/streaks + all bench units with stats + difficulty
// File: docs/savegame.dat (overwrite mode)
// Serialization: Each unit stores base stats + star level (no re-scaling on load)
// Purpose: Enable mid-game suspend/resume functionality
// NOTE: Use Record::saveGame(game, player, slot) instead for slot support
// =====================================================================
void Game::saveGame() const {
    // Deprecated: Use Record::saveGame(player, board, shop, ai, phase, event, slot) instead
    // This function kept for backwards compatibility only
    Record::saveGame(player_, board_, shop_, ai_, currentPhase_, currentEvent_, 1);  // Default to slot 1
}

// =====================================================================
// loadGame - Deserialize complete game state from file
// Loads: Player state + bench units (exact stats preserved) + difficulty
// File: docs/savegame.dat
// Unit Reconstruction: Uses forceSetStarLevel to avoid re-scaling original stats
// Return: true=success, false=file not found or corruption
// Purpose: Resume interrupted games without loss of progression
// =====================================================================
bool Game::loadGame() {
    // Deprecated: Use Record::loadGame(game, player, slot) instead
    // This function kept for backwards compatibility only
    return Record::loadGame(player_, board_, shop_, ai_, currentPhase_, currentEvent_, shouldResumeShopPhase_, 1);  // Default to slot 1
}

// =====================================================================
// displayLeaderboard - Display sorted top 10 players by rounds survived
// Sorting: Descending order by rounds completed
// Format: Rank, Name, Rounds, Final Gold, Difficulty
// Source: docs/records.txt
// Purpose: Show player achievements and top performers
// =====================================================================
void Game::displayLeaderboard() {
    std::ifstream file(RECORD_FILE);
    if (!file.is_open()) {
        std::cout << BOLD << YELLOW << "\n  No records found. Play a game first!" << std::endl;
        return;
    }

    struct Record {
        std::string name;
        int rounds, gold;
        std::string difficulty;
    };

    std::vector<Record> records;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Record rec;
        if (iss >> rec.name >> rec.rounds >> rec.gold >> rec.difficulty)
            records.push_back(rec);
    }
    file.close();

    if (records.empty()) {
        std::cout << "\n  No records found." << std::endl;
        return;
    }

    for (size_t i = 0; i < records.size(); ++i)
        for (size_t j = i + 1; j < records.size(); ++j)
            if (records[j].rounds > records[i].rounds) {
                Record t = records[i]; records[i] = records[j]; records[j] = t;
            }

    const int W = 43;
    std::cout << std::endl;
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxTitle("LEADERBOARD", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;

    {
        std::ostringstream h;
        h << "  " << std::left << std::setw(6) << "Rank"
          << std::setw(12) << "Name" << std::setw(8) << "Rounds"
          << std::setw(7) << "Gold" << std::setw(6) << "Diff";
        printBoxLine(h.str(), W);
    }
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;

    int show = std::min((int)records.size(), 10);
    for (int i = 0; i < show; ++i) {
        std::ostringstream row;
        row << "  " << std::left << std::setw(6) << (i + 1)
            << std::setw(12) << records[i].name
            << std::setw(8) << records[i].rounds
            << std::setw(7) << records[i].gold
            << std::setw(6) << records[i].difficulty;
        printBoxLine(row.str(), W);
    }
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
}

// =====================================================================
// printHelp - Display comprehensive command reference
// Sections: Shopping, Bags, Formation, Game, Class Abilities, Tips
// Purpose: Educate new players on available commands and game mechanics

// printHelp

// =====================================================================
void Game::printHelp() const {
    const int W = 55;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;

    std::cout << BOLD << CYAN;
    printBoxTitle("COMMANDS", W);
    std::cout << RESET;

    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;

    // SHOPPING
    std::cout << BOLD << YELLOW;
    printBoxLine("  SHOPPING:", W);
    std::cout << RESET;

    printBoxLine("    buy 1-5        Buy unit from shop slot", W);
    printBoxLine("    sell 1-N       Sell unit from bench", W);
    printBoxLine("    refresh        Re-roll shop ($" + std::to_string(shop_.getRefreshCost()) + ")", W);
    printBoxLine("    info N         Show bench unit details", W);
    printBoxLine("    info shop N    Show shop unit details", W);

    // BAGS
    std::cout << BOLD << YELLOW;
    printBoxLine("  BAGS:", W);
    std::cout << RESET;

    printBoxLine("    gold           Number of gold remained", W);
    printBoxLine("    status         HP, Gold, Round, Win Streak", W);

    // FORMATION
    std::cout << BOLD << YELLOW;
    printBoxLine("  FORMATION:", W);
    std::cout << RESET;

    printBoxLine("    place 1 2 3    Bench# -> row col", W);
    printBoxLine("    remove 2 3     Pick up unit at row col", W);
    printBoxLine("    auto           Smart-place (by class)", W);
    printBoxLine("    formation      Show current formation", W);

    // GAME
    std::cout << BOLD << YELLOW;
    printBoxLine("  GAME:", W);
    std::cout << RESET;

    printBoxLine("    ready          Start the battle!", W);
    printBoxLine("    pace 0|1|2|3   Set battle speed", W);
    printBoxLine("      0=SLOW (extra delay)", W);
    printBoxLine("      1=NORMAL (press Enter)", W);
    printBoxLine("      2=FAST (auto with 600ms)", W);
    printBoxLine("      3=FASTEST (no display)", W);
    printBoxLine("    settings       Adjust game preferences", W);
    printBoxLine("    save           Save game to file", W);
    printBoxLine("    menu           Return to main menu", W);
    printBoxLine("    quit           Exit game completely", W);

    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxTitle("CLASS ABILITIES", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxLine("  Warrior [Rage]    : Below 50% HP: +30% ATK", W);
    printBoxLine("  Mage    [AOE]     : 30% splash to nearby enemies", W);
    printBoxLine("  Tank    [Block]   : 25% chance: block 40% dmg", W);
    printBoxLine("  Assassin [Crit]   : 25% extra damage (backstab)", W);
    printBoxLine("  Archer  [DblShot] : 20% chance: attack twice", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxTitle("TIPS", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
    printBoxLine("  SYNERGIES: 2+ same class = bonus. 3+ = stronger!", W);
    printBoxLine("  MERGE: Buy 3 of same unit -> auto-upgrade star!", W);
    printBoxLine("  INTEREST: Earn 1 gold per 10 saved (max +5/round)", W);
    printBoxLine("  FORMATION: Tanks front (col3), Mages back (col0)", W);
    printBoxLine("  Assassins in middle (col1) for balanced engage", W);
    printBoxLine("  Saving gold earns interest - balance spending!", W);
    std::cout << "  +" << std::string(W, '-') << "+" << std::endl;
}

// =====================================================================
// printCommandTips - Display compact quick reference during gameplay
// Contents: Essential commands only (buy, sell, place, ready, etc.)
// Purpose: Quick reminder of available commands without full help screen
// =====================================================================
void Game::printCommandTips() const {
    const int W = 55;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    printBoxTitle(BOLD BR_YELLOW "QUICK COMMANDS" RESET, W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    printBoxLine(BOLD BR_CYAN "  buy <1-5>  sell <idx>  refresh  info <idx>" RESET, W);
    printBoxLine(BOLD BR_CYAN "  place <idx> <row> <col>  remove <row> <col>" RESET, W);
    printBoxLine(BOLD BR_CYAN "  auto    ready    formation    help    settings" RESET, W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
}

// =====================================================================
// printStatusBar - Display compact player status with HP bar
// Shows: Gold amount and Current HP with color-coded bar visualization
// Colors: HP bar green (>50%), red (<=50%)
// Purpose: At-a-glance player resource display
// =====================================================================

void Game::printStatusBar() const {
    const int W = 62;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    
    // Line 1: Gold
    std::ostringstream line1;
    line1 << BOLD << BR_YELLOW << "  Gold: " << player_.getGold() << RESET;
    std::string s1 = line1.str();
    if ((int)s1.size() < W) s1 += std::string(W - s1.size(), ' ');
    std::cout << "  |" << s1 << "|" << std::endl;
    
    // Line 2: HP with bar  
    const int HP_BAR_WIDTH = 25;
    int hp = player_.getHp();
    int maxHp = STARTING_HP;
    int filled = (hp * HP_BAR_WIDTH) / maxHp;
    if (filled < 0) filled = 0;
    if (filled > HP_BAR_WIDTH) filled = HP_BAR_WIDTH;

    
    std::string hpBar;
    if (hp > maxHp * 0.5) {
        hpBar = "[" + std::string(BOLD) + std::string(BR_GREEN) 
              + std::string(filled, '=') 
              + std::string(RESET) 
              + std::string(HP_BAR_WIDTH - filled, ' ') 
              + "]";
    } else {
        hpBar = "[" + std::string(BOLD) + std::string(BR_RED) 
              + std::string(filled, '=') 
              + std::string(RESET) 
              + std::string(HP_BAR_WIDTH - filled, ' ') 
              + "]";
    }

    std::ostringstream line2;
    line2 << BOLD << BR_CYAN << "  HP: " << RESET
          << std::left << std::setw(3) << hp
          << "/" << maxHp << " " << hpBar;
    std::string s2 = line2.str();
    if ((int)s2.size() < W) s2 += std::string(W - s2.size(), ' ');
    std::cout << "  |" << s2 << "|" << std::endl;

    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
}

// =====================================================================
// getMaxDeployUnits - Get deployment limit for current round
// Scaling: Round 1-3: 3-5 units, Round 4+: 6 units (capped at round 20)
// Purpose: Progressive difficulty - more units available as game advances
// =====================================================================
int Game::getMaxDeployUnits() const {
    int round = player_.getRoundsPlayed();
    if (round <= 0) return MAX_DEPLOY_UNITS[0];
    if (round > 20) return MAX_DEPLOY_UNITS[19];
    return MAX_DEPLOY_UNITS[round - 1];
}

// =====================================================================
// printDeployLimit - Show current/max unit deployment count
// Display: "Units deployed: X/Y" where X=current, Y=maximum this round
// Purpose: Inform player of army size constraints
// =====================================================================
void Game::printDeployLimit() const {
    int maxUnits = getMaxDeployUnits();
    int currentUnits = board_.getPlayerUnits().size();
    std::cout << BOLD << BR_GREEN << "  Units deployed: " << currentUnits << "/" << maxUnits << RESET << std::endl;
}

// =====================================================================
// printFormation - Display current player unit formation with HP bars
// Shows: Each deployed unit with name, HP bar, and HP values
// Purpose: Visual confirmation of army composition before battle
// =====================================================================
void Game::printFormation() const {
    std::vector<Unit*> units = board_.getPlayerUnits();
    if (units.empty()) {
        std::cout << BOLD << BR_RED << "  No units deployed." << RESET << std::endl;
        return;
    }

    const int W = 55;
    std::cout << std::endl;
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
    printBoxTitle(BOLD BR_YELLOW "YOUR FORMATION" RESET, W);
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;

    for (size_t i = 0; i < units.size(); ++i) {
        Unit* u = units[i];
        std::ostringstream line;
        line << BOLD << BR_WHITE << "  " << u->getName() << " " << RESET
             << "[" << hpBar(u->getHp(), u->getMaxHp(), 10) << "] "
             << u->getHp() << "/" << u->getMaxHp() << " HP";
        printBoxLine(line.str(), W);
    }
    std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
}

// =====================================================================
// setCombatPace - Set battle display speed (0-3 valid range)
// Speeds:
//   0 = SLOW: Extra pause between ticks
//   1 = NORMAL: Wait for user input (default)
//   2 = FAST: Auto-advance with 600ms delay
//   3 = FASTEST: No display, instant resolution
// Purpose: Accommodate different player preferences for pacing
// =====================================================================
void Game::setCombatPace(int pace) {
    if (pace < 0) pace = 0;
    if (pace > 3) pace = 3;
    combatPace_ = pace;
}

// =====================================================================
// getCombatPace - Retrieve current battle display speed setting
// Return: int 0-3 representing current pace setting
// Purpose: Query pace for conditional display logic in battle resolution
// =====================================================================
int Game::getCombatPace() const {
    return combatPace_;
}

// =====================================================================
// initializeLog - Create and initialize battle log file for this game session
// What it does: Creates a new log file with timestamp in docs/ folder
// Returns: void
// =====================================================================
void Game::initializeLog() {
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);
    
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", timeinfo);
    
    logFilename_ = std::string("docs/battle_log_") + timestamp + ".txt";
    
    // Create/open file and write header
    std::ofstream file(logFilename_);
    if (file.is_open()) {
        file << "========================================" << std::endl;
        file << "AUTO-BATTLER ARENA - BATTLE LOG" << std::endl;
        file << "Session Start: " << std::asctime(timeinfo);
        file << "========================================" << std::endl << std::endl;
        file.close();
    }
}

// =====================================================================
// writeToLog - Write round result information to the battle log file
// Parameters:
//   round - Current round number
//   won - Whether the player won this round
//   gold - Current player gold amount
//   winStreak - Current win streak count
//   eventTriggered - Whether an event was triggered
// Returns: void
// =====================================================================
void Game::writeToLog(int round, bool won, int gold, int winStreak, bool eventTriggered) {
    if (logFilename_.empty()) return;
    
    std::ofstream file(logFilename_, std::ios::app);
    if (!file.is_open()) return;
    
    file << "--- Round " << round << " ---" << std::endl;
    file << "Result: " << (won ? "WIN" : "LOSS") << std::endl;
    file << "Gold: " << gold << std::endl;
    file << "Win Streak: " << winStreak << std::endl;
    file << "Event Triggered: " << (eventTriggered ? "YES" : "NO") << std::endl;
    file << std::endl;
    
    file.close();
}

// =====================================================================
// displayLogWithBattleReport - Show battle log for current round in terminal
// What it does: Displays a formatted log of the current round's results
// Includes: Round number, result (WIN/LOSS), player stats, event info
// Returns: void
// =====================================================================
void Game::displayLogWithBattleReport() {
    const int LW = 39;  // Log width
    
    int round = player_.getRoundsPlayed();
    int gold = player_.getGold();
    int hp = player_.getHp();
    int maxHp = STARTING_HP;
    int winStreak = player_.getWinStreak();
    int lossStreak = player_.getLossStreak();
    
    std::cout << std::endl;
    std::cout << "  +" << std::string(LW, '=') << "+" << std::endl;
    printBoxTitle("ROUND LOG", LW);
    std::cout << "  +" << std::string(LW, '-') << "+" << std::endl;
    
    // Round info
    printBoxLine("  Round: " + std::to_string(round), LW);
    printBoxLine("  Gold: " + std::to_string(gold) + " " + BOLD BR_YELLOW "💰" RESET, LW);
    
    // HP display
    std::ostringstream hpLine;
    hpLine << "  HP: " << hp << "/" << maxHp;
    int barLength = 15;
    int filledBars = (hp * barLength) / maxHp;
    hpLine << " [";
    for (int i = 0; i < barLength; ++i) {
        if (i < filledBars) {
            if (hp > maxHp * 0.6) hpLine << BR_GREEN "█" RESET;
            else if (hp > maxHp * 0.3) hpLine << BR_YELLOW "█" RESET;
            else hpLine << BR_RED "█" RESET;
        } else {
            hpLine << "░";
        }
    }
    hpLine << "]";
    printBoxLine(hpLine.str(), LW);
    
    // Streak info
    if (winStreak > 0) {
        std::string streakLine = std::string("  ") + BOLD BR_GREEN "Win Streak: " + std::to_string(winStreak) + RESET;
        printBoxLine(streakLine, LW);
    }
    if (lossStreak > 0) {
        std::string streakLine = std::string("  ") + BOLD BR_RED "Loss Streak: " + std::to_string(lossStreak) + RESET;
        printBoxLine(streakLine, LW);
    }
    
    // Event info
    if (currentEvent_ != EVENT_NONE) {
        std::string eventLine = std::string("  ") + BOLD MAGENTA "EVENT TRIGGERED! 🎁" RESET;
        printBoxLine(eventLine, LW);
    }
    
    std::cout << "  +" << std::string(LW, '=') << "+" << std::endl;
}

// =====================================================================
// performAutosave - Automatically save game to slot 1 after each round
// Silent background operation with minimal visual feedback
// Saves: Player state, bench units, board units, game phase, current event
// Purpose: Enable auto-recovery if game crashes or closes unexpectedly
// =====================================================================
void Game::performAutosave() {
    try {
        Record::saveGame(player_, board_, shop_, ai_, currentPhase_, currentEvent_, 1);
        
        // Subtle autosave indicator (optional - can comment out for completely silent save)
        std::cout << GRAY << "  [autosave]" << RESET << std::endl;
    } catch (const std::exception& e) {
        // Silent catch - don't disrupt gameplay
        // Autosave failure is not critical to game flow
    } catch (...) {
        // Silent catch all
    }
}

// =====================================================================
// settingsMenu - Display and manage game settings
// Purpose: Allow player to toggle animations, colors, and sound preferences
// =====================================================================
void Game::settingsMenu() {
    bool settingsDone = false;
    
    while (!settingsDone && running_) {
        // Clear screen for clean display
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        const int W = 50;
        std::cout << std::endl;
        std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
        printBoxTitle(BOLD BR_YELLOW "SETTINGS" RESET, W);
        std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
        
        // Show current settings status
        std::cout << BOLD << YELLOW;
        printBoxLine("  CURRENT STATUS:", W);
        std::cout << RESET;
        
        std::string animStatus = settings_.animationsEnabled ? "ON" : "OFF";
        std::string colorStatus = settings_.colorEnabled ? "ON" : "OFF";
        std::string soundStatus = settings_.soundEnabled ? "ON" : "OFF";
        
        printBoxLine("    1. Animations:  " + animStatus, W);
        printBoxLine("    2. Colors:      " + colorStatus, W);
        printBoxLine("    3. Sound:       " + soundStatus, W);
        printBoxLine("    4. Show Status  (view all settings)", W);
        printBoxLine("    0. Back to Game", W);
        
        std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
        
        std::cout << BLUE << "  Select (0-4) > " << RESET;
        std::string input;
        if (!std::getline(std::cin, input)) {
            running_ = false;
            break;
        }
        
        std::string choice = trim(input);
        
        if (choice == "1") {
            settings_.animationsEnabled = !settings_.animationsEnabled;
            std::string newStatus = settings_.animationsEnabled ? "ENABLED" : "DISABLED";
            std::cout << GREEN << "  ✅ Animations " << newStatus << RESET << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            
        } else if (choice == "2") {
            settings_.colorEnabled = !settings_.colorEnabled;
            std::string newStatus = settings_.colorEnabled ? "ENABLED" : "DISABLED";
            if (settings_.colorEnabled) {
                std::cout << GREEN << "  ✅ Colors " << newStatus << RESET << std::endl;
            } else {
                std::cout << "  OK: Colors DISABLED" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            
        } else if (choice == "3") {
            settings_.soundEnabled = !settings_.soundEnabled;
            std::string newStatus = settings_.soundEnabled ? "ENABLED" : "DISABLED";
            std::cout << GREEN << "  ✅ Sound " << newStatus << RESET << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            
        } else if (choice == "4") {
            // Show detailed status
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            
            std::cout << std::endl;
            std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
            printBoxTitle(BOLD BR_YELLOW "SETTINGS STATUS" RESET, W);
            std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
            
            std::cout << BOLD << YELLOW;
            printBoxLine("  ANIMATIONS:", W);
            std::cout << RESET;
            printBoxLine("    " + std::string(settings_.animationsEnabled ? "✅ ON" : "❌ OFF"), W);
            printBoxLine("    Shows visual effects during events", W);
            
            std::cout << BOLD << YELLOW;
            printBoxLine("  COLORS:", W);
            std::cout << RESET;
            if (settings_.colorEnabled) {
                std::cout << GREEN;
                printBoxLine("    ✅ ON  (using ANSI colors)", W);
                std::cout << RESET;
            } else {
                printBoxLine("    ❌ OFF (plain text only)", W);
            }
            printBoxLine("    ANSI color codes for text coloring", W);
            
            std::cout << BOLD << YELLOW;
            printBoxLine("  SOUND:", W);
            std::cout << RESET;
            printBoxLine("    " + std::string(settings_.soundEnabled ? "✅ ON" : "❌ OFF"), W);
            printBoxLine("    Text-based sound notifications", W);
            
            std::cout << BOLD << CYAN << "  +" << std::string(W, '-') << "+" << RESET << std::endl;
            std::cout << BLUE << "  Press ENTER to continue..." << RESET;
            std::string dummy;
            std::getline(std::cin, dummy);
            
        } else if (choice == "0") {
            settingsDone = true;
            
        } else {
            std::cout << RED << "  Invalid choice. Please select 0-4." << RESET << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}
