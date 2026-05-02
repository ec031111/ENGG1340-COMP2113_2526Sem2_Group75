# Player Guide — Auto-Battler Arena

**Document Type:** End-User Guide  
**Project:** Auto-Battler Arena  
**Audience:** New and intermediate players  
**Version:** 1.0  
**Last Updated:** 2026-04-25  

---

## Welcome to Auto-Battler Arena

Auto-Battler Arena is a terminal-based strategy game where you build a team of heroes, deploy them on a battlefield, and watch them fight automatically. Your job is to make smart decisions during the preparation phase — choosing which units to buy, which to upgrade, and where to position them. Combat takes care of itself.

This guide walks you through everything you need to know to play your first successful game.

---

## 1. Starting the Game

### 1.1 Launching

From the project directory, compile and run the game:

```bash
make
./game
```

You will be greeted by the main menu. Use the number keys to navigate:

```
=== AUTO-BATTLER ARENA ===
1. New Game
2. Tutorial
3. View Records
4. Exit
```

If this is your first time, we recommend option `2` to run the interactive tutorial first.

### 1.2 Starting a New Game

Select `1. New Game`. You will be asked for your name, which is saved alongside your results in the records file. After that, the game begins at Round 1.

---

## 2. The Game Screen

The game interface is divided into several sections displayed in the terminal:

```
╔══════════════════════════════════════════╗
║  ROUND 3    HP: 85    GOLD: 12           ║
╠══════════════════════════════════════════╣
║              YOUR BOARD                  ║
║  [Warrior]  [Tank]   [     ]  [     ]    ║
║  [     ]    [Mage]   [Archer] [     ]    ║
╠══════════════════════════════════════════╣
║                  BENCH                   ║
║  [Mage]  [Warrior]  [     ]  [     ]     ║
╠══════════════════════════════════════════╣
║                   SHOP                   ║
║  1.[Tank/$1] 2.[Mage/$1] 3.[Warrior/$2]  ║
║  4.[Archer/$2] 5.[Assassin/$3]           ║
╚══════════════════════════════════════════╝
```

**Status Bar** — Shows your current round, health points, and available gold.  
**Your Board** — The 2×4 grid where your deployed units fight. Row 1 is the frontline.  
**Bench** — Up to 9 units waiting to be deployed or sold.  
**Shop** — Five units available to purchase each round.  

---

## 3. The Planning Phase

The planning phase is when you make all your decisions. There is no timer — take as long as you need.

### 3.1 Buying Units

In the shop, each unit shows its role and gold cost. To buy a unit, press the corresponding number key (1–5).

- Purchased units go to your bench automatically.  
- If your bench is full (9 units), you cannot buy more. You must sell a unit first.  
- You cannot buy a unit if you don't have enough gold.  

### 3.2 Selling Units

To sell a unit from your bench, select it and choose the sell option. Every unit sells for **1 gold**, regardless of tier or how much you paid for it. Sell strategically: selling a unit you are close to upgrading (two copies on bench) is almost always a mistake.

### 3.3 Moving Units to the Board

Units on the bench are not participating in combat. You need to drag them to your board positions.

- Press `M` (Move) and select the unit, then select the target board position.  
- You can move units between bench and board at any time during the planning phase.  
- Only units on the board fight in combat. Units on the bench are safe.  

### 3.4 Repositioning Units on the Board

You can swap units between board positions the same way you move bench units to the board. Positioning matters — see Section 5 for guidance.

### 3.5 Re-rolling the Shop

Press `R` to re-roll the shop for **2 gold**. This replaces all five shop slots with new random units. Use re-rolls when:
- You are hunting for the third copy of a unit to complete an upgrade.  
- The current shop offers no units useful for your composition.  
- You have extra gold and have nothing else to spend it on.  

Do not re-roll every round early in the game — saving gold for interest income is often more valuable.

### 3.6 Buying Experience (Leveling Up)

Press `X` to buy 4 XP for **4 gold**. Accumulate enough XP to reach the next player level.

Higher player levels unlock better shop odds — more Tier 2 and Tier 3 units appear in the shop. Leveling up also increases the number of units you can deploy on the board (see Section 4).

### 3.7 Locking the Shop

Press `L` to lock the current shop. A locked shop does not refresh at the start of the next round — the same five units remain available. Use this when you see a unit you want but cannot afford it yet.

### 3.8 Ending the Planning Phase

Press `Enter` or `Space` when you are ready to proceed to combat. Make sure your board is arranged the way you want before confirming.

---

## 4. Unit Limits and Player Level

Your player level determines how many units you can have deployed on the board at once:

| Level | Max Units on Board |
|-------|--------------------|
| 1–2   | 2                  |
| 3–4   | 4                  |
| 5–6   | 6                  |
| 7+    | 8 (full board)     |

You can always have up to 9 units on the bench regardless of level. Exceeding the on-board limit simply means you have to choose which units to field each round.

---

## 5. Positioning Guide

Where you place your units affects the outcome of combat significantly. Follow these general rules and adjust based on your specific composition.

### 5.1 The Front Line (Row 1)

Row 1 engages melee enemies first. Always put your tankiest units here:

- **Tanks** should anchor the centre of Row 1. Their taunt ability draws enemy attacks.  
- **Warriors** should fill the remaining Row 1 slots. They are durable enough to withstand front-line pressure.  
- **Assassins** start in Row 1 but immediately jump to the enemy backline at combat start, so their Row 1 position is temporary.  

**Never leave Row 1 empty** if you have units available. An empty front line lets enemies route directly to your Mages and Archers.

### 5.2 The Back Line (Row 2)

Row 2 is safer and ideal for fragile, high-damage units:

- **Mages** belong in Row 2. They have very low HP and will die in one or two hits if reached by melee.  
- **Archers** gain an attack speed bonus when stationed in Row 2, making back-row placement also mechanically optimal.  

### 5.3 Column Positioning

Column placement within a row matters less than row placement, but a few tips apply:

- **Spread Mages** across Row 2 columns to maximize the area covered by their splash damage.  
- **Group Tanks** toward the centre columns to ensure their taunt covers a wider area.  
- **Avoid clustering Assassins** in adjacent columns — they all jump at the start of combat and do not benefit from being near each other.  

### 5.4 Adapting to Your Composition

As the game progresses and your composition evolves, revisit your positioning:

- If you switch from a Warrior-heavy to a Mage-heavy composition mid-game, move your Mages to Row 2 and use any remaining Warriors or a lone Tank as Row 1 placeholders.  
- If you run out of frontline units due to losses, consider temporary sacrificial positioning — place your weakest unit in Row 1 to absorb one or two hits before dying.  

---

## 6. The Combat Phase

Once you confirm your board, combat begins automatically. You watch your units fight without any input required.

### 6.1 Combat Order

Units act in order of their **Speed** stat, highest first. When two units have equal speed, the tiebreaker is randomised.

Each unit's action:
1. Select a target (based on role logic).  
2. Calculate damage: `damage = attack - target.defense` (minimum 1).  
3. Apply critical hit if the random roll beats crit chance.  
4. Apply any status effects (bleed, etc.).  
5. Check if the target is eliminated.  

### 6.2 Reading the Combat Log

During combat, the terminal displays a turn-by-turn log:

```
[Turn 3] Your Blademaster attacks Enemy Stone Guardian for 32 damage.
[Turn 3] Enemy Nightblade jumps to your backline and crits Arcane Scholar for 150 damage! (CRITICAL)
[Turn 3] Arcane Scholar is eliminated.
```

Read the log carefully to understand why you won or lost. Common loss patterns:
- Your Mages are being one-shot by Assassins → add a frontline unit in Row 2 or a Tank with high defense.  
- Your Warriors are dying before the enemy Mages → your Warriors need more defense (sell a Warrior, buy a Tank).  
- You win combat but barely → your opponent's next upgrade will be more significant unless you also upgrade.  

### 6.3 Combat Result

After one side is completely eliminated or the combat timer expires:

- **Win:** You deal no damage. Your surviving units are preserved for the next round.  
- **Loss:** You take damage equal to a base value (5) plus the number of enemy units that survived combat. If you are at 30 HP and lose to 4 surviving enemies, you drop to 30 − (5 + 4) = 21 HP.  

---

## 7. The Economy

Managing gold is just as important as managing your team composition.

### 7.1 Income Per Round

| Source          | Gold Gained | Condition                      |
|----------------|-------------|--------------------------------|
| Base income     | 5           | Every round                    |
| Interest        | 1 per 10g   | Capped at +5 (hold 50+ gold)   |
| Win streak      | +2          | Three+ consecutive wins        |
| Loss streak     | +2          | Three+ consecutive losses      |

### 7.2 Interest Income

If you hold **10 gold**, you earn **1 bonus gold** next round. If you hold **20 gold**, you earn **2 bonus gold**, and so on, up to a maximum of **5 bonus gold** for holding 50+ gold.

Interest makes a large difference over a full game. A player who consistently holds 50 gold earns 5 extra gold per round — that's 3–4 extra units worth of purchases in a 10-round game.

### 7.3 Early Economy vs. Early Power

The fundamental economic decision in Auto-Battler Arena:

**Save for interest (Economy Build):** Accept weaker units in early rounds. Hold 50 gold for maximum interest. Use the gold spike in rounds 5–8 to buy multiple Tier 2 and Tier 3 units simultaneously.

**Spend aggressively (Fast Power Build):** Spend all gold each round to maximise unit quality. Win early rounds and earn win-streak bonuses. Risk falling behind if early units are bad or you start losing.

Neither approach is universally correct. Read the shop each round and adjust your strategy based on what units are available.

---

## 8. Unit Upgrades

When you have **three copies of the same unit** across your board and bench combined, they automatically merge into a single **Starred** unit. The starred unit has approximately doubled stats and a distinct visual appearance.

### 8.1 Upgrade Priority

Prioritise upgrading units that are already part of your active synergy. Upgrading a Warrior when you already have two Warriors on the board amplifies the Warrior synergy bonus while also providing a stat boost.

### 8.2 Managing Upgrade Progress

Keep track of how many copies of each unit you have. Common mistake: buying a fourth copy of a unit when you already have three (the fourth is wasted — the three merged and the fourth is just one normal copy). Plan purchases to avoid overspending on units that are already upgraded.

---

## 9. Synergies

Synergies are bonuses that activate when you have enough units of the same role on the battlefield. A brief summary:

| Role     | 2 Units              | 4 Units                             |
|----------|----------------------|-------------------------------------|
| Warrior  | +15% attack          | +30% attack + HP regeneration       |
| Mage     | Splash damage        | Bigger splash + +20% attack         |
| Tank     | +20 defense          | +20 defense + combat-start shield   |
| Assassin | +10% crit chance     | +10% crit + bleed on crits          |
| Archer   | +15% attack speed    | +15% attack speed + Pierce          |

You do not need to manually activate synergies. They activate automatically based on how many qualifying units you have on the board when combat begins.

---

## 10. Events

At the end of certain rounds, an Event card is drawn and announced. Events modify the rules for the following round. Examples:

- **"All units gain +20 HP for the next combat."** — Benefit all units equally; plan accordingly.  
- **"Mages deal 25% less damage this round."** — If you run Mages, compensate by positioning them more aggressively or rely on synergy bonuses to offset.  
- **"The shop re-roll cost is halved."** — Re-roll more aggressively this planning phase to hunt for upgrades.  

Events are announced in the result phase. Read them carefully before entering the planning phase of the next round.

---

## 11. Records

After the game ends, your results are saved to `docs/records.txt`. The record includes:

- Player name  
- Date and time  
- Total rounds survived  
- Final team composition  
- Win/loss outcome  

You can view all records from the main menu by selecting `3. View Records`.

---

## 12. Quick Reference

**Planning Phase actions:**
| Key | Action            | Cost      |
|-----|-------------------|-----------|
| 1–5 | Buy shop unit     | Unit cost |
| R   | Re-roll shop      | 2 gold    |
| X   | Buy XP            | 4 gold    |
| L   | Lock/unlock shop  | Free      |
| M   | Move unit         | Free      |
| S   | Sell bench unit   | +1 gold   |
| Enter | Start combat   | —         |

**Good habits:**
- Always fill Row 1 before Row 2.  
- Check your synergy counts before confirming combat.  
- Keep at least 10 gold on hand for interest income.  
- Read the combat log after every loss.  
- Don't panic-sell units needed for upgrades.  

---

*Auto-Battler Arena — Player Guide v1.0*  
*Good luck, Commander. Aethelia is counting on you.*

---

## 13. Extended FAQ

### 13.1 Gameplay Questions

**Q1: Why did my unit not attack first even though it has high damage?**  
A1: Action order is based on Speed, not Attack. A high-attack unit with lower Speed can still act later.

**Q2: Does bench position matter for combat?**  
A2: No. Bench units do not participate in combat and their placement on bench slots has no battle impact.

**Q3: If I lock the shop, do the units stay forever?**  
A3: They stay until you unlock or reroll. If you keep lock on, the same shop appears next planning phase.

**Q4: Do event effects stack with synergy bonuses?**  
A4: Yes. Event modifiers and synergy modifiers are both applied for that round unless an event states otherwise.

**Q5: Why did I lose HP by more than expected?**  
A5: Loss damage includes base damage plus the number of enemy survivors.

**Q6: Can I get Tier 3 units early?**  
A6: Yes, but odds are very low before Level 6.

**Q7: Is it always correct to chase 4-unit synergy?**  
A7: Not always. If forcing 4-unit synergy weakens your board too much, a stronger 2-unit hybrid can be better.

**Q8: Should I sell a starred unit if off-composition?**  
A8: Usually no in mid-game; starred units often provide enough raw stats to remain useful during transition.

**Q9: Why do my Assassins sometimes fail to kill backline quickly?**  
A9: Low crit rolls, high-defense targets, or poor follow-up support can reduce Assassin burst.

**Q10: How many rerolls are safe each round?**  
A10: In stable economy rounds, 0-1 reroll is typical. In emergency stabilization, you may reroll more aggressively.

### 13.2 Economy Questions

**Q11: Is 50 gold always mandatory?**  
A11: No. 50 gold is an economic target, not a hard rule. Spend when your board is unstable.

**Q12: Why do I feel rich but still lose?**  
A12: Unspent gold has no combat value. Convert gold into board power before your HP becomes critical.

**Q13: Should I buy XP before or after rerolling?**  
A13: Usually before rerolling if level-up changes shop odds meaningfully.

**Q14: Is loss streak strategy valid?**  
A14: Yes, if controlled. You need enough HP buffer and a clear future spike plan.

**Q15: When should I break interest threshold?**  
A15: Break threshold if a purchase prevents major HP loss or secures a key upgrade.

### 13.3 Positioning Questions

**Q16: Where should I place one single Tank?**  
A16: Front row centre is usually best for coverage and taunt value.

**Q17: Should Archers always be back row?**  
A17: Usually yes. Front-row Archers lose survivability and role advantage.

**Q18: Is corner stacking good?**  
A18: Situational. Good against some melee patterns, bad against splash and jump threats.

**Q19: Why spread backline units?**  
A19: To reduce multi-target splash damage and avoid chain losses.

**Q20: Can I intentionally use bait units?**  
A20: Yes. One bait slot can protect your main carry for a full turn.

---

## 14. Troubleshooting and Stability Notes

### 14.1 Terminal Display Issues

If colors or symbols look incorrect:
1. Ensure terminal supports ANSI colors.  
2. Use a monospace font with Unicode coverage.  
3. Increase terminal width if board wrapping occurs.  
4. Avoid legacy shell modes that strip ANSI escape sequences.

If text appears misaligned:
1. Reset terminal zoom to default.  
2. Disable mixed-width fonts.  
3. Use full-screen terminal mode for stable layout.

### 14.2 Input Issues

If key input feels delayed:
1. Check terminal focus; click into terminal pane first.  
2. Avoid running heavy background tasks on the same shell.  
3. Do not paste multi-line key sequences accidentally.

If wrong command is triggered:
1. Press Escape to cancel current submenu when available.  
2. Re-enter the intended action from main planning options.  
3. Confirm selected unit index before final action.

### 14.3 Save/Record Issues

If records are not updating:
1. Confirm game ended naturally (win or game over).  
2. Ensure write permissions exist for records path.  
3. Verify file is not locked by another process/editor.

If records contain unexpected formatting:
1. Keep file in plain text mode.  
2. Avoid manual edits during active game session.  
3. Ensure line endings are consistent across edits.

---

## 15. New Player Training Plan (5 Sessions)

### Session 1: Basics Only

Focus goals:
1. Learn buy, sell, move, and start combat actions.  
2. Keep Row 1 filled every round.  
3. Avoid rerolls in first 3 rounds.

Success metric:
- Reach Round 6 without confusion about controls.

### Session 2: Economy Awareness

Focus goals:
1. Track interest thresholds (10, 20, 30, 40, 50).  
2. Keep at least 20 gold by Round 5 in one game.  
3. Avoid panic spending after one loss.

Success metric:
- Finish one game with positive gold management and stable board.

### Session 3: Positioning Practice

Focus goals:
1. Place Tanks center front.  
2. Spread backline carries.  
3. Use one bait unit vs Assassin-heavy fights.

Success metric:
- Reduce rounds lost due to instant backline collapse.

### Session 4: Composition Discipline

Focus goals:
1. Commit to one core composition by Round 5.  
2. Avoid buying off-plan units unless emergency frontline.  
3. Complete at least one starred upgrade.

Success metric:
- Reach Round 8 with one clear synergy plan.

### Session 5: Adaptive Play

Focus goals:
1. Read events and adjust one round ahead.  
2. Identify enemy threat type each round.  
3. Execute one pivot when current plan fails.

Success metric:
- Win at least one game or consistently reach late game.

---

## 16. Glossary for Quick Recall

**Carry:** Your main damage dealer unit.  
**Frontline:** Units in Row 1 that absorb first contact.  
**Backline:** Units in Row 2, usually ranged damage dealers.  
**Spike:** A sudden board power increase from key purchase/upgrade.  
**Pivot:** Changing composition direction mid-game.  
**Tempo:** Spending now for immediate round strength.  
**Greed:** Saving gold for later strength.  
**Stabilize:** Spend resources to stop HP bleeding and survive.

---

## 17. One-Page Quick Start

If you only remember ten rules, remember these:
1. Fill front row first.  
2. Protect your carry.  
3. Bench does not give synergy.  
4. Buy XP when odds improvement matters.  
5. Do not reroll mindlessly early.  
6. Spend when HP is in danger.  
7. Keep economy thresholds in mind.  
8. Upgrade completion is high priority.  
9. Adjust to events and enemy threats.  
10. Review losses, then change one thing at a time.
