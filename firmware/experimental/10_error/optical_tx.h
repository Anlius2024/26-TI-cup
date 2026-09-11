#ifndef OPTICAL_TX_H
#define OPTICAL_TX_H

#include <stdint.h>
#include <stdbool.h>
#include "ti_msp_dl_config.h"

/* ==================== 协议常量 ==================== */

#define OPTICAL_BAUD_RATE           (1000U)
#define OPTICAL_BIT_PERIOD_US       (1000U)
#define OPTICAL_HALF_PERIOD_US      (500U)

#define OPTICAL_FRAME_HEADER        (0xAAu)
#define OPTICAL_FRAME_FOOTER        (0x55u)

/* 命令字定义 (与 optical_comm.h 一致) */
#define OPTICAL_CMD_CONFIG          (0x01)  /* 参数配置 */
#define OPTICAL_CMD_SCAN_START      (0x02)  /* 扫频开始 */
#define OPTICAL_CMD_SINGLE_TRG      (0x03)  /* 单触发 */
#define OPTICAL_CMD_HEARTBEAT       (0x04)  /* 心跳同步 */



/* ==================== API 接口 ==================== */
void manchester_send_bit(uint8_t bit);
void send_frame_raw(uint8_t cmd, uint16_t freq, uint8_t volt);

/* 初始化发送模块（必须首先调用） */
void optical_tx_init(void);

/* 发送配置帧
 * @param freq_hz: 频率 (10-20000Hz)
 * @param voltage_x10: 电压×10 (10-30 对应 1.0V-3.0V)
 * @return: 发送是否成功
 */
bool optical_tx_send_config(uint16_t freq_hz, uint8_t voltage_x10);

/* 发送扫频开始帧
 * @param freq_start: 起始频率
 * @param freq_end: 终止频率
 * @param step: 步长
 * @return: 发送是否成功
 */
bool optical_tx_send_scan_start(uint16_t freq_start, uint16_t freq_end, uint16_t step);

/* 发送单次触发帧 */
bool optical_tx_send_trigger(void);

/* 发送心跳包 */
bool optical_tx_send_heartbeat(void);

/* 上电自动配置（发送 3 次确保可靠） */
void optical_tx_auto_config(uint16_t freq, uint8_t volt);

/* 扫频模式发送（自动完成整个扫频过程） */
void optical_tx_sweep_run(uint16_t start, uint16_t end, uint16_t step);

/* 快速测试模式（验证协议） */
void optical_tx_quick_test(void);

#endif /* OPTICAL_TX_H */
