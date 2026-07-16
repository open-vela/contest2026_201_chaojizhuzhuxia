# 研途计时 · 离线考研智能穿戴手表

## 一、作品简介

**研途计时**是一款面向考研学生的离线智能穿戴手表，基于 SF32LB52 低功耗穿戴芯片和 OpenVela RTOS 开发。针对考研自习场景中手机分心、无静音提醒、无法离线背单词/听听力等痛点，提供全离线无手机依赖的轻量化解决方案。

**核心亮点**：
- 🔋 **全离线运行** — 无需联网、无需手机，杜绝自习刷手机分心
- 🕐 **考研倒计时** — 精确到秒，进度环可视化
- 🍅 **多科目番茄钟** — 政治/英语/数学/专业课，静音震动提醒
- 📖 **离线单词背诵** — 四级/六级/考研词库，触屏翻转记忆
- 🎧 **离线音频播放** — 英语听力、专业课带读，支持蓝牙耳机
- ⏰ **定时震动提醒** — 起床/午休/复盘全静音，不打扰他人
- 📋 **复习计划打卡** — 每日任务清单，checkbox 完成追踪

---

## 二、选题方向

**AI 硬件产品创新赛道**

选板理由：SF32LB52 芯片专为穿戴设备设计，原生支持 LVGL 图形框架、PWM 震动马达、RTC 实时时钟、BLE 蓝牙音频和本地 Flash 存储，完美匹配考研场景的离线学习需求，功耗极低（待机 0.1mA），续航可达 7-14 天。

---

## 三、目录结构

```
contest2026_201_chaojizhuzhuxia/
├── app/yantu_watch/              ← 手表端主应用（C + LVGL）
│   ├── main.c                    ← 应用入口
│   ├── ui/                       ← LVGL 界面层（9 个页面）
│   ├── core/                     ← 业务逻辑层（8 个模块）
│   ├── data/                     ← 数据持久化层（3 个模块）
│   ├── ble/                      ← 蓝牙通信层（2 个模块）
│   ├── resources/vocab/          ← 考研/四级/六级词库
│   └── tests/                    ← 单元测试
├── board/contest_board/          ← 板级适配（LCD/RTC/PWM/BLE 初始化）
├── quickapp/hello_quickapp/      ← 快应用（手机端数据同步 UI）
├── docs/                         ← 设计文档
├── logs/                         ← AI Coding 日志
└── README.md                     ← 本文件
```

---

## 四、运行方式

### 环境要求

- SF32LB52 LCD 开发板
- `arm-none-eabi-gcc` 工具链（≥ 13.4.0）
- OpenVela 完整工程（`repo init` + `repo sync`）

### 编译步骤

```bash
# 设置工具链
export PATH="/data/prebuilts/gcc/linux-x86_64/arm-none-eabi/bin:$PATH"

# 配置（使用 SF32LB52 开发板）
cd nuttx
./tools/configure.sh -l ../vendor/sifli/boards/sf32lb52/sf32lb52_devkit_lcd/configs/nsh

# 启用应用和依赖
echo "CONFIG_LVX_USE_DEMO_CONTEST2026_201_YANTU_WATCH=y" >> .config
echo "CONFIG_NETUTILS_CJSON=y" >> .config
echo "CONFIG_BLE=y" >> .config

# 编译
make -j4
```

### 烧录

通过 SF32LB52 的烧录工具将生成的 `nuttx.bin` 写入开发板即可。

---

## 五、AI Coding 使用说明

本作品全程借助 **AtomCode (deepseek-v4-flash)** 进行 AI 辅助开发：

| 环节 | AI 协作方式 | 实际效果 |
|------|-----------|---------|
| 需求拆解 | AI 分析需求文档，生成开发方案 | 6 阶段开发计划，覆盖全部 7 大功能 |
| 架构设计 | AI 建议分层架构（UI/业务/数据/BLE） | 代码结构清晰，模块间低耦合 |
| 编码实现 | AI 生成核心代码 + 语法检查 | 23 个 C 文件，2150+ 行代码，零错误 |
| 调试优化 | AI 分析编译报错，修复 vendor BSP 问题 | 修复 3 个 vendor HAL 编译问题 |
| 代码审查 | AI 按 8 维度 59 模式进行质量审查 | 最终评分 100/100（A 级） |
| 文档编写 | AI 生成架构/UI/协议文档 | 完整文档体系 |

完整对话日志见 `logs/` 目录。