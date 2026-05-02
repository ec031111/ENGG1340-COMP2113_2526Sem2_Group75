# Auto-Battler Arena — Game Design Document (GDD)

**Project:** Auto-Battler Arena  
**Course:** ENGG1340 / COMP2113 — Programming Technologies  
**Group:** 75  
**Version:** 1.0  
**Last Updated:** 2026-04-25  

---

## 1. Game Overview

### 1.1 Concept Summary

Auto-Battler Arena is a terminal-based strategic auto-battle game inspired by popular titles such as Teamfight Tactics and Hearthstone Battlegrounds. Players purchase, position, and upgrade units on a grid-based battlefield, then watch automated combat resolve each round. The game runs entirely in a POSIX-compatible terminal, using ANSI escape codes for colour rendering and Unicode characters for visual clarity.

The core design philosophy is **strategic depth with accessible mechanics**: a new player can understand the basic loop within minutes, while experienced players can explore deep synergy interactions, economic management, and positional strategy.

### 1.2 Genre and Target Audience

- **Genre:** Auto-battler / Roguelite strategy  
- **Platform:** Terminal (Linux / macOS / Windows with ANSI support)  
- **Target Audience:** Computer science students and strategy game enthusiasts who are comfortable with terminal-based interfaces  
- **Session Length:** A single game session lasts approximately 15–30 minutes depending on player skill  

### 1.3 Unique Selling Points

1. **No GUI required** — the entire experience is delivered through carefully structured terminal output using ANSI colours and Unicode symbols.  
2. **Synergy-driven strategy** — fourteen distinct unit roles interact through a layered synergy system that rewards thoughtful team composition.  
3. **Dynamic shop economy** — a rolling-odds shop system forces players to make economic decisions each round: spend gold to re-roll, save to level up, or lock preferred units.  
4. **Adaptive AI opponents** — the AI builds coherent teams using a scored decision tree that mimics rational player behaviour.  
5. **Persistent records** — game results are saved to a structured file, enabling players to track improvement over time.  

---

## 2. Core Gameplay Loop

### 2.1 Round Structure

Each game round follows a fixed sequence of phases:

```
[Planning Phase] → [Combat Phase] → [Result Phase] → [Economy Phase]
```

**Planning Phase**
- The player is presented with the Shop, which offers a randomised selection of units.  
- The player may: (a) purchase units, (b) sell units on the bench, (c) re-roll the shop for 2 gold, or (d) buy experience points for 4 gold.  
- Units purchased are placed on the bench; the player then drags them onto the battlefield grid.  
- The planning phase ends when the player confirms readiness.  

**Combat Phase**
- All units on the battlefield act autonomously according to their role-specific AI behaviour.  
- Combat is turn-based internally but presented as a continuous animation to the player.  
- Each unit selects targets, performs attacks, activates abilities, and applies status effects.  
- The combat phase ends when all units on one side are eliminated or a maximum round limit is reached.  

**Result Phase**
- The winner of combat is determined. If the player loses, they take damage equal to a base value plus the number of surviving enemy units.  
- Event cards may trigger at the end of certain rounds, modifying rules for the next planning phase.  
- A summary of combat outcomes is displayed, including damage dealt, units lost, and gold earned.  

**Economy Phase**
- The player receives base gold income (5 per round) plus interest income (1 per 10 gold saved, up to 5 bonus).  
- Win/loss streaks award bonus gold (up to 3 for a three-round streak in either direction).  
- Any gold held over from previous rounds is preserved (no gold cap mid-game).  

### 2.2 Progression and Victory Conditions

- The game begins at round 1 with the player having 100 health points.  
- Each loss reduces health. The player is eliminated when health drops to 0.  
- The game is won by being the last player standing (in multiplayer modes) or by surviving all predefined rounds (in single-player challenge mode).  
- Unit upgrades (three copies of the same unit combine into a starred version with doubled stats) represent the primary power progression vector.  

---

## 3. Unit System

### 3.1 Unit Attributes

Every unit in the game is defined by the following attributes:

| Attribute      | Description                                              | Range      |
|----------------|----------------------------------------------------------|------------|
| `name`         | Unique identifier string                                 | —          |
| `role`         | Combat archetype (Warrior, Mage, Tank, Assassin, Archer) | enum       |
| `tier`         | Shop cost in gold (1–3)                                  | 1, 2, 3    |
| `maxHP`        | Maximum health points                                    | 50–400     |
| `currentHP`    | Current health (depleted during combat)                  | 0–maxHP    |
| `attack`       | Base damage per attack                                   | 15–120     |
| `defense`      | Flat damage reduction applied to each incoming hit       | 0–40       |
| `speed`        | Determines action order within a combat turn             | 1–10       |
| `critChance`   | Probability of landing a critical hit (×1.5 damage)      | 0–0.5      |
| `isStarred`    | Whether the unit has been upgraded to star tier           | bool       |
| `synergies`    | List of synergy tags associated with the unit            | vector     |

### 3.2 Unit Roles

Five primary combat roles define unit behaviour:

**Warrior** — melee attacker with moderate HP and attack. Targets the nearest enemy on the front line. Gains bonus attack speed when below 50% HP.  

**Mage** — ranged attacker with low HP but high damage output. Targets the enemy with lowest current HP. Can hit multiple enemies with splash damage when two or more Mages are fielded together.  

**Tank** — defensive frontline unit with very high HP and defense. Taunts the nearest enemy, drawing their attacks. Gains a damage-absorbing shield at combat start when the Tank synergy is active.  

**Assassin** — high-risk melee attacker with extremely high attack but very low defense. Jumps to the enemy backline (highest-damage unit) at combat start. Crits apply a bleed effect.  

**Archer** — balanced ranged attacker with moderate stats. Targets the enemy with the highest attack stat. Receives an attack speed bonus when positioned in the back two rows of the grid.  

### 3.3 Unit Upgrade System

Units follow the classic three-copy upgrade rule:

- **Tier 1 (Base):** Purchased from the shop at face value. Standard stats.  
- **Tier 2 (Starred):** Three copies of the same unit auto-combine. Stats are approximately doubled. Visual display changes to indicate the upgraded state.  

Upgrading is the most reliable path to a power spike and should be a key planning consideration throughout the game. A Tier 2 Warrior often outperforms a Tier 1 Mage even in a mage-favourable composition.

---

## 4. Board System

### 4.1 Grid Layout

The battlefield is a 2×4 grid (2 rows × 4 columns) for each player, giving 8 possible positions per side. The board renders to the terminal with clear row and column indicators.

```
  [Col 1]   [Col 2]   [Col 3]   [Col 4]
  --------  --------  --------  --------
R1| Unit  | Unit   |        |        |   ← Frontline
  --------  --------  --------  --------
R2|       | Unit   | Unit   | Unit   |   ← Backline
  --------  --------  --------  --------
```

Front-row positions (Row 1) are engaged first in melee combat. Back-row positions (Row 2) are safer for ranged units and are the jump destination for Assassins targeting the enemy team.

### 4.2 Positioning Strategy

Positioning is a critical skill dimension in Auto-Battler Arena:

- **Tanks should anchor Row 1** to maximise their taunt effect and protect fragile back-line units.  
- **Mages and Archers belong in Row 2** where they can attack unimpeded and avoid priority targeting.  
- **Assassins should be placed in Row 1** because they immediately leap to the enemy backline at combat start; their starting position only affects whether they contribute to front-line synergy counts.  
- **Warriors are flexible** and can fill either row depending on the composition's needs.  
- Leaving gaps in Row 1 allows enemies to route directly to back-line units, so covering as many front-row slots as possible is generally advisable.  

### 4.3 Bench

The bench holds up to 9 units that are purchased but not deployed. Units on the bench:

- Do not participate in combat.  
- Still count toward upgrade thresholds (three copies on bench + board will combine).  
- Can be sold at any time for 1 gold regardless of tier.  

---

## 5. Synergy System

### 5.1 Overview

Synergies are role-based bonuses that activate when a minimum number of units sharing the same tag are simultaneously present on the battlefield. Synergies are the primary driver of strategic team-building and differentiate compositions from random unit collections.

### 5.2 Synergy Activation Tiers

Most synergies have two or more activation tiers:

| Synergy     | 2 Units Active            | 4 Units Active                    |
|-------------|---------------------------|-----------------------------------|
| Warrior     | +15% attack               | +30% attack, gain HP regen        |
| Mage        | Splash on attacks         | Double splash radius, +20% damage |
| Tank        | +20 defense               | Shield at combat start            |
| Assassin    | +10% crit chance          | Bleed on crit                     |
| Archer      | +15% attack speed         | Pierce: attacks hit 2 enemies     |

Partial activation (e.g., only 1 Warrior on board) grants no bonus. Synergy counts include only units currently on the battlefield, not those on the bench.

### 5.3 Synergy Interactions

Some synergies interact with each other in meaningful ways:

- **Warrior + Mage:** A hybrid composition with 2 Warriors and 2 Mages can achieve moderate bonuses in both, but rarely maximises either. Suitable for early-game flexibility.  
- **Tank + Assassin:** Tanks draw enemy fire while Assassins leap undetected to the backline — a classic two-pronged composition. Requires careful management of the gold economy since high-tier Assassins are expensive.  
- **Archer + Mage:** A pure ranged composition with high burst damage but extremely fragile if the enemy reaches the backline. Requires a sacrificial Tank or Warrior in Row 1.  

---

## 6. Shop and Economy

### 6.1 Shop Mechanics

At the start of each planning phase the shop presents five unit slots. The probability of each tier appearing in the shop is level-dependent:

| Player Level | Tier 1 | Tier 2 | Tier 3 |
|-------------|--------|--------|--------|
| 1–3         | 70%    | 25%    | 5%     |
| 4–5         | 50%    | 35%    | 15%    |
| 6+          | 30%    | 40%    | 30%    |

The unit pool is shared and finite. When many copies of a unit are purchased (by the player or AI opponents), later copies become rarer in the shop.

### 6.2 Gold Income Sources

| Source              | Amount           | Condition                    |
|--------------------|------------------|------------------------------|
| Round income        | 5 gold           | Every round                  |
| Interest            | 1 per 10 gold    | Up to 5 bonus gold           |
| Win streak (3+)     | +2 gold          | Three or more consecutive wins|
| Loss streak (3+)    | +2 gold          | Three or more consecutive losses|
| Sell unit           | 1 gold           | Any unit, any tier           |

### 6.3 Economy Strategies

**Economy Build (Saving):** Maximise interest income by keeping gold above 50 throughout early rounds. Accept some early losses in exchange for a power-spike when high-tier units become purchasable.  

**Fast Level-Up:** Spend gold on XP aggressively to reach level 6+ early, unlocking higher-tier unit probabilities. Pair with a flexible unit pool that doesn't require many copies to function.  

**Re-Roll Strategy:** Spend spare gold each round to re-roll the shop, hunting for specific three-copy completions. Most effective in mid-game when targeting Tier 1 or Tier 2 upgrades.  

---

## 7. Event System

### 7.1 Overview

Event cards introduce round-specific modifiers that alter the rules of the next planning or combat phase. Events are drawn from a predefined pool and applied at the end of certain rounds (typically rounds 3, 6, and 9).

### 7.2 Event Categories

**Buff Events:** Grant a temporary bonus to all player units (e.g., "Blessing of the Sun — all units gain +20 attack for one round").  

**Debuff Events:** Apply a penalty that the player must navigate around (e.g., "Cursed Ground — all Mages lose 30% attack this round").  

**Economy Events:** Alter gold income rules temporarily (e.g., "Market Crash — re-rolling costs 4 gold instead of 2 this planning phase").  

**Chaos Events:** Introduce random or unpredictable effects (e.g., "Dimensional Rift — two random bench units swap to the battlefield").  

### 7.3 Design Rationale

Events are designed to prevent dominant strategies from becoming too stable. A player consistently winning with an Archer-Mage composition may suddenly find their approach challenged by a Mage debuff event. This injects variance and encourages adaptive thinking without making outcomes purely luck-dependent.

---

## 8. AI Opponent

### 8.1 AI Design Philosophy

The AI is designed to be a credible opponent that demonstrates rational auto-battler play without being omniscient. It should feel like playing against an intermediate human player who understands synergies but makes occasional suboptimal decisions.

### 8.2 AI Decision Loop

Each AI planning phase follows this priority sequence:

1. **Upgrade check** — if the AI is one copy away from completing a three-of-a-kind, it prioritises buying that unit.  
2. **Synergy completion** — identify the active synergy with the highest activation threshold not yet met; prefer units that contribute to it.  
3. **Power score purchase** — if no upgrade or synergy opportunity exists, buy the unit with the highest raw power score (weighted combination of HP, attack, and tier).  
4. **Gold management** — the AI holds a minimum gold reserve to maintain interest income unless a high-priority purchase is available.  

### 8.3 AI Positioning

The AI uses a simplified positioning heuristic:

- Tanks → Row 1, centre columns.  
- Warriors → Row 1, remaining slots.  
- Assassins → Row 1, edge columns (for maximum backline coverage after jump).  
- Mages and Archers → Row 2.  

The AI does not dynamically adapt its positioning based on the player's composition, which represents the primary exploitable weakness for skilled players.

---

## 9. Game Flow Diagram

```
START
  │
  ▼
[Initialise Game State]
  │
  ▼
┌─────────────────────┐
│   PLANNING PHASE     │
│  - View Shop         │
│  - Buy / Sell Units  │
│  - Arrange Board     │
│  - Buy XP            │
└────────┬────────────┘
         │
         ▼
┌─────────────────────┐
│   COMBAT PHASE       │
│  - Units act by speed│
│  - Apply damage      │
│  - Check win/loss    │
└────────┬────────────┘
         │
         ▼
┌─────────────────────┐
│   RESULT PHASE       │
│  - Apply HP damage   │
│  - Trigger Events    │
│  - Show summary      │
└────────┬────────────┘
         │
         ▼
   Player HP ≤ 0?
      │       │
     YES      NO
      │       │
      ▼       └──→ [Next Round]
  [GAME OVER]
```

---

## 10. Non-Functional Requirements

### 10.1 Performance

- The game must not block the terminal for more than 2 seconds during combat animation.  
- File I/O for saving records must complete within 500 milliseconds.  

### 10.2 Portability

- The codebase must compile without warnings under `g++ -std=c++17 -Wall -Wextra -pedantic`.  
- Terminal rendering must function correctly in standard Linux/macOS terminals and Windows terminals with ANSI support enabled.  

### 10.3 Maintainability

- Each major subsystem (board, shop, unit, synergy, event, AI, player, game) is encapsulated in its own `.cpp`/`.h` pair.  
- Global state is minimised; game state is passed by reference through well-defined interfaces.  

---

*End of Game Design Document v1.0*
