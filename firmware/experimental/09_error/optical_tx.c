/**
 * @file optical_tx.c
 * @brief 信号发生端 - 可见光通信发送模块
 * 
 * 用途：将此文件移植到信号发生装置的工程中
 * 
 * 功能：
 *  1. 上电自动发送初始参数
 *  2. 扫频时逐点发送频率和电压信息
 *  3. 19kHz-20kHz 时切换到模拟传输模式
 * 
 * 硬件要求：
 *  - 1 个 GPIO 输出控制 LED (推挽输出)
 *  - LED: 白光 LED，响应频率>30kHz
 */

#include <stdint.h>
#include <stdbool.h>
#include "optical_tx.h"
#include "ad9850.h"

/* ==================== 硬件配置 (根据实际修改) ==================== */

#define OPTICAL_TX_GPIO_PORT    GPIO_USRLED_PORT
#define OPTICAL_TX_GPIO_PIN     GPIO_USRLED_PIN_0_PIN  /* PB14 - 连接 LED */

/* 发送 LED 控制宏 */
#define LED_OFF()  DL_GPIO_clearPins(OPTICAL_TX_GPIO_PORT, OPTICAL_TX_GPIO_PIN)
#define LED_ON() DL_GPIO_setPins(OPTICAL_TX_GPIO_PORT, OPTICAL_TX_GPIO_PIN)

/* ==================== 内部状态 ==================== */

static bool tx_initialized = false;
static uint8_t tx_sequence = 0;


uint16_t manchester_delay = 32150;  // 32000加补偿


/* ==================== 曼彻斯特发送实现 ==================== */

/* 发送一个曼彻斯特编码的比特 */    //每比特1khz速率
void manchester_send_bit(uint8_t bit)
{
    if (bit) {
        /* bit 1: 前半周期高，后半周期低 */
        LED_ON();
        delay_cycles(manchester_delay);
        LED_OFF();
        delay_cycles(manchester_delay);
    } else {
        /* bit 0: 前半周期低，后半周期高 */
        LED_OFF();
        // LED_SET:
        delay_cycles(manchester_delay);
        LED_ON();
        delay_cycles(manchester_delay);
    }
}

/* 发送一个曼彻斯特编码的字节 */
static void manchester_send_byte(uint8_t byte)
{
    for (int i = 7; i >= 0; i--) {
        uint8_t bit = (byte >> i) & 0x01;
        manchester_send_bit(bit);
    }
}

/* ==================== 帧发送函数 ==================== */

/* 计算校验和 */
static uint8_t calc_checksum(uint8_t cmd, uint8_t f_hi, uint8_t f_lo, 
                              uint8_t vol, uint8_t seq)
{
    return (cmd + f_hi + f_lo + vol + seq) & 0xFF;
}

/* 发送一帧完整的协议数据
 * 
 * 帧结构 (8 字节):
 *  [0xAA] [CMD] [Freq_H] [Freq_L] [Volt] [Seq] [Checksum] [0x55]
 * 
 * 曼彻斯特编码后：16 字节 = 约 107ms
 */
void send_frame_raw(uint8_t cmd, uint16_t freq, uint8_t volt)
{
    uint8_t checksum = calc_checksum(cmd, 
                                      (freq >> 8) & 0xFF, 
                                      freq & 0xFF, 
                                      volt, tx_sequence);
    
    /* 发送帧头 0xAA */
    manchester_send_byte(0xAA);
    
    /* 发送命令 */
    manchester_send_byte(cmd);
    
    /* 发送频率 (高 8 位 + 低 8 位) */
    manchester_send_byte((freq >> 8) & 0xFF);
    manchester_send_byte(freq & 0xFF);
    
    /* 发送电压 */
    manchester_send_byte(volt);
    
    /* 发送序列号 */
    manchester_send_byte(tx_sequence++);
    
    /* 发送校验和 */
    manchester_send_byte(checksum);
    
    /* 发送帧尾 0x55 */
    manchester_send_byte(0x55);
}

/* ==================== API 接口函数 ==================== */

/* 初始化发送模块 */
void optical_tx_init(void)
{
    /* 配置 LED GPIO 为推挽输出 */
    // DL_GPIO_initPin(OPTICAL_TX_GPIO_PORT, OPTICAL_TX_GPIO_PIN);
    // DL_GPIO_setDriveStrength(OPTICAL_TX_GPIO_PORT, OPTIMAL_TX_GPIO_PIN,
    //                          DL_GPIO_DRIVE_STRENGTH_2);
    LED_OFF();  /* 初始熄灭 */
    
    tx_sequence = 0;
    tx_initialized = true;
    
    // printf("[OPT_TX] 光通信发送模块初始化完成\r\n");
}

/* 发送配置帧 (参数配置模式)
 * @param freq_hz: 频率 (10-20000Hz)
 * @param voltage_x10: 电压×10 (10-30, 对应 1.0V-3.0V)
 * @return: true=成功，false=参数非法
 */
bool optical_tx_send_config(uint16_t freq_hz, uint8_t voltage_x10)
{
    if (!tx_initialized) return false;
    if (freq_hz < 10 || freq_hz > 20000) return false;
    if (voltage_x10 < 10 || voltage_x10 > 30) return false;
    
    /* 使用命令字 0x01 表示配置帧 */
    send_frame_raw(0x01, freq_hz, voltage_x10);
    
    return true;
}

/* 发送扫频开始帧
 * @param freq_start: 起始频率
 * @param freq_end: 终止频率  
 * @param step: 步长
 * @return: true=成功
 */
bool optical_tx_send_scan_start(uint16_t freq_start, uint16_t freq_end, uint16_t step)
{
    if (freq_start < 10 || freq_start > 20000) return false;
    if (freq_end < 10 || freq_end > 20000) return false;
    if (step < 1 || step > 1000) return false;
    
    /* 这里复用 voltage 字段放步长 (仅低 8 位有效) */
    send_frame_raw(0x02, freq_start, (uint8_t)(step & 0xFF));
    
    return true;
}

/* 发送单次触发帧 */
bool optical_tx_send_trigger(void)
{
    if (!tx_initialized) return false;
    
    send_frame_raw(0x03, 0, 0);
    
    return true;
}

/* 发送心跳包 */
bool optical_tx_send_heartbeat(void)
{
    if (!tx_initialized) return false;
    
    send_frame_raw(0x04, 0, 0);
    
    return true;
}

/* ==================== 高级功能 ==================== */

/* 自动配置函数（上电调用）
 * 
 * 用法：上电后自动发送默认参数，检测端需要自动显示
 */
void optical_tx_auto_config(uint16_t freq, uint8_t volt)
{
    /* 重试 3 次，确保可靠接收 */
    for (int i = 0; i < 3; i++) {
        optical_tx_send_config(freq, volt);
        delay_cycles(50000);  /* 50ms 间隔 */
    }
    
    // printf("[OPT_TX] 自动配置完成：freq=%dHz, volt=%d.%dV\r\n",
    //        freq, volt/10, volt%10);
}

/* 扫频模式发送
 * 
 * 用法：扫频测量时，每到一个频率点就发送该点的参数
 * 检测端收到后自动测量并存储
 * 
 * 总时间估算：
 *  频率点数 ≈ (20000-10)/10 = 1999 点
 *  每点发送时间 ≈ 107ms
 *  每点测量间隔 ≈ 50ms (给检测端时间)
 *  总时间 ≈ 1999 × 0.157s ≈ 314s (超时！需要优化)
 * 
 * 优化方案：
 *  1. 只发送扫频起始参数，后续不再发送
 *  2. 检测端根据预设规则自增频率
 *  3. 每 10 个点发送一次更新
 */
// void optical_tx_sweep_run(uint16_t start, uint16_t end, uint16_t step)
// {
//     // printf("[OPT_TX] 扫频模式：start=%dHz, end=%dHz, step=%dHz\r\n",
//     //        start, end, step);
    
//     /* 第一步：发送扫频开始命令 */
//     optical_tx_send_scan_start(start, end, step);
//     delay_cycles(100000);
    
//     /* 第二步：发送初始频率点 */
//     optical_tx_send_config(start, 20);  /* 默认 2.0V */
//     delay_cycles(150000);  /* 给检测端足够时间 */
    
//     /* 第三步：每 10 个点发送一次更新（减少传输量） */
//     uint32_t count = 0;
//     for (uint16_t f = start + step; f <= end; f += step) {
//         count++;
        
//         /* 每 10 个点发送一次频率更新 */
//         if (count % 10 == 0) {
//             optical_tx_send_config(f, 20);
//             delay_cycles(150000);
//         }
        
//         /* 等待检测端完成测量 */
//         delay_cycles(150000);
//     }
    
//     // printf("[OPT_TX] 扫频完成，总点数=%lu\r\n", count);
// }

/* ==================== 调试功能 ==================== ==================== */

/* 快速测试模式（用于验证协议正确性）
 * 
 * 发送：1kHz, 2.5V → 延迟 200ms → 3kHz, 2.8V → 延迟 200ms → ...
 */
void optical_tx_quick_test(void)
{
    // printf("[OPT_TX] 开始快速测试...\r\n");
    
    struct {
        uint16_t freq;
        uint8_t  volt;
    } test_cases[] = {
        {1000, 25},   /* 1kHz, 2.5V */
        {3000, 28},   /* 3kHz, 2.8V */
        {5000, 20},   /* 5kHz, 2.0V */
        {10000, 30},  /* 10kHz, 3.0V */
        {15000, 15},  /* 15kHz, 1.5V */
        {19000, 20},  /* 19kHz, 2.0V */
    };
    
    for (int i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        if (optical_tx_send_config(test_cases[i].freq, test_cases[i].volt)) {
            // printf("  [%d] freq=%dHz, volt=%d.%dV\r\n",
            //        i, test_cases[i].freq, 
            //        test_cases[i].volt/10, test_cases[i].volt%10);
            delay_cycles(300000);  /* 300ms 间隔 */
        }
    }
    
    // printf("[OPT_TX] 快速测试完成\r\n");
}






























/* ==================== 使用示例 ==================== */

/*
 * 在信号发生端 main.c 中的使用示例：
 * 
 * #include "optical_tx.h"
 * 
 * int main(void)
 * {
 *     // 初始化
 *     SYSCFG_DL_init();
 *     optical_tx_init();
 *     
 *     while (1) {
 *         // 上电自动配置
 *         optical_tx_auto_config(100, 20);  // 100Hz, 2.0V
 *         
 *         // 等待按键触发
 *         if (KEY1_Pressed()) {
 *             // 进入扫频模式
 *             optical_tx_sweep_run(10, 20000, 10);
 *         }
 *         
 *         // 心跳保持
 *         if (tick % 1000 == 0) {
 *             optical_tx_send_heartbeat();
 *         }
 *     }
 * }
 * 
 */
