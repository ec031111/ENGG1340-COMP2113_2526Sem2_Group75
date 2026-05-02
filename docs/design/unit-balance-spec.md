# Unit Balance Specification

**Document Type:** Internal Design Reference  
**Project:** Auto-Battler Arena  
**Version:** 1.2  
**Last Updated:** 2026-04-25  

---

## 1. Purpose

This document defines the numerical baseline for every unit in Auto-Battler Arena, the rationale behind each unit's stat distribution, and the framework used to evaluate balance changes during development. It serves as the single source of truth for unit statistics and should be consulted before any tuning changes are introduced.

---

## 2. Balance Framework

### 2.1 Power Score Formula

Every unit's overall power is assessed using the following weighted formula:

```
PowerScore = (HP × 0.3) + (Attack × 1.5) + (Defense × 1.2) + (Speed × 5) + (CritChance × 80)
```

This formula was derived empirically by running simulated 1v1 combats between all unit pairs and adjusting weights until the computed score correlated with actual win rate within ±5%.

### 2.2 Tier Balance Targets

Units are grouped into three cost tiers. Each tier has a target power-score range at base (non-starred) level:

| Tier | Gold Cost | Target PowerScore Range |
|------|-----------|-------------------------|
| 1    | 1         | 80 – 120                |
| 2    | 2         | 140 – 190               |
| 3    | 3         | 210 – 280               |

Starred (upgraded) units should score approximately 180–200% of their base score, reflecting doubled stats.

### 2.3 Role Balance Principles

Each role should be distinctly viable and fill a clear niche:

- **No role should be universally dominant.** Every role must have at least one reliable counter.  
- **Tier-appropriate power.** A Tier 3 unit should not be outperformed by a Tier 1 unit of the same role in a straight fight.  
- **Synergy multiplier.** At maximum synergy activation (4 units), a pure composition should be approximately 30–40% stronger than the same units with no synergy — strong enough to matter, but not so dominant that synergies feel mandatory.  

---

## 3. Unit Roster and Statistics

### 3.1 Tier 1 Units

#### Iron Warrior (Warrior)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 150    | 300      |
| Attack      | 25     | 50       |
| Defense     | 8      | 16       |
| Speed       | 4      | 4        |
| CritChance  | 0.05   | 0.05     |
| PowerScore  | 98.5   | 197.0    |

**Design Notes:** Iron Warrior is the entry-level frontline unit. His moderate stats make him reliable but not spectacular. He is intentionally easy to combine (Tier 1 means more copies in the pool) to introduce new players to the upgrade system.

#### Aether Mage (Mage)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 80     | 160      |
| Attack      | 35     | 70       |
| Defense     | 2      | 4        |
| Speed       | 5      | 5        |
| CritChance  | 0.10   | 0.10     |
| PowerScore  | 102.5  | 205.0    |

**Design Notes:** Aether Mage has a higher PowerScore than Iron Warrior despite being Tier 1, which is intentional. Mages are harder to protect; their squishiness (80 HP, 2 defense) means they die quickly if reached by melee units. Their high-value score rewards players who successfully keep them safe.

#### Pebble Shield (Tank)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 200    | 400      |
| Attack      | 15     | 30       |
| Defense     | 15     | 30       |
| Speed       | 3      | 3        |
| CritChance  | 0.00   | 0.00     |
| PowerScore  | 93.0   | 186.0    |

**Design Notes:** Pebble Shield is the simplest unit in the game — pure tankiness with negligible offense. His low PowerScore is compensated by his longevity in combat; he drains enemy resources even when he does not deal much damage. Beginners are encouraged to use him as a free Row 1 placeholder.

#### Shadow Cub (Assassin)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 90     | 180      |
| Attack      | 40     | 80       |
| Defense     | 3      | 6        |
| Speed       | 8      | 8        |
| CritChance  | 0.20   | 0.20     |
| PowerScore  | 115.0  | 230.0    |

**Design Notes:** Shadow Cub is intentionally above the Tier 1 power target. This is a deliberate decision: the Assassin role is high-risk (extremely low defense) and requires the player to protect them via positioning. Their high speed and crit chance make them exciting and reward players who understand the jump mechanic.

#### Leaf Archer (Archer)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 100    | 200      |
| Attack      | 28     | 56       |
| Defense     | 5      | 10       |
| Speed       | 6      | 6        |
| CritChance  | 0.10   | 0.10     |
| PowerScore  | 99.5   | 199.0    |

**Design Notes:** Leaf Archer sits at the midpoint of Tier 1, with balanced stats across the board. Archers are most effective in the back row, and Leaf Archer teaches this positioning lesson early. The moderate crit chance gives him occasional exciting moments that help players emotionally invest in the unit.

---

### 3.2 Tier 2 Units

#### Blademaster (Warrior)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 220    | 440      |
| Attack      | 50     | 100      |
| Defense     | 15     | 30       |
| Speed       | 5      | 5        |
| CritChance  | 0.10   | 0.10     |
| PowerScore  | 162.0  | 324.0    |

**Design Notes:** Blademaster significantly outpaces Iron Warrior in both offense and defense. His PowerScore is at the upper end of the Tier 2 range, justified by the Warrior role's lower mobility (no backline jump, no range). Players who invest in Warriors should feel clearly rewarded at Tier 2.

#### Arcane Scholar (Mage)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 120    | 240      |
| Attack      | 65     | 130      |
| Defense     | 5      | 10       |
| Speed       | 6      | 6        |
| CritChance  | 0.15   | 0.15     |
| PowerScore  | 175.5  | 351.0    |

**Design Notes:** Arcane Scholar has the highest PowerScore among Tier 2 units. This is balanced by the fact that Mage splash damage depends on having two or more Mages active, so a solo Arcane Scholar performs below expectation unless supported.

#### Stone Guardian (Tank)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 320    | 640      |
| Attack      | 20     | 40       |
| Defense     | 25     | 50       |
| Speed       | 3      | 3        |
| CritChance  | 0.00   | 0.00     |
| PowerScore  | 150.0  | 300.0    |

**Design Notes:** Stone Guardian at Tier 2 represents a meaningful jump in survivability. The low PowerScore reflects an intentional design choice: Tanks derive value from time — the longer they stay alive, the more damage they absorb on behalf of back-line units. Raw PowerScore does not capture this temporal contribution.

#### Nightblade (Assassin)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 130    | 260      |
| Attack      | 75     | 150      |
| Defense     | 8      | 16       |
| Speed       | 9      | 9        |
| CritChance  | 0.25   | 0.25     |
| PowerScore  | 191.0  | 382.0    |

**Design Notes:** Nightblade is the most powerful unit at Tier 2 by PowerScore but is balanced by the cost of buying multiple Mages to survive against the common Mage-Archer compositions encountered in mid-game.

#### Hawk Ranger (Archer)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 150    | 300      |
| Attack      | 55     | 110      |
| Defense     | 10     | 20       |
| Speed       | 7      | 7        |
| CritChance  | 0.15   | 0.15     |
| PowerScore  | 167.5  | 335.0    |

**Design Notes:** Hawk Ranger adds meaningful range DPS without hitting the near-broken numbers of Nightblade. The Pierce synergy bonus at 4 Archers makes a stacked Archer team viable as a late-game win condition, and Hawk Ranger is the keystone unit for that strategy.

---

### 3.3 Tier 3 Units

#### Dragonborn Champion (Warrior)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 380    | 760      |
| Attack      | 90     | 180      |
| Defense     | 25     | 50       |
| Speed       | 6      | 6        |
| CritChance  | 0.15   | 0.15     |
| PowerScore  | 254.0  | 508.0    |

**Design Notes:** Dragonborn Champion is the ultimate Warrior unit and represents the late-game payoff for a Warrior-focused build. At starred tier, a single Dragonborn Champion can anchor an entire front line. The moderate PowerScore growth from Tier 2 to Tier 3 is intentional — the true power comes from his passive HP regeneration (triggered at sub-50% HP) which PowerScore does not account for.

#### Void Sorcerer (Mage)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 160    | 320      |
| Attack      | 110    | 220      |
| Defense     | 8      | 16       |
| Speed       | 7      | 7        |
| CritChance  | 0.20   | 0.20     |
| PowerScore  | 261.0  | 522.0    |

**Design Notes:** Void Sorcerer has the highest attack value in the game. A starred Void Sorcerer paired with two other Mages can wipe an entire enemy back line in two turns. This is intentionally powerful — Tier 3 units should feel like late-game win conditions.

#### Fortress Titan (Tank)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 500    | 1000     |
| Attack      | 30     | 60       |
| Defense     | 40     | 80       |
| Speed       | 2      | 2        |
| CritChance  | 0.00   | 0.00     |
| PowerScore  | 228.0  | 456.0    |

**Design Notes:** Fortress Titan is the most defensively extreme unit in the game. A starred Titan with the Tank synergy shield active can absorb enormous amounts of damage. The extremely low speed (2) means he never attacks first, which is the key counterplay mechanic: burst-damage compositions can kill him before he survives long enough to be useful.

#### Phantom Sovereign (Assassin)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 180    | 360      |
| Attack      | 120    | 240      |
| Defense     | 12     | 24       |
| Speed       | 10     | 10       |
| CritChance  | 0.35   | 0.35     |
| PowerScore  | 279.0  | 558.0    |

**Design Notes:** Phantom Sovereign is the highest-PowerScore unit in the game and the strongest win condition for Assassin compositions. At speed 10 (maximum), he always acts first. With 35% crit chance and the Assassin synergy bleed effect active, a single crit from Phantom Sovereign can eliminate most non-Tank back-line units in one hit. He is deliberately expensive to obtain.

#### Storm Sentinel (Archer)

| Stat        | Base   | Starred  |
|-------------|--------|----------|
| HP          | 200    | 400      |
| Attack      | 85     | 170      |
| Defense     | 18     | 36       |
| Speed       | 8      | 8        |
| CritChance  | 0.20   | 0.20     |
| PowerScore  | 241.5  | 483.0    |

**Design Notes:** Storm Sentinel is the most defensively robust Archer in the game. Unlike Tier 1 and 2 Archers, he can survive a hit or two from melee units, making him a safer pick in compositions without a full front line. The Pierce bonus (from 4 Archers) on Storm Sentinel hits particularly hard given his high base attack.

---

## 4. Balance Review History

| Version | Change                                          | Reason                                                     |
|---------|-------------------------------------------------|------------------------------------------------------------|
| 1.0     | Initial stat distribution                       | Baseline set from design targets                           |
| 1.1     | Nightblade attack reduced from 80 to 75         | Starred Nightblade was one-shotting Tanks at 4 Assassins   |
| 1.1     | Shadow Cub crit increased from 0.15 to 0.20     | Tier 1 Assassin felt unrewarding; early-game crit too rare |
| 1.2     | Void Sorcerer HP increased from 130 to 160      | Too fragile; died before contributing splash damage        |
| 1.2     | Fortress Titan speed reduced from 3 to 2        | Titan was attacking before Assassins could jump            |

---

## 5. Known Balance Issues (Under Review)

1. **Archer + Mage dominance in rounds 5–7** — The combination of Hawk Ranger's Pierce and Arcane Scholar's splash creates a near-unbeatable damage output window if the player reaches level 6 before encountering a Tank-heavy composition.  
2. **Underuse of Pebble Shield** — Players tend to drop Pebble Shield as soon as Stone Guardian becomes available, even when a starred Pebble Shield would be statistically competitive. This may indicate that visual feedback for the upgrade is not sufficiently rewarding.  
3. **Phantom Sovereign snowball** — Once a player achieves a starred Phantom Sovereign, the combat outcome becomes highly deterministic. Further testing is needed to evaluate whether this is an acceptable late-game win condition or an unfun experience for the opposing player.  

---

*End of Unit Balance Specification v1.2*
