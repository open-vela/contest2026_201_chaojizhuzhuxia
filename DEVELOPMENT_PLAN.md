# 研途计时・离线考研智能穿戴手表 — 开发方案

## 一、项目概述

| 项目 | 内容 |
|------|------|
| 项目名称 | 研途计时・离线考研智能穿戴手表 |
| 目标开发板 | SF32LB52 LCD（SiFli 低功耗穿戴芯片） |
| 操作系统 | OpenVela（基于 NuttX RTOS） |
| UI 框架 | LVGL（官方适配 SF32LB52） |
| 开发语言 | C（LVGL 应用层）+ Python/JS（手机端同步工具） |
| 参赛赛道 | AI 硬件产品创新 |

---

## 二、整体架构设计

```
┌─────────────────────────────────────────────────────────┐
│                    手表端 (SF32LB52)                      │
│  ┌─────────────────────────────────────────────────────┐│
│  │                  LVGL UI 层                          ││
│  │  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐     ││
│  │  │倒计时 │ │番茄钟 │ │单词库 │ │音频   │ │复习   │     ││
│  │  │表盘   │ │计时器 │ │背诵   │ │播放器 │ │备忘录 │     ││
│  │  └──────┘ └──────┘ └──────┘ └──────┘ └──────┘     ││
│  └─────────────────────────────────────────────────────┘│
│  ┌─────────────────────────────────────────────────────┐│
│  │              业务逻辑层 (C Modules)                   ││
│  │  ┌──────────┐ ┌──────────┐ ┌──────────────────┐    ││
│  │  │Countdown │ │ Pomodoro │ │ Vocabulary       │    ││
│  │  │Manager   │ │ Timer    │ │ Engine           │    ││
│  │  ├──────────┤ ├──────────┤ ├──────────────────┤    ││
│  │  │Audio     │ │ Reminder │ │ StudyPlan        │    ││
│  │  │Player    │ │ Manager  │ │ Manager          │    ││
│  │  └──────────┘ └──────────┘ └──────────────────┘    ││
│  └─────────────────────────────────────────────────────┘│
│  ┌─────────────────────────────────────────────────────┐│
│  │              数据持久化层                              ││
│  │  ┌──────────┐ ┌──────────┐ ┌──────────────────┐    ││
│  │  │Flash FS  │ │  JSON    │ │  SQLite (可选)    │    ││
│  │  │(LittleFS)│ │  Parser  │ │  轻量数据库       │    ││
│  │  └──────────┘ └──────────┘ └──────────────────┘    ││
│  └─────────────────────────────────────────────────────┘│
│  ┌─────────────────────────────────────────────────────┐│
│  │           OpenVela / NuttX 硬件抽象层                 ││
│  │  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐     ││
│  │  │ RTC  │ │ PWM  │ │ BLE  │ │Audio │ │LCD   │     ││
│  │  │驱动  │ │震动马达│ │GATT  │ │解码  │ │触控  │     ││
│  │  └──────┘ └──────┘ └──────┘ └──────┘ └──────┘     ││
│  └─────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────┘
         ▲ BLE GATT (单向同步)
         ▼
┌─────────────────────────────────────────────────────────┐
│                   手机端 (可选)                           │
│  简易文件传输工具 — 通过 BLE 将词库/音频/计划推送到手表     │
│  不上传任何数据，纯本地操作                                │
└─────────────────────────────────────────────────────────┘
```

---

## 三、目录结构规划

```
contest2026_201_chaojizhuzhuxia/
├── app/
│   └── yantu_watch/                    # 主应用（手表端 LVGL 应用）
│       ├── CMakeLists.txt              # 构建配置
│       ├── Kconfig                     # 菜单配置
│       ├── Make.defs                   # 构建系统定义
│       ├── Makefile                    # 构建文件
│       ├── main.c                      # 应用入口
│       ├── config.h                    # 全局配置（引脚、参数等）
│       │
│       ├── ui/                         # LVGL 界面层
│       │   ├── ui_main.c/h             # 主界面/表盘
│       │   ├── ui_countdown.c/h        # 考研倒计时页面
│       │   ├── ui_pomodoro.c/h         # 番茄计时器页面
│       │   ├── ui_vocabulary.c/h       # 单词背诵页面
│       │   ├── ui_audio.c/h            # 音频播放页面
│       │   ├── ui_reminder.c/h         # 提醒管理页面
│       │   ├── ui_study_plan.c/h       # 复习计划页面
│       │   ├── ui_settings.c/h         # 设置页面
│       │   └── ui_components.c/h       # 共享 UI 组件（按钮、进度条等）
│       │
│       ├── core/                       # 业务逻辑层
│       │   ├── countdown_manager.c/h   # 倒计时管理
│       │   ├── pomodoro_timer.c/h      # 番茄钟计时器
│       │   ├── vocabulary_engine.c/h   # 单词引擎（加载/查询/标记）
│       │   ├── audio_player.c/h        # 音频播放控制
│       │   ├── reminder_manager.c/h    # 震动提醒管理
│       │   ├── study_plan_manager.c/h  # 复习计划管理
│       │   └── study_record.c/h        # 学习时长统计与记录
│       │
│       ├── data/                       # 数据持久化层
│       │   ├── storage.c/h             # Flash 文件读写抽象
│       │   ├── json_parser.c/h         # 轻量 JSON 解析（基于 cJSON）
│       │   └── database.c/h            # 本地数据库接口（可选 SQLite/UnQLite）
│       │
│       ├── ble/                        # 蓝牙通信层
│       │   ├── ble_sync.c/h            # BLE GATT 同步服务
│       │   └── ble_protocol.c/h        # 同步协议定义
│       │
│       ├── resources/                  # 资源文件
│       │   ├── vocab/                  # 考研英语词库 JSON 文件
│       │   │   ├── cet4.json
│       │   │   ├── cet6.json
│       │   │   └── kaoyan.json
│       │   ├── audio/                  # 示例音频（占位）
│       │   └── fonts/                  # 自定义字体（中英文）
│       │
│       └── tests/                      # 单元测试
│           ├── test_countdown.c
│           ├── test_pomodoro.c
│           └── test_vocabulary.c
│
├── board/
│   └── contest_board/                  # 板级适配（SF32LB52 定制）
│       ├── CMakeLists.txt
│       ├── Kconfig
│       ├── configs/
│       │   └── nsh/
│       │       └── defconfig           # 板级默认配置
│       └── src/
│           ├── CMakeLists.txt
│           ├── board_boot.c            # 板级初始化（LCD、RTC、PWM、BLE 等）
│           └── board_lcd.c             # LCD 初始化配置
│
├── tools/                              # 手机端同步工具 & 开发辅助脚本
│   ├── phone_sync/                     # 手机端 BLE 文件推送工具（可选）
│   │   └── README.md                   # 使用说明
│   └── gen_vocab.py                    # 词库生成/转换脚本
│
├── docs/                               # 设计文档
│   ├── architecture.md
│   ├── BLE_PROTOCOL.md                 # 蓝牙同步协议设计
│   └── UI_FLOW.md                      # UI 交互流程
│
├── logs/                               # AI Coding 日志
├── README.md                           # 作品说明（最终替换）
└── contest2026_201_chaojizhuzhuxia.xml # Manifest 清单
```

---

## 四、分阶段开发计划（共 6 个阶段）

### Phase 0：环境搭建与工程配置（0.5 天）

**目标**：完成 `repo init/sync`，确认编译链可用，烧录验证最小系统。

| 步骤 | 内容 | 产出 |
|------|------|------|
| 0.1 | `repo init` + `repo sync` 拉取完整工程 | 工作区就绪 |
| 0.2 | 验证 SF32LB52 工具链 | `arm-none-eabi-gcc` 可编译 |
| 0.3 | 编译最小 NSH 固件并烧录 | 串口可交互 |
| 0.4 | 配置 LVGL 和 LCD 驱动 | 屏幕点亮显示 LVGL 示例 |
| 0.5 | 更新 manifest 中的 linkfile 映射 | 应用软链到编译树 |

**关键操作**：
```bash
# 从仓库根目录
cd ..
repo init -u https://github.com/open-vela/contest2026_201_chaojizhuzhuxia \
  -b dev-ai-contest-2026 -m contest2026_201_chaojizhuzhuxia.xml
repo sync -c -j8

# 验证 SDK 编译
./build.sh vendor/openvela/boards/sil/sf32lb52/lcd/configs/nsh menuconfig
./build.sh vendor/openvela/boards/sil/sf32lb52/lcd/configs/nsh -j8
```

---

### Phase 1：基础框架 + 倒计时与学习时长统计（2 天）

**目标**：搭建 LVGL 应用框架，实现考研倒计时表盘 + 每日学习时长统计。

#### 1.1 应用主框架
- 创建 `yantu_watch` 应用目录，配置 CMakeLists.txt / Kconfig
- 实现 `main.c`：LVGL 初始化、页面管理器、触摸事件循环
- 实现页面路由框架（首页/倒计时/番茄钟/单词/音频/设置）

#### 1.2 考研倒计时模块
- `core/countdown_manager.c`：
  - 通过 RTC 读取当前日期
  - 计算距离考研日期（固定 2026年12月19日，或用户可配置）的天数
  - 精确到秒的倒计时（天/时/分/秒）
- `ui/ui_countdown.c`：
  - LVGL 倒计时表盘界面（大数字显示天数，辅以进度环）
  - 显示"距离考研还有 XX 天"
  - 支持触控切换显示模式

#### 1.3 学习时长统计
- `core/study_record.c`：
  - 每日学习时长记录（精度：分钟）
  - 持久化到 Flash（LittleFS），格式：`{date: "2026-07-15", minutes: 480}`
  - 提供本周/本月统计接口
- `ui/ui_main.c`：
  - 主表盘显示今日学习时长
  - 周/月统计柱状图（LVGL chart 组件）

**核心 API 接口**：
```c
// countdown_manager.h
int  countdown_init(void);
int  countdown_get_days_remaining(void);
void countdown_get_time_remaining(int *days, int *hours, int *mins, int *secs);
void countdown_set_exam_date(int year, int month, int day);

// study_record.h
int  study_record_init(void);
int  study_record_add_minutes(int minutes);
int  study_record_get_today_minutes(void);
int  study_record_get_week_data(int *data, int *count);
int  study_record_get_month_data(int *data, int *count);
```

---

### Phase 2：番茄自习计时器 + 震动提醒（2 天）

**目标**：实现多科目番茄钟，支持 PWN 震动马达分段提醒。

#### 2.1 番茄钟计时器逻辑
- `core/pomodoro_timer.c`：
  - 基于 OpenVela 硬件定时器 SDK 实现精准计时
  - 支持多科目切换（政治/英语/数学/专业课）
  - 标准番茄钟：25min 学习 + 5min 休息
  - 可自定义学习/休息时长
  - 状态机：空闲 → 学习中 → 休息中 → 完成
  - 每完成一个番茄，自动记录到学习时长统计

#### 2.2 震动提醒
- 集成 PWM 驱动接口控制震动马达
- 不同的震动模式：
  - 番茄结束：长震 3 次（500ms 震，200ms 停）
  - 休息结束：短震 2 次（200ms 震，100ms 停）
  - 自定义提醒：单次震（可配置时长）

#### 2.3 UI
- `ui/ui_pomodoro.c`：
  - LVGL 圆形进度条显示剩余时间
  - 科目选择按钮（触控切换）
  - 开始/暂停/停止控制
  - 今日番茄完成数统计展示

**核心 API 接口**：
```c
// pomodoro_timer.h
typedef enum {
    POMODORO_IDLE,
    POMODORO_FOCUS,
    POMODORO_BREAK,
    POMODORO_COMPLETED
} pomodoro_state_t;

typedef enum {
    SUBJECT_POLITICS,
    SUBJECT_ENGLISH,
    SUBJECT_MATH,
    SUBJECT_MAJOR,
    SUBJECT_CUSTOM
} subject_t;

int  pomodoro_init(void);
int  pomodoro_start(subject_t subject);
int  pomodoro_pause(void);
int  pomodoro_resume(void);
int  pomodoro_stop(void);
pomodoro_state_t pomodoro_get_state(void);
int  pomodoro_get_remaining_seconds(void);
int  pomodoro_get_today_count(void);
int  pomodoro_set_duration(int focus_minutes, int break_minutes);
```

---

### Phase 3：离线单词背诵模块（2.5 天）

**目标**：实现离线英语单词库，支持触屏翻页、记忆标记。

#### 3.1 词库数据结构
- 词汇 JSON 格式设计：
```json
{
  "version": 1,
  "words": [
    {
      "word": "abandon",
      "phonetic": "/əˈbændən/",
      "meaning": "v. 放弃，遗弃",
      "example": "He abandoned his plan.",
      "mark": 0
    }
  ]
}
```
- `mark` 字段：0=未标记，1=已掌握，2=需复习，3=收藏

#### 3.2 单词引擎
- `core/vocabulary_engine.c`：
  - 从 Flash 加载词库 JSON（支持多词库切换：四级/六级/考研）
  - 单词列表管理（当前词库、当前索引）
  - 记忆标记状态持久化（单独存储标记状态，不修改原始词库）
  - 复习模式：随机抽取已标记"需复习"的单词
  - 学习进度统计（已学/总数/掌握率）

#### 3.3 UI
- `ui/ui_vocabulary.c`：
  - 单词卡片界面（LVGL）
  - 点击卡片翻转：英文面 → 中文释义面
  - 左滑/右滑切换上下一个单词
  - 底部标记按钮：已掌握/需复习/收藏
  - 进度条显示当前词库学习进度
  - 词库选择列表

---

### Phase 4：离线音频播放 + 静音提醒 + 复习计划（2.5 天）

**目标**：实现本地 MP3 播放、定时震动提醒、复习计划备忘录。

#### 4.1 离线音频播放
- `core/audio_player.c`：
  - 基于 OpenVela 内置音频解码 SDK（libhelix-mp3）
  - 支持 MP3 格式本地音频播放
  - 支持 BLE A2DP 蓝牙耳机输出
  - 播放控制：播放/暂停/上一曲/下一曲/进度拖拽
  - 音频文件列表从 Flash 扫描
- `ui/ui_audio.c`：
  - 播放器界面（专辑封面占位、歌曲名、进度条）
  - 音量控制
  - 播放模式（顺序/单曲循环/列表循环）

#### 4.2 定时震动提醒
- `core/reminder_manager.c`：
  - 基于 OpenVela RTC 闹钟中断 SDK
  - 支持多组闹钟（起床/午休/晚复盘/自定义）
  - 可配置重复（每天/工作日/仅一次）
  - 静音震动（PWM 控制马达）
  - 闹钟持久化到 Flash
- `ui/ui_reminder.c`：
  - 闹钟列表（显示时间、标签、重复周期）
  - 添加/编辑/删除闹钟
  - 开关控制

#### 4.3 复习计划备忘录
- `core/study_plan_manager.c`：
  - 每日任务清单（科目、任务内容、是否完成）
  - 周计划概览
  - 持久化存储（JSON 格式）
- `ui/ui_study_plan.c`：
  - 今日任务列表（checkbox 打卡）
  - 添加新任务
  - 周计划视图

---

### Phase 5：蓝牙同步 + 整体联调（2 天）

**目标**：实现手机 BLE 单向同步，整体功能联调与优化。

#### 5.1 BLE GATT 同步服务
- `ble/ble_sync.c`：
  - 基于 OpenVela BLE SDK 实现 GATT Service
  - 定义 GATT 服务 UUID 和特征值
  - 文件传输协议：分包发送（每包 512 字节）+ CRC 校验
  - 支持的文件类型：词库 JSON、音频 MP3、复习计划 JSON
  - 手表端接收完成自动保存到 Flash
- `ble/ble_protocol.c`：
  - 同步协议定义
  - 命令帧：`{cmd, file_type, file_name, file_size, total_packets}`
  - 数据帧：`{packet_id, data, crc16}`
  - 确认帧：`{ack, packet_id}`

#### 5.2 手机端工具（可选轻量实现）
- 微信小程序 或 Python 脚本（通过 PC 蓝牙中转）
- 功能：本地选择文件 → 通过 BLE 推送到手表
- 不上传任何数据到云端，保护隐私

#### 5.3 整体联调
- 所有功能模块联动测试
- 功耗优化：待机模式切换、屏幕亮度调节
- 内存优化：LVGL 内存池调优、图片资源压缩
- 启动速度优化：资源预加载、懒加载策略

---

### Phase 6：文档编写 + 作品提交（1 天）

**目标**：完善文档，提交最终作品。

- 替换 README.md 为作品说明
- 编写 docs/ 下的架构文档、UI 流程文档
- 导出 AI Coding 日志到 logs/
- 最终 PR 合并

---

## 五、关键技术实现细节

### 5.1 LVGL 页面管理 (Page Manager)

采用简单的页面栈管理方案，不依赖复杂框架：

```c
typedef enum {
    PAGE_MAIN,        // 主表盘
    PAGE_COUNTDOWN,   // 倒计时
    PAGE_POMODORO,    // 番茄钟
    PAGE_VOCABULARY,  // 单词
    PAGE_AUDIO,       // 音频
    PAGE_REMINDER,    // 提醒
    PAGE_STUDY_PLAN,  // 复习计划
    PAGE_SETTINGS,    // 设置
} page_id_t;

// 页面切换：隐藏当前页，显示目标页
void page_switch(page_id_t target);
```

### 5.2 数据持久化方案

使用 **LittleFS**（轻量日志型 Flash 文件系统，OpenVela 已集成）：

| 文件路径 | 内容 |
|----------|------|
| `/data/study_record.json` | 每日学习时长日志 |
| `/data/vocab_progress.json` | 单词记忆标记状态 |
| `/data/reminders.json` | 闹钟配置 |
| `/data/study_plan.json` | 复习计划任务 |
| `/data/settings.json` | 用户设置 |
| `/vocab/kaoyan.json` | 考研英语词库（只读） |
| `/audio/*.mp3` | 音频文件（只读） |

### 5.3 功耗管理策略

| 场景 | 操作 | 预期功耗 |
|------|------|----------|
| 表盘显示 | 低刷新率（1Hz）、背光低亮度 | ~5mA |
| 番茄计时 | 屏幕常亮低亮度 + 定时器运行 | ~10mA |
| 单词背诵 | 触摸交互 + 全刷 | ~20mA |
| 音频播放 | 蓝牙耳机输出 + 音频解码 | ~25mA |
| 待机休眠 | 屏幕关闭，RTC 运行 | ~0.1mA |
| BLE 同步 | 蓝牙传输 | ~15mA |

### 5.4 编译配置要点

需要在 `defconfig` 或 `menuconfig` 中启用的关键配置：

```
# Board: SF32LB52 LCD
CONFIG_ARCH_BOARD_SIL_SF32LB52_LCD=y

# LVGL
CONFIG_LVGL=y
CONFIG_LV_USE_DEMO_CONTEST2026_201_YANTU_WATCH=y

# File System
CONFIG_FS_LITTLEFS=y
CONFIG_FS_FAT=y

# RTC
CONFIG_RTC=y
CONFIG_RTC_ALARM=y

# PWM (Vibration Motor)
CONFIG_PWM=y

# Audio
CONFIG_AUDIO=y
CONFIG_AUDIO_CODEC=y
CONFIG_LIBHELIX_MP3=y

# BLE
CONFIG_BLE=y
CONFIG_BLE_GATT=y

# JSON
CONFIG_CJSON=y

# Timer
CONFIG_POSIX_TIMERS=y
CONFIG_SCHED_HPWORK=y
```

---

## 六、资源估算

| 资源 | 需求 | 说明 |
|------|------|------|
| Flash 存储 | ≥ 8MB | 系统 ~2MB + 词库 ~1MB + 音频 ~3MB + 数据 ~0.5MB + 预留 |
| RAM | ≥ 512KB | LVGL 缓冲 ~128KB + 音频解码 ~64KB + 应用 ~256KB + 堆 ~64KB |
| CPU 主频 | ≥ 200MHz | SF32LB52 典型主频 240MHz，满足需求 |
| 电池 | ≥ 200mAh | 预期续航 7-14 天 |

---

## 七、里程碑与时间线

| 阶段 | 内容 | 预计工时 | 交付物 |
|------|------|----------|--------|
| Phase 0 | 环境搭建 | 0.5 天 | 可烧录最小系统 |
| Phase 1 | 倒计时 + 学习统计 | 2 天 | 倒计时表盘可运行 |
| Phase 2 | 番茄钟 + 震动 | 2 天 | 番茄钟可运行 |
| Phase 3 | 单词背诵 | 2.5 天 | 单词模块可运行 |
| Phase 4 | 音频 + 提醒 + 计划 | 2.5 天 | 全部功能就绪 |
| Phase 5 | 蓝牙同步 + 联调 | 2 天 | 全功能联调通过 |
| Phase 6 | 文档 + 提交 | 1 天 | 最终作品提交 |
| **合计** | | **~12.5 天** | |

---

## 八、风险与应对

| 风险 | 可能性 | 影响 | 应对 |
|------|--------|------|------|
| SF32LB52 BLE A2DP 驱动未完善 | 中 | 高（音频蓝牙输出） | 备选：使用板载扬声器/3.5mm 接口 |
| Flash 空间不足 | 中 | 中 | 优先内置核心词库，其余通过手机按需同步 |
| LVGL 中文显示问题 | 低 | 中 | 使用 LVGL 字体工具预生成中文字体 bin |
| 电池续航不达标 | 低 | 高 | 优化屏幕刷新率、休眠策略、降低音频解码频率 |
| 编译环境搭建问题 | 低 | 中 | 参考官方文档，使用 Docker 环境 |

---

## 九、当前项目状态与下一步行动

**当前状态**：项目骨架已就绪（hello_app 示例 + contest_board 占位），下一步进入 **Phase 0**。

**立即执行**：
1. 执行 `repo init && repo sync` 拉取完整 openvela 源码
2. 验证 SF32LB52 最小编译
3. 将 `app/hello_app` 重构为 `app/yantu_watch` 并更新 manifest 映射
4. 点亮屏幕，跑通 LVGL Hello World