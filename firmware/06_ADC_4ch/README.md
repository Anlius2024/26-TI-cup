# adc12_sequence_conversion —— ADC 顺序转换：一次触发采集 4 个通道

> 芯片型号：LP_MSPM0G3507（MSPM0G3507）
> 示例来源：TI MSPM0 SDK DriverLib 例程

## 一、工程简介（这个例子是做什么的？）

这个例子演示了 ADC 的**顺序（序列）转换**功能：**一次触发，依次把 4 个不同的输入通道各采一遍**，结果分别存进 4 个数组：

- 🎯 4 个通道：通道 2（PA25）、通道 0（PA27）、通道 7（PA22）、通道 3（PA24），依次转换。
- 📦 每个通道的结果存入各自的缓冲区：`gAdcResult0 ~ gAdcResult3`，每个缓冲区能存 64 个采样值。
- 🔁 程序**循环采集**：采满 64 组后停在断点，然后清空下标重新开始。
- ⏱ 中断里监听的是**最后一个通道（MEM3）**的结果装载事件，用它来代表「这一轮 4 个通道全部采完」。

💡 类比：顺序转换就像「排队打饭」——4 个窗口（通道）排好队，师傅（ADC）一个一个轮流打，全部打完才算一轮。适合需要同时监测多个传感器（如温度、电压、电流）的场景。

## 二、外设与引脚分配

| 外设 | 引脚 | 功能 |
| --- | --- | --- |
| SYSCTL |  | 系统控制（时钟等） |
| ADC0 | PA25 | ADC12 通道 2 输入引脚 |
| ADC0 | PA27 | ADC12 通道 0 输入引脚 |
| ADC0 | PA22 | ADC12 通道 7 输入引脚 |
| ADC0 | PA24 | ADC12 通道 3 输入引脚 |
| EVENT |  | 事件系统（内部互连） |
| DEBUGSS | PA20 | 调试时钟（SWCLK） |
| DEBUGSS | PA19 | 调试数据（SWDIO） |

### LaunchPad 跳线设置

访问 [LP_MSPM0G3507](https://www.ti.com/tool/LP-MSPM0G3507) 可获取 LaunchPad 用户指南和硬件资料。

| 引脚 | 外设 | 功能 | LaunchPad 引脚 | LaunchPad 设置 |
| --- | --- | --- | --- | --- |
| PA25 | ADC0 | A2 | J1_2 | 无特殊设置（N/A） |
| PA27 | ADC0 | A0 | J1_8 | <ul><li>PA27 可连接到光电二极管 D1<br><ul><li>`J17 OFF` 断开与光电二极管 D1 的连接</ul></ul> |
| PA22 | ADC0 | A7 | J3_24 | <ul><li>PA22 可连接到光电二极管 D1<br><ul><li>`J16 ON` 连接到光电二极管 D1<br><li>`J16 OFF` 断开与光电二极管 D1 的连接</ul></ul> |
| PA24 | ADC0 | A3 | J3_27 | 无特殊设置（N/A） |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 调试时用于 SWD<br><ul><li>`J101 15:16 ON` 调试时连接到 XDS-110 SWCLK<br><li>`J101 15:16 OFF` 应用中使用该引脚时断开与 XDS-110 SWCLK 的连接</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 调试时用于 SWD<br><ul><li>`J101 13:14 ON` 调试时连接到 XDS-110 SWDIO<br><li>`J101 13:14 OFF` 应用中使用该引脚时断开与 XDS-110 SWDIO 的连接</ul></ul> |

## 三、代码流程说明（一步步看 main 函数）

### 1. 全局变量

```c
volatile bool gCheckADC;                 // “本轮采集完成”标志位

#define RESULT_SIZE (64)                 // 每个缓冲区最多存 64 个采样值
volatile uint16_t gAdcResult0[RESULT_SIZE];  // 通道 0 的结果缓冲区
volatile uint16_t gAdcResult1[RESULT_SIZE];  // 通道 1 的结果缓冲区
volatile uint16_t gAdcResult2[RESULT_SIZE];  // 通道 2 的结果缓冲区
volatile uint16_t gAdcResult3[RESULT_SIZE];  // 通道 3 的结果缓冲区
```

### 2. main 函数流程

```c
int main(void)
{
    /* 初始化外设并打开中断 */
    SYSCFG_DL_init();
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);   // 打开 ADC0 中断

    gCheckADC  = false;
    uint16_t i = 0;                          // 缓冲区下标

    while (1) {
        DL_ADC12_startConversion(ADC12_0_INST);  // ① 触发一次顺序转换（4 个通道依次采）

        /* ② 等待所有通道采完（中断里等 MEM3 装载完成） */
        while (gCheckADC == false) {
            __WFE();
        }

        /* ③ 把 4 个通道的结果分别存入各自的缓冲区 */
        gAdcResult0[i] = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
        gAdcResult1[i] = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
        gAdcResult2[i] = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_2);
        gAdcResult3[i] = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_3);

        i++;                                  // ④ 下标加 1
        gCheckADC = false;                    //    复位标志位
        if (i >= RESULT_SIZE) {               // ⑤ 采满 64 组，停在断点查看数据
            __BKPT(0);
            i = 0;                            //    然后从头再来
        }
        DL_ADC12_enableConversions(ADC12_0_INST); // ⑥ 重新使能转换，准备下一轮
    }
}
```

### 3. 中断处理函数

```c
/* 监听最后一个结果是否装载完成，代表本轮 4 个通道全部采完 */
void ADC12_0_INST_IRQHandler(void)     // ADC0 的中断服务函数
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM3_RESULT_LOADED:   // MEM3（第 4 个通道）结果已装载
            gCheckADC = true;
            break;
        default:
            break;
    }
}
```

## 四、如何编译、烧录与运行

1. 用 CCS / Keil / IAR 打开对应子目录（`ticlang/`、`keil/`、`iar/`、`gcc/`）里的工程文件。
2. 用 USB 线连接 **LP-MSPM0G3507 LaunchPad** 到电脑。
3. 编译并下载调试，运行程序。
4. 给 4 个 ADC 通道接上 **0 ~ VCC 之间的有效电压**，采满 64 组后程序停在 `__BKPT(0)` 断点，在 Watch 窗口查看 4 个缓冲区。

> ⚠️ 悬空的引脚读数不保证为 0，务必给每个通道接有效输入。

## 五、初学者的几个关键概念

| 概念 | 通俗解释 |
| --- | --- |
| ADC（模数转换器） | 把模拟电压变成数字量的「翻译官」 |
| 顺序转换（Sequence） | 一次触发后按顺序依次转换多个通道，像排队打饭 |
| 转换存储（MEM） | 保存每次转换结果的小格子，MEM0~MEM3 对应 4 个通道 |
| 中断（Interrupt） | 外设干完活后「举手报告」，CPU 暂停手头的事来处理 |
| WFE（Wait For Event） | 让 CPU 低功耗等待，有事件到来再唤醒 |
| 缓冲区（Buffer） | 用来暂存一批数据的数组，这里每个通道一个 |
| SysConfig | TI 图形化配置工具，自动生成初始化代码 |

## 六、注意事项

- **每个通道都要接有效输入**（0 ~ VCC），悬空引脚读数不保证为 0，会污染你的数据。
- 中断监听的是**最后一个通道（MEM3）**的结果装载，这样能保证 4 个通道都已采完，读取时不会拿到半截数据。
- 通道与 MEM 的对应关系、采样顺序由 SysConfig 中 ADC 的顺序表（Sequence Table）决定，改动时记得在 `.syscfg` 里同步修改。
- 移植建议：本工程基于 LP_MSPM0G3507（超集器件）。若要移植到其他 MSPM0 器件，请参考 [CCS 用户指南的 SysConfig 工程迁移章节](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)。
- 低功耗建议：未使用的引脚应配置为 GPIO 并输出低电平，或输入模式加内部上/下拉电阻，可在 SysConfig 中点击 **Board → Configure Unused Pins** 一键配置。更多见 [LP-MSPM0G3507 用户指南](https://www.ti.com/lit/slau873)。
