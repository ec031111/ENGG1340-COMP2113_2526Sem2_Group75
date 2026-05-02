<img src="logo.png" width="400"/>

## Gameplay Video

<video src="video.mov" poster="video-cover.jpg" controls></video>

**YouTube Demo Link**: https://www.youtube.com/watch?v=QIBhIWE3lAY

# Auto-Battler Arena

A text-based auto-chess strategy game built in C++ for the Linux terminal.
COMP2113 / ENGG1340 Course Project - 2025-2026 2nd Semester, Group 75.

## Team Members


| Name           | UID        | GitHub                      |
| :--------------- | :----------- | :---------------------------- |
| Liu Jing Zhong | 3036393984 | https://github.com/ec031111  |
| Fang Chu Ran   | 3036480206 | https://github.com/fffcr     |
| Yu Zi Yi       | 3036666185 | https://github.com/Elisa-Y   |
| Li Xin Yang     | 3036590984 | https://github.com/oscarlixy |
| Ye Chin Ho     | 3036589222 | https://github.com/chinho-hub|

## Quick Start

### System Requirements

- **Compiler**: GCC (g++) version 5.0 or higher (supporting C++11)
- **Build Tool**: GNU Make
- **Terminal**: Any standard terminal emulator


### Step 1: Compile the Game

Navigate to the project directory and compile using Make:

```bash
make game
```

This will:
- Compile all source files in the `src/` directory
- Generate object files in the `build/` directory
- Link them into the executable `game` in the project root

To clean previous builds:

```bash
make clean
```

### Step 2: Run the Game

Once compiled, you can start the game with:

```bash
./game
```

## Game Description

Auto-Battler Arena is a single-player strategy game inspired by auto-chess games like Teamfight Tactics and Dota Auto Chess. The player recruits units from a randomly refreshed shop, manually arranges them into a formation on a 5x8 grid, and watches them automatically battle an AI opponent. The game features a star upgrade system (merging 3 identical units into a stronger version), class synergies that reward building around specific unit types, unique class abilities that trigger during combat, and random events between rounds that shake up each playthrough.

Each round alternates between two phases:

- **Shop Phase**: The player receives gold income (with win-streak bonuses), browses 5 randomly generated units, purchases and sells units, and manually places them on their half of the board to form a strategy. Commands like `place`, `remove`, and `auto` control the formation. Buying 3 copies of the same unit triggers an automatic merge into a starred-up version with boosted stats.

- **Battle Phase**: All units on both sides fight automatically on the 5x8 grid. Each tick, every unit finds the nearest enemy and either attacks (if in range) or moves one step closer. Class abilities such as Mage AOE splash, Assassin backstab, Tank damage blocking, and Archer double shot trigger randomly during combat. The battle ends when one side is completely eliminated.

**Symmetric Combat System**: Both the player and AI have HP (100). With each battle:
- If the **player wins**: AI loses HP (5 + surviving player units × 2)
- If the **player loses**: Player loses HP (5 + surviving AI units × 2)  
- If it's a **draw** (stalemate after 200 ticks): Neither side takes damage

The game ends when either the **AI is defeated (player victory)** or the **player's HP reaches zero (player defeat)**. The outcome (WIN/LOSS) is recorded to the leaderboard. Players can save their game mid-session and load it later from the main menu.

## Features and Coding Elements

### 1. Generation of Random Events

This requirement is satisfied through multiple randomized systems:

- **Shop randomization**: Each round, 5 units are randomly selected from a pool of 15 unit templates with different classes, stats, and costs. The selection uses `rand()` seeded with `srand(time(nullptr))` at program start.
- **Critical hit system**: Every attack has a base 20% critical hit chance (modified by unit class and synergy bonuses), dealing 1.5x damage when triggered.
- **Class ability triggers**: Each class has a unique ability with a random activation chance per attack - Mage AOE (30%), Assassin Backstab (25%), Tank Block (25%), Archer Double Shot (20%).
- **Between-round events**: After round 1, there is a 40% chance of a random event occurring. The 6 possible events are: Gold Rush (gain 3-6 bonus gold), Shop Sale (all units cost $1 less), Divine Blessing (all units fully healed), Lucky Find (receive a free random unit), Tax Collector (lose 2-4 gold), and Enemy Buff (AI gets stronger).

Implementation: `event.h/event.cpp` handles event rolling and application. Random number generation throughout uses `rand() % N` for different probability checks.

### 2. Data Structures for Storing Data

Multiple data structures are used throughout the project:

- **2D `std::vector<std::vector<Unit*>>`** in `board.h`: Represents the 5x8 battle grid. Supports spatial queries (nearest enemy search), unit placement, movement, and removal. Used for both player and AI sides.
- **`std::vector<Unit*>`** in `player.h`: The player's bench (reserve roster), storing purchased units before they are deployed to the board. Supports dynamic insertion, removal by index, and iteration for merge checks.
- **`std::vector<UnitTemplate>`** in `shop.h`: Stores the 15 unit blueprint templates that the shop randomly samples from each round.
- **`std::vector<SynergyBonus>`** in `synergy.h`: Stores calculated synergy bonuses each battle round, used for display and application.
- **`std::vector<Record>`** in `game.cpp`: Temporarily stores leaderboard records read from file, sorted by rounds survived for display.

### 3. Dynamic Memory Management

Dynamic memory allocation is a core part of the game's unit lifecycle:

- **Allocation with `new`**: Every unit purchased from the shop is dynamically allocated on the heap (`new Unit(...)` in `shop.cpp`). AI units are similarly allocated in `ai.cpp`.
- **Deallocation with `delete`**: Units are freed when killed in battle (`cleanupDeadUnits()` in `game.cpp`), sold by the player (`sellUnit()` in `player.cpp`), or consumed during star merges.
- **Star merge system**: When 3 identical units are detected on the bench, 2 are deleted and 1 is upgraded in place (`checkAndMerge()` in `game.cpp`). This demonstrates meaningful create-use-destroy memory lifecycle.
- **Ownership transfer**: Units move between shop -> player bench -> board -> destruction, with clear ownership semantics. The AI's `releaseArmy()` method transfers ownership to the board without double-freeing.
- **Data cleanup**: Players can manually delete game records and save files from the main menu (option 5: "Clear Data") to free disk space. The system allows selective deletion of leaderboard records (`records.txt`) or individual save slots (`save1.dat/save2.dat/save3.dat`), or clearing all data at once using `remove()` function (`clearDataMenu()` in `main.cpp`).

### 4. File Input/Output

The game uses file I/O for both persistence and record-keeping:

- **Leaderboard** (`records.txt`): After each game ends, the player's name, rounds survived, remaining gold, difficulty, and **game outcome (WIN/LOSS)** are appended to `records.txt` using `std::ofstream`. The leaderboard reads this file with `std::ifstream`, parses each line with `std::istringstream`, separates WIN and LOSS records, sorts each category by rounds (descending), and displays them in priority order (victories first, defeats second).
- **Save/Load system** (`save1.dat/save2.dat/save3.dat`): The `save` command (or quit prompt) writes the full game state to file: player HP, gold, round number, win/loss streaks, and all bench units with their base name, class, stats, and star level. The `loadGame()` function reads this file, reconstructs all `Unit` objects with `new`, restores player state via `loadState()`, and resumes the game from the saved round.

Implementation: `saveGame()` and `loadGame()` in `game.cpp`, `saveRecord()` (with `isVictory` parameter) and `displayLeaderboard()` in `record.cpp`.

### 5. Program Codes in Multiple Files

The project is organized into 11 modules with separate header and implementation files:

```
project/
├── include/                     Header files (.h)
│   ├── unit.h                   Unit class: stats, star levels, upgrade, synergy bonuses
│   ├── board.h                  Board class: 5x8 grid, display, pathfinding, placement
│   ├── shop.h                   Shop: random generation, purchase, affordability display
│   ├── player.h                 Player: HP, gold, bench management, win/loss streaks
│   ├── ai.h                     AI opponent: Easy/Hard buying and placement strategies
│   ├── game.h                   Game loop: combat engine, abilities, merging, events
│   ├── synergy.h                Synergy system: class-based team bonuses
│   ├── event.h                  Event system: 6 event types between rounds
│   ├── record.h                 Record system: leaderboard, save/load game state
│   └── tutorial.h               Tutorial mode: interactive game guide
├── src/                         Implementation files (.cpp)
│   ├── main.cpp                 Entry point, main menu, difficulty selection
│   ├── unit.cpp
│   ├── board.cpp
│   ├── shop.cpp
│   ├── player.cpp
│   ├── ai.cpp
│   ├── game.cpp
│   ├── synergy.cpp
│   ├── event.cpp
│   ├── record.cpp               Leaderboard display, game save/load with WIN/LOSS tracking
│   └── tutorial.cpp
├── Makefile                     Build automation with dependency tracking
└── README.md
```

Each module has a `.h` header file in `include/` that declares classes and interfaces, with corresponding `.cpp` implementation files in `src/`. The `Makefile` tracks header dependencies so that changing a header recompiles only affected files. The `record.cpp` module handles both leaderboard I/O (with WIN/LOSS categorization) and game state persistence.

### 6. Multiple Difficulty Levels

The game offers two difficulty levels, selectable at the start of each new game:

| Difficulty | AI Gold Budget | AI Buying Strategy | AI Formation |
|:-----------|:---------------|:-------------------|:-------------|
| **Easy** | Base (10 + round * 2) | Buys the cheapest available unit each refresh | Places all units on the front row randomly |
| **Hard** | Base + 3 extra gold | Evaluates units by score (ATK*2 + HP/5), buys highest value | Sorts units: Tanks/Warriors in front column, Mages/Archers/Assassins in back column |

The difficulty affects the AI's purchasing intelligence and tactical formation, creating a meaningful difference in challenge.

## Additional Features

### Interactive Tutorial Mode

A comprehensive 8-lesson tutorial guides new players through all game mechanics: shop system, unit classes, placement strategy, combat rules, synergies, gold management, and unit upgrades. Accessible from the main menu, it provides hands-on learning without pressure.

### Star Upgrade System (3-to-1 Merge)

Buying 3 copies of the same unit automatically merges them into a higher star version:

- Star 1 (base stats) -> Star 2 (1.8x HP, 1.5x ATK) -> Star 3 (3.0x HP, 2.5x ATK)
- Merged units are fully healed on upgrade
- The merge consumes 2 copies (freed with `delete`) and upgrades 1 in place
- Chain merges are supported (3 star-2 units -> 1 star-3 unit)
- Star level is displayed in unit names (e.g., `Warrior**` for star 2)

### Class Synergy System

Having 2 or more units of the same class on the battlefield activates team bonuses:

| Class | 2-Unit Bonus (Tier 1) | 3-Unit Bonus (Tier 2) |
|:------|:----------------------|:----------------------|
| Warrior | All units +5 ATK | All units +10 ATK |
| Mage | All Mages +15% crit chance | All Mages +30% crit chance |
| Tank | All Tanks heal +20% max HP | All units heal +15% max HP |
| Assassin | All Assassins +10 ATK | All Assassins +20 ATK and +10% crit |
| Archer | All Archers +3 ATK | All Archers +5 ATK |

Active synergies are displayed before each battle. Bonuses are applied at battle start and cleared after.

### Class Abilities

Each unit class has a unique ability that can trigger during combat:

| Class | Ability | Trigger Chance | Effect |
|:------|:--------|:---------------|:-------|
| Tank | Block | 25% when hit | Reduces incoming damage by 40% |
| Mage | AOE Splash | 30% on attack | Deals 50% ATK as splash damage to nearby enemies within 2 cells |
| Assassin | Backstab | 25% on attack | Deals an extra hit for 50% ATK damage |
| Archer | Double Shot | 20% on attack | Attacks the same target twice in one tick |
| Warrior | Rage | Passive | Fights with increased intensity when below 50% HP |

### Manual Formation System

Players can manually position units on their half of the board (rows 0-4, columns 0-3) during the shop phase:

- `place 1 2 3` - deploy bench unit #1 to row 2, column 3
- `remove 2 3` - pick up the unit at row 2, column 3 back to bench
- `auto` - automatically place all bench units (front-to-back fill)
- `formation` - display current formation

Strategic tip: Place Tanks in column 3 (frontline, first to engage enemies) and ranged units like Archers and Mages in columns 0-1 (backline, protected).

### Win/Loss Tracking and Leaderboard

Every game's outcome is tracked and recorded:

- **Victory (WIN)**: Achieved when the AI's HP is reduced to 0 or below
- **Defeat (LOSS)**: Occurs when the player's HP is reduced to 0 or below
- **Recording**: Game outcomes (WIN/LOSS), rounds survived, remaining gold, and difficulty are saved to `records.txt`
- **Leaderboard Display**: Victories are prioritized and displayed first, sorted by rounds in descending order. Defeats follow, also sorted by rounds. This reflects the new symmetric combat system where defeating the AI is the primary goal.

### Random Events

Starting from round 2, there is a 40% chance of a random event:

| Event | Effect |
|:------|:-------|
| Gold Rush | Gain 3-6 bonus gold |
| Shop Sale | All units cost $1 less this round |
| Divine Blessing | All bench units fully healed |
| Lucky Find | Receive a free random unit |
| Tax Collector | Lose 2-4 gold |
| Enemy Buff | AI receives extra resources this round |

### Save and Load System

- During the shop phase, type `save` to save the current game state
- When quitting with `quit`, the game prompts whether to save before exiting
- From the main menu, select "Load Game" to resume a saved game
- The save file (`savegame.dat`) stores: player HP, gold, round number, win/loss streaks, and all bench units with their full stats and star levels

## Non-Standard Libraries

**None.** This project uses only standard C/C++ libraries:

`<iostream>` `<vector>` `<string>` `<fstream>` `<sstream>` `<iomanip>` `<algorithm>` `<cstdlib>` `<ctime>` `<cmath>` `<climits>` `<cctype>` `<thread>` `<chrono>`

No additional installation is required on the grader's side. All libraries are included in the standard C++11 library.

## Basic commands

### Main Menu

| Input | Action |
|:------|:-------|
| 1 | 🎮 New Game (select difficulty) |
| 2 | 📚 Tutorial (interactive gameplay guide) |
| 3 | 🏅 Leaderboard (view HIGH scores) |
| 4 | 💾 Load Game (resume saved session) |
| 5 | 🗑️ Clear Data (reset save files) |
| 6 | 🚪 Quit (exit game) |

### Shop Phase

| Command | Action |
|:--------|:-------|
| `buy 1-5` | Purchase unit from shop slot (1-5) |
| `sell 1-N` | Sell unit from bench by index |
| `place 1 2 3` | Deploy bench unit #1 to row 2, col 3 |
| `remove 2 3` | Pick up unit at row 2, col 3 back to bench |
| `auto` | Auto-place remaining bench units |
| `formation` or `form` or `f` | Display current board formation |
| `refresh` | Re-roll shop for $2 |
| `status` | Show player HP, gold, stats |
| `gold` | Display current gold amount |
| `info <bench#>` | Show detailed info for bench unit |
| `info shop <slot#>` | Show detailed info for shop unit |
| `pace 0-3` | Set combat speed (0=SLOW, 1=NORMAL, 2=FAST, 3=FASTEST) |
| `settings` | Toggle colors/animations/sound |
| `save` | Save game to slot (1-3) |
| `shop` | Re-display shop |
| `ready` | Confirm formation & start battle |
| `help` | Show all available commands |
| `menu` | Return to main menu |
| `quit` or `exit` | Quit (prompts to save) |

### Battle Phase

| Input | Action |
|:------|:-------|
| Enter | Advance to next combat tick |
| `s` or `S` | Skip to battle result |
