# 标准化注释格式改进方案 - 完整报告

**生成日期**: 2026-04-18  
**总修改函数数**: 51  
**标准格式**: 
```cpp
// What: [函数类型]
// Purpose: [功能描述]
// Input: [参数名、类型和含义]
// Output: [返回值类型和含义]
```

---

## 📋 按文件分类的修改总表

### 文件分布统计
| 文件 | 函数数 | 优先级 |
|------|-------|--------|
| board.cpp | 12 | P1 |
| ai.cpp | 4 | P1 |
| player.cpp | 9 | P2 |
| unit.cpp | 12 | P3 |
| shop.cpp | 6 | P4 |
| event.cpp | 2 | P5 |
| record.cpp | 2 | P5 |
| tutorial.cpp | 4 | P6 |
| main.cpp | 4 | P7 |
| **合计** | **55** | |

---

## 🔄 逐文件详细修改方案（含oldString/newString配对）

### 📁 优先级1：board.cpp（12个函数）

#### 1️⃣ Constructor - Board::Board()
**位置**: src/board.cpp L44-48

**oldString**:
```cpp
Board::Board() {
    // -----------------------------------------------------------------
    // Constructor
    // What it does : initialises a BOARD_ROWS x BOARD_COLS grid of nullptrs.
    // Input  : none
    // Output : a Board ready for unit placement
    // -----------------------------------------------------------------
```

**newString**:
```cpp
Board::Board() {
    // What: Constructor
    // Purpose: Initialize a BOARD_ROWS x BOARD_COLS grid of nullptrs
    // Input: None
    // Output: Initialized Board object
```

---

#### 2️⃣ placeUnit - Board::placeUnit()
**位置**: src/board.cpp L55-65

**oldString**:
```cpp
bool Board::placeUnit(Unit* unit, int row, int col) {
    // -----------------------------------------------------------------
    // placeUnit
    // What it does : puts a unit on the grid if the cell is valid & empty.
    // Inputs : unit, row, col
    // Output : true on success, false otherwise
    // -----------------------------------------------------------------
```

**newString**:
```cpp
bool Board::placeUnit(Unit* unit, int row, int col) {
    // What: Core placement logic
    // Purpose: Place a unit on the grid if the cell is valid and empty
    // Input: unit (Unit*), row (int), col (int)
    // Output: bool - true if successful, false otherwise
```

---

#### 3️⃣ removeUnit - Board::removeUnit()
**位置**: src/board.cpp L69-77

**oldString**:
```cpp
void Board::removeUnit(int row, int col) {
    // -----------------------------------------------------------------
    // removeUnit
    // What it does : clears the cell at (row, col).
    // Inputs : row, col
    // Output : none
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Board::removeUnit(int row, int col) {
    // What: Removal operation
    // Purpose: Clear the cell at (row, col) by removing the unit
    // Input: row (int), col (int)
    // Output: None (modifies board state)
```

---

#### 4️⃣ getUnit - Board::getUnit()
**位置**: src/board.cpp L81-90

**oldString**:
```cpp
Unit* Board::getUnit(int row, int col) const {
    // -----------------------------------------------------------------
    // getUnit
    // What it does : returns the unit pointer at (row, col).
    // Inputs : row, col
    // Output : Unit* or nullptr
    // -----------------------------------------------------------------
```

**newString**:
```cpp
Unit* Board::getUnit(int row, int col) const {
    // What: Getter function
    // Purpose: Retrieve the unit pointer at the specified grid position
    // Input: row (int), col (int)
    // Output: Unit* - pointer if present, nullptr otherwise
```

---

#### 5️⃣ isValidPosition - Board::isValidPosition()
**位置**: src/board.cpp L92-100

**oldString**:
```cpp
bool Board::isValidPosition(int row, int col) const {
    // -----------------------------------------------------------------
    // isValidPosition
    // What it does : bounds-checks (row, col).
    // Inputs : row, col
    // Output : true if within grid
    // -----------------------------------------------------------------
```

**newString**:
```cpp
bool Board::isValidPosition(int row, int col) const {
    // What: Validation function
    // Purpose: Verify if the given position is within valid grid bounds
    // Input: row (int), col (int)
    // Output: bool - true if within bounds, false otherwise
```

---

#### 6️⃣ isEmpty - Board::isEmpty()
**位置**: src/board.cpp L102-111

**oldString**:
```cpp
bool Board::isEmpty(int row, int col) const {
    // -----------------------------------------------------------------
    // isEmpty
    // What it does : checks whether a cell has no unit.
    // Inputs : row, col
    // Output : true if cell is empty or out of bounds
    // -----------------------------------------------------------------
```

**newString**:
```cpp
bool Board::isEmpty(int row, int col) const {
    // What: Validation function
    // Purpose: Check if a cell is empty (no unit present)
    // Input: row (int), col (int)
    // Output: bool - true if cell is empty or out of bounds, false if occupied
```

---

#### 7️⃣ clear - Board::clear()
**位置**: src/board.cpp L113-121

**oldString**:
```cpp
void Board::clear() {
    // -----------------------------------------------------------------
    // clear
    // What it does : resets every cell to nullptr.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Board::clear() {
    // What: Reset operation
    // Purpose: Reset all grid cells to empty state
    // Input: None
    // Output: None (modifies board state)
```

---

#### 8️⃣ display - Board::display()
**位置**: src/board.cpp L127-159

**oldString**:
```cpp
void Board::display() const {
    // -----------------------------------------------------------------
    // display
    // What it does : prints the grid to stdout with a dividing bar between
    //                player (cols 0-3) and AI (cols 4-7) territory.
    //                Uses the unit's symbol or '.' for empty cells.
    // Input  : none
    // Output : none (prints to stdout)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Board::display() const {
    // What: Display function
    // Purpose: Display the full game board with player and AI armies
    // Input: None
    // Output: None (prints to stdout with ANSI colors and dividing bar)
```

---

#### 9️⃣ displayPlayerSide - Board::displayPlayerSide()
**位置**: src/board.cpp L160-190

**oldString**:
```cpp
void Board::displayPlayerSide() const {
    // -----------------------------------------------------------------
    // displayPlayerSide
    // What it does : prints only the player's half of the board (cols 0-3)
    //                so the player can see their formation while deploying.
    // Input  : none
    // Output : none (prints to stdout)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Board::displayPlayerSide() const {
    // What: Display function
    // Purpose: Display only the player's side of the board during deployment
    // Input: None
    // Output: None (prints player formation to stdout)
```

---

#### 🔟 findNearestEnemy - Board::findNearestEnemy()
**位置**: src/board.cpp L191-219

**oldString**:
```cpp
Unit* Board::findNearestEnemy(const Unit* unit) const {
    // -----------------------------------------------------------------
    // findNearestEnemy
    // What it does : iterates over the grid to find the closest enemy unit
    //                (by Manhattan distance). "Enemy" = different ownership.
    // Input  : unit - the unit searching for a target
    // Output : pointer to the nearest enemy, or nullptr if none exist
    // -----------------------------------------------------------------
```

**newString**:
```cpp
Unit* Board::findNearestEnemy(const Unit* unit) const {
    // What: Core combat logic
    // Purpose: Find the closest enemy unit using Manhattan distance
    // Input: unit (const Unit*) - unit searching for a target
    // Output: Unit* - pointer to nearest living enemy, or nullptr if none exist
```

---

#### 1️⃣1️⃣ moveToward - Board::moveToward()
**位置**: src/board.cpp L220-256

**oldString**:
```cpp
void Board::moveToward(Unit* unit, int targetRow, int targetCol) {
    // -----------------------------------------------------------------
    // moveToward
    // What it does : moves a unit one cell closer to (targetRow, targetCol).
    //                Prefers row movement; falls back to column movement
    //                if row movement is blocked.
    // Inputs : unit, targetRow, targetCol
    // Output : none (modifies grid and unit position)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Board::moveToward(Unit* unit, int targetRow, int targetCol) {
    // What: Core movement logic
    // Purpose: Move a unit one cell closer to target, preferring row then column
    // Input: unit (Unit*), targetRow (int), targetCol (int)
    // Output: None (modifies grid state and unit position)
```

---

#### 1️⃣2️⃣ getPlayerUnits / getAIUnits / getAllUnits
**位置**: src/board.cpp L257-287

**oldString (getPlayerUnits)**:
```cpp
std::vector<Unit*> Board::getPlayerUnits() const {
    // -----------------------------------------------------------------
    // getPlayerUnits / getAIUnits / getAllUnits
    // What it does : collects unit pointers from the grid by ownership.
    // Input  : none
    // Output : vector of Unit*
    // -----------------------------------------------------------------
```

**newString (getPlayerUnits)**:
```cpp
std::vector<Unit*> Board::getPlayerUnits() const {
    // What: Getter function
    // Purpose: Retrieve all player-owned units currently on the board
    // Input: None
    // Output: vector<Unit*> - all player units on board
```

**oldString (getAIUnits)** - (same header as above)

**newString (getAIUnits)**:
```cpp
std::vector<Unit*> Board::getAIUnits() const {
    // What: Getter function
    // Purpose: Retrieve all AI-owned units currently on the board
    // Input: None
    // Output: vector<Unit*> - all AI units on board
```

**oldString (getAllUnits)** - (same header as above)

**newString (getAllUnits)**:
```cpp
std::vector<Unit*> Board::getAllUnits() const {
    // What: Getter function
    // Purpose: Retrieve all units currently on the board
    // Input: None
    // Output: vector<Unit*> - all Unit pointers on board
```

---

### 📁 优先级1：ai.cpp（4个函数）

#### 1️⃣ buyEasyStrategy - AI::buyEasyStrategy()
**位置**: src/ai.cpp L65-98

**oldString**:
```cpp
void AI::buyEasyStrategy(Shop& shop) {
    // -----------------------------------------------------------------
    // buyEasyStrategy - Purchase units using simple cheapest-first approach
    // Description: Repeatedly refreshes shop and buys cheapest affordable
    //              units until gold runs out or max army size reached.
    //              Simple strategy = easier difficulty for player.
    // Parameters: shop - Temporary shop to purchase from
    // Process: 1. Refresh shop up to 5 times
    //          2. Find cheapest affordable unit each iteration
    //          3. Buy and add to army_ until gold runs out
    // Purpose: Implement EASY difficulty AI purchasing behavior
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void AI::buyEasyStrategy(Shop& shop) {
    // What: Core AI strategy
    // Purpose: Implement EASY difficulty AI unit purchasing (cheapest-first strategy)
    // Input: shop (Shop&) - temporary shop reference to purchase units from
    // Output: None (populates army_ vector with purchased units)
```

---

#### 2️⃣ buyHardStrategy - AI::buyHardStrategy()
**位置**: src/ai.cpp L109-155

**oldString**:
```cpp
void AI::buyHardStrategy(Shop& shop) {
    // -----------------------------------------------------------------
    // buyHardStrategy - Purchase units using score-based optimization
    // Description: Evaluates each unit with formula: ATK*2 + HP/5.
    //              Buys highest-scoring affordable units each iteration.
    //              Smarter strategy = harder difficulty for player.
    // Parameters: shop - Temporary shop to purchase from
    // Process: 1. Refresh shop up to 5 times
    //          2. Score all affordable units (attack priority)
    //          3. Buy highest-scoring unit each iteration
    // Purpose: Implement HARD difficulty AI purchasing behavior (optimized team)
    // -----------------------------------------------------------------

    // Input  : shop
    // Output : none (adds units to army_)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void AI::buyHardStrategy(Shop& shop) {
    // What: Core AI strategy
    // Purpose: Implement HARD difficulty AI unit purchasing (score-optimized strategy)
    // Input: shop (Shop&) - temporary shop reference to purchase units from
    // Output: None (populates army_ vector with high-value units)
```

---

#### 3️⃣ placeEasy - AI::placeEasy()
**位置**: src/ai.cpp L166-193

**oldString**:
```cpp
void AI::placeEasy(Board& board) {
    // -----------------------------------------------------------------
    // placeEasy - Place units in front column sequentially (simple formation)
    // Description: Places all AI units in front column (AI_MIN_COL),
    //              filling rows left-to-right. Spills to next column if full.
    //              No strategic positioning = easier for player.
    // Parameters: board - Game board to place units on
    // Process: Iterate through army_, place each unit in order
    // Purpose: Implement EASY difficulty unit placement (predictable)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void AI::placeEasy(Board& board) {
    // What: Deployment function
    // Purpose: Deploy AI units in simple sequential formation (EASY difficulty)
    // Input: board (Board&) - game board reference to place units on
    // Output: None (places all army_ units on board)
```

---

#### 4️⃣ placeHard - AI::placeHard()
**位置**: src/ai.cpp L195-261

**oldString**:
```cpp
void AI::placeHard(Board& board) {
    // -----------------------------------------------------------------
    // placeHard - Place units in optimized formation (tanks front, damage back)
    // Description: Separates units by class: tanks/warriors in front,
    //              ranged/mages in back. Maximizes defensive and offensive
    //              positioning for harder challenge.
    // Parameters: board - Game board to place units on
    // Process: 1. Split army into tanks and damage dealers
    //          2. Place tanks in front (AI_MIN_COL)
    //          3. Place damage in back (right columns)
    // Purpose: Implement HARD difficulty unit placement (strategic formation)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void AI::placeHard(Board& board) {
    // What: Deployment function
    // Purpose: Deploy AI units in optimized strategic formation (HARD difficulty)
    // Input: board (Board&) - game board reference to place units on
    // Output: None (places army_ units in strategic positions by class)
```

---

### 📁 优先级2：player.cpp（9个函数）

#### 1️⃣ addGold - Player::addGold()
**位置**: src/player.cpp L121-124

**oldString**:
```cpp
void Player::addGold(int amount) {
    // -----------------------------------------------------------------
    // addGold
    // What it does : increases the player's gold.
    // Input  : amount (positive integer)
    // Output : none
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Player::addGold(int amount) {
    // What: Setter function
    // Purpose: Increase player's gold reserves by the given amount
    // Input: amount (int) - gold to add
    // Output: None (modifies gold_ state)
```

---

#### 2️⃣ spendGold - Player::spendGold()
**位置**: src/player.cpp L131-139

**oldString**:
```cpp
bool Player::spendGold(int amount) {
    // -----------------------------------------------------------------
    // spendGold
    // What it does : deducts gold if the player can afford it.
    // Input  : amount
    // Output : true if transaction succeeded, false if insufficient gold
    // -----------------------------------------------------------------
```

**newString**:
```cpp
bool Player::spendGold(int amount) {
    // What: Transaction function
    // Purpose: Deduct gold from player's reserves if affordable
    // Input: amount (int) - gold amount to spend
    // Output: bool - true if transaction succeeded, false if insufficient gold
```

---

#### 3️⃣ takeDamage - Player::takeDamage()
**位置**: src/player.cpp L143-150

**oldString**:
```cpp
void Player::takeDamage(int damage) {
    // -----------------------------------------------------------------
    // takeDamage
    // What it does : reduces HP, floored at 0.
    // Input  : damage
    // Output : none
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Player::takeDamage(int damage) {
    // What: Damage function
    // Purpose: Reduce player HP by damage amount (minimum 0)
    // Input: damage (int) - damage to apply
    // Output: None (modifies hp_ state)
```

---

#### 4️⃣ addToBench - Player::addToBench()
**位置**: src/player.cpp L154-162

**oldString**:
```cpp
bool Player::addToBench(Unit* unit) {
    // -----------------------------------------------------------------
    // addToBench
    // What it does : appends a unit to the bench if space is available.
    // Input  : unit (caller transfers ownership)
    // Output : true on success, false if bench is full
    // -----------------------------------------------------------------
```

**newString**:
```cpp
bool Player::addToBench(Unit* unit) {
    // What: Inventory management
    // Purpose: Add a unit to player's bench if space is available
    // Input: unit (Unit*) - pointer to unit (ownership transferred)
    // Output: bool - true if added successfully, false if bench is full
```

---

#### 5️⃣ removeFromBench - Player::removeFromBench()
**位置**: src/player.cpp L166-175

**oldString**:
```cpp
Unit* Player::removeFromBench(int index) {
    // -----------------------------------------------------------------
    // removeFromBench
    // What it does : removes and returns the unit at the given index.
    // Input  : index (0-based)
    // Output : Unit* (caller takes ownership), nullptr if invalid
    // -----------------------------------------------------------------
```

**newString**:
```cpp
Unit* Player::removeFromBench(int index) {
    // What: Inventory management
    // Purpose: Remove and return a unit from bench at given index
    // Input: index (int) - bench position (0-based)
    // Output: Unit* - pointer (ownership transferred to caller), nullptr if invalid index
```

---

#### 6️⃣ sellUnit - Player::sellUnit()
**位置**: src/player.cpp L179-194

**oldString**:
```cpp
bool Player::sellUnit(int index) {
    // -----------------------------------------------------------------
    // sellUnit
    // What it does : sells a bench unit — adds gold refund and deletes it.
    // Input  : index
    // Output : true on success
    // -----------------------------------------------------------------
```

**newString**:
```cpp
bool Player::sellUnit(int index) {
    // What: Transaction function
    // Purpose: Sell a bench unit, award gold, and remove it
    // Input: index (int) - bench position of unit to sell
    // Output: bool - true if sale succeeded, false if invalid index
```

---

#### 7️⃣ displayBench - Player::displayBench()
**位置**: src/player.cpp L207-238

**oldString**:
```cpp
void Player::displayBench() const {
    // -----------------------------------------------------------------
    // displayBench
    // What it does : prints a formatted list of bench units.
    // Input  : none
    // Output : none (stdout)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Player::displayBench() const {
    // What: Display function
    // Purpose: Display all units on the player's bench with formatted details
    // Input: None
    // Output: None (prints formatted bench list to stdout)
```

---

#### 8️⃣ displayStatus - Player::displayStatus()
**位置**: src/player.cpp L240-281

**oldString**:
```cpp
void Player::displayStatus() const {
    // -----------------------------------------------------------------
    // displayStatus
    // What it does : prints the player's HP, gold, and streak info with colored output.
    // Input  : none
    // Output : none (stdout)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Player::displayStatus() const {
    // What: Display function
    // Purpose: Display player status including HP, gold, round, and streak info
    // Input: None
    // Output: None (prints formatted status box to stdout with colors)
```

---

#### 9️⃣ startNewRound - Player::startNewRound()
**位置**: src/player.cpp L283-321

**oldString**:
```cpp
void Player::startNewRound() {
    // -----------------------------------------------------------------
    // startNewRound
    // What it does : increments round counter and awards income.
    //                Income = base gold + 1 per win streak level.
    // Input  : none
    // Output : none
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Player::startNewRound() {
    // What: State management
    // Purpose: Start new round by incrementing counter and awarding income
    // Input: None
    // Output: None (updates roundsPlayed_, gold_ with income + bonuses)
```

---

### 📁 优先级3：unit.cpp（12个函数）

#### 1️⃣ getSymbol - Unit::getSymbol()
**位置**: src/unit.cpp L67-76

**oldString**:
```cpp
char Unit::getSymbol() const {
    // -----------------------------------------------------------------
    // getSymbol
    // What it does : returns display symbol for the board.
    //                Star 1: first char uppercase (e.g. 'W')
    //                Star 2: first char + '2' indicator
    //                Star 3: first char + '3' indicator
    // Input  : none
    // Output : char (first character of name)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
char Unit::getSymbol() const {
    // What: Getter function
    // Purpose: Return display character for board representation
    // Input: None
    // Output: char - first character of unit's name
```

---

#### 2️⃣ getSymbolString - Unit::getSymbolString()
**位置**: src/unit.cpp L78-89

**oldString**:
```cpp
std::string Unit::getSymbolString() const {
    // -----------------------------------------------------------------
    // getSymbolString
    // What it does : returns a 2-char display string for the board.
    // Input  : none
    // Output : string like "W ", "W2", "W3"
    // -----------------------------------------------------------------
```

**newString**:
```cpp
std::string Unit::getSymbolString() const {
    // What: Getter function
    // Purpose: Return 2-character display string including star level indicator
    // Input: None
    // Output: string - "X " (star 1), "X2" (star 2), "X3" (star 3)
```

---

#### 3️⃣ takeDamage - Unit::takeDamage()
**位置**: src/unit.cpp L117-124

**oldString**:
```cpp
void Unit::takeDamage(int damage) {
    // -----------------------------------------------------------------
    // takeDamage - Reduce unit health by damage amount (floor at 0)
    // Description: Applies damage to the unit's current HP, preventing
    //              negative values. Dead units remain at 0 HP.
    // Parameters: damage - Amount of damage to apply (positive integer)
    // Purpose: Core combat mechanic for inflicting injuries
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Unit::takeDamage(int damage) {
    // What: Damage function
    // Purpose: Apply damage to unit, reducing HP (minimum 0)
    // Input: damage (int) - damage amount to apply
    // Output: None (modifies hp_ state)
```

---

#### 4️⃣ heal - Unit::heal()
**位置**: src/unit.cpp L129-138

**oldString**:
```cpp
void Unit::heal(int amount) {
    // -----------------------------------------------------------------
    // heal - Restore unit health by amount (cap at maxHp)
    // Description: Increases current HP toward maximum, never exceeding it.
    //              Used for healing abilities and passive recovery.
    // Parameters: amount - Health to restore (positive integer)
    // Purpose: Support mechanic for healing and recovery effects
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Unit::heal(int amount) {
    // What: Healing function
    // Purpose: Restore unit health by amount (capped at maxHp)
    // Input: amount (int) - health to restore
    // Output: None (modifies hp_ state)
```

---

#### 5️⃣ healToFull - Unit::healToFull()
**位置**: src/unit.cpp L140-147

**oldString**:
```cpp
void Unit::healToFull() {
    // -----------------------------------------------------------------
    // healToFull - Instantly restore unit to maximum health
    // Description: Fully heals the unit to its maxHp value.
    //              Called before combat rounds and for special events.
    // Purpose: Reset unit state between battles (no resource cost)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Unit::healToFull() {
    // What: Healing function
    // Purpose: Instantly restore unit to full health
    // Input: None
    // Output: None (sets hp_ to maxHp_)
```

---

#### 6️⃣ getSellPrice - Unit::getSellPrice()
**位置**: src/unit.cpp L148-158

**oldString**:
```cpp
int Unit::getSellPrice() const {
    // -----------------------------------------------------------------
    // getSellPrice
    // What it does : returns cost/2 * starLevel, minimum 1.
    // -----------------------------------------------------------------
```

**newString**:
```cpp
int Unit::getSellPrice() const {
    // What: Getter function
    // Purpose: Calculate unit's sell value based on cost and star level
    // Input: None
    // Output: int - sell price (cost/2 * starLevel, minimum 1)
```

---

#### 7️⃣ getDistanceTo - Unit::getDistanceTo()
**位置**: src/unit.cpp L161-173

**oldString**:
```cpp
int Unit::getDistanceTo(const Unit* other) const {
    // -----------------------------------------------------------------
    // getDistanceTo - Calculate Manhattan distance to another unit
    // Description: Computes |row_diff| + |col_diff| for pathfinding
    //              and attack range checking during combat.
    // Parameters: other - Target unit to measure distance to
    // Returns: Integer distance (always >= 0)
    // Purpose: Core mechanic for determining attack viability and target selection
    // -----------------------------------------------------------------
```

**newString**:
```cpp
int Unit::getDistanceTo(const Unit* other) const {
    // What: Utility function
    // Purpose: Calculate Manhattan distance to another unit
    // Input: other (const Unit*) - target unit for distance calculation
    // Output: int - distance value (always >= 0)
```

---

#### 8️⃣ upgrade - Unit::upgrade()
**位置**: src/unit.cpp L226-244

**oldString**:
```cpp
void Unit::upgrade() {
```

**newString**:
```cpp
void Unit::upgrade() {
    // What: Upgrade function
    // Purpose: Upgrade unit to next star level with enhanced stats
    // Input: None
    // Output: None (increases starLevel_, maxHp_, atk_)
```

---

#### 9️⃣ forceSetStarLevel - Unit::forceSetStarLevel()
**位置**: src/unit.cpp L247-256

**oldString**:
```cpp
void Unit::forceSetStarLevel(int level) {
    // -----------------------------------------------------------------
    // forceSetStarLevel - Manually set unit's star level (for testing/loading)
    // Description: Sets star level directly with bounds checking (1-3).
    //              Does NOT update stats; call upgrade() for stat scaling.
    // Parameters: level - Target star level (must be 1-3, otherwise ignored)
    // Purpose: Used for save game loading and debug/testing scenarios
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Unit::forceSetStarLevel(int level) {
    // What: Utility function
    // Purpose: Manually set unit's star level for testing and save loading
    // Input: level (int) - target star level (must be 1-3)
    // Output: None (sets starLevel_ if valid, ignores otherwise)
```

---

#### 🔟 resetBonuses - Unit::resetBonuses()
**位置**: src/unit.cpp L268-276

**oldString**:
```cpp
void Unit::resetBonuses() {
    // -----------------------------------------------------------------
    // resetBonuses - Clear all temporary stat bonuses from synergies
    // Description: Removes all synergy-granted bonuses (ATK, crit),
    //              resetting unit to base stats before recalculating.
    // Purpose: Clean synergy slate each round for proper recalculation
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Unit::resetBonuses() {
    // What: Reset function
    // Purpose: Clear all temporary stat bonuses from synergies
    // Input: None
    // Output: None (resets atkBonus_ and critBonusExtra_ to 0)
```

---

#### 1️⃣1️⃣ clearRage - Unit::clearRage()
**位置**: src/unit.cpp L279-284

**oldString**:
```cpp
void Unit::clearRage() {
    // -----------------------------------------------------------------
    // clearRage - Reset unit's rage status flag
    // Description: Clears the raged_ flag at combat start/reset.
    //              Warriors use rage mode for double-turn mechanic.
    // Purpose: Reset rage state for new combat round
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Unit::clearRage() {
    // What: State management
    // Purpose: Reset unit's rage status flag
    // Input: None
    // Output: None (sets raged_ to false)
```

---

#### 1️⃣2️⃣ setRaged - Unit::setRaged()
**位置**: src/unit.cpp L289-294

**oldString**:
```cpp
void Unit::setRaged() {
    // -----------------------------------------------------------------
    // setRaged - Mark unit as currently in rage mode
    // Description: Sets raged_ flag to true when Warrior class activates.
    //              Enables special ability effects (double turn, etc).
    // Purpose: Activate Warrior class special ability state
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Unit::setRaged() {
    // What: State management
    // Purpose: Mark unit as currently in rage mode for special ability
    // Input: None
    // Output: None (sets raged_ to true)
```

---

### 📁 优先级4：shop.cpp（6个函数）

#### 1️⃣ initTemplatePool - Shop::initTemplatePool()
**位置**: src/shop.cpp L105-120

**oldString**:
```cpp
void Shop::initTemplatePool() {
    // -----------------------------------------------------------------
    // -----------------------------------------------------------------
    // initTemplatePool - Load all available unit templates and their base stats
    // Description: Populates the template pool with 15 pre-defined unit templates.
    //              Each template specifies: name, class, HP, ATK, cost, crit, range
    //              Templates are used as blueprints for random shop generation.
    // Purpose: Initialize the master list of recruitable unit types
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Shop::initTemplatePool() {
    // What: Initialization function
    // Purpose: Initialize master template pool with all available unit types
    // Input: None
    // Output: None (populates templatePool_ with 15 pre-defined unit templates)
```

---

#### 2️⃣ clearSlots - Shop::clearSlots()
**位置**: src/shop.cpp L130-145

**oldString**:
```cpp
void Shop::clearSlots() {
    // -----------------------------------------------------------------
    // clearSlots - Delete all unsold units in shop and reset inventory
    // Description: Safely deletes all Unit pointers in slots and clears vector.
    //              Prevents memory leaks before regenerating shop inventory.
    // Purpose: Clean up shop inventory and free memory before refresh
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Shop::clearSlots() {
    // What: Memory management
    // Purpose: Delete all unsold units and clear shop inventory
    // Input: None
    // Output: None (frees memory and clears slots_ vector)
```

---

#### 3️⃣ refresh - Shop::refresh()
**位置**: src/shop.cpp L149-162

**oldString**:
```cpp
void Shop::refresh() {
    // -----------------------------------------------------------------
    // refresh - Generate new random shop inventory from template pool
    // Description: Clears old slots and populates 5 new random units.
    //              Each unit is randomly selected from template pool and
    //              instantiated with stats unchanged from template.
    // Process: 1. Clear current slots (free memory)
    //          2. For each slot, pick random template and create Unit
    // Purpose: Provide fresh unit choices each round
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Shop::refresh() {
    // What: Shop operation
    // Purpose: Generate new random shop inventory from template pool
    // Input: None
    // Output: None (populates slots_ with 5 new random units)
```

---

#### 4️⃣ buyUnit - Shop::buyUnit()
**位置**: src/shop.cpp L167-180

**oldString**:
```cpp
Unit* Shop::buyUnit(int slotIndex) {
    // -----------------------------------------------------------------
    // buyUnit
    // What it does : removes the unit from the given slot and returns it.
    //                The caller takes ownership of the pointer.
    // Input  : slotIndex (0-based)
    // Output : Unit* on success, nullptr if slot is empty or invalid
    // -----------------------------------------------------------------
```

**newString**:
```cpp
Unit* Shop::buyUnit(int slotIndex) {
    // What: Transaction function
    // Purpose: Remove and return unit from specified shop slot
    // Input: slotIndex (int) - shop slot position (0-based)
    // Output: Unit* - pointer (ownership transferred to caller), nullptr if invalid
```

---

#### 5️⃣ display - Shop::display()
**位置**: src/shop.cpp L183-226

**oldString**:
```cpp
void Shop::display() const {
    // -----------------------------------------------------------------
    // display - Print formatted shop table with unit stats and affordability
    // Description: Shows all 5 shop slots with unit details and pricing.
    //              Marks units player cannot afford with "X" indicator.
    //              Shows sold-out slots as "-- SOLD --".
    // Purpose: Visual shop interface for player browsing and purchasing
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Shop::display() const {
    // What: Display function
    // Purpose: Display formatted shop inventory with unit stats and affordability
    // Input: None
    // Output: None (prints shop table to stdout with colors)
```

---

#### 6️⃣ getUnit - Shop::getUnit()
**位置**: src/shop.cpp L228-237

**oldString**:
```cpp
Unit* Shop::getUnit(int slotIndex) const {
```

**newString**:
```cpp
Unit* Shop::getUnit(int slotIndex) const {
    // What: Getter function
    // Purpose: Retrieve unit from specified shop slot without removing it
    // Input: slotIndex (int) - shop slot position (0-based)
    // Output: Unit* - pointer if present, nullptr if invalid or empty
```

---

### 📁 优先级5：event.cpp（2个函数）

#### 1️⃣ rollEvent - Event::rollEvent()
**位置**: src/event.cpp L26-36

**oldString**:
```cpp
EventType Event::rollEvent(int round) {
    // -----------------------------------------------------------------
    // rollEvent
    // What it does : 40% chance an event occurs each round (after round 1).
    //                Returns a random event type.
    // Input  : round number
    // Output : EventType
    // -----------------------------------------------------------------
```

**newString**:
```cpp
EventType Event::rollEvent(int round) {
    // What: Random generation
    // Purpose: Randomly determine if event occurs this round (40% chance after round 1)
    // Input: round (int) - current game round number
    // Output: EventType - EVENT_NONE or random event type
```

---

#### 2️⃣ applyEvent - Event::applyEvent()
**位置**: src/event.cpp L49-104

**oldString**:
```cpp
std::string Event::applyEvent(EventType event, Player& player) {
    // -----------------------------------------------------------------
    // applyEvent - Execute event effect and return description to player
    // Description: Processes random event occurrence (gold bonus, heal, etc).
    //              Modifies player state (gold, units) based on event type.
    //              Returns user-facing description of what happened.
    // Parameters: event - EventType enum (GOLD_BONUS, HEAL_ALL, SHOP_DISCOUNT, etc)
    //             player - Player object to modify
    // Returns: String describing event result for display
    // Purpose: Implement random event system for game variety and surprises
    // -----------------------------------------------------------------
```

**newString**:
```cpp
std::string Event::applyEvent(EventType event, Player& player) {
    // What: Event execution
    // Purpose: Apply event effect to player and return description
    // Input: event (EventType), player (Player&)
    // Output: String - describing event result for player display
```

---

### 📁 优先级5：record.cpp（2个函数）

#### 1️⃣ saveGame - Record::saveGame()
**位置**: src/record.cpp L51-113

**oldString**:
```cpp
void Record::saveGame(...) {
    // =====================================================================
    // Record::saveGame - save complete game state
    // =====================================================================
```

**newString**:
```cpp
void Record::saveGame(...) {
    // What: Data persistence
    // Purpose: Save complete game state to file for later resumption
    // Input: player, board, shop, ai, currentPhase, currentEvent, slot
    // Output: None (writes to save file)
```

---

#### 2️⃣ loadGame - Record::loadGame()
**位置**: src/record.cpp L127-308

**oldString**:
```cpp
bool Record::loadGame(...) {
```

**newString**:
```cpp
bool Record::loadGame(...) {
    // What: Data loading
    // Purpose: Load complete game state from save file
    // Input: player, board, shop, ai, currentPhase, currentEvent, shouldResumeShop, slot
    // Output: bool - true if successful, false if file corrupted or invalid
```

---

### 📁 优先级6：tutorial.cpp（4个函数）

#### 1️⃣ clearScreen - Tutorial::clearScreen()
**位置**: src/tutorial.cpp L48-56

**oldString**:
```cpp
void Tutorial::clearScreen() {
    // -----------------------------------------------------------------
    // Helper: Clear screen
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void Tutorial::clearScreen() {
    // What: Utility function
    // Purpose: Clear terminal screen for clean tutorial display
    // Input: None
    // Output: None (clears screen - works on macOS/Linux and Windows)
```

---

#### 2️⃣ printBox - Tutorial::printBox()
**位置**: src/tutorial.cpp L92-117

**oldString**:
```cpp
void Tutorial::printBox(const std::string& text, int width) {
```

**newString**:
```cpp
void Tutorial::printBox(const std::string& text, int width) {
    // What: Display function
    // Purpose: Print formatted boxed text for tutorial UI
    // Input: text (const std::string&), width (int)
    // Output: None (prints formatted box to stdout)
```

---

#### 3️⃣ waitForInput - Tutorial::waitForInput()
**位置**: src/tutorial.cpp L119-125

**oldString**:
```cpp
void Tutorial::waitForInput() {
```

**newString**:
```cpp
void Tutorial::waitForInput() {
    // What: Input function
    // Purpose: Pause tutorial and wait for user input to continue
    // Input: None
    // Output: None (reads from stdin)
```

---

#### 4️⃣ run - Tutorial::run()
**位置**: src/tutorial.cpp L932-991

**oldString**:
```cpp
bool Tutorial::run() {
```

**newString**:
```cpp
bool Tutorial::run() {
    // What: Main tutorial loop
    // Purpose: Execute tutorial sequence with all lessons
    // Input: None
    // Output: bool - true if completed successfully
```

---

### 📁 优先级7：main.cpp（4个函数）

#### 1️⃣ displayMainMenu - displayMainMenu()
**位置**: src/main.cpp L53-73

**oldString**:
```cpp
void displayMainMenu() {
    // -----------------------------------------------------------------
    // displayMainMenu
    // What it does : prints the main menu options to stdout.
    // Input  : none
    // Output : none (prints to stdout)
    // -----------------------------------------------------------------
```

**newString**:
```cpp
void displayMainMenu() {
    // What: Display function
    // Purpose: Display main menu with game options to player
    // Input: None
    // Output: None (prints formatted menu to stdout)
```

---

#### 2️⃣ selectPlayerName - selectPlayerName()
**位置**: src/main.cpp L75-114

**oldString**:
```cpp
std::string selectPlayerName() {
    // -----------------------------------------------------------------
    // selectPlayerName
    // What it does: Prompts user to input their player name for the game
    // Input: none
    // Output: player name as std::string (empty string defaults to "Player")
    // -----------------------------------------------------------------
```

**newString**:
```cpp
std::string selectPlayerName() {
    // What: Input function
    // Purpose: Prompt and get player name for new game session
    // Input: None
    // Output: std::string - player name (defaults to "Player" if empty)
```

---

#### 3️⃣ selectDifficulty - selectDifficulty()
**位置**: src/main.cpp L116-144

**oldString**:
```cpp
Difficulty selectDifficulty() {
    // -----------------------------------------------------------------
    // selectDifficulty
    // What it does : prompts the user to choose Easy or Hard difficulty.
    // Input  : none
    // Output : the chosen Difficulty enum value
    // -----------------------------------------------------------------
```

**newString**:
```cpp
Difficulty selectDifficulty() {
    // What: Input function
    // Purpose: Prompt and get difficulty selection from player
    // Input: None
    // Output: Difficulty enum - EASY or HARD (defaults to EASY)
```

---

#### 4️⃣ main - main()
**位置**: src/main.cpp L148-241

**oldString**:
```cpp
int main() {
    // -----------------------------------------------------------------
    // main
    // What it does : program entry point. Seeds the random number generator,
    //                displays the main menu, and dispatches to the
    //                appropriate function based on user input.
    // Input  : none (standard argc/argv not used)
    // Output : 0 on normal exit
    // -----------------------------------------------------------------
```

**newString**:
```cpp
int main() {
    // What: Program entry point
    // Purpose: Program entry point - seed RNG and manage game main loop
    // Input: None (argc/argv not used)
    // Output: 0 on normal exit
```

---

## 📋 快速应用指南

### 方法1：逐个应用（replace_string_in_file）
如果修改函数数量较少（< 5个），可以逐个使用 `replace_string_in_file`。

### 方法2：批量应用（multi_replace_string_in_file）- **推荐**
将多个修改打包成单个工具调用，大幅提高效率。

### 修改优先级建议
```
优先级1 (核心系统):
  ✅ board.cpp (12个)
  ✅ ai.cpp (4个)

优先级2 (玩家系统):
  ✅ player.cpp (9个)
  ✅ unit.cpp (12个)

优先级3 (商店系统):
  ✅ shop.cpp (6个)

优先级4 (事件/记录):
  ✅ event.cpp (2个)
  ✅ record.cpp (2个)

优先级5 (教程/主程序):
  ✅ tutorial.cpp (4个)
  ✅ main.cpp (4个)
```

### 验证修改
```bash
# 编译检查
make clean
make

# 如果出现编译错误，检查注释格式是否正确
```

---

## ✅ 修改统计

| 指标 | 数值 |
|------|------|
| 总函数数 | 55 |
| 标准格式应用率 | 100% |
| 注释完整性检查 | What/Purpose/Input/Output 完整 |
| 类型标注完整性 | 100% |
| 代码示例 | oldString/newString 完整配对 |

---

**报告生成时间**: 2026-04-18 23:50  
**格式标准化完成度**: ✅ 100%
