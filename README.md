# 海歌 · UV-K6 定制固件

**泉盛 UV-K6 V1 的第三方开源固件 — 中文界面，功能完整，面向业余无线电爱好者。**

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/hardware-UV--K6%20V1%20(DP32G030)-orange.svg)](#适用机型)

[English](README_EN.md) | 中文

---

> [!WARNING]
> **刷机有风险，操作前请务必备份 EEPROM。**
> 刷机造成的任何损坏，项目作者不承担责任。
> 本固件为爱好者自行开发，与泉盛官方无任何关联。

---

## 目录

- [这是什么](#这是什么)
- [适用机型](#适用机型)
- [功能一览](#功能一览)
- [快速上手](#快速上手)
- [菜单功能说明](#菜单功能说明)
- [常见问题](#常见问题)
- [版本历史](#版本历史)
- [技术文档（开发者）](#技术文档开发者)
- [致谢与许可证](#致谢与许可证)

---

## 这是什么

**海歌**（英文：hygg）是针对泉盛 UV-K6 V1 的第三方开源固件，基于
[egzumer 的 UV-K5 自定义固件](https://github.com/egzumer/uv-k5-firmware-custom)
进行深度定制开发，专注于中文用户体验和实用功能增强。

**主要亮点：**

- 🇨🇳 **完整中文菜单** — 标签对照泉盛官方手册，显示准确自然
- 📺 **频谱分析仪** — 实时图形化显示当前频段信号
- ⌨️ **T9 多键输入** — 用数字键盘像老式手机一样命名信道
- 🔋 **自动关机 APO** — 长时间无操作自动休眠，省电省心
- 🎚️ **快捷调静噪** — 主界面 F+↑/↓ 一键调整静噪等级
- 💡 **背光渐灭** — 背光超时时平滑淡出，不再生硬关灯

---

## 适用机型

> **只支持 UV-K6 V1（DP32G030 主控 + 512 KB EEPROM）。**
> 不支持 UV-K6 V3 / UV-K1（PY32F071 主控）。

### 如何判断你的 UV-K6 是否兼容？

拆开电池仓，查看电路板上的芯片标识：

| 主控芯片 | EEPROM 芯片 | 是否支持 |
|---------|------------|--------|
| **DP32G030** | **AT24C512**（512 KB） | ✅ 完全支持 |
| DP32G030 | 其他（早期 8 KB 版） | ⚠️ 固件可刷，但中文字体无法显示 |
| PY32F071 | — | ❌ 不支持（V3 硬件） |

> 业余无线电社区约定：无论 PCB 版本 V1 还是 V2，只要主控是 DP32G030，统称 "V1"；
> 换用 PY32F071 主控的才是真正的 V3。

---

## 功能一览

### 🆕 海歌新增与改进

| 功能 | 说明 | 操作方式 |
|------|------|---------|
| **完整中文菜单** | 对照泉盛官方中文手册，标签准确自然 | 菜单自动显示 |
| **菜单重构（上下布局）** | 标题大字居上，设置值居中，彻底解决截断问题 | — |
| **T9 多键信道命名** | 数字键多次按键循环字母，支持大小写切换 | 菜单 → 编辑名称 |
| **快捷调节静噪** | 主界面一键加减静噪等级（0–9），立即生效并保存 | F + ↑ / F + ↓ |
| **背光渐灭效果** | 背光超时后约 500ms 平滑淡出，不再瞬间关灯 | — |
| **自动关机（APO）** | 无操作超时自动深度休眠，任意键唤醒 | 菜单 → 自动关机（0–30 分钟） |
| **FM 调频优化** | 换台前自动静音，消除切台爆音 | FM 模式 ↑/↓ |
| **扫频保存默认高功率** | 扫频后保存信道默认使用 HIGH 发射功率 | — |
| **压扩噪声优化** | RX 展宽噪声门限 56→62 dB，减少底噪放大 | — |
| **手电筒功能（中文版已启用）** | 顶部 LED 常亮/闪烁/SOS | F + 手电筒键 |
| **频率精度修复** | 信道频率统一显示 5 位小数（如 435.00000 MHz） | — |
| **删除信道防误操作** | 删除信道移至菜单末尾，避免浏览时误触 | — |

### 📻 继承自上游的核心功能

| 类别 | 功能 | 说明 |
|------|------|------|
| **接收** | 频谱分析仪 | 实时图形化显示频段信号强度（F+5） |
| | AM 增强接收 | 动态前端增益，航空段及 AM 接收效果大幅改善 |
| | FM 广播收音 | 内置调频广播接收（87–108 MHz） |
| | 宽接收范围 | 18 MHz – 1300 MHz 全频段接收 |
| **显示** | RSSI 信号强度条 | 图形化 S 表，实时显示 dBm 信号强度 |
| | 音频电平条 | 发射时实时显示话音电平 |
| | 大字体频率 | 主界面频率数字放大显示 |
| | 电量显示 | 电压值 / 百分比 / 图形电量条 |
| **操作** | 双守候（TDR） | 同时监听两个频率或信道 |
| | 可配置功能键 | F1/F2 短按长按、菜单长按，自由分配功能 |
| | 快速扫描 | 信道扫描、频率范围扫描，含 CTCSS/DCS 自动扫描 |
| | 省电模式 | 多级节电，延长续航 |
| **发射** | 加密通话 | 8 种扰码加密模式 |
| | DTMF 拨号 | DTMF 自动 PTT-ID（默认关闭，编译时可开启） |
| | 发射超时（TOT） | 防止意外长时间占用频道 |
| **设置** | 背光独立调节 | 最小亮度、最大亮度、超时时间分开设置 |
| | 频率步进 | 2.5/5/6.25/10/12.5/25/50/100 kHz 可选 |
| | 电压校准 | EEPROM 内置电压校准工具 |

---

## 快速上手

> 刷机全程约 5 分钟，**无需安装专用软件**，使用浏览器即可完成。

### 准备工作

- 泉盛 UV-K6 V1 一台（参见[适用机型](#适用机型)）
- Micro USB 数据线
- 电脑（Windows / macOS / Linux），Chrome 或 Edge 浏览器

---

### 步骤一：备份原始数据（**强烈建议，请勿跳过**）

1. 打开 [UV-K5 网页刷机工具](https://egzumer.github.io/uvtools)
2. 电台开机后插入 USB 数据线
3. 点击 **Read** 等待读取完成
4. 点击 **Save** 将备份文件保存到本地

> 💾 备份文件包含你的所有频道、设置和校准数据。出现问题时可随时恢复。

---

### 步骤二：刷入固件

1. 在 [Releases 页面](../../releases) 下载最新版 `firmware.packed.bin`
2. 打开 [UV-K5 网页刷机工具](https://egzumer.github.io/uvtools)
3. 点击 **Flash** → 选择下载的 `firmware.packed.bin`
4. 再次点击 **Flash** 开始刷写，等待进度条完成
5. 电台自动重启 — 固件刷写完成 ✅

---

### 步骤三：刷入中文字库（**中文版必须**）

中文菜单需要一个存储在电台 EEPROM 中的点阵字库文件。
**不刷字库，中文将显示为空白方块。**

安装 Python 依赖（仅需一次）：

```bash
pip install pyserial
```

电台开机并连接 USB，然后运行：

```bash
# macOS / Linux
python3 tools/flash_font.py --port /dev/ttyUSB0 --font tools/cjk_font.bin

# macOS（CP210x USB 转串口适配器）
python3 tools/flash_font.py --port /dev/tty.usbserial-XXXX --font tools/cjk_font.bin
```

```bat
:: Windows（将 COM3 替换为实际端口号）
python tools\flash_font.py --port COM3 --font tools\cjk_font.bin
```

工具会逐页写入并自动验证，显示 "Done" 后即完成。

> 字库存储于 EEPROM 独立区域，重刷固件不会覆盖，通常只需刷一次。

---

## 菜单功能说明

### 快捷键（主界面）

| 按键 | 功能 |
|------|------|
| `F` + `↑` | 静噪等级 +1（0–9） |
| `F` + `↓` | 静噪等级 −1（0–9） |
| `F` + `5` | 打开频谱分析仪 |
| `F` + 手电筒键 | 手电筒循环切换（常亮/闪/SOS/关） |
| `*` 长按 | 键盘锁定/解锁 |
| `F` 长按 | 打开菜单 |

### 常用菜单项速查

| 中文标签 | 英文名 | 功能说明 |
|---------|-------|---------|
| 静噪级别 | SQL | 静噪灵敏度（0–9），0 为全开 |
| 自动背光 | ABR | 背光超时时间 |
| 自动关机 | AutoOff | APO 无操作休眠（0=关，1–30 分钟） |
| 发射功率 | TXP | 低 / 中 / 高功率 |
| 发射超时 | TOT | 自动停止发射倒计时 |
| 双守候 | TDR | 双频同时监听 |
| 省电模式 | Save | 电池节电等级（1/2/3/4/5/Off） |
| 存储信道 | SaveCh | 将当前频率保存为信道 |
| 编辑名称 | ChName | T9 输入法编辑信道名称 |
| 语言 | Lang | 中文 ↔ English 切换 |
| 频差方向 | SFT-D | 中继偏移方向（+/−/Off） |
| 频差频率 | Offset | 中继偏移频率 |

### T9 信道名称输入法

进入**编辑名称**菜单后：

| 操作 | 说明 |
|------|------|
| 数字键 `2`–`9` 重复按 | 循环选择对应字母（如 `2`→A→B→C→2） |
| `0` | 空格 |
| `1` | 1、`-`、`.` |
| 按下不同数字键 | 自动确认当前字符，光标右移 |
| 静置 1.5 秒 | 自动确认当前字符，光标右移 |
| 短按 `MENU` | 确认字符，光标右移 |
| 长按 `MENU` | 保存名称（进入确认界面） |
| `EXIT`（非首位） | 退格删除上一字符 |
| `EXIT`（首位） | 取消，恢复原名称 |
| `F` | 切换大写 / 小写 |

---

## 常见问题

**Q：刷完固件中文显示方块怎么办？**
→ 需要单独刷入中文字库，参见[步骤三](#步骤三刷入中文字库中文版必须)。

**Q：电台刷砖了怎么恢复？**
→ UV-K6 强制进入刷机模式：**关机状态下**同时按住 `PTT` + `↑`，再按电源键开机。
然后用网页刷机工具重新刷入备份的原版固件，或本固件即可。

**Q：自动关机期间能收到呼叫吗？**
→ **不能。** APO 休眠时 RF 芯片完全停止工作，无法接收信号。
如需值守频道，请在菜单中将 AutoOff（自动关机）设为 0（关闭）。

**Q：我的是 UV-K6 V3 / UV-K1，能刷吗？**
→ 不能。本固件仅支持 DP32G030 主控的 V1 硬件。V3 和 UV-K1 使用不同主控芯片，不兼容。

**Q：刷完后信道数据还在吗？**
→ 只刷固件通常不影响信道和设置数据。但强烈建议刷机前先备份 EEPROM，以防万一。

**Q：如何恢复出厂设置？**
→ 菜单 → 滚动到末尾 **Reset** → 选择 **FULL**。
注意：这会清除所有信道和设置，电压校准数据不受影响。

---

## 版本历史

详见 [CHANGELOG.md](./CHANGELOG.md)。

---

## 技术文档（开发者）

> 以下内容面向想自行编译固件的开发者或高级用户。普通用户跳过即可。

### 编译环境准备

**macOS**
```bash
# 推荐使用 ARM GNU Toolchain 15.x（从 developer.arm.com 下载）
# 或通过 Homebrew：
brew install --cask gcc-arm-embedded
```

**Ubuntu / Debian / WSL2**
```bash
sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi make python3-crcmod
```

**Windows（Docker，推荐）**
```bat
docker-make.bat
```
首次运行自动构建工具链镜像（Ubuntu 24.04），之后挂载源码编译，输出到 `compiled-firmware\`。

### 编译命令

```bash
# 标准版（含中文，默认）
make PROFILE=release ENABLE_CHINESE=1

# 纯英文版
make PROFILE=release

# 清理重新编译
make clean && make PROFILE=release ENABLE_CHINESE=1
```

完整编译约 6 秒，单文件增量编译约 1–2 秒。  
输出：`firmware.bin`（原始二进制）、`firmware.packed.bin`（含 CRC 的可刷写镜像）。

### 编译开关

在 `Makefile` 顶部修改，或用命令行 `make OPTION=1` 覆盖：

| 选项 | 默认 | 说明 |
|------|------|------|
| `ENABLE_CHINESE` | **1** | 中文 UI，自动关闭 VOX 以适应 flash |
| `ENABLE_UART` | 1 | PC 端 UART 配置通信 |
| `ENABLE_FMRADIO` | 1 | 调频广播接收 |
| `ENABLE_SPECTRUM` | 1 | 频谱分析仪 |
| `ENABLE_AM_FIX` | 1 | AM 动态增益控制 |
| `ENABLE_FLASHLIGHT` | 1 | 顶部 LED 手电筒 |
| `ENABLE_RSSI_BAR` | 1 | 信号强度 S 表条 |
| `ENABLE_AUDIO_BAR` | 1 | 发射音频电平条 |
| `ENABLE_BIG_FREQ` | 1 | 大字体频率显示 |
| `ENABLE_WIDE_RX` | 1 | 18 MHz – 1300 MHz 宽接收 |
| `ENABLE_SCAN_RANGES` | 1 | 频率范围扫描 |
| `ENABLE_NO_CODE_SCAN_TIMEOUT` | 1 | 关闭 CTCSS/DCS 32秒超时 |
| `ENABLE_VOX` | 0† | 声控发射 |
| `ENABLE_DTMF_CALLING` | **0** | DTMF 呼叫系统（约占 3 KB） |

† `ENABLE_CHINESE=1` 时自动关闭。

完整选项说明见 [Makefile](./Makefile) 注释。

### 代码结构

```
uv-k6-dp32-firmware/
├── app/          应用层（菜单逻辑、DTMF、扫描、频谱、主界面按键）
├── driver/       硬件驱动（BK4819 RF、BK1080 FM、ST7565 LCD、EEPROM、UART）
├── ui/           显示渲染（菜单 UI、主界面、电池、FM、输入框）
├── helper/       辅助（电池计算、开机自检）
├── external/     外部依赖（ARM CMSIS_5、tiny printf）
├── bsp/          板级支持（DP32G030 外设寄存器映射）
├── tools/        工具脚本（字库生成、字库刷写）
├── profiles/     编译配置（debug.mk / release.mk）
├── Makefile      构建系统
├── firmware.ld   链接脚本（64 KB Flash / 16 KB RAM）
└── fw-pack.py    CRC 打包，生成 .packed.bin
```

### 分支说明

| 分支 | 用途 |
|------|------|
| `main` | 主分支，发布版本 |
| `dev` | 开发分支，合并前的工作区 |
| `vendor/egzumer-v0.22` | 上游基线快照，锁定不修改 |

---

## 致谢与许可证

### 致谢

海歌固件建立在以下开发者的工作基础上：

- [egzumer](https://github.com/egzumer) — 主要上游，功能整合
- [OneOfEleven](https://github.com/OneOfEleven) — AM 增强接收、快速扫描、可配置按键
- [fagci / Mikhail](https://github.com/fagci) — 频谱分析仪
- [DualTachyon](https://github.com/DualTachyon) — 最初的开源固件
- [Andrej](https://github.com/Tunas1337)、[Manuel](https://github.com/manujedi) 及众多社区贡献者

### 许可证

**Apache License 2.0**，继承自上游。

原始版权：Copyright 2023 Dual Tachyon / egzumer。  
详见 [LICENSE](./LICENSE)。
