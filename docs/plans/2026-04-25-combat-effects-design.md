# 战斗技能 ASCII 动画系统设计

## 目标

为 Auto-Battler Arena 的 5 种职业技能设计独特的终端动画，通过颜色 + 符号变化在战斗过程中自动播放，增强战斗的视觉冲击力和叙事感。

## 核心原则

- 回合开始 → 播放入场动画
- 每回合战斗 tick 中 → 技能触发时播放对应动画
- 回合结束 → 播放结算动画
- 可在设置中开关

---

## 1. 动画框架

每个回合的战斗流程中，动画嵌入点如下：

```
[ROUND N 开始]
  → 播放回合入场动画（所有存活单位闪烁边框）
  → 进入战斗 Tick 循环
    → 每 tick：单位移动 / 攻击判定
    → 技能触发时 → 播放对应技能动画
  → Tick 循环结束
  → 显示回合结算动画
[ROUND N 结束]
```

**动画显示时长：** 每个动画 0.5–1 秒（可配置），期间阻塞战斗日志输出，动画结束后继续。

**终端颜色方案：**
- 红色 (`\033[31m`) — 伤害、攻击
- 绿色 (`\033[32m`) — 治疗、增益
- 黄色 (`\033[33m`) — 暴击、特殊
- 蓝色 (`\033[34m`) — 法师技能
- 紫色 (`\033[35m`) — 刺客技能
- 青色 (`\033[36m`) — 坦克技能
- 白色/重置 (`\033[0m`) — 默认

---

## 2. 5 种职业技能动画设计

每种技能有独特的颜色、符号和动画模式：

| 职业 | 颜色 | 符号 | 动画描述 |
|------|------|------|---------|
| **Warrior (Rage)** | 红色 | `🔥` 或 `*` | 从攻击者向外扇形扩散，红色火焰符号向外扩散 3 帧 |
| **Mage (AOE)** | 蓝色 | `✨` 或 `@` | 圆形波纹从施法者向外扩散，蓝色星光向外扩散 4 帧 |
| **Tank (Block)** | 青色 | `▓` 或 `#` | 青色盾牌符号在坦克周围闪烁 3 次 |
| **Assassin (Backstab)** | 紫色 | `✦` 或 `>` | 紫色闪电从背后划过，短促闪烁 2 帧 |
| **Archer (Double Shot)** | 黄色 | `→` 或 `|` | 两道黄色箭矢从弓箭手射向两个目标 |

**动画示例（战士 Rage — 扇形扩散）：**
```
Frame 1:     *         (攻击者位置)
Frame 2:    * *        (向外扩散一步)
Frame 3:   * * *       (继续扩散到最大范围)
```

**技能触发时的输出格式：**
```
[ROUND 3] ⚔️ Warrior "Karl" activates RAGE! 🔥🔥🔥
         → Backline Mage takes 8 bonus damage!
```

---

## 3. 回合入场 & 结算动画

**回合入场动画：**
```
╔════════════════════════════╗
║  ⚔️ ROUND 3 — BATTLE PHASE  ⚔️
╚════════════════════════════╝

Player Units:            Enemy Units:
┌──────┐               ┌──────┐
│Warrior│               │ Mage │
│ HP:45 │               │ HP:30│
└──────┘               └──────┘
```
用颜色渐变（从暗到亮）打印存活单位，表示"他们准备战斗了"。

**回合结算动画：**

| 结果 | 动画 | 日志 |
|------|------|------|
| 玩家胜利 | 绿色 `✓` 闪烁 + "VICTORY!" | `You eliminated 2 enemies! Enemy takes 5 HP damage.` |
| 玩家失败 | 红色 `✗` 闪烁 + "DEFEAT!" | `Your 1 unit fell! You take 3 HP damage.` |
| 平局 | 黄色 `~` 闪烁 | `Both sides survived! No damage dealt.` |

**绝杀/翻盘特殊动画：**
```
★ ★ ★ ★ ★ ★ ★ ★ ★ ★
★  CONGRATULATIONS!  ★
★    ENEMY ELIMINATED ★
★ ★ ★ ★ ★ ★ ★ ★ ★ ★
```

---

## 4. 战斗 tick 动态标注

| 动作 | 符号 | 示例输出 |
|------|------|---------|
| 移动 | `🏃` | `Warrior moves toward enemy (2 steps)` |
| 攻击 | `⚔️` | `Warrior ⚔️ Archer — 12 damage dealt!` |
| 暴击 | `💥` | `Assassin 💥 Backstab crit! 24 damage!` |
| 技能触发 | `✨` | `Mage ✨ casts AOE! Hits 3 enemies for 10 each!` |
| 闪避 | `👻` | `Tank 👻 evades the attack!` |
| 击杀 | `☠️` | `Archer ☠️ Assassin slain!` |
| 治疗 | `💚` | `Tank 💚 heals for 5 HP!` |
| 护盾生效 | `🛡️` | `Tank 🛡️ Block absorbs 8 damage!` |

---

## 5. 技术实现

**新建文件：**
- `include/effects.h` — 动画函数声明
- `src/effects.cpp` — 动画实现

**核心函数：**
```cpp
void playEnterAnimation();          // 回合入场
void playExitAnimation(bool win);    // 回合结算
void playSkillEffect(ClassType t);  // 5种技能动画
void printTickAction(Unit* u, ActionType a, ...);  // tick 动态标注
```

**改动点汇总：**
| 文件 | 改动 |
|------|------|
| `include/effects.h` | 新增动画函数声明 |
| `src/effects.cpp` | 实现所有动画逻辑（~200行） |
| `src/game.cpp` | 战斗循环中嵌入动画调用 |
| `include/game.h` | 添加 effects.h 引用 |
| `src/settings.cpp` | 添加动画开关选项 |
