#include "ti_msp_dl_config.h"
#include "DigitalRes.h"

int main(void)
{
    SYSCFG_DL_init();

    DigitalRes_init();

    /* Set LED to indicate start of transfer */
    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);

    
    DL_SYSCTL_disableSleepOnExit();

    

    
    DigitalRes_SendData(0x0E);

    
    /* Add delay between transfers */
    // delay_cycles(1000);

    // /* Send a read request to Target */
    // gRxLen               = I2C_RX_PACKET_SIZE;
    // gRxCount             = 0;
    // gI2cControllerStatus = I2C_STATUS_RX_STARTED;
    // DL_I2C_startControllerTransfer(
    //     I2C_INST, I2C_TARGET_ADDRESS, DL_I2C_CONTROLLER_DIRECTION_RX, gRxLen);

    // /* Wait for all bytes to be received in interrupt */
    // while (gI2cControllerStatus != I2C_STATUS_RX_COMPLETE) {
    //     __WFE();
    // }

    // while (DL_I2C_getControllerStatus(I2C_INST) &
    //        DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
    //     ;

    /* If write and read were successful, toggle LED */
    while (1) {
        DL_GPIO_togglePins(GPIO_LEDS_PORT,
            GPIO_LEDS_USER_LED_1_PIN | GPIO_LEDS_USER_TEST_PIN);
        delay_cycles(16000000);
    }
}

