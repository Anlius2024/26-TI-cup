# 26-TI-cup

**2026 年 TI 杯模拟电子系统设计专题邀请赛 —— 电路网络特性测量装置 备赛工程集**

本仓库是参加 2026 年 TI 杯模拟电子系统设计专题邀请赛（"电路网络特性测量装置"，赛题见 [`docs/00_PROBLEM_2026.md`](docs/00_PROBLEM_2026.md)）期间的**硬件仿真模块**与 **MSPM0G3507 固件工程**归档。内容以工程快照形式保留，包含可运行的部分、未完成的探索和失败的迭代，供后续接手者参考。

- 主控平台：**TI MSPM0G3507**（Arm Cortex-M0+ @ 80 MHz，LP-MSPM0G3507 LaunchPad）
- 开发环境：**CCS 20.x (Theia-based) + MSPM0 SDK + SysConfig + TI Arm Clang**
- 硬件设计：**NI Multisim (`.ms14`)**

---

## 目录结构

```
26-TI-cup/
├── docs/                       文档
│   ├── 00_PROBLEM_2026.md        2026 年赛题（逐字录入）+ 要点拆解
│   ├── 10_HARDWARE_MODULES.md    硬件模块说明与实测结论
│   ├── 20_DEV_ENVIRONMENT.md     开发环境与编译烧录方法
│   └── 90_THIRD_PARTY_NOTICES.md 第三方代码来源与授权说明
├── hardware/                   模拟电路 Multisim 仿真模块
│   ├── 01_RLC_measurement/       RLC 参数测量（三仪表放大器/电桥/峰值检波/鉴相/最终电路）
│   ├── 02_voltage_source/        稳压电源（LM117 / LM317+LM337）
│   ├── 03_current_source/        恒流源
│   ├── 04_wien_bridge/           文氏桥振荡器（含教程 PDF）
│   ├── 05_level_shift/           直流抬升电路
│   ├── 06_programmable_amp/      程控放大器（数字电位器 / VCA810）
│   ├── 07_MC1496_modulator/      MC1496 AM / DSB 调制
│   └── 08_misc/                  74HC161 倍频 / 加法器 / 50 Hz 陷波器 / 移相器
└── firmware/                   MSPM0G3507 CCS 工程
    ├── 01_LCD_1.8/               1.8" SPI LCD 驱动
    ├── 02_LCD_2.4/               2.4" ILI9341 SPI LCD + GUI
    ├── 03_OLED/                  0.96" I2C OLED 驱动
    ├── 04_LCD_key_matrix/        LCD + 4×4 矩阵键盘
    ├── 05_digital_resistor/      TPL0401A-10 数字电位器驱动
    ├── 06_ADC_4ch/               ADC12 四通道序列采样
    ├── 07_AD9850_DDS/            AD9850 DDS 正弦信号源
    ├── 08_ADC_digital_resistor/  AD9850 + 数字电位器联合
    ├── 09_optical_tx_B/          **26 邀请赛 B 题发送端**（DDS + 程控增益 + 键盘 + LCD）
    ├── 10_tim_adc_dma/           TIM 触发 ADC + DMA + FFT
    └── experimental/             失败/中间版本迭代（保留演进过程）
        ├── 09_error/               含可见光通信发送模块 optical_tx.c
        └── 10_error/
```

---

## 题目要求 → 本仓库实现对照

| 赛题要求 | 对应实现 | 状态 |
|---|---|---|
| 发生端产生 10 Hz–20 kHz 正弦 | `firmware/07_AD9850_DDS`、`09_optical_tx_B` | 可用 |
| 峰峰值 1–3 V 可设、步长 0.1 V | `firmware/09_optical_tx_B` + `hardware/06_programmable_amp` | 可用 |
| 可见光数字 + 模拟通信 | `firmware/experimental/09_error/optical_tx.c` | 部分集成到发送端工程 |
| 检测端显示幅频/相频曲线 | `firmware/02_LCD_2.4`（ILI9341，含 `LCD_Spectrum` 频谱绘图）| 显示驱动可用，测量链路未闭环 |
| 检测端显示冲激响应曲线 | 同上 + `firmware/10_tim_adc_dma` | 采样链路可用，冲激响应未实现 |
| 频率特性测量（FFT）| `firmware/10_tim_adc_dma`（TIM 触发 ADC+DMA+FFT）| 单点采频可用，扫频未闭环 |
| 被测网络 RLC 测量 | `hardware/01_RLC_measurement` | 仿真验证完成，未接入固件 |
| 参数设置交互（发生端）| `firmware/04_LCD_key_matrix`、`09_optical_tx_B` | 可用 |

> **总体状态**：各**模块**基本打通，**系统级闭环尚未完成**。主要缺口是「扫频—测量—显示」的自动化流程。

---

## 关键成果

### 1. 可见光通信协议（Manchester 编码）

位于 `firmware/experimental/09_error/optical_tx.c` / `.h`：

- **物理层**：GPIO 推挽驱动白光 LED（PB14），软件 `delay_cycles` 精确延时
- **编码**：Manchester 编码，比特率 **1 kHz**（半周期 500 µs，实测补偿值 `manchester_delay = 32150`）
- **帧结构**（8 字节，编码后 16 字节 ≈ 107 ms）：

  ```
  [0xAA] [CMD] [Freq_H] [Freq_L] [Volt] [Seq] [Checksum] [0x55]
  ```

- **命令字**：`0x01` 配置、`0x02` 扫频开始、`0x03` 单次触发、`0x04` 心跳同步
- **参数范围校验**：频率 10–20000 Hz，电压 ×10 编码 10–30（即 1.0–3.0 V）
- **可靠性**：上电自动重发 3 次；帧尾校验和

> ⚠️ **已知的性能瓶颈**：按 1999 个频点、每点 107 ms 传输计算，完整扫频需 **314 秒**，远超赛题"每项测量 ≤ 1 分钟"的限制。源码注释中已记录该问题并给出优化思路（只传起始参数、检测端按规则自增），但**尚未实现**。参见 `optical_tx.c` 第 206–221 行。

### 2. RLC 参数测量方案

`hardware/01_RLC_measurement/RLC_Final.ms14` 采用**间接测量**思路：

1. 被测元件与已知参考电阻串联
2. 用两个三仪表放大器（本质减法器）分别提取两者两端的**差分电压**
3. 经过零比较器得到相位信息，交给单片机
4. 由**幅值比**与**相位差**两个标量反算 R / L / C

这样避开了高频下直接测阻抗的困难。

### 3. FFT 采样链路

`firmware/10_tim_adc_dma`：TIM 定时触发 ADC12 → DMA 搬运 → `arm_cfft_radix4_f32` 变换 → 频谱绘图 + 峰值分析。

---

## 已知问题

以下问题在源码中真实存在，**归档时未修改代码**，特此标注以免误导。

### `hardware`

- 文氏桥高频版**实物未起振**（仿真正常）。
- 峰值检波在 **400 kHz 以上性能劣化**（二极管旁路电容影响）。

完整硬件问题清单见 [`docs/10_HARDWARE_MODULES.md`](docs/10_HARDWARE_MODULES.md)。

---

## 快速上手

```bash
git clone https://github.com/Anlius2024/26-TI-cup.git
```

1. 安装 **CCS 20.x** 与 **MSPM0 SDK**（见 [`docs/20_DEV_ENVIRONMENT.md`](docs/20_DEV_ENVIRONMENT.md)）
2. CCS 中 `File → Import → CCS Projects`，选择 `firmware/` 下任一工程目录
3. 双击工程根目录的 `.syscfg` 可查看/修改外设与引脚配置
4. 编译后通过板载 **XDS-110** 调试器烧录
5. `hardware/` 下的 `.ms14` 需用 **NI Multisim** 打开

---

## 说明

- 归档时对源码做了**编码统一（GBK→UTF-8）**、**剥离第三方厂商联系方式**、**移除机器相关绝对路径**等整理，代码逻辑未作改动。详见 [`docs/90_THIRD_PARTY_NOTICES.md`](docs/90_THIRD_PARTY_NOTICES.md)。
- 各工程自带的 CCS 生成版 `README.md` / `README.html`（TI 模板内容）予以保留。
- 部分工程目录下的 `prompt.txt` 是开发期间使用的 AI 辅助提示词记录，作为开发过程留档保留。

## 许可

本项目以 **MIT License** 发布，详见 [`LICENSE`](LICENSE)。

其中 TFT LCD 驱动源自第三方参考代码，授权说明见 [`docs/90_THIRD_PARTY_NOTICES.md`](docs/90_THIRD_PARTY_NOTICES.md)。
