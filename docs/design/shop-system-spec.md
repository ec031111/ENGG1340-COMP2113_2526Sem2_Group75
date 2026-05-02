# Shop System — Design and Implementation Specification

**Document Type:** Internal Design Reference  
**Project:** Auto-Battler Arena  
**Version:** 1.0  
**Last Updated:** 2026-04-25  

---

## 1. Overview

The Shop System is the primary interface between the player and the unit acquisition pool. Each planning phase, the shop generates a randomised selection of five units drawn from the global unit pool. Players spend gold to acquire units, re-roll the shop, or buy experience points to increase their player level. The shop is the central engine of Auto-Battler Arena's economy and is the mechanism through which all strategic decisions are expressed in gold.

---

## 2. Unit Pool

The unit pool is a finite collection of all available unit copies. Each unit type has exactly 15 copies in the pool (shared across all players and the AI). When a copy is purchased, it is removed from the pool. When a unit is sold, the copy returns to the pool.

This pool-sharing mechanic means that if multiple players are pursuing the same composition, specific units become harder to find in the shop. The pool scarcity is the primary mechanism that prevents all players from running identical compositions.

**Pool sizes by unit type:**

| Tier | Copies per Unit Type |
|------|--------------------|
| 1    | 15                 |
| 2    | 12                 |
| 3    | 9                  |

Higher-tier units are rarer in the pool. This is compounded by the fact that fewer of them appear in the shop at low player levels (see Section 4).

---

## 3. Shop Slot Generation

At the start of each planning phase, the five shop slots are filled by:
1. Rolling a tier (1, 2, or 3) according to the player-level probability table.  
2. Selecting a random unit of that tier from the pool (weighted equally among available units of that tier).  
3. Repeating until all five slots are filled.  

If a tier has no available units in the pool (all copies have been purchased), the selection falls back to the next available tier.

---

## 4. Level-Based Shop Odds

| Player Level | Tier 1 Probability | Tier 2 Probability | Tier 3 Probability |
|-------------|-------------------|-------------------|-------------------|
| 1           | 100%              | 0%                | 0%                |
| 2           | 80%               | 20%               | 0%                |
| 3           | 70%               | 25%               | 5%                |
| 4           | 60%               | 30%               | 10%               |
| 5           | 50%               | 35%               | 15%               |
| 6           | 35%               | 40%               | 25%               |
| 7           | 20%               | 40%               | 40%               |
| 8           | 10%               | 35%               | 55%               |

At Level 1, only Tier 1 units appear. From Level 3 onward, Tier 3 units have a small but nonzero chance. At Level 8, Tier 3 units are the most common. Leveling up quickly is the fastest path to accessing late-game win conditions.

---

## 5. Shop Actions

### 5.1 Purchase

The player pays the unit's gold cost and the unit is moved to their bench. The unit copy is removed from the pool. If the bench is at capacity (9 units), purchasing is blocked until bench space is freed.

### 5.2 Re-Roll

Costs 2 gold. All current shop slots are returned to the pool and five new slots are generated. Units that were in the shop but not purchased are returned to the pool before the new draw. The re-roll is immediate — the player cannot "undo" a re-roll.

### 5.3 Buy Experience

Costs 4 gold. Adds 4 XP to the player's XP total. When the XP threshold for the next level is reached, the player levels up and their shop odds improve immediately. XP thresholds are cumulative and increase with each level.

| Level Up To | XP Required |
|-------------|-------------|
| Level 2     | 2 XP        |
| Level 3     | 6 XP        |
| Level 4     | 12 XP       |
| Level 5     | 20 XP       |
| Level 6     | 32 XP       |
| Level 7     | 48 XP       |
| Level 8     | 68 XP       |

### 5.4 Shop Lock

The player may lock the current shop at no cost. A locked shop retains its five units at the start of the next round (they are not returned to the pool when the round transitions). The lock persists until the player manually unlocks the shop or re-rolls. This is primarily useful when the player cannot afford a desired unit this round but expects to next round.

---

## 6. Auto-Combine on Purchase

When a player purchases a unit and already owns two copies of the same unit (across bench and board combined), the three copies automatically combine into a starred (upgraded) unit. The combine:
- Removes the three copies from the board and bench.  
- Places a single starred unit on the bench.  
- The two pool copies (from the sold non-shop copies) return to the pool; the shop copy is consumed.  

Auto-combine does not trigger if only two copies exist across all locations — it requires exactly three before triggering.

---

## 7. Implementation Notes

The Shop is implemented in `src/shop.cpp`, declared in `include/shop.h`.

Key data structures:

```cpp
struct ShopSlot {
    Unit unit;
    bool available;  // false if already purchased this phase
};

class Shop {
    std::vector<ShopSlot> slots;   // 5 slots
    std::vector<Unit>& unitPool;   // shared reference to global pool
    int rerollCost;                // default 2, modified by events
    
public:
    void generateShop(int playerLevel);
    bool purchaseUnit(int slotIndex, Player& player);
    void reroll(Player& player);
    void lock();
    void unlock();
};
```

The `unitPool` is passed by reference to ensure all Shop instances (player and AI) draw from the same shared pool, correctly simulating pool scarcity.

---

*End of Shop System Specification v1.0*
