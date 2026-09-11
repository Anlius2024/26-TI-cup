# empty —— 最简空工程（DriverLib + SysConfig 起点模板）

> 芯片型号：LP_MSPM0G3507（MSPM0G3507）
> 示例来源：TI MSPM0 SDK DriverLib 例程

## 一、工程简介（这个例子是做什么的？）

- 这是一个**最基础的空工程模板**：只有一个 `main` 函数 + SysConfig 生成的初始化代码，**没有配置任何外设**，程序运行后就在 `while(1)` 里空转。
- 它的用途是**给初学者当“地基”**：想从零开始写自己的程序时，直接复制这个工程，然后在 SysConfig 里添加需要的引脚/外设，在 `main` 里写自己的代码即可。
- 工程结构：`empty.c`（主程序）+ `empty.syscfg`（SysConfig 图形化配置）+ `ti_msp_dl_config.c/h`（由 SysConfig 自动生成的配置代码）。
- 💡 打个比方：这就像「毛坯房」——水电（时钟/系统）已通好，但里面空空如也，怎么装修（加外设、写功能）全由你来定。

## 二、外设与引脚分配

| 外设 | 引脚 | 功能 |
| --- | --- | --- |
| SYSCTL |  | 系统控制（时钟等，仅保持系统正常运行） |
| DEBUGSS | PA20 | 调试时钟 SWCLK |
| DEBUGSS | PA19 | 调试数据 SWDIO |

> 本工程**没有配置任何用户外设/引脚**，只有调试接口占用 PA19/PA20。需要外设时请在 SysConfig（`.syscfg`）里添加。

### LaunchPad 跳线设置

LaunchPad 详情请访问 [LP_MSPM0G3507](https://www.ti.com/tool/LP-MSPM0G3507)（含用户指南和硬件文件）。

| 引脚 | 外设 | 功能 | LaunchPad 引脚 | LaunchPad 跳线设置 |
| --- | --- | --- | --- | --- |
| PA20 | DEBUGSS | SWCLK | 无 | <ul><li>PA20 在调试时被 SWD 占用<br><ul><li>`J101 15:16 ON` 调试时连接 XDS-110 SWCLK<br><li>`J101 15:16 OFF` 应用中使用该引脚时断开</ul></ul> |
| PA19 | DEBUGSS | SWDIO | 无 | <ul><li>PA19 在调试时被 SWD 占用<br><ul><li>`J101 13:14 ON` 调试时连接 XDS-110 SWDIO<br><li>`J101 13:14 OFF` 应用中使用该引脚时断开</ul></ul> |

## 三、代码流程说明（一步步看 main 函数）

```c
#include "ti_msp_dl_config.h"

int main(void)
{
    SYSCFG_DL_init();   // ① 初始化所有 SysConfig 里配置的外设（目前只有时钟/系统）

    while (1) {
        // ② 空循环：什么都不做。把你的业务代码写在这里
    }
}
```

💡 `SYSCFG_DL_init()` 由 SysConfig 根据 `.syscfg` 自动生成，你不用手写初始化代码。以后在 SysConfig 里加了 GPIO/TIM/UART 等外设，这一行会自动把它们的初始化都做掉。

## 四、如何编译、烧录与运行

1. 打开工程：用 CCS 打开 `ticlang/`，或用 Keil 打开 `keil/`，或用 IAR 打开 `iar/`。
2. USB 连接 LaunchPad 板载 XDS-110 调试器。
3. 编译下载运行——程序进入 `while(1)` 空转，**看不到任何现象**（这是正常的）。
4. 💡 验证方法：在 `while(1)` 里设个断点，全速运行后能停在断点上，说明程序正常运行；也可在此处添加 LED 翻转代码测试。

## 五、初学者的几个关键概念

| 概念 | 通俗解释 |
| --- | --- |
| 空工程模板 | 可复制的“地基”工程，适合从零开始开发 |
| SysConfig | TI 的图形化配置工具，点选外设/引脚，自动生成初始化代码 |
| `SYSCFG_DL_init()` | 由 SysConfig 生成的统一初始化入口函数 |
| `while(1)` 死循环 | 嵌入式主程序的经典写法，保证程序不退出 |
| DEBUGSS | 调试子系统，SWD 调试用 |

## 六、注意事项

- **建议从空工程起步**：新项目直接复制本工程，避免从其他例程里删删改改留下隐患。
- **SysConfig 是核心**：加外设时用 `empty.syscfg` 图形化配置，保存后会自动重新生成 `ti_msp_dl_config.c/h`，不要手改这两个生成文件。
- **低功耗建议**：把不用的引脚在 SysConfig 里配置为 GPIO 输出低或内部上/下拉（**Board → Configure Unused Pins**），详见 [LP-MSPM0G3507 用户指南](https://www.ti.com/lit/slau873)。
- **移植到其他芯片**：请参考 [CCS 用户指南 - SysConfig 工程迁移](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)。
