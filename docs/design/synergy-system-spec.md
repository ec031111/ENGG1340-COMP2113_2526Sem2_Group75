# Synergy System — Design and Implementation Specification

**Document Type:** Internal Design Reference  
**Project:** Auto-Battler Arena  
**Version:** 1.1  
**Last Updated:** 2026-04-25  

---

## 1. Overview

The Synergy System is the primary strategic layer of Auto-Battler Arena. It rewards players for fielding multiple units of the same role by granting collective bonuses that exceed what individual unit upgrades can provide. Understanding and exploiting synergies is the defining difference between a beginner and an intermediate player.

This document covers the intended design of each synergy, the mathematical reasoning behind bonus values, the implementation logic, and guidelines for future additions.

---

## 2. Design Goals

The Synergy System was designed with the following goals in mind:

1. **Encourage diverse compositions.** Every game session should feel different depending on which units the shop offers and which synergies the player chooses to pursue.  
2. **Make synergies legible at a glance.** The bonus provided by each synergy should be immediately intuitive: Warriors hit harder, Tanks get tougher, Assassins become more dangerous, and so on.  
3. **Avoid single dominant meta.** No single synergy combination should be so powerful that all other approaches are rendered nonviable. Balance testing must confirm that every synergy has at least one reliable counter.  
4. **Scale with investment.** A player who commits deeply to one synergy (4 units) should feel meaningfully more powerful than one who only partially activates it (2 units).  
5. **Interact meaningfully with positioning.** Synergy bonuses should amplify the value of correct positioning rather than making positioning irrelevant.  

---

## 3. Synergy Definitions

### 3.1 Warrior Synergy

**Theme:** Controlled aggression. Warriors are the backbone of any melee-focused composition and reward sustained fighting ability.

**Activation Thresholds:**
| Active Warriors | Bonus                                                        |
|-----------------|--------------------------------------------------------------|
| 2               | +15% attack to all Warriors on the battlefield               |
| 4               | +30% attack to all Warriors + passive HP regeneration (5 HP/turn) |

**Design Rationale:**  
The 2-unit bonus is intentionally modest: +15% attack is a noticeable improvement but not game-breaking. This is because Warriors are easy to field — most compositions include at least one. If the 2-unit bonus were too powerful, Warriors would appear in every team regardless of strategy.

The 4-unit bonus adds HP regeneration to compensate for Warriors' lack of range and mobility. Without regeneration, a full Warrior lineup faces the problem of being whittled down by Archers and Mages before reaching the enemy backline. Regeneration shifts this dynamic and rewards full commitment to the Warrior path.

**Counter-play:**  
High burst-damage Assassin compositions can kill Warriors before regeneration provides meaningful value. Mage splash damage hits the entire frontline simultaneously, punishing clustered Warrior formations.

---

### 3.2 Mage Synergy

**Theme:** Amplified magical output. Mages are individually powerful but fragile; the synergy system compensates by making multiple Mages dramatically more effective together than separately.

**Activation Thresholds:**
| Active Mages | Bonus                                                                  |
|--------------|------------------------------------------------------------------------|
| 2            | All Mage attacks gain splash damage (25% of attack to adjacent targets)|
| 4            | Splash radius doubles (2 adjacent targets each side) + +20% base attack|

**Design Rationale:**  
The splash mechanic is the most distinctive bonus in the game because it changes Mage behaviour qualitatively, not just quantitatively. A Mage without splash is a powerful single-target attacker. A Mage with splash reshapes entire combats by softening multiple enemies simultaneously.

At 4 Mages, the doubled splash radius means every Mage attack potentially hits up to five targets (main target + 2 on each side in a row of 4). Combined with the 20% attack bonus, this creates the highest total damage output of any synergy when fully activated against a wide enemy board.

**Counter-play:**  
Mages are the softest units in the game. A single Assassin jumping to the backline can eliminate an Arcane Scholar in one hit. Compositions that can rapidly reach the Mage backline hard-counter pure Mage builds. Additionally, the Tank synergy's frontline shield absorbs the splash damage that would otherwise soften the front row.

---

### 3.3 Tank Synergy

**Theme:** Immovable defence. Tanks serve as the foundation of melee-forward compositions and provide the breathing room that back-line units need to deal damage.

**Activation Thresholds:**
| Active Tanks | Bonus                                                                    |
|--------------|--------------------------------------------------------------------------|
| 2            | +20 flat defense to all Tanks on the battlefield                         |
| 4            | +20 flat defense (stacks with above) + combat-start shield equal to 30% of max HP |

**Design Rationale:**  
Flat defense reduction is the simplest and most readable Tank bonus. It is intentionally not a percentage reduction: percentage reductions compound with each other in complex ways, while flat reduction is easy for players to reason about.

The combat-start shield at 4 Tanks is a major differentiator. Against compositions that rely on burst-damage (Assassins), the shield absorbs the most dangerous hits at the start of combat. Against sustained-damage compositions (Warriors), it adds effectively an extra 30% HP to every Tank on the field. This bonus rewards full Tank commitment while remaining counterable by sufficiently high damage output.

**Counter-play:**  
4 Tanks have very low collective damage output. A full Tank composition cannot deal enough damage to win if the opponent fields even moderately aggressive units. The strategy of fielding 4 Tanks requires a mixed backline of Mages or Archers, which dilutes the Tank synergy count and creates a natural tension.

---

### 3.4 Assassin Synergy

**Theme:** Explosive burst damage with high risk. Assassins reward technical play — understanding the jump mechanic and positioning enemies who counter the jump is the skill ceiling of Assassin play.

**Activation Thresholds:**
| Active Assassins | Bonus                                                                |
|------------------|----------------------------------------------------------------------|
| 2                | +10% critical hit chance to all Assassins                           |
| 4                | +10% crit chance (stacks) + critical hits apply bleed (3 damage/turn for 3 turns) |

**Design Rationale:**  
Critical hit chance is the most appropriate Assassin bonus because it directly amplifies their already-high attack stat, creating occasional one-hit-kill moments that define the Assassin identity.

Bleed at 4 Assassins serves two functions. First, it provides reliable follow-through damage against high-HP targets (Tanks) that might survive an initial crit hit. Second, it creates a tension between Assassins and Tanks: even if a Fortress Titan absorbs the initial hit with his shield, bleed damage bypasses the shield and ticks down his HP over subsequent turns.

**Counter-play:**  
Assassins are fragile. If the enemy frontline contains many Warriors or Tanks, the Assassin jump lands the Assassin in the middle of a crowded melee where they can be focused quickly. High-defense compositions — particularly those with Stone Guardian or Fortress Titan — can survive the initial burst and destroy the Assassins before bleed finishes the job.

---

### 3.5 Archer Synergy

**Theme:** Sustained ranged pressure. Archers are the most balanced role — moderate stats across the board — and their synergy rewards patient positioning and consistent damage output.

**Activation Thresholds:**
| Active Archers | Bonus                                                              |
|----------------|--------------------------------------------------------------------|
| 2              | +15% attack speed to all Archers (they act more frequently)       |
| 4              | +15% attack speed (stacks) + attacks Pierce (each hit strikes both the target and the unit directly behind them) |

**Design Rationale:**  
Attack speed as the primary bonus reinforces the Archer identity as consistent damage dealers rather than burst threats. More frequent attacks mean more total damage over a long combat — the Archer advantage grows the longer the fight lasts.

Pierce at 4 Archers is the most unique mechanical bonus in the game. It allows Archers to effectively threaten two rows simultaneously, making it difficult for opponents to use a Tank to shield their Mages. The pierce target takes 50% of the original hit's damage, balanced to avoid making Pierce an instant win condition against melee-heavy compositions.

**Counter-play:**  
Archers depend on a long combat to maximize their output. High-burst Assassin compositions that eliminate the Archer backline quickly are the natural counter. Additionally, Archers have limited effectiveness against opponents who have spread their units across all board positions (reducing the number of pierce chains).

---

## 4. Multi-Synergy Interactions

When two or more synergies are active simultaneously, their bonuses stack additively unless otherwise noted. No special interaction bonuses (beyond those listed) exist by design — this keeps the system readable for new players.

### 4.1 Documented Pairings

**Warrior + Tank (Classic Frontline)**  
Activate 2-unit bonuses of both roles with 2 Warriors and 2 Tanks. This frees two backline slots for Archers or Mages. The Warriors gain attack, the Tanks gain defense, and the backline deals damage uninterrupted. Widely considered the most stable generalist composition.

**Assassin + Archer (Speed Composition)**  
Assassins jump to the enemy backline immediately while Archers deal sustained damage to the frontline. With both 2-unit synergies active, Assassins gain crit and Archers gain attack speed. The weakness is the complete absence of a frontline: if the enemy has Warriors or Tanks, they will route directly to the Archers. Best used against pure Mage or Archer opponents.

**Mage + Tank (Control Composition)**  
2 Mages in the backline and 2 Tanks in the frontline. Tanks absorb damage and taunt enemies while Mages splash down the entire enemy row. Against Warriors this is highly effective: splash damage hits all Warriors simultaneously regardless of where they are positioned. Against Assassins it struggles, as the jump bypasses the Tanks entirely.

---

## 5. Synergy Implementation Notes

The Synergy System is implemented in `src/synergy.cpp` and declared in `include/synergy.h`. The key function signatures are:

```
// Count how many units of a given role are currently active on the board
int countRoleOnBoard(const Board& board, UnitRole role);

// Apply all active synergy bonuses to units currently on the board
void applySynergies(Board& board);

// Remove all synergy bonuses (called at end of combat to reset state)
void removeSynergies(Board& board);
```

`applySynergies()` is called once at the start of each combat phase after units are placed but before any unit takes an action. The function iterates over all roles, counts active units, determines which bonus tier applies, and applies the corresponding modifiers to each qualifying unit's stats.

`removeSynergies()` reverses all applied modifiers by storing the original stats in a temporary snapshot before `applySynergies()` runs. This ensures that unit stats never drift between rounds due to accumulated bonuses.

---

## 6. Future Synergy Candidates

The following synergy concepts have been designed but not implemented in the current version:

**Paladin (Hybrid Role):** Units with the Paladin tag count as both Warrior and Tank for synergy purposes. At 2 active Paladins: all Paladin units heal adjacent allies for 5 HP each time they deal damage.

**Sorcerer Knight (Mage + Warrior Hybrid):** A unit archetype that deals melee damage but scales with Mage synergy count. Intended to bridge the Warrior/Mage divide and make mid-game transitions smoother.

**Commander (Support Role):** Non-damaging units that grant bonuses to adjacent allies. The Commander synergy (2 units) would let all Commander-adjacent units attack as if one synergy tier higher. This would be the most complex synergy in the game to implement and balance correctly.

---

*End of Synergy System Specification v1.1*
