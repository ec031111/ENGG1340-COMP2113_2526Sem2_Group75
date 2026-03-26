# Auto-Battler Arena

A text-based auto-chess strategy game built in C++ for the Linux terminal.
COMP2113 / ENGG1340 Course Project - 2025-2026 2nd Semester, Group 75.

## Team Members


| Name           | UID        | GitHub                      |
| :--------------- | :----------- | :---------------------------- |
| Liu Jing Zhong | 3036393984 | https://github.com/ec031111 |
| Member 2       | 3XXXXXXXXX | @username2                  |
| Member 3       | 3XXXXXXXXX | @username3                  |

## Game Description

Auto-Battler Arena is a single-player strategy game that runs in the Linux terminal. The player recruits units from a shop, arranges them on a grid battlefield, and watches them fight an AI opponent automatically. The game features unit upgrades, class synergies, special abilities, and random events.

Each round has two phases:

- **Shop Phase**: Receive gold, browse 5 random units, buy/sell units, manually arrange your formation on a 5x8 grid, then start battle.
- **Battle Phase**: Units auto-fight on the grid. Each tick, units find the nearest enemy, move closer or attack if in range. Class abilities and critical hits trigger during combat. Battle continues until one side is eliminated.

The goal is to survive as many rounds as possible. Losing a battle costs player HP. Game ends when HP reaches zero.

## Features Implemented

### 1. Generation of Random Events

- The shop refreshes with 5 randomly selected units each round from a pool of 15 unit types.
- Each attack has a base 20% critical hit chance, modified by unit class and synergy bonuses.
- Random events occur between rounds (40% chance after round 1): Gold Rush (bonus gold), Shop Sale (discounted units), Divine Blessing (full heal), Lucky Find (free unit), Tax Collector (lose gold), Enemy Buff (stronger AI).
- Class abilities have random trigger chances: Mage AOE (30%), Assassin Backstab (25%), Tank Block (25%), Archer Double Shot (20%).

### 2. Data Structures for Storing Data

- A 2D `std::vector<std::vector<Unit*>>` represents the 5x8 battle grid for spatial combat logic.
- `std::vector<Unit*>` serves as the player's bench for storing purchased units before deployment.
- `std::vector<SynergyBonus>` stores active synergy calculations each battle.
- `std::vector<UnitTemplate>` in the Shop class holds unit blueprints for random generation.
- `std::vector<Record>` in the leaderboard system stores and sorts historical scores.

### 3. Dynamic Memory Management

- Every purchased unit is dynamically allocated with `new` on the heap.
- Units are freed with `delete` when killed in battle, sold by the player, or consumed during 3-to-1 star upgrades.
- The merge system (3 same units -> 1 upgraded unit) demonstrates dynamic memory lifecycle: 2 units are deleted, 1 is upgraded in place.
- Shop slots, AI armies, and board cleanup all manage dynamic memory with proper ownership transfer.

### 4. File Input/Output

- Game results (rounds survived, gold, difficulty) are appended to `records.txt` after each game.
- A leaderboard reads from `records.txt` and displays the top 10 scores sorted by rounds survived.
- The save system writes full game state (player HP, gold, bench units with star levels) to `savegame.dat`.
- The load system reads save files and displays saved progress from the main menu.

### 5. Program Codes in Multiple Files

```
auto-battler-arena/
├── main.cpp              Entry point, main menu, difficulty selection
├── unit.h / unit.cpp     Unit class: stats, star levels, upgrade, synergy bonuses
├── board.h / board.cpp   Board class: 5x8 grid, display, pathfinding, placement
├── shop.h / shop.cpp     Shop: random generation, purchase, affordability display
├── player.h / player.cpp Player: HP, gold, bench, win/loss streaks
├── ai.h / ai.cpp         AI opponent: Easy/Hard buying and placement strategies
├── game.h / game.cpp     Game loop: combat, abilities, merging, events, save/load
├── synergy.h / synergy.cpp  Synergy system: class bonuses for 2+ same-class units
├── event.h / event.cpp   Random event system: 6 event types between rounds
├── Makefile              Build automation: make game
└── README.MD
```

### 6. Multiple Difficulty Levels


| Difficulty | AI Buying Strategy                                            | AI Formation                           |
| :----------- | :-------------------------------------------------------------- | :--------------------------------------- |
| Easy       | Buys cheapest units available                                 | Places all units on front row randomly |
| Hard       | Uses score-based evaluation (ATK*2 + HP/5) to pick best units | Tanks in front, damage dealers in back |

## Additional Features

### Star Upgrade System (3-to-1 Merge)

Buy 3 copies of the same unit and they automatically merge into a higher star version:

- Star 1 (base) -> Star 2 (1.8x HP, 1.5x ATK) -> Star 3 (3x HP, 2.5x ATK)
- Merged units are fully healed. The merge consumes the extra copies (dynamic memory freed).

### Class Synergy System

Having 2 or more units of the same class activates team bonuses:


| Class    | 2-Unit Bonus       | 3-Unit Bonus                 |
| :--------- | :------------------- | :----------------------------- |
| Warrior  | All units +5 ATK   | All units +10 ATK            |
| Mage     | Mages +15% crit    | Mages +30% crit              |
| Tank     | Tanks heal +20% HP | All units heal +15% HP       |
| Assassin | Assassins +10 ATK  | Assassins +20 ATK, +10% crit |
| Archer   | Archers +3 ATK     | Archers +5 ATK               |

### Class Abilities (trigger during combat)


| Class    | Ability     | Effect                                                 |
| :--------- | :------------ | :------------------------------------------------------- |
| Tank     | Block       | 25% chance to reduce incoming damage by 40%            |
| Mage     | AOE Splash  | 30% chance to deal 50% ATK as splash to nearby enemies |
| Assassin | Backstab    | 25% chance to deal an extra 50% ATK hit                |
| Archer   | Double Shot | 20% chance to attack twice in one tick                 |
| Warrior  | Rage        | Fights harder when below 50% HP                        |

### Manual Formation System

Players can manually place units on their half of the board (columns 0-3):

- `place 1 2 3` - place bench unit #1 at row 2, column 3
- `remove 2 3` - pick up unit at (2,3) back to bench
- `auto` - auto-place all bench units
- `formation` - view current formation

### Random Events Between Rounds

After round 1, there is a 40% chance of a random event:

- **Gold Rush**: Gain 3-6 bonus gold
- **Shop Sale**: All units cost $1 less this round
- **Divine Blessing**: All bench units fully healed
- **Lucky Find**: Receive a free random unit
- **Tax Collector**: Lose 2-4 gold
- **Enemy Buff**: AI gets stronger this round

## Non-Standard Libraries

None. This project uses only standard C/C++ libraries:
`<iostream>`, `<vector>`, `<list>`, `<cstdlib>`, `<ctime>`, `<fstream>`,
`<sstream>`, `<string>`, `<algorithm>`, `<iomanip>`, `<cmath>`, `<climits>`

No additional installation is required.

## Compilation and Execution Instructions

### Compile

```bash
make game
```

### Run

```bash
./game
```

### Clean

```bash
make clean
```

## Controls

### Main Menu


| Input | Action                       |
| :------ | :----------------------------- |
| 1     | New Game (select difficulty) |
| 2     | View Leaderboard             |
| 3     | Load Game                    |
| 4     | Quit                         |

### Shop Phase


| Command     | Action                              |
| :------------ | :------------------------------------ |
| buy 1-5     | Purchase unit from shop slot        |
| sell 1-N    | Sell unit from bench for gold       |
| place 1 2 3 | Place bench unit #1 at row 2, col 3 |
| remove 2 3  | Pick up unit at row 2, col 3        |
| auto        | Auto-place all bench units          |
| formation   | View current formation              |
| refresh     | Re-roll shop ($2)                   |
| save        | Save game to file                   |
| ready       | Start the battle                    |
| help        | Show all commands                   |
| quit        | Exit game                           |

### Battle Phase


| Input | Action                |
| :------ | :---------------------- |
| Enter | Advance to next tick  |
| s     | Skip to battle result |
