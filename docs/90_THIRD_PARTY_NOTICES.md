# 第三方代码来源与授权说明

本仓库在整理归档时，对第三方代码做了**编码修复**与**厂商联系信息剥离**。本文档说明来源与处理方式。

---

## 一、QDtech / lcdwiki TFT LCD 驱动

### 涉及文件

| 文件 | 说明 |
|---|---|
| `firmware/02_LCD_2.4/src/lcd.c`, `lcd.h` | ILI9341 2.4" TFT 驱动 |
| `firmware/02_LCD_2.4/src/GUI.c`, `GUI.h` | 绘图与字符显示层 |
| `firmware/02_LCD_2.4/src/FONT.H` | ASCII 字库与汉字点阵 |
| `firmware/04_LCD_key_matrix/src/` 下同名文件 | 同一套驱动的副本 |
| `firmware/01_LCD_1.8/src/lcd.c` 等 | 同源的 1.8" 版本 |
| `firmware/09_optical_tx_B/lcd*.c/h`、`firmware/experimental/*/lcd*.c/h` | 同源副本 |
| `*/src/lcd111.c.txt`、`lcd_init111*.txt` 等 | 带英文注释的参考版本 |

### 原始出处

该驱动源自 **QDtech（深圳市全动电子技术有限公司）/ lcdwiki** 为 STM32 提供的 TFT 液晶模块配套例程，原始文件头包含：

- 公司网站 `www.qdtft.com`、淘宝站点、wiki 技术站 `www.lcdwiki.com`
- 固话/传真、**手机号**、**邮箱**、**技术支持 QQ**、**技术交流 QQ 群**
- 版权声明：`Copyright(C) 深圳市全动电子技术有限公司 2018-2028`，版本 V1.0，日期 2018/08/09
- 英文免责声明：`THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY…QD electronic SHALL NOT BE HELD LIABLE…`

本项目将上述代码**从 STM32 平台移植到 TI MSPM0G3507**，改用 TI DriverLib（`DL_GPIO_*` 等 API），硬件接线由 SysConfig 重新配置。

### 归档时的处理

1. **移除全部厂商联系信息** —— 手机号、固话、邮箱、QQ 号（含技术支持 QQ 与 QQ 群）均已从源码中删除，仅保留在中性的"出处说明"注释里提及 QDtech / lcdwiki 名称与网址，以便溯源。
2. **保留原始免责声明** —— 英文 `@attention` 免责条款（QD electronic 不对固件适用性负责）原样保留。
3. **重建损坏的注释** —— 见下节。
4. **编码统一** —— 原文件为 GBK，已转为 UTF-8。

### ⚠️ 关于注释损坏（重要）

`lcd.c` / `lcd.h` / `GUI.c` / `GUI.h`（2.4" 版本）中的中文注释在归档**之前**就已损坏。经字节级分析确认原因：

> 文件曾被以错误编码另存 —— 原本的 GBK 中文被当作某种编码转换，最终每个中文字符被写成了 UTF-8 的替换字符 `U+FFFD`（字节序列 `EF BF BD`）。残留的个别 GBK 字节（如 `D6 BB` = "只"）证明原文确为 GBK。

**该损坏不可逆** —— 原文信息已丢失，无法通过任何编码转换恢复。

处理方式：

- 头部版权/接线说明块，依据**同一套驱动中未损坏的 `FONT.H`**（同目录，GBK 完好，含完整接线表）**重建**为英文说明，并保留原始 STM32 接线对照表
- 其余零散损坏的注释（纯注释行 80 行、行尾注释 212 处）**予以删除**，未臆造内容
- 代码本体**未作任何逻辑改动** —— 归档后已用括号配平与语法结构校验确认无破坏

### 授权状态

QDtech 原始声明为"本程序只供学习使用"（`for study only`）。本仓库对该驱动的使用与再分发**遵循原声明，仅用于学习与非商业用途**。若需商业使用，请自行联系原作者获取授权。

本项目自身新增/修改的部分（MSPM0G3507 移植适配、登记整理）以 MIT License 发布。

---

## 二、TI 生成的工程模板文件

各 `firmware/*` 目录下的 `.project` / `.cproject` / `.ccsproject` / `targetConfigs/*.ccxml` 与 `README.md` / `README.html`，由 **TI Code Composer Studio 与 MSPM0-SDK 的工程模板**自动生成，版权归 Texas Instruments，按其 SDK 许可条款使用。

其中 `README.md` / `README.html` 部分被原作者扩写为中文说明（如 `03_OLED`、`05_digital_resistor`、`06_ADC_4ch`、`09_optical_tx_B`），这些增量内容随本项目以 MIT 发布。

---

## 三、`文氏桥教程.pdf`

位于 `hardware/04_wien_bridge/文氏桥教程.pdf`。

这是 `hardware/` 模块设计中**唯一收录的第三方教程 PDF**（其余备课教程 PDF 未纳入本仓库）。原始来源与作者未知，为网络流传的电路设计教程。**版权归原作者所有**，此处仅作为工程设计参考一并归档。

若原作者认为不妥，请联系仓库维护者删除。

---

## 四、CMSIS-DSP

`firmware/10_tim_adc_dma` 使用 **ARM CMSIS-DSP** 库（`arm_cfft_radix4_f32` 等）做 FFT 运算。

CMSIS-DSP 由 ARM 提供，以 **Apache License 2.0** 发布，通过 MSPM0-SDK 分发，本仓库不重复打包该库。

---

## 五、归档时的其他整理（非第三方内容）

以下整理针对本项目**自己的代码**，不影响第三方授权：

| 处理 | 说明 |
|---|---|
| 编码统一 | 6 个 GBK 文件转为 UTF-8（含 3 个 `prompt.txt`、2 个 `FONT.H`、`06_ADC_4ch/ADC.c`）|
| 移除机器路径 | 12 个 `.ccsproject` 中的 `<origin value="...">` 含本机绝对路径（`E:\college\...`、`C:\ti\...`）与用户名，已删除 |
| 排除编译产物 | `Debug/`（含 `.o` / `.out` / `.map` / clangd 索引缓存）未纳入仓库 |
| 排除自动生成文件 | 12 个 `.clangd`（文件自称 "auto-generated - do not add it to source-control"）已删除 |
| 去重 | `module_design` 下 26 个 `(Security copy)` 文件（Multisim 崩溃保护副本）未纳入 |
| 保留调试配置 | `targetConfigs/MSPM0G3507.ccxml` 予以保留（被 `.ccsproject` 引用，缺失会导致导入告警）|

**代码逻辑本身未作任何修改**，包括其中已知的缺陷（见主 [README](../README.md#已知问题)）。

---

## 六、汇编与声明

本仓库的 `hardware/`（Multisim 仿真）与 `firmware/`（MSPM0G3507 固件）中的原创设计内容，版权归 **Anlius2024** 所有，以 MIT License 发布。

第三方内容按上述各自条款处理。如任何权利人认为本仓库的使用方式不当，请通过仓库 Issue 联系，我们将及时处理。
