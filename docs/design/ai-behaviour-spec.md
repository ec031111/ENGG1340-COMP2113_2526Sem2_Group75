# AI Opponent — Behaviour Design Document

**Document Type:** Internal Design Reference  
**Project:** Auto-Battler Arena  
**Version:** 1.0  
**Last Updated:** 2026-04-25  

---

## 1. Overview

The AI opponent in Auto-Battler Arena is designed to simulate an intermediate human player. It should be capable of building coherent synergy-based compositions, managing a basic economy, and making positioning decisions that reflect a genuine understanding of role mechanics. It should not, however, be omniscient — it should make believable mistakes and be beatable by a player who understands the game's systems.

This document describes the AI's decision-making logic, the internal scoring functions used to evaluate purchases, and the heuristics applied to positioning and economy management.

---

## 2. Design Philosophy

### 2.1 Approachability

New players should be able to defeat the AI with basic competency. An AI that always wins discourages learning; an AI that always loses provides no feedback or challenge. The AI is calibrated to win approximately 40–60% of combats against an average player, resulting in a competitive but not oppressive experience.

### 2.2 Legibility

The AI should make moves the player can understand in retrospect. After losing to the AI, the player should be able to look at the AI's board and think "I see what they were doing." This requires the AI to pursue recognisable strategies — known composition archetypes — rather than random unit accumulations.

### 2.3 Limitations by Design

The AI has several deliberate limitations:

- It does not inspect the player's board composition and does not counter-build.  
- It does not dynamically reposition units based on the player's formation.  
- It does not save gold for interest beyond a basic threshold.  
- Its re-roll decisions are heuristic rather than optimal.  

These limitations create exploitable patterns that experienced players can learn to take advantage of.

---

## 3. Planning Phase Decision Loop

Each AI planning phase executes the following steps in order:

### Step 1: Evaluate Upgrades

The AI scans its bench and board for units where two copies already exist. If adding a third copy (from the current shop) would complete an upgrade, the AI assigns this purchase the highest priority (Priority Score = 100). The AI will spend up to its entire gold reserve to complete an upgrade if one is available.

### Step 2: Evaluate Synergy Completion

The AI calculates the synergy count for each role present in its current board. If it is one unit away from activating the next synergy tier (e.g., 3 Warriors on board, one more would activate the 4-unit bonus), it assigns the missing unit a Priority Score of 75.

Priority is given to the synergy tier that provides the largest relative bonus:
- 2-unit to 4-unit transitions are weighted more heavily than 0-unit to 2-unit.  
- Synergies with high-impact bonuses (Mage splash, Assassin bleed) are weighted 20% higher than average.  

### Step 3: Evaluate Power Score

For each unit in the shop that does not contribute to an upgrade or synergy completion, the AI calculates a raw Power Score:

```
PowerScore = (HP × 0.3) + (Attack × 1.5) + (Defense × 1.2) + (Speed × 5) + (CritChance × 80)
```

The unit with the highest Power Score that the AI can afford is assigned Priority Score = 50.

### Step 4: Economy Check

Before any purchase, the AI checks its gold reserve:

- If gold ≥ 30, proceed with all purchases normally.  
- If gold < 30, reduce non-critical purchases by one unit (skip the lowest-priority item on the current list).  
- The AI never holds more than 50 gold intentionally — unlike a human player, it does not plan for interest income in the same way.  

### Step 5: Re-Roll Decision

After making all purchases it can afford, the AI evaluates whether to re-roll:

- If it has an active upgrade opportunity (2 copies, needs a third) AND has 2+ gold remaining: re-roll once per round.  
- If no upgrade opportunity: do not re-roll.  

The AI re-rolls at most once per planning phase regardless of gold. This limits its ability to chain re-rolls for perfect units, which would make it too strong.

---

## 4. Positioning Heuristics

After purchasing, the AI arranges its units on the board using the following rules:

### 4.1 Row Assignment

The AI assigns rows based purely on role:

| Role     | Preferred Row | Fallback Row |
|----------|---------------|--------------|
| Tank     | Row 1         | Row 1 (no fallback) |
| Warrior  | Row 1         | Row 2 if Row 1 full |
| Assassin | Row 1         | Row 2 (rare) |
| Mage     | Row 2         | Row 1 only if no Row 2 space |
| Archer   | Row 2         | Row 1 if forced |

The AI fills Tank slots first, then Warriors, then Assassins in Row 1. Mages fill Row 2 before Archers.

### 4.2 Column Assignment

Within each row, the AI fills columns left-to-right in the order units were purchased. There is no column-specific optimisation. This is a deliberate weakness — a human player who understands column positioning (e.g., centering Tanks, spreading Mages) gains a meaningful positional advantage.

### 4.3 Board Validation

Before confirming, the AI validates:
- The board has at least 2 units deployed.  
- No unit has been placed outside valid grid positions.  
- The unit count does not exceed the player's current level cap.  

If validation fails (e.g., the AI has too many units on board for its level), it moves excess units to the bench starting with the lowest Priority Score unit.

---

## 5. AI Composition Tendencies

The AI does not randomly select units. Its scoring system creates observable tendencies:

**Early Game (Rounds 1–4):** The AI tends to buy whatever is cheapest and highest Power Score. This usually results in a mixed bag of Tier 1 units with no clear synergy direction. This is the easiest phase for human players to exploit — the AI's board is incoherent and the synergy bonuses are low.

**Mid Game (Rounds 5–7):** Upgrade completions and synergy bonuses begin to drive the AI's decisions. By Round 6, the AI typically has one 2-unit synergy active. It will begin making more focused purchases aligned with its emerging composition.

**Late Game (Rounds 8+):** The AI should have a recognisable composition with at least one 2-unit synergy and is actively hunting for upgrades. At this stage, the AI's composition is close to its final form, and the human player's strategic choices from Rounds 4–6 determine whether they have built a stronger composition.

---

## 6. AI Difficulty Calibration

The AI has been calibrated against the following benchmarks during development:

| Round Range | Target Win Rate Against New Player | Target Win Rate Against Intermediate Player |
|-------------|-----------------------------------|---------------------------------------------|
| 1–3         | 20%                               | 40%                                         |
| 4–6         | 40%                               | 50%                                         |
| 7–9         | 55%                               | 55%                                         |
| 10+         | 60%                               | 50%                                         |

These targets reflect the intention that new players should survive to mid-game and learn the mechanics, while intermediate players should face a fair challenge that rewards skill.

If the AI is winning too frequently against new players, the primary adjustment is to reduce the re-roll rate (making the AI's composition less coherent in early rounds). If the AI is too easy for intermediate players, the primary adjustment is to increase the synergy threshold weighting in the scoring function.

---

## 7. Technical Implementation

The AI is implemented in `src/ai.cpp`, declared in `include/ai.h`. The primary function is:

```cpp
// Execute one full planning phase for the AI player
void AI::executePlanningPhase(Player& aiPlayer, Shop& shop);
```

Internal helper functions:

```cpp
// Score a unit for purchase priority
int AI::scorePurchase(const Unit& unit, const Player& aiPlayer);

// Determine best board arrangement
void AI::arrangeBoard(Player& aiPlayer);

// Decide whether to re-roll
bool AI::shouldReroll(const Player& aiPlayer, const Shop& shop);
```

The AI uses the same `Player` class as the human player, ensuring that all game rules (level caps, bench limits, gold deduction) apply equally to both.

---

## 8. Known AI Weaknesses and Future Improvements

**Current limitations:**
- Does not counter-build against the player's observed composition.  
- Suboptimal column positioning (left-to-right fill only).  
- Does not plan for interest income or maintain a target gold reserve.  
- Cannot evaluate multi-synergy compositions; optimises one synergy at a time.  

**Planned improvements (future versions):**
- Add a counter-build mode where the AI adjusts unit selection after scouting the player's board.  
- Implement a gold target system where the AI deliberately holds 30 gold for interest before spending.  
- Add position-aware column assignment (centre Tanks, spread Mages).  
- Multi-synergy evaluation to allow the AI to pursue hybrid compositions (e.g., 2 Warriors + 2 Tanks) instead of always going deep into one role.  

---

*End of AI Behaviour Design Document v1.0*

---

## 9. AI Parameter Tuning Sheet

This sheet defines tunable constants used by the AI logic and recommended ranges for calibration.

| Parameter Name | Default | Min | Max | Purpose |
|---|---:|---:|---:|---|
| `UpgradePriority` | 100 | 60 | 150 | Weight for buying third-copy upgrades |
| `SynergyPriority` | 75 | 40 | 120 | Weight for completing synergy thresholds |
| `RawPowerPriority` | 50 | 20 | 90 | Weight for best standalone unit purchase |
| `MinGoldReserve` | 30 | 10 | 50 | Economy floor before reducing purchases |
| `RerollLimitPerRound` | 1 | 0 | 3 | Maximum rerolls in planning phase |
| `LowHPSpendThreshold` | 35 | 20 | 50 | HP threshold for emergency spending mode |
| `EmergencySpendMultiplier` | 1.3 | 1.0 | 2.0 | How aggressively AI spends when low HP |
| `SynergyTier2Bonus` | 1.0 | 0.8 | 1.5 | Additional factor for 0->2 synergy transition |
| `SynergyTier4Bonus` | 1.2 | 1.0 | 2.0 | Additional factor for 2->4 synergy transition |
| `CarryProtectionWeight` | 0.6 | 0.0 | 1.5 | Positioning weight for protecting top DPS unit |

### 9.1 Suggested Presets

**Easy Preset**
- UpgradePriority = 80  
- SynergyPriority = 60  
- RawPowerPriority = 55  
- MinGoldReserve = 35  
- RerollLimitPerRound = 0

**Normal Preset**
- UpgradePriority = 100  
- SynergyPriority = 75  
- RawPowerPriority = 50  
- MinGoldReserve = 30  
- RerollLimitPerRound = 1

**Hard Preset**
- UpgradePriority = 120  
- SynergyPriority = 95  
- RawPowerPriority = 45  
- MinGoldReserve = 20  
- RerollLimitPerRound = 2

---

## 10. AI Behaviour State Machine

The AI can be represented with the following high-level states:

1. `EvaluateBoard`  
2. `PurchasePhase`  
3. `PositionPhase`  
4. `ValidateBoard`  
5. `CommitPhase`

### 10.1 State Details

`EvaluateBoard`
- Count current role distribution.  
- Identify missing frontline slots.  
- Detect near-upgrades (two copies owned).  
- Compute threat score based on recent loss streak.

`PurchasePhase`
- Build candidate list from shop.  
- Score each candidate with weighted function.  
- Buy until no high-value purchase remains or gold floor hit.  
- Optional reroll once if upgrade chance remains high.

`PositionPhase`
- Place Tanks first (front centre preference).  
- Place Warriors/Assassins in remaining front slots.  
- Place Mages/Archers in backline.  
- Reassign one durable bait if enemy dive risk estimated high.

`ValidateBoard`
- Ensure max unit cap not exceeded.  
- Ensure minimum frontline presence.  
- Ensure at least one carry remains protected.

`CommitPhase`
- Lock board for combat.  
- Persist simple telemetry snapshot (for future tuning).  
- End planning phase.

---

## 11. Telemetry Fields for AI Review

If collecting AI tuning data, store these fields per round:

| Field | Type | Description |
|---|---|---|
| `roundIndex` | int | Current round number |
| `aiHP` | int | AI HP at start of planning |
| `aiGoldStart` | int | Gold before purchases |
| `aiGoldEnd` | int | Gold after purchases |
| `aiLevel` | int | Player level |
| `frontlineCount` | int | Units in Row 1 |
| `backlineCount` | int | Units in Row 2 |
| `activeSynergies` | string | Comma list of active synergies |
| `rerollCount` | int | Number of rerolls this round |
| `upgradesCompleted` | int | Number of auto-combines this round |
| `combatResult` | string | Win/Loss |
| `survivors` | int | Surviving AI units after combat |
| `damageTaken` | int | HP lost this round |

This telemetry can be reviewed in aggregate to detect weak AI phases. Example: if damageTaken spikes on rounds 5-6, mid-game transition logic may be too passive.

---

## 12. Scenario Test Matrix

Use the following test matrix for regression checks after AI tuning changes.

### 12.1 Economy Scenarios

Scenario E1:
- Start with gold 8, HP 100, mixed Tier 1 shop.  
- Expected: AI buys one frontline + one carry, no reroll.

Scenario E2:
- Start with gold 34, HP 100, no upgrade opportunities.  
- Expected: AI preserves reserve, minimal purchases.

Scenario E3:
- Start with gold 52, HP 100, one copy away from star.  
- Expected: AI purchases upgrade path, may reroll once.

Scenario E4:
- Start with gold 22, HP 28, two recent losses.  
- Expected: AI enters emergency spending mode.

Scenario E5:
- Start with gold 15, HP 75, high-value Tier 3 in shop at Level 5.  
- Expected: AI buys if composition-compatible; avoids random greed.

### 12.2 Positioning Scenarios

Scenario P1:
- Board has 2 Tanks, 2 Mages, 2 Archers.  
- Expected: Tanks front, ranged back, no empty frontline centre.

Scenario P2:
- Board full of fragile units and one Tank.  
- Expected: Tank front centre, fragile units spread backline.

Scenario P3:
- Assassin-heavy lineup with no Warriors.  
- Expected: Assassins front row edges, one durable bait preserved.

Scenario P4:
- Unit cap exceeded by one due to level mismatch.  
- Expected: lowest-value unit moved to bench automatically.

Scenario P5:
- No frontline unit available in roster.  
- Expected: highest HP backline unit temporarily moved front.

### 12.3 Synergy Scenarios

Scenario S1:
- 3 Warriors on board, one Warrior in shop.  
- Expected: Warrior purchase strongly preferred.

Scenario S2:
- 2 Mages active, no Mage in shop, high raw-power Assassin appears.  
- Expected: buy Assassin if no better synergy completion exists.

Scenario S3:
- 1 copy short of star and 1 copy short of 4-unit synergy simultaneously.  
- Expected: upgrade completion chosen first.

Scenario S4:
- Multiple synergies possible with equal score.  
- Expected: tie-break uses current composition commitment.

Scenario S5:
- 4-unit synergy active but weak frontline causing heavy losses.  
- Expected: AI buys frontline stabilization even if synergy purity drops.

### 12.4 Event Response Scenarios

Scenario V1 (Market Boom):
- Reroll cost reduced to 1.  
- Expected: AI allows one additional reroll when near-upgrade.

Scenario V2 (War Tax):
- Base income reduced.  
- Expected: AI trims speculative purchases.

Scenario V3 (Mage Debuff):
- Mages penalized this round.  
- Expected: AI still follows comp but increases non-Mage buy tolerance.

Scenario V4 (Wild Shop):
- Tier 3-rich shop appears.  
- Expected: AI buys composition-compatible Tier 3 first.

Scenario V5 (Chaos Position Swap):
- Random swap event active.  
- Expected: AI re-validates frontline after swap in next planning phase.

---

## 13. Fail-Safe Rules

To avoid degenerate AI behaviour, enforce these safeguards:

1. AI cannot end planning with zero units on board.  
2. AI cannot spend below zero gold via chained operations.  
3. AI cannot reroll if bench is full and no sell path exists.  
4. AI cannot lock shop and reroll in the same action cycle.  
5. AI must resolve auto-combine before final positioning.  
6. AI must recalculate synergy counts after every purchase.  
7. AI must treat invalid board position as hard error and repair immediately.

---

## 14. Future Upgrade Paths

### 14.1 Opponent-Aware AI

Add lightweight scouting:
- Read enemy top two damage sources.  
- Increase purchase score for direct counters.  
- Add one-time reposition each round against observed threat type.

### 14.2 Adaptive Economy AI

Adjust reserve dynamically:
- If HP high and streak positive -> increase reserve target.  
- If HP low and streak negative -> lower reserve and spend aggressively.

### 14.3 Explainable AI Debug Output

Optional debug mode should print:
- Top 3 purchase candidates and scores.  
- Reason for chosen purchase.  
- Why reroll was or was not performed.  
- Positioning rationale summary.

This improves maintainability and speeds up balancing cycles.
