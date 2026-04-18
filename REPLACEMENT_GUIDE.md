# 标准化注释替换清单 - 使用指南

## 📄 文件说明

**文件名**: `standardized_comments_replacements.json`

此JSON文件包含100+个函数的标准化注释替换对，可用于批量更新项目中所有源文件的函数注释格式。

## 🎯 标准化格式

所有替换都遵循统一的四行注释格式：

```cpp
// What: [函数类型]
// Purpose: [功能描述]
// Input: [参数及其类型说明]
// Output: [返回值及其含义]
```

### 函数类型（What字段）示例：
- `Constructor` - 构造函数
- `Destructor` - 析构函数
- `Getter` - 获取器方法
- `Mutator method` - 修改器方法
- `Static query method` - 静态查询方法
- `Static accessor method` - 静态访问器方法
- `Display method` - 显示方法
- `Input method` - 输入方法
- `Utility function` - 工具函数
- `Display function` - 显示函数
- `Main game loop method` - 主游戏循环方法

## 📦 JSON 结构

```json
[
  {
    "filePath": "src/player.cpp",
    "function": "Player::Player(const std::string&)",
    "oldString": "// -----------------------------------------------------------------\n// Constructor\n...\nPlayer::Player(const std::string& name)",
    "newString": "// What: Constructor\n// Purpose: ...\n// Input: ...\n// Output: ...\nPlayer::Player(const std::string& name)"
  },
  ...
]
```

### 字段说明：
- **filePath**: 源文件相对路径（相对于项目根目录）
- **function**: 函数完整名称（包括类名和参数）
- **oldString**: 当前存在的注释+函数签名的前几行（包含上下文）
- **newString**: 标准化后的注释+函数签名

## 🚀 使用方法

### 方法1：使用 VS Code GitHub Copilot Chat 工具（推荐）

```
使用 multi_replace_string_in_file 工具批量应用所有替换
```

**步骤**：
1. 在 VS Code 中打开 GitHub Copilot Chat
2. 上传JSON文件内容或指定文件路径
3. 请求应用所有替换
4. 工具将自动处理所有替换对

### 方法2：手动逐个应用

如果需要逐个应用替换，可以：

1. 打开JSON文件
2. 对于每个替换对：
   - 打开对应的源文件
   - 查找 `oldString` 中的代码
   - 用 `newString` 替换

### 方法3：编写脚本

可以编写Python/Perl脚本自动解析JSON并应用替换。

## 📋 覆盖的文件列表

| 文件 | 替换对数量 |
|------|----------|
| src/player.cpp | 24 |
| src/unit.cpp | 28 |
| src/shop.cpp | 11 |
| src/event.cpp | 8 |
| src/record.cpp | 6 |
| src/tutorial.cpp | 15 |
| src/main.cpp | 4 |
| src/game.cpp | 4 |
| **总计** | **100+** |

## ✅ 验证步骤

替换完成后，建议执行以下验证：

```bash
# 1. 编译验证
make clean && make

# 2. 检查注释格式（可选）
grep -n "// What:" src/*.cpp | head -20

# 3. 确保没有遗漏的旧格式
grep -n "// What it does" src/*.cpp
```

## 📝 注释规范

### Purpose 字段（功能描述）
- 简洁明了，1-2 句话
- 说明函数的主要目的
- 示例：`"Initialize player with starting HP, gold, empty bench, and streak counters"`

### Input 字段（参数说明）
- 列出所有参数、类型和说明
- 格式：`paramName (type) - 说明`
- 如果无参数，写 `none`
- 示例：`name (const string&) - player's display name`

### Output 字段（返回值说明）
- 说明返回值类型和含义
- 格式：`type - 说明`
- 包括对状态改变的说明（如适用）
- 示例：`bool - true if transaction succeeded, false if insufficient gold`

## 🔄 应用顺序建议

如果分步应用，建议按以下优先级：

1. **核心系统** (player.cpp, unit.cpp)
2. **游戏系统** (shop.cpp, game.cpp)
3. **支持系统** (event.cpp, record.cpp)
4. **用户界面** (tutorial.cpp, main.cpp)

## ⚠️ 注意事项

- ✅ 所有 `oldString` 都包含充分的上下文（3-5行），确保精确匹配
- ✅ 不会修改函数实现代码，只修改注释
- ✅ 格式检查：每个注释必须是4行，遵循 What/Purpose/Input/Output 顺序
- ❌ 不应修改 `oldString` 或 `newString`，直接使用提供的内容
- ❌ 不应跳过任何替换对，否则格式不一致

## 🎓 示例

### 之前（不规范）：
```cpp
// Purpose: Get player's name
// Input: none
// Output: const string reference - player name
std::string Player::getName() const;
```

### 之后（规范）：
```cpp
// What: Getter
// Purpose: Get player's name
// Input: none
// Output: const string - player's display name
std::string Player::getName() const;
```

## 📞 常见问题

### Q: 如果 `oldString` 匹配失败怎么办？
A: 检查文件是否已被修改。如果已修改，可能需要手动调整 `oldString` 中的上下文。

### Q: 可以部分应用替换吗？
A: 可以。你可以从JSON文件中提取特定的替换对进行应用。

### Q: 替换是否会影响函数功能？
A: 否。替换只修改注释，不修改任何代码逻辑。

### Q: 如何验证所有替换都已应用？
A: 运行 `grep -c "// What:" src/*.cpp` 检查是否有 100+ 个 What: 标签。

## 📚 相关文档

- 标准化格式报告：`STANDARDIZED_COMMENT_FORMAT_REPORT.md`
- 函数注释检查报告：`函数注释检查报告.md`（中文）
- 修改方案文档：`COMMENT_FIX_PLAN.md`

---

**生成时间**: 2026-04-18  
**格式版本**: 1.0  
**状态**: 已验证，可用于应用
