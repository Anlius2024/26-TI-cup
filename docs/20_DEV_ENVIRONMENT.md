# 开发环境与编译烧录

## 一、实际使用的环境版本

以下版本从各工程的 `.cproject` / `.ccsproject` 中**直接读取**，为归档时的真实配置。

| 项目 | 版本 |
|---|---|
| IDE | **Code Composer Studio 20.x**（Theia-based，`ccsVersion 71.0.0`）|
| 器件 | **MSPM0G3507**（`DEVICE_CONFIGURATION_ID=Cortex M.MSPM0G3507`）|
| SDK | **MSPM0-SDK 2.11.0.07** |
| SysConfig | **1.28.0** |
| 编译器 | **TI Arm Clang `TICLANG_5.1.1.LTS`** |
| RTOS | **NoRTOS**（裸机）|
| 调试器 | 板载 **XDS-110**（`TIXDS110_Connection`）|
| 优化等级 | `-O2`（Debug 配置默认值）|
| 硬件仿真 | **NI Multisim**（`.ms14` 格式，`MSMCompressedEle` 私有压缩）|

> **注意**：仓库根目录下的学长代码（不包含在本仓库）使用的是 CCS 12.6.0 + MSPM0 SDK 2.01.00.03 + SysConfig 1.19.0 + TICLANG_3.2.1.LTS，与本仓库不同，不要混用。

---

## 二、环境安装

1. **Code Composer Studio**：从 [TI 官网](https://www.ti.com/tool/CCSTUDIO) 下载 CCS 20.x，安装时勾选 **MSPM0** 器件支持。
2. **MSPM0 SDK**：CCS 内置 Resource Explorer 可直接安装；或从 [MSPM0-SDK 下载页](https://www.ti.com/tool/MSPM0-SDK) 获取 `mspm0_sdk_2_11_00_07`。
3. **SysConfig**：随 SDK / CCS 一起安装，通常无需单独装。
4. **TI Arm Clang**：CCS 安装时自动获取 `TICLANG_5.1.1.LTS`。

安装完成后，在 CCS 中确认 `Windows → Preferences → Code Composer Studio → Products` 里 MSPM0-SDK 与 SysConfig 已被识别。

---

## 三、导入工程

每个 `firmware/*` 子目录都是一个**独立的 CCS 工程**（含 `.project` / `.cproject` / `.ccsproject`）。

```
File → Import… → Code Composer Studio → CCS Projects
  → Select search-directory: <仓库路径>/firmware/<工程名>
  → 勾选列出的工程 → Finish
```

**更省事的做法**：`firmware/` 下批量导入 —— 在 Import 对话框把 search-directory 指向 `firmware/`，CCS 会自动发现全部子工程（`experimental/` 下的两个也会一并列出）。

导入后工程名形如 `empty_LP_MSPM0G3507_nortos_ticlang`，多个工程同名属正常（它们来自同一 TI 模板）。

---

## 四、工程结构与关键文件

```
<工程目录>/
├── .project / .cproject / .ccsproject   CCS 工程元数据
├── <name>.syscfg                        ★ SysConfig 外设与引脚配置（图形化）
├── <name>.c                             主程序
├── *.c / *.h                            业务与驱动模块
├── targetConfigs/MSPM0G3507.ccxml       ★ XDS-110 调试目标配置
└── README.md / README.html              CCS 生成的工程说明
```

### 关于 `ti_msp_dl_config.c/h`

这两个文件**不在仓库中** —— 它们由 SysConfig 在构建时自动生成到 `Debug/syscfg/`。这是 TI 的标准工作流：

```
双击 .syscfg 用 SysConfig 图形化配置外设
        ↓
构建时自动生成 ti_msp_dl_config.c / .h（DriverLib 初始化代码）
        ↓
业务代码调用 DL_* API
```

所以 **`Debug/` 目录被有意排除在仓库之外**（编译产物 + 生成代码），首次构建时 CCS 会重新生成。

### 关于 `targetConfigs/`

已保留。其中的 `MSPM0G3507.ccxml` 定义 XDS-110 调试探针与器件连接，被 `.ccsproject` 的 `activeTargetConfiguration` 引用。若缺失，CCS 导入时会提示目标配置无效。

---

## 五、编译与烧录

1. 确认 LaunchPad 已通过 USB 连接，设备管理器中出现 **XDS110** 调试端口
2. 选中工程 → 右键 `Build Project`（快捷键 `Ctrl+B`）
3. 点击 `Debug`（虫子图标）或 `Run → Load`
4. CCS 会自动编译、烧录并进入调试会话

生成的固件为 `Debug/<工程名>.out`。

### 常见问题

| 现象 | 原因与处理 |
|---|---|
| `ti_msp_dl_config.h` 找不到 | SysConfig 未生成。右键工程 → `Build`，或检查 `.syscfg` 是否被正确关联 |
| 提示 MSPM0-SDK 版本不匹配 | 工程锁定 2.11.0.07；若本机版本不同，可在工程属性 → `Products` 中改为本机已装版本 |
| 目标配置无效 / 无法连接 | 缺少 `targetConfigs/MSPM0G3507.ccxml`；从同仓库其它工程复制一份即可 |
| 串口无输出 | 检查 `.syscfg` 中 UART 引脚；`10_tim_adc_dma` 用 `BSP/uart.c` 输出 FFT 结果 |
| 编译报中文注释乱码 | 本仓库源码**已统一为 UTF-8**，如仍乱码请在编辑器里确认编码为 UTF-8 |

---

## 六、各工程依赖的外部组件

| 工程 | 额外依赖 |
|---|---|
| `10_tim_adc_dma` | **CMSIS-DSP**（`arm_cfft_radix4_f32`），通过 MSPM0-SDK 内的 CMSIS 提供 |
| `02_LCD_2.4` / `04_LCD_key_matrix` | 需外接 ILI9341 2.4" SPI 模块 |
| `01_LCD_1.8` | 需外接 ST7735 1.8" SPI 模块 |
| `03_OLED` | 需外接 SSD1306 0.96" I2C OLED |
| `05_digital_resistor` / `08_...` / `09_...` | 需外接 **TPL0401A-10** 数字电位器（I2C）|
| `07_AD9850_DDS` / `08_...` / `09_...` | 需外接 **AD9850** DDS 模块（串行模式）|

**引脚分配请以各工程的 `.syscfg` 为准** —— 不同工程即使功能相同，引脚也可能不同（例如 `09_optical_tx_B` 的 LED 用 PB14）。
