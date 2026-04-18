# Auto-Battler Arena - Game Design Document

**Project**: ENGG1340-COMP2113 (2525-2026 Semester 2)  
**Group**: 75  
**Platform**: C++ Terminal Game

---

## 📋 目录
1. [游戏理念](#游戏理念)
2. [核心游戏流程](#核心游戏流程)
3. [主要系统与类](#主要系统与类)
4. [重要函数详解](#重要函数详解)
5. [游戏机制](#游戏机制)
6. [数据结构](#数据结构)

---

## 🎮 游戏理念

### 项目概述
**Auto-Battler Arena** 是一个文本模式的自动象棋策略游戏，灵感来自自动象棋类游戏（如Auto Chess、Teamfight Tactics）。

### 核心设计原则

1. **自动战斗**：玩家不能控制战斗过程，只能在购物阶段配置队伍
2. **资源管理**：有限的金币每回合增加，需要明智地选择购买和升级
3. **运气与策略的结合**：商店有随机单位，但玩家可以通过策略进行购买和合并
4. **协同系统**：不同单位类型组合可以产生协同效果增强战力
5. **进阶机制**：相同单位可以合并升级为更强的星级版本
6. **难度选择**：Easy（AI随机购买）vs Hard（AI智能策略）

### 游戏目标
- 与AI对手轮流战斗，争取存活尽可能多的回合
- 通过赢得战斗获得更多金币和奖励
- 最终打败对手或成为排行榜顶端玩家

---

## 🔄 核心游戏流程

### 整体流程架构

```
┌─────────────────────┐
│   Main Menu         │  displayMainMenu()
├─────────────────────┤
│  1. New Game        │─── selectPlayerName() ──┐
│  2. Tutorial        │                         │
│  3. Leaderboard     │                    selectDifficulty()
│  4. Load Game       │                         │
│  5. Clear Data      │                         ▼
│  6. Quit            │                    Game::run()
└─────────────────────┘                        │
                                               ▼
                                    ┌──────────────────────┐
                                    │ Game Main Loop       │
                                    │ (Each Round):        │
                                    │ 1. Shop Phase        │
                                    │ 2. Deploy Phase      │
                                    │ 3. Battle Phase      │
                                    │ 4. Results Phase     │
                                    └──────────────────────┘
```

### 每个回合的详细流程

#### 1️⃣ **Shop Phase (购物阶段)** - `Game::shopPhase()`
- 玩家获得当回合的金币（基础 + 连胜/连败奖励 + 存钱利息）
- 显示商店中的5个随机单位
- 玩家可以：
  - `buy N`: 购买商店中的单位到卡组
  - `sell N`: 销售卡组中的单位获得金币
  - `refresh`: 花费金币重新生成商店单位
  - `place/remove`: 手动放置/移除卡组单位到战场
  - `info`: 查看单位详细信息
  - `ready`: 确认准备战斗

#### 2️⃣ **Deploy Phase (部署阶段)**
- 检查玩家的协同效果组合
- 应用协同加成到玩家单位属性
- 更新单位的攻击力、生命值等

#### 3️⃣ **Battle Phase (战斗阶段)** - `Game::battlePhase()`
- AI生成对应难度的对手队伍
- 双方单位在5x8的棋盘上相遇
- 执行战斗解算 - `Game::resolveCombat()`

#### 4️⃣ **Battle Resolution (战斗分辨)**
- 逐tick执行，每个tick中：
  - 所有存活单位按攻击范围寻找敌人
  - 执行攻击 - `Game::performAttack()`
  - 执行特殊能力 - `Game::performAbility()`
  - 清理死亡单位 - `Game::cleanupDeadUnits()`
- 继续直到一方全灭

#### 5️⃣ **Round Results (回合结算)**
- 如果玩家赢：获胜金币，连胜+1，连败重置为0
- 如果玩家输：受伤害，连败+1，连胜重置为0
- 检查玩家是否已死亡（HP ≤ 0）
- 保存游戏进度
- 进入下一回合

---

## 🏗️ 主要系统与类

### 类型系统概览

```
Auto-Battler Arena 系统架构
│
├─ Unit (单位)
│  ├─ WARRIOR (战士) - 高HP，近战，Rage能力
│  ├─ MAGE (法师) - 低HP，高ATK，AOE能力
│  ├─ TANK (坦克) - 超高HP，低ATK，Block能力
│  ├─ ASSASSIN (刺客) - 中HP，高ATK，高暴击，Backstab能力
│  └─ ARCHER (弓手) - 中HP，中ATK，远程，Double Shot能力
│
├─ Player (玩家)
│  ├─ HP (生命值)
│  ├─ Gold (金币)
│  ├─ Bench (卡组，最多8个单位)
│  ├─ WinStreak / LossStreak (连胜/连败)
│  └─ RoundsPlayed (已存活回合数)
│
├─ AI (对手AI)
│  ├─ Difficulty (难度)
│  ├─ Army (队伍)
│  ├─ buyEasyStrategy() - Easy模式购买策略
│  └─ buyHardStrategy() - Hard模式购买策略
│
├─ Shop (商店)
│  ├─ 5个随机单位库
│  ├─ refresh() - 重新生成单位
│  └─ buyUnit() - 购买单位
│
├─ Board (5x8棋盘)
│  ├─ Player方 (列0-3)
│  ├─ AI方 (列4-7)
│  └─ 单位管理和空间查询
│
├─ Synergy (协同系统)
│  ├─ calculateSynergies() - 计算协同加成
│  └─ displaySynergies() - 显示协同信息
│
├─ Event (随机事件)
│  ├─ 影响金币、生命值等
│  └─ 增加游戏变数
│
└─ Record (记录系统)
   ├─ 排行榜管理
   ├─ 游戏存档/读档
   └─ 数据持久化
```

---

## ⚙️ 重要函数详解

### 🎮 游戏主循环函数

#### 1. `Game::run(bool showIntro)`
**作用**：游戏主循环，协调所有游戏阶段  
**输入**：showIntro - 是否显示游戏介绍  
**输出**：0表示正常退出  
**逻辑流程**：
- 如果showIntro为true，显示故事介绍
- 进入主游戏循环，每个循环处理一个回合：
  1. 更新玩家金币和回合计数
  2. 执行商店阶段
  3. 应用协同加成
  4. 执行战斗阶段
  5. 处理战斗结果（胜负/伤害）
  6. 保存游戏进度
- 直到玩家死亡或主动退出

#### 2. `Game::shopPhase()`
**作用**：处理整个购物阶段的交互  
**输入**：无  
**输出**：无  
**用户命令**：
- `buy 1-5`：从商店购买单位
- `sell 1-N`：销售卡组中的单位
- `refresh`：花费金币重新生成商店
- `place 1 row col`：手动放置单位到棋盘
- `remove row col`：从棋盘移除单位回卡组
- `info N` 或 `info shop N`：显示单位详情
- `ready`：确认准备战斗

#### 3. `Game::battlePhase()`
**作用**：初始化并执行一场战斗  
**输入**：无  
**输出**：true=玩家胜利，false=玩家失败  
**流程**：
1. 清除棋盘上的AI单位
2. AI生成对应难度的队伍
3. AI放置单位到棋盘
4. 应用协同加成到两方单位
5. 调用resolveCombat()执行战斗
6. 收集战斗结果和伤害值
7. 存活的玩家单位恢复到满血并回到卡组

### ⚔️ 战斗系统函数

#### 4. `Game::resolveCombat(std::vector<Unit*>& deadUnits)`
**作用**：执行完整的自动化战斗，处理所有攻击轮次  
**输入**：deadUnits - 用于收集死亡单位的引用  
**输出**：true=玩家方存活，false=玩家方全灭  
**战斗流程**（每tick）：
- 获取所有存活单位
- 清除所有单位的Rage状态标记
- 对每个存活单位：
  1. 查找最近的敌人目标
  2. 执行攻击计算
  3. 执行特殊能力
  4. 显示战斗日志（如果不跳过）
- 检查是否有一方全灭
- 继续下一tick或结束战斗

#### 5. `Game::performAttack(Unit* attacker, Unit* defender)`
**作用**：计算一次攻击的伤害（包含暴击）  
**输入**：attacker - 发动攻击的单位，defender - 防守单位  
**输出**：实际造成的伤害值  
**伤害计算**：
1. 基础伤害 = 攻击者的ATK
2. 暴击检查：`rand() % 100 < 基础暴击率 + 暴击奖励`
   - 若暴击成功，伤害 × 1.5
3. TANK防守能力：25%概率阻挡40%伤害
4. 应用最终伤害到防守者

#### 6. `Game::performAbility(Unit* attacker, Unit* defender, std::vector<Unit*>& allUnits)`
**作用**：执行攻击者的特殊职业能力  
**输入**：attacker-发动者，defender-主目标，allUnits-所有单位列表  
**输出**：无（直接修改单位HP）  

**各职业能力**：
- **WARRIOR (Rage)**：当HP < 50%时触发一次，ATK提升30%
- **MAGE (AOE)**：30%概率造成50%溅伤到周围敌人
- **TANK (Block)**：25%概率阻挡40%伤害（已在performAttack中）
- **ASSASSIN (Backstab)**：25%概率造成50%额外伤害
- **ARCHER (Double Shot)**：20%概率额外攻击一次（可能暴击）

### 📊 单位与协同函数

#### 7. `Unit::takeDamage(int damage)`
**作用**：对单位造成伤害  
**输入**：damage - 伤害值  
**输出**：无  
**逻辑**：
- HP -= damage
- 如果HP < 0，则HP = 0
- 单位存活时发动反应能力

#### 8. `Synergy::calculateSynergies(const std::vector<Unit*>& units)`
**作用**：计算当前阵容中存在的所有协同组合  
**输入**：units - 玩家所有单位  
**输出**：SynergyBonus向量，包含所有激活的协同及加成  

**协同示例**：
- 3个WARRIOR → 所有单位ATK+20%
- 2个TANK → 所有单位HP+15%
- 3个MAGE → AOE伤害提升

#### 9. `Shop::refresh()`
**作用**：花费金币重新生成商店单位  
**输入**：无  
**输出**：无  
**成本**：
- 第1次重新生成：1金币
- 后续：2金币

### 🎯 玩家资源管理函数

#### 10. `Player::startNewRound()`
**作用**：计算并奖励本回合的金币  
**输入**：无  
**输出**：无  
**金币计算**：
- 基础金币：5（每回合）
- 连胜奖励：连胜 ≥ 3时，每场3金币
- 连败奖励：连败 ≥ 3时，每场2金币
- 利息：已存金币 ÷ 10（最多5金币）

#### 11. `Player::addToBench(Unit* unit)`
**作用**：将单位添加到玩家卡组  
**输入**：unit - 要添加的单位指针  
**输出**：true=成功，false=卡组已满  

#### 12. `Player::sellUnit(int index)`
**作用**：销售卡组中的单位获得金币  
**输入**：index - 卡组中的位置  
**输出**：true=成功，false=位置无效  
**销售价格**：单位购买成本 ÷ 2 × 星级

### 🎮 主菜单和用户交互函数

#### 13. `selectDifficulty()`
**作用**：让玩家选择游戏难度  
**输入**：无  
**输出**：Difficulty枚举 (EASY 或 HARD)  
**验证**：
- 只接受1或2的整数输入
- 非法输入提示重试
- 使用cin.clear()和cin.ignore()进行错误恢复

#### 14. `selectPlayerName()`
**作用**：获取玩家输入的名字  
**输入**：无  
**输出**：std::string - 玩家名字  
**默认值**：如果输入为空，使用"Player"

#### 15. `clearDataMenu()`
**作用**：提供清除游戏数据的菜单  
**输入**：无  
**输出**：无  
**功能**：
1. 删除排行榜 (records.txt)
2. 删除特定存档 (save1.dat/save2.dat/save3.dat)
3. 一键删除全部数据
4. 返回主菜单

### 📁 持久化和记录函数

#### 16. `Record::saveGame(...)`
**作用**：保存当前游戏状态到文件  
**输入**：player, board, shop, ai, currentPhase, currentEvent, slot  
**输出**：无  
**保存内容**：
- 玩家HP、金币、连胜/连败
- 棋盘上的所有单位信息
- 商店中的单位
- AI队伍信息
- 当前游戏阶段和事件

#### 17. `Record::loadGame(...)`
**作用**：从存档文件恢复游戏状态  
**输入**：所有需要恢复的游戏对象引用，slot编号  
**输出**：true=加载成功，false=加载失败  

#### 18. `Record::saveRecord(const Player&, const AI&)`
**作用**：将胜利记录添加到排行榜  
**输入**：player - 最终玩家状态，ai - 对手AI  
**输出**：无  
**记录格式**：PlayerName RoundsPlayed FinalGold Difficulty

#### 19. `Record::displayLeaderboard()`
**作用**：显示所有历史记录排序的排行榜  
**输入**：无  
**输出**：无（打印到console）  
**排序规则**：按存活回合数降序

---

## 🎮 游戏机制

### 🔀 单位协同系统

协同系统允许相同职业的单位组合产生增强效果：

| 协同 | 数量 | 效果 |
|------|------|------|
| Warrior | 3 | 所有单位ATK +20% |
| Mage | 3 | 所有单位AOE伤害 +30% |
| Tank | 2 | 所有单位HP +15% |
| Assassin | 3 | 所有单位暴击 +25% |
| Archer | 3 | 所有单位ATK range +1 |

### ⭐ 单位升级系统

- **星级1**：单个购买的单位（默认）
- **星级2**：3个相同的星级1单位合并而成
  - 属性提升：HP +50%, ATK +30%
- **星级3**：3个相同的星级2单位合并而成
  - 属性提升：HP +80%, ATK +50%

### 💰 经济系统

**每回合金币收入**：
```
总金币 = 基础(5) + 连胜奖励(0-3) + 连败奖励(0-2) + 利息(0-5)
```

### ❤️ 伤害系统

**战败伤害计算**：
```
伤害 = 基础伤害(5) + 敌方存活单位数 × 2
```

例：敌方剩3个单位 → 伤害 = 5 + 3×2 = 11 HP

### 🎲 随机事件

某些回合可能触发特殊事件：
- 金币改变事件
- HP改变事件
- 单位强化/弱化事件

---

## 📁 数据结构

### Unit 类

```cpp
class Unit {
private:
    std::string baseName_;              // 单位名称
    UnitClass unitClass_;               // 职业类型
    int hp_, maxHp_, baseMaxHp_;        // HP相关
    int atk_, baseAtk_;                 // 攻击力
    int cost_;                          // 购买成本
    int row_, col_;                     // 棋盘位置
    int critBonus_;                     // 暴击加成
    int attackRange_;                   // 攻击范围
    int starLevel_;                     // 星级（1-3）
    bool raged_;                        // 当前是否已发动Rage
    int atkBonus_, critBonusExtra_;     // 临时加成
    bool playerOwned_;                  // 是否为玩家单位
};
```

### Player 类

```cpp
class Player {
private:
    std::string name_;                  // 玩家名字
    int hp_;                            // 生命值
    int gold_;                          // 金币
    std::vector<Unit*> bench_;          // 卡组
    int winStreak_, lossStreak_;        // 连胜/连败
    int roundsPlayed_;                  // 已进行回合数
};
```

### Board 类

```cpp
class Board {
private:
    std::vector<std::vector<Unit*>> grid_; // 5x8棋盘
    // 玩家方：列0-3
    // AI方：列4-7
};
```

---

## 🚀 核心游戏循环伪代码

```cpp
void Game::run() {
    while (player_.isAlive()) {
        // 1. 开始新回合
        player_.startNewRound();        // 计算并奖励金币
        
        // 2. 商店阶段 - 玩家策略决定
        shopPhase();                    // 购买/销售单位，放置阵容
        
        // 3. 应用协同加成
        auto synergies = Synergy::calculateSynergies(playerUnits);
        for (auto unit : playerUnits) {
            applyBonus(unit, synergies);
        }
        
        // 4. 战斗阶段 - 自动执行
        bool playerWon = battlePhase(); // 初始化战斗并执行
        
        // 5. 处理结果
        if (playerWon) {
            handleWinRound();
        } else {
            handleLossRound();          // 计算伤害
        }
        
        // 6. 保存进度
        saveRecord();
    }
    
    // 游戏结束 - 显示最终成绩
    showGameOver();
}
```

---

## 📝 文件系统

```
docs/
├── records.txt          # 排行榜记录（追加模式）
├── save1.dat           # 游戏存档位置1
├── save2.dat           # 游戏存档位置2
└── save3.dat           # 游戏存档位置3
```

---

## 🎯 设计亮点

1. **模块化架构**：清晰的职责分离（Unit, Player, AI, Board等）
2. **自动化战斗**：简化的战斗系统但保留策略深度
3. **存档系统**：支持3个独立存档位置，随时保存进度
4. **难度平衡**：Easy AI随机，Hard AI有策略
5. **协同系统**：增加组队策略的深度
6. **星级升级**：鼓励玩家聚焦单位升级
7. **连胜连败**：动态的经济激励系统
8. **视觉反馈**：丰富的颜色和符号显示

---

**最后更新**: 2026-04-18  
**版本**: 1.0  
**制作组**: Group 75
