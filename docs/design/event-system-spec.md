# Event System — Design and Implementation Specification

**Document Type:** Internal Design Reference  
**Project:** Auto-Battler Arena  
**Version:** 1.0  
**Last Updated:** 2026-04-25  

---

## 1. Overview

The Event System introduces round-specific modifiers that vary the game state and prevent dominant strategies from becoming unassailably safe. Events are drawn from a predefined pool, announced at the end of specific rounds, and take effect during the following round's combat or planning phase.

Events serve three design functions:
1. **Disrupt stable states** — Force players with strong compositions to adapt.  
2. **Create catch-up opportunities** — Some events benefit losing players by lowering costs or granting universal bonuses.  
3. **Add narrative flavour** — Each event is written in-world, connecting to the Aethelia lore.  

---

## 2. Event Timing

Events trigger at the end of specific rounds:

| Round | Event Trigger |
|-------|---------------|
| 3     | First event drawn and announced |
| 6     | Second event drawn and announced |
| 9     | Third event drawn and announced |

Events are announced during the Result Phase and take effect at the start of the next Planning Phase (for economy events) or the start of the next Combat Phase (for buff/debuff events).

An event lasts exactly one round unless its description specifies otherwise.

---

## 3. Event Categories

### 3.1 Buff Events

Buff events grant a bonus to all units regardless of role. They increase overall combat lethality and can accelerate close combats.

**Design Rule:** Buff events must benefit both the player and the AI equally. No buff event should be role-specific (that would be a debuff event for the opposing roles).

**Buff Event Pool:**

---

**E-B01: Blessing of the Solaris Crystal**  
*"Ancient crystals scattered across Aethelia's surface glow with renewed energy, empowering all combatants."*  
Effect: All units on both sides gain +20 attack for this combat.  
Design Note: This event shortens combats by increasing damage. Most beneficial in situations where one side has a slight HP advantage, as it accelerates the inevitable.

---

**E-B02: Adrenaline Surge**  
*"The warcry of the Allied Forces echoes across the battlefield, filling every warrior with renewed vigour."*  
Effect: All units gain +2 speed for this combat.  
Design Note: Speed bonuses change action order. Units near a speed threshold (e.g., a unit with Speed 4 acting after Speed 5 enemies) may shift into a new priority bracket. This can have unexpected cascade effects on combat order.

---

**E-B03: Sacred Ground**  
*"The battlefield was consecrated by the Aethelian priesthood. Those who fight upon it are touched by divine protection."*  
Effect: All units gain +15 HP at the start of combat (does not exceed original maximum HP).  
Design Note: Flat HP bonuses benefit all units equally regardless of role. This is the most "neutral" buff event — it rarely changes the outcome of a combat, but it can allow a unit that would have barely died to survive one more turn.

---

**E-B04: Armory Cache**  
*"An abandoned Alliance armory was discovered, its weapons freely distributed among all fighters."*  
Effect: All units gain +10 defense for this combat.  
Design Note: Defense bonuses extend combats. Most effective in situations where both sides have moderate attack values; less impactful in high-attack late-game scenarios.

---

**E-B05: Fortune's Wind**  
*"An unpredictable arcane wind sweeps the arena, aligning the stars in favour of the bold."*  
Effect: All units' critical hit chance increases by +0.10 for this combat.  
Design Note: The most volatile buff event. High-crit compositions (Assassins) gain disproportionately more value. This event is intentionally slightly unfair — it introduces variance and can dramatically swing a close fight.

---

### 3.2 Debuff Events

Debuff events apply a penalty targeting a specific role. They are the most strategically disruptive events and require players to adapt their plans.

**Design Rule:** Debuff events are always role-specific, never universal. This ensures at least one side (the one not running that role) gains a relative advantage.

**Debuff Event Pool:**

---

**E-D01: Malachar's Curse**  
*"The Dark Lord has extended his corrupting reach. Magical energies are suppressed across the battlefield."*  
Effect: All Mages on both sides deal 25% less attack damage this combat.  
Design Note: The most impactful debuff for Mage-heavy compositions. A player running 4 Mages loses approximately 25% of their total damage output for the round. The event represents the strongest incentive to diversify roles.

---

**E-D02: Iron Fog**  
*"A thick metallic mist rises from the canyon depths, slowing heavy armour and weighing down the strongest warriors."*  
Effect: All Warriors on both sides lose -2 speed for this combat.  
Design Note: Speed reduction moves Warriors lower in the action order, meaning they attack later and some may be eliminated before they get to act. This event is most impactful when Warriors have speed values close to other role thresholds.

---

**E-D03: Spirit Severance**  
*"Dark rituals have severed the spiritual contracts of shadow walkers. Their strikes lose their lethal edge."*  
Effect: All Assassins on both sides have their critical hit chance reduced by 0.15 (minimum 0) for this combat.  
Design Note: Reducing Assassin crit chance is the most targeted debuff for Assassin compositions. Without crits, Assassins are simply fast attackers with average damage — still useful but not the burst threats they are intended to be.

---

**E-D04: Heavy Shields**  
*"Commanders have ordered defensive formations across all units, slowing offensive manoeuvres."*  
Effect: All Tanks on both sides lose -5 attack for this combat.  
Design Note: Tanks have low attack anyway; this debuff has minimal direct impact on combat outcomes. Its primary effect is psychological — it signals to players not to rely on Tanks for damage output, which is correct regardless.

---

**E-D05: Arrow Scatter**  
*"Magical interference disrupts ranged ballistics. Arrows and bolts fly unpredictably."*  
Effect: All Archers on both sides have a 20% chance of missing each attack entirely (dealing 0 damage) this combat.  
Design Note: The miss mechanic is the only source of RNG in combat beyond critical hits. This event is intentionally RNG-heavy — some combats, all Archer shots hit; in others, a run of misses completely negates the Archer contribution. The intent is to create memorable moments rather than a precise balance adjustment.

---

### 3.3 Economy Events

Economy events modify gold income rules for one planning phase. They are always announced before the start of that planning phase.

**Design Rule:** Economy events should never benefit one player exclusively. All income changes apply universally.

**Economy Event Pool:**

---

**E-EC01: Market Boom**  
*"Trade caravans from the Merchant Guilds of Thornveil have arrived, flooding the market with excess stock."*  
Effect: The shop re-roll cost is reduced to 1 gold (from 2) for this planning phase.  
Design Note: Encourages aggressive re-rolling. Players who are one copy away from a critical upgrade benefit most. This event commonly triggers composition changes as players hunt for specific units at half price.

---

**E-EC02: War Tax**  
*"The Alliance Council has imposed an emergency war tax to fund the campaign against Malachar."*  
Effect: Base round income is reduced from 5 to 3 gold for this round.  
Design Note: A 2 gold reduction per round is significant. Players who planned to buy a specific unit may find themselves 1–2 gold short. This event creates meaningful scarcity and forces prioritisation decisions.

---

**E-EC03: XP Scholarship**  
*"The Scholars of Aethelia's Academy offer free training to any who seek it."*  
Effect: Buying XP costs 2 gold instead of 4 for this planning phase.  
Design Note: Players who are one level threshold away (e.g., needing 2 XP to reach Level 5 for better shop odds) gain significant value. This event rewards players who have been managing their XP purchases efficiently.

---

**E-EC04: Windfall**  
*"Forgotten Alliance gold reserves have been discovered in a hidden vault beneath the arena."*  
Effect: All players receive 3 bonus gold at the start of this planning phase.  
Design Note: A universal gold injection. The player who benefits most is the one who was closest to affording a key purchase (e.g., 1 gold short of a Tier 3 unit). Windfall events often produce memorable "just barely made it" moments.

---

### 3.4 Chaos Events

Chaos events introduce effects that are unpredictable and outside standard game mechanics. They are drawn from a smaller pool and appear less frequently than other event types.

**Design Rule:** Chaos events must always have a plausible upside for any player. No chaos event should be a guaranteed loss for any specific composition.

**Chaos Event Pool:**

---

**E-CH01: Dimensional Rift**  
*"The battlefield tears open as Malachar attempts to collapse space itself. Units are flung across the arena."*  
Effect: Two random bench units from each player are swapped onto the battlefield (displacing two current board units to the bench).  
Design Note: The most disruptive event in the pool. Randomising two of your eight board positions eliminates precise positioning plans. This event most severely impacts compositions that depend on specific Row assignments (e.g., Assassins forced to Row 2, Mages forced to Row 1).

---

**E-CH02: Mirror Match**  
*"A shadow dimension creates spectral copies of every unit. For one round, all units fight as their reflection."*  
Effect: All units' attack and defense values are swapped for this combat (e.g., a unit with 80 attack and 10 defense becomes 10 attack and 80 defense).  
Design Note: This event completely inverts the power dynamic. Tanks become offensive powerhouses; Mages become nearly invulnerable but deal negligible damage. Players who understand this event in advance can pre-position their "new attackers" (Tanks) in the front line for maximum effectiveness.

---

**E-CH03: Wild Shop**  
*"A traveling dimensional merchant appears, selling rarities that defy explanation."*  
Effect: The shop for this planning phase contains only Tier 3 units, all at 2 gold cost.  
Design Note: The most universally positive chaos event. Every player can afford at least two Tier 3 units this round. This creates dramatic power spikes and often causes the most significant composition pivots of the entire game.

---

## 4. Event Selection Algorithm

Events are selected from the pool using weighted random selection:

| Category | Probability |
|----------|-------------|
| Buff     | 30%         |
| Debuff   | 35%         |
| Economy  | 25%         |
| Chaos    | 10%         |

Within each category, all events are equally weighted. The same event cannot be drawn twice in the same game — a flag is set when an event is drawn, and it is excluded from subsequent draws.

Debuff events are weighted slightly higher to ensure strategic disruption occurs reliably. Pure buff events are less exciting from a design perspective and are intentionally underrepresented.

---

## 5. Implementation Notes

The Event System is implemented in `src/event.cpp`, declared in `include/event.h`. Key structures:

```cpp
enum class EventType { Buff, Debuff, Economy, Chaos };

struct Event {
    int id;
    EventType type;
    std::string name;
    std::string flavourText;
    std::string description;
};

// Draw the next event from the pool
Event drawEvent(std::vector<int>& usedEventIds);

// Apply an event's effects to the game state
void applyEvent(const Event& event, Game& game);

// Remove event effects at end of round
void removeEvent(const Event& event, Game& game);
```

Event effects that modify unit stats (buff and debuff events) are stored temporarily in a delta structure on each unit. `removeEvent()` reverses the delta, ensuring stats return to their base values after the round.

Economy events modify the `Game::goldModifiers` map, which is applied when computing income during the next Economy Phase. The map is cleared at the end of that phase.

---

*End of Event System Specification v1.0*
