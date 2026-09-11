#include "ti_msp_dl_config.h"
#include "DigitalRes.h"
#include "ad9850.h"
#include "key.h"
#include "lcd_init.h"
#include "lcd.h"



volatile uint16_t   Phase;
volatile float      Vpp;
volatile uint8_t    KeyNum;

int main(void)
{
    SYSCFG_DL_init();

    DigitalRes_init();
    ad9850_reset_serial();
    LCD_Init(); 
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);

    Phase = 10;
    Vpp   = 1.0f;
    KeyNum = 0;

    DL_SYSCTL_disableSleepOnExit();

    

    DigitalRes_Amplifier_Set(2);
    ad9850_wr_serial(0x00,1000);

    
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
        KeyNum = keymatrix_get();
        if(KeyNum){
            LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
            if(KeyNum == 1)         Phase += 10;
            else if(KeyNum == 2)    Phase += 100;
            else if(KeyNum == 3)    Phase += 1000;
            else if(KeyNum == 4)    Phase  = 10;

            else if(KeyNum == 5)    Vpp   += 0.1;
            else if(KeyNum == 6)    Vpp   += 1.0;
            else if(KeyNum == 7)    Vpp   =  1.0;
            else if(KeyNum == 8){                  // 锁存输出，第四次会死机
                ad9850_wr_serial(0x00, Phase);
                DigitalRes_Amplifier_Set(Vpp);
                LCD_ShowString(1, 33, "Output Locked", WHITE, BLACK, 16, 0);
            }

            else if(KeyNum == 9){                   // 开始扫频
                LCD_ShowString(1, 33, "Bode Testing", WHITE, BLACK, 16, 0);
            }      
            else if(KeyNum == 10){                  // 切换模拟开关到dac，输出脉冲
                LCD_ShowString(1, 33, "Pulse Testing", WHITE, BLACK, 16, 0);
            }             
            else if(KeyNum == 11)   ;
            else if(KeyNum == 12)   ;



            else{
                
                LCD_ShowString(1, 33, "INVALID KEYNUM", WHITE, BLACK, 16, 0);
            }      

            // LCD显示部分
            if(Phase > 20000){
                Phase = 20000;
                LCD_ShowString(1, 33, "Phase too large", WHITE, BLACK, 16, 0);
            }
            if(Vpp > 3.0){
                Vpp = 3.0;
                LCD_ShowString(1, 33, "Vpp too large", WHITE, BLACK, 16, 0);
            }
            LCD_VppShow(Vpp);
            LCD_PhaseShow(Phase);

            
        }

        // delay_cycles(16000000);
    }
}

