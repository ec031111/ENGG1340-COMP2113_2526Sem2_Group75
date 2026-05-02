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
