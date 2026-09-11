# i2c_controller_rw_multibyte_fifo_interrupts —— I2C 控制器使用 FIFO 与中断读写多字节数据

> 芯片型号：LP_MSPM0G3507（MSPM0G3507）
> 示例来源：TI MSPM0 SDK DriverLib 例程

## 一、工程简介（这个例子是做什么的？）

这个例子演示了 **MSPM0G3507 作为 I2C 控制器（Controller，主设备）**，如何与一个 **目标设备（Target，从设备）** 完成多字节读写：

- 本芯片工作在 **控制器模式**，先向地址为 `0x48` 的目标设备 **写入 16 个字节**（0x00~0x0F），再向它 **读取 16 个字节**。
- 收发过程使用 **硬件 FIFO（8 字节深）+ 中断**：FIFO 一次装不下的数据，通过 `TXFIFO_TRIGGER` 中断持续补发、通过 `RXFIFO_TRIGGER` 中断持续接收。
- 当「写入 + 读取」都成功时，板载 **LED（PA0 / USER_LED_1）会周期性翻转闪烁**，直观地告诉你通信成功。
- 目标设备必须**提前上电并处于激活状态**，本例子才能跑通。

💡 这个工程常被用来：验证 I2C 通信是否正常、作为控制任意 I2C 目标设备的起点、或者在此基础上编写自己的 I2C 驱动。

## 二、外设与引脚分配

### 外设引脚表

| 外设 | 引脚 | 功能 |
| --- | --- | --- |
| GPIOA | PA0 | 开漏输出（USER_LED_1，状态指示灯） |
| GPIOA | PA15 | 标准输出（USER_TEST，测试引脚，与 LED 同步翻转） |
| SYSCTL |  | 系统控制（时钟、电源等） |
| I2C1 | PB3 | I2C 数据线（SDA） |
| I2C1 | PB2 | I2C 时钟线（SCL） |
| EVENT |  | 事件（本例未使用） |
| DEBUGSS | PA20 | 调试时钟（SWCLK） |
| DEBUGSS | PA19 | 调试数据（SWDIO） |

> I2C1 总线速率：**400 kHz**（`I2C_BUS_SPEED_HZ = 400000`）。

### LaunchPad 跳线设置

| 引脚 | 外设 | 功能 | LaunchPad 引脚 | LaunchPad 设置 |
| --- | --- | --- | --- | --- |
| PA0 | GPIOA | PA0 | J27_9 | PA0 是 5V 容忍的开漏引脚，需要上拉：`J19 1:2` 用 3.3V 上拉；`J19 2:3` 用 5V 上拉。PA0 可连接 LED1：`J4 ON` 连接 LED1；`J4 OFF` 断开 LED1 |
| PA15 | GPIOA | PA15 | J3_30 | 用于 BoosterPack 扩展排针测试，可按需重新配置为普通 GPIO |
| PB3 | I2C1 | SDA | J1_10 | PB3 可连接板上上拉电阻：`R60` 默认未焊接，焊接后才使用板上上拉 |
| PB2 | I2C1 | SCL | J1_9 | PB2 可连接板上上拉电阻：`R59` 默认未焊接，焊接后才使用板上上拉 |
| PA20 | DEBUGSS | SWCLK | N/A | 调试时使用：`J101 15:16 ON` 连接 XDS-110 SWCLK；应用占用时 `J101 15:16 OFF` 断开 |
| PA19 | DEBUGSS | SWDIO | N/A | 调试时使用：`J101 13:14 ON` 连接 XDS-110 SWDIO；应用占用时 `J101 13:14 OFF` 断开 |

📖 更多 LaunchPad 信息（用户指南、硬件资料）见 [LP_MSPM0G3507](https://www.ti.com/tool/LP-MSPM0G3507)。

## 三、代码流程说明（一步步看 main 函数）

### 1. 初始化与全局变量

```c
SYSCFG_DL_init();  // 初始化时钟、GPIO、I2C1 等（由 SysConfig 生成）

/* 点亮 LED，表示传输开始 */
DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);

NVIC_EnableIRQ(I2C_INST_INT_IRQN);   // 使能 I2C 中断
DL_SYSCTL_disableSleepOnExit();      // 退出中断后不自动进入睡眠
```

`gTxPacket[16]` 存放要发送的 16 字节数据（0x00~0x0F）；`gRxPacket[16]` 存放从目标设备读回的数据。`gI2cControllerStatus` 是一个枚举状态机，用来记录传输进行到哪一步（空闲 / 发送中 / 发送完成 / 接收中 / 接收完成 / 出错）。

### 2. 写入（TX）：把 16 字节发给目标设备

```c
gTxCount = DL_I2C_fillControllerTXFIFO(I2C_INST, &gTxPacket[0], gTxLen);
/* 如果 FIFO（8 字节深）没装下全部数据，就打开 TXFIFO 触发中断，由中断继续补发 */
if (gTxCount < gTxLen) {
    DL_I2C_enableInterrupt(I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
} else {
    DL_I2C_disableInterrupt(I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
}

/* 发起发送：自动产生起始位（Start）+ 停止位（Stop） */
DL_I2C_startControllerTransfer(I2C_INST, I2C_TARGET_ADDRESS,
    DL_I2C_CONTROLLER_DIRECTION_TX, gTxLen);

/* 等待中断把状态置为 TX_COMPLETE 或 ERROR */
while ((gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) &&
       (gI2cControllerStatus != I2C_STATUS_ERROR)) {
    __WFE();   // 等待事件（中断）唤醒，省电
}
```

### 3. 读取（RX）：从目标设备读回 16 字节

```c
gI2cControllerStatus = I2C_STATUS_RX_STARTED;
DL_I2C_startControllerTransfer(I2C_INST, I2C_TARGET_ADDRESS,
    DL_I2C_CONTROLLER_DIRECTION_RX, gRxLen);   // 发起读取请求

while (gI2cControllerStatus != I2C_STATUS_RX_COMPLETE) {
    __WFE();   // 等待 RX_DONE 中断
}
```

### 4. 成功后 LED 闪烁

```c
while (1) {
    DL_GPIO_togglePins(GPIO_LEDS_PORT,
        GPIO_LEDS_USER_LED_1_PIN | GPIO_LEDS_USER_TEST_PIN);  // 翻转 LED 与测试引脚
    delay_cycles(16000000);   // 延时
}
```

### 5. 中断处理函数 I2C_INST_IRQHandler（I2C 状态机）

```c
switch (DL_I2C_getPendingInterrupt(I2C_INST)) {
    case DL_I2C_IIDX_CONTROLLER_RX_DONE:          // 接收完成
        gI2cControllerStatus = I2C_STATUS_RX_COMPLETE;
        break;
    case DL_I2C_IIDX_CONTROLLER_TX_DONE:          // 发送完成
        DL_I2C_disableInterrupt(I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
        gI2cControllerStatus = I2C_STATUS_TX_COMPLETE;
        break;
    case DL_I2C_IIDX_CONTROLLER_RXFIFO_TRIGGER:   // RX FIFO 有数据了
        while (DL_I2C_isControllerRXFIFOEmpty(I2C_INST) != true) {
            gRxPacket[gRxCount++] = DL_I2C_receiveControllerData(I2C_INST);  // 逐个读走
        }
        break;
    case DL_I2C_IIDX_CONTROLLER_TXFIFO_TRIGGER:   // TX FIFO 空了，继续补数据
        gTxCount += DL_I2C_fillControllerTXFIFO(
            I2C_INST, &gTxPacket[gTxCount], gTxLen - gTxCount);
        break;
    case DL_I2C_IIDX_CONTROLLER_NACK:             // 目标设备没应答（可能没接）
        gI2cControllerStatus = I2C_STATUS_ERROR;
        break;
    /* 其余情况本例子不使用 */
}
```

💡 这种「主循环等待 + 中断推进状态机」的写法，是嵌入式开发里非常经典的模式，请重点体会。

## 四、如何编译、烧录与运行

1. **连接硬件**：把 LaunchPad 的 SDA（PB3 / J1_10）和 SCL（PB2 / J1_9）与目标设备的 SDA/SCL 相连（注意共地）。I2C 需要上拉电阻：可在 SysConfig 中使能内部上拉，或按上表焊接 `R60`/`R59` 使用板上上拉；总线速率高、线缆电容大时可能还需要外部上拉。
2. **打开工程**：用 CCS / Keil / IAR / ticlang 打开本目录对应子目录（`ticlang/`、`keil/`、`iar/`、`gcc/`）中的工程文件。
3. **编译下载**：编译后通过板载 XDS-110 调试器下载到 LaunchPad。
4. **先让目标设备上电运行**，再运行本控制器工程。
5. **观察结果**：
   - 传输成功 → LED 周期性闪烁（USER_TEST 引脚会同步翻转，可在排针上用万用表/示波器验证）；
   - 初始化出问题 → LED 保持熄灭；
   - 数据传输出问题（如目标未接、地址不对）→ LED 保持常亮（程序停在 `__BKPT(0)`）。

## 五、初学者的几个关键概念

| 概念 | 通俗解释 |
| --- | --- |
| I2C 协议 | 一种两根线（SDA 数据 + SCL 时钟）的串行总线，可挂多个设备，速率从几十 kHz 到几 MHz |
| 控制器 / 目标 | 控制器主动发起通信（主），目标被寻址后应答（从）。本工程 MSPM0 是控制器 |
| 7 位地址 | 每个目标设备有唯一的 7 位地址，本例子是 `0x48`，主设备用它点名通信 |
| ACK / NACK | 每收一个字节，接收方回一个应答位；不应答（NACK）通常说明目标没接或地址不对 |
| FIFO | 硬件自带的「数据暂存队列」，8 字节深，配合中断/轮询可连续搬运大量数据 |
| 中断 vs 轮询 | 轮询是 CPU 死等标志位；中断是硬件主动通知 CPU「我好了」，CPU 可以睡觉，更高效省电 |
| Start / Stop | I2C 通信的开始信号（起始位）和结束信号（停止位），一次完整通信由它们包起来 |

## 六、注意事项

- **目标设备必须最先上电**：先跑目标端，再跑本控制器端，否则控制器发地址会得到 NACK。
- **上拉电阻不能省**：I2C 是开漏总线，没有上拉 SDA/SCL 就是浮空，通信会失败。
- **移植到其他 MSPM0**：本工程基于 LP-MSPM0G3507 开发，迁移到其它型号请参考 [CCS 用户指南-工程迁移](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)。
- **低功耗建议**：未使用的引脚建议在 SysConfig 中通过 **Board → Configure Unused Pins** 配置为 GPIO 输出低或带上/下拉的输入，避免悬空耗电；低功耗跳线配置见 [LP-MSPM0G3507 用户指南](https://www.ti.com/lit/slau873)。
- **相关例程**：轮询版本见 `i2c_controller_rw_multibyte_fifo_poll`；目标端见 `i2c_target_rw_multibyte_fifo_interrupts`。
