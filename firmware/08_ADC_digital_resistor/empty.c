#include "ti_msp_dl_config.h"
#include "delay.h"
#include "ad9850.h"
#include "DigitalRes.h"


int main(void)
{
    SYSCFG_DL_init();

    ad9850_reset_serial();
    DigitalRes_init();

    DigitalRes_Amplifier_Set(2.925);

    ad9850_wr_serial(0x00,2000);

    while (1) {
        DL_GPIO_setPins(GPIO_USRLED_PORT, GPIO_USRLED_PIN_0_PIN);
        delay_ms(5);
        DL_GPIO_clearPins(GPIO_USRLED_PORT, GPIO_USRLED_PIN_0_PIN);
        delay_ms(5);
    }
    
}




