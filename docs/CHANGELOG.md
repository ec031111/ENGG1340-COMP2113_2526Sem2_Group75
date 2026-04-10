# Auto-Battler Arena - Change Report

**Branch:** `feature/game-improvements`
**Date:** 2026-04-09
**Base:** `main` (commit 1e69830)
**Commit:** abfe189

---

## Overview

474 lines added, 102 lines removed across 11 files. This update addresses 5 critical bugs, rebalances the early-game economy, adds a complete story/lore system, and improves the player experience with better UI and a new `info` command.

---

## 1. Bug Fixes

### 1.1 Save/Load Double-Upgrade (Critical)

**File:** `src/game.cpp`, `include/unit.h`, `src/unit.cpp`

**Problem:** When loading a saved game, star-2 and star-3 units had their stats multiplied again by the upgrade formula. For example, a saved Warrior with 216 HP (120 x 1.8) would be loaded with base 216 HP, then `upgrade()` would scale it to 388 HP (216 x 1.8).

**Fix:**
- Added `forceSetStarLevel(int level)` to `Unit` class (`unit.h:125-133`, `unit.cpp:152-155`).
- `loadGame()` now creates units with saved post-upgrade stats directly and calls `forceSetStarLevel()` to set the star level without re-scaling (`game.cpp:881-888`).

### 1.2 Warrior Rage Ability Non-functional (Critical)

**File:** `src/game.cpp`, `include/unit.h`, `src/unit.cpp`

**Problem:** The Warrior's Rage ability only printed flavor text with a 10% chance. It never actually modified the unit's ATK stat.

**Fix:**
- Added `raged_` boolean flag to `Unit` (`unit.h:194`).
- Added `isRaged()`, `setRaged()`, `clearRage()` methods (`unit.h:60,162-168`, `unit.cpp:45,175-183`).
- Rage now triggers when HP drops below 50%: applies +30% of current ATK as a bonus, once per combat (`game.cpp:737-747`).
- Rage flags are cleared at the start of each combat via `clearRage()` (`game.cpp:541-545`).

### 1.3 Hard AI `placeHard()` Losing Units

**File:** `src/ai.cpp`

**Problem:** When the front column (col 4) was full, tanks that couldn't be placed were silently dropped from the army. The old code incremented `row` unconditionally even when placement failed.

**Fix:** Rewrote `placeHard()` (`ai.cpp:187-241`) with explicit `placed` tracking and fallback columns:
- Tanks try col 4 first, then col 5.
- Damage dealers try col 7 first, then scan backwards through cols 6, 5, 4.

### 1.4 Board Display Cannot Distinguish Star Levels

**File:** `src/unit.cpp`, `src/board.cpp`

**Problem:** `getSymbol()` returned the same character regardless of star level, making it impossible to tell a 1-star and 2-star unit apart on the board.

**Fix:**
- Added `getSymbolString()` returning "W " for 1-star, "W2" for 2-star, "W3" for 3-star (`unit.cpp:66-75`).
- Updated `board.cpp` `display()` and `displayPlayerSide()` to use `getSymbolString()` instead of `getSymbol()`.

### 1.5 Archer Double Shot Skips Crit/Block

**File:** `src/game.cpp`

**Problem:** The Archer's Double Shot ability dealt flat ATK damage without checking for critical hits or Tank block ability.

**Fix:** Rewrote the ARCHER case in `performAbility()` (`game.cpp:665-695`):
- Second attack now independently rolls for critical hits using `BASE_CRIT_CHANCE + getCritBonus()`.
- Tank block is also checked for the second shot.
- Kill notifications and damage reporting are handled correctly.

---

## 2. Balance Changes

### 2.1 Starting Gold Increase

**File:** `include/player.h`

`STARTING_GOLD` changed from 10 to 15. Players can now afford 3-4 units in round 1 instead of 2-3, bringing the player closer to parity with the AI's starting budget of 12+ gold.

### 2.2 Loss Streak Bonus

**File:** `include/player.h`, `src/player.cpp`

Added `LOSS_STREAK_BONUS = 1` constant. Players now earn +1 gold per consecutive loss (matching the existing win streak bonus). This prevents death spirals where a losing player falls further and further behind.

### 2.3 Interest System

**File:** `include/player.h`, `src/player.cpp`

Added `INTEREST_PER_10 = 1`. Players earn +1 gold for every 10 gold in their balance at the start of each round, capped at +5 bonus gold. This rewards economic play and strategic saving.

### 2.4 Income Display

**File:** `src/player.cpp`

`startNewRound()` now displays income breakdown: `(base:5 +win streak:2 +loss streak:1 +interest:3)`. Players can see exactly where their gold comes from.

---

## 3. UX Improvements

### 3.1 Smart Auto-Placement

**File:** `src/game.cpp`

The `auto` and `ready` commands now use class-aware placement via `smartPlaceUnit()`:
- **Tanks/Warriors** placed in column 3 (front line, closest to enemy)
- **Archers/Mages** placed in column 0 (back line, safest)
- **Assassins** placed in column 1 (middle)
- Falls back to adjacent columns if preferred column is full

Previously, all units were stacked in column 3 regardless of class, putting ranged units in melee range.

### 3.2 `info` Command

**File:** `src/game.cpp`, `include/game.h`

New command `info <N>` or `info shop <N>` displays a detailed unit card:
```
  +---------------------------------------------+
  |                  UNIT INFO                   |
  +---------------------------------------------+
  |  Name:   Sniper*                             |
  |  Class:  Archer                              |
  |  Star:   *                                   |
  +---------------------------------------------+
  |  HP:     65                                  |
  |  ATK:    30                                  |
  |  Crit:   30%                                 |
  |  Range:  4                                   |
  +---------------------------------------------+
  |  Ability: DblShot                            |
  |  20% chance: attack again (can crit)         |
  |  Lore: Precise marksmen who can fire twice   |
  +---------------------------------------------+
  |  Cost: $5  |  Sell: $2                       |
  +---------------------------------------------+
```

### 3.3 Enhanced Shop Display

**File:** `src/shop.cpp`

Shop now shows attack range and ability tags:
```
  [1] Sniper    Arch HP:65  ATK:30 R:4 [DblShot] $5
  [2] Tank      Tank HP:180 ATK:10 R:1 [Block]   $3
```

Added `getAbilityTag()` to `Unit` class (`unit.h:63`, `unit.cpp:80-89`).

### 3.4 Enhanced Help Screen

**File:** `src/game.cpp`

`printHelp()` now includes:
- **CLASS ABILITIES** section listing every class ability with description
- **TIPS** section explaining synergies, merge system, interest, and formation strategy

### 3.5 Status Bar

**File:** `src/player.cpp`

`displayStatus()` now shows both win and loss streak: `Win: 3  Loss: 0` instead of only `Win Streak: 3`.

---

## 4. Lore and World-Building

### 4.1 Story Introduction

**File:** `src/game.cpp`, `include/game.h`

New `showIntro()` method displays a boxed story prologue about the realm of Aethoria, the Dark Lord Malachar, and the player as the last commander of the Allied Forces. Shown only for new games (not loaded games).

`run()` signature changed to `run(bool showIntro = true)` (`game.h:41`).

### 4.2 Round Flavor Text

**File:** `src/game.cpp`

Themed messages at key milestones:
- Round 1: "The first wave approaches..."
- Round 5: "The enemy grows restless..."
- Round 10: "A formidable army approaches!"
- Round 15: "The Dark Lord himself sends his elite guard!"
- Round 20+: "The final battle rages on..."
- Other rounds: randomly selected from 4 atmospheric messages

### 4.3 Faction Naming

**File:** `src/game.cpp`

- Player army renamed from "YOUR ARMY"/"YOU" to "ALLIED FORCES"/"ALLIED"
- Enemy army renamed from "ENEMY" to "Dark Army"/"DARK ARMY"

### 4.4 Class Lore Descriptions

**File:** `src/unit.cpp`, `include/unit.h`

New static method `Unit::getClassDescription(UnitClass)` returns flavor text:
- Warrior: "Fearless fighters who grow stronger when wounded."
- Mage: "Glass cannons wielding devastating area magic."
- Tank: "Iron-walled defenders who shrug off blows."
- Assassin: "Shadowy killers with lethal critical strikes."
- Archer: "Precise marksmen who can fire twice in succession."

### 4.5 Victory/Defeat Messages

**File:** `src/game.cpp`

Replaced single victory/defeat messages with random selection from themed arrays:
- 3 victory variants ("The enemy retreats!", "The Dark Army scatters!", etc.)
- 2 defeat variants ("The Dark Army advances.", "Your soldiers couldn't hold the line.")

### 4.6 Game Over Narrative

**File:** `src/game.cpp`

Narrative text based on rounds survived:
| Rounds | Message |
|--------|---------|
| < 5 | "The Dark Lord's forces overwhelmed you..." |
| 5-9 | "You fought bravely, but Malachar's army was too strong." |
| 10-14 | "A valiant effort! Your name will be remembered." |
| 15-19 | "Legendary commander! You held the line longer than anyone." |
| 20+ | "IMMORTAL! The bards will sing of your deeds forever!" |

---

## 5. Playtest Log

### Test 1: Easy Mode, Round 1

- Starting gold: 21 (15 base + 6 income with 1 interest)
- Bought: Assassin ($4), Guardian ($5), Sniper ($5)
- `auto` smart-placed: Sniper[col 0], Assassin[col 1], Guardian[col 3]
- AI army: Warrior, Mage, Rogue (Easy mode: random front column)
- Result: **Victory** in 6 ticks (294 player damage vs 200 enemy damage)

### Test 2: Hard Mode, Round 1

- Starting gold: 21
- Ran `help`: CLASS ABILITIES table and TIPS section displayed correctly
- Ran `info shop 1`: Full unit card for Sniper shown with lore, ability, stats
- Bought: Sniper ($5), Tank ($3), Assassin ($4), Archer ($3)
- `auto` smart-placed: Sniper+Archer[col 0], Assassin[col 1], Tank[col 3]
- Synergy triggered: Archer x2 = +3 ATK
- AI army: Sorcerer, Sniper x2 (Hard mode: back column col 7)
- Result: **Victory**

### Verified Features

- [x] Story intro displays on new game
- [x] Story intro skipped on loaded game
- [x] Income breakdown shows base/win streak/loss streak/interest
- [x] Shop shows ability tags and range
- [x] `info` command works for bench and shop units
- [x] `help` shows class abilities and tips
- [x] Smart placement assigns correct columns by class
- [x] Star levels visible on board (W, W2, W3)
- [x] "ALLIED FORCES" / "Dark Army" labels in battle
- [x] Round flavor text displays at correct milestones
- [x] Synergy display works
- [x] Zero compilation errors, zero warnings (`-Wall -Wextra -pedantic`)

---

## Files Changed

| File | Changes |
|------|---------|
| `include/game.h` | +3 lines: `run(bool)`, `showIntro()` declaration |
| `include/player.h` | +3 lines: `LOSS_STREAK_BONUS`, `INTEREST_PER_10`, `STARTING_GOLD=15` |
| `include/unit.h` | +35 lines: `forceSetStarLevel()`, `isRaged()`, `setRaged()`, `clearRage()`, `getSymbolString()`, `getAbilityTag()`, `getClassDescription()`, `raged_` member |
| `src/ai.cpp` | +30/-16 lines: Rewrote `placeHard()` with fallback columns |
| `src/board.cpp` | +6/-4 lines: Use `getSymbolString()` in display |
| `src/game.cpp` | +300/-45 lines: `showIntro()`, flavor text, victory/defeat messages, game-over narrative, smart placement, `info` command, `hpBar()`, `abilityName()`, `abilityDesc()`, rage clearing, faction labels, updated `printHelp()` |
| `src/main.cpp` | +2/-2 lines: `run(true)` for new game, `run(false)` for loaded game |
| `src/player.cpp` | +15/-9 lines: Loss streak income, interest calculation, updated status display |
| `src/shop.cpp` | +8/-8 lines: Wider display, ability tags, range column |
| `src/unit.cpp` | +55/-28 lines: `forceSetStarLevel()`, `getSymbolString()`, `getAbilityTag()`, `getClassDescription()`, `isRaged()`, `setRaged()`, `clearRage()`, `raged_` member init |
| `docs/records.txt` | New file (leaderboard data) |
