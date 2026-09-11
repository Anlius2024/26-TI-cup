#include "ti_msp_dl_config.h"
#include "delay.h"
#include "ad9850.h"
#include "DigitalRes.h"
#include "key.h"
#include "lcd_init.h"
#include "lcd.h"
#include "optical_tx.h"


volatile uint16_t   Phase;
volatile float      Vpp;
volatile uint8_t    KeyNum;



int main(void)
{
    SYSCFG_DL_init();

    ad9850_reset_serial();
    DigitalRes_init();
    // optical_tx_init();
    LCD_Init(); 
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);

    Phase = 10;
    Vpp   = 1.0f;
    KeyNum = 0;

    DigitalRes_Amplifier_Set(1.925);
    ad9850_wr_serial(0x00,10);

    LCD_ShowString(1, 33, "Output Locked", WHITE, BLACK, 16, 0);

    while (1) {
        // KeyNum = keymatrix_get();
        // if(KeyNum){
        //     LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
        //     if(KeyNum == 1)         Phase += 10;
        //     else if(KeyNum == 2)    Phase += 100;
        //     else if(KeyNum == 3)    Phase += 1000;
        //     else if(KeyNum == 4)    Phase  = 10;

        //     else if(KeyNum == 5)    Vpp   += 0.1;
        //     else if(KeyNum == 6)    Vpp   += 1.0;
        //     else if(KeyNum == 7)    Vpp   =  1.0;
        //     else if(KeyNum == 8){                  // 锁存输出，第四次会死机
        //         ad9850_wr_serial(0x00, Phase);
        //         DigitalRes_Amplifier_Set(Vpp);
        //         LCD_ShowString(1, 33, "Output Locked", WHITE, BLACK, 16, 0);
        //     }

        //     else if(KeyNum == 9){                   // 开始扫频
        //         LCD_ShowString(1, 33, "Bode Testing", WHITE, BLACK, 16, 0);
        //     }      
        //     else if(KeyNum == 10){                  // 切换模拟开关到dac，输出脉冲
        //         LCD_ShowString(1, 33, "Pulse Testing", WHITE, BLACK, 16, 0);
        //     }             
        //     else if(KeyNum == 11)   ;
        //     else if(KeyNum == 12)   ;



        //     else{
                
        //         LCD_ShowString(1, 33, "INVALID KEYNUM", WHITE, BLACK, 16, 0);
        //     }      

        //     // LCD显示部分
        //     if(Phase > 20000){
        //         Phase = 20000;
        //         LCD_ShowString(1, 33, "Phase too large", WHITE, BLACK, 16, 0);
        //     }
        //     if(Vpp > 3.0){
        //         Vpp = 3.0;
        //         LCD_ShowString(1, 33, "Vpp too large", WHITE, BLACK, 16, 0);
        //     }
        //     LCD_VppShow(Vpp);
        //     LCD_PhaseShow(Phase);

            
        // }
        // delay_ms(100);
        // MyI2C_SendByte(0x5C);

        delay_ms(10);
        DIG_I2C_CLR_SDA();
        delay_ms(10);
        DIG_I2C_SET_SDA();








        // manchester_send_bit(1);
        // send_frame_raw(0x01, 2000, 15);
        // DL_GPIO_setPins(GPIO_USRLED_PORT, GPIO_USRLED_PIN_0_PIN);
        // delay_us(500);
        // DL_GPIO_clearPins(GPIO_USRLED_PORT, GPIO_USRLED_PIN_0_PIN);
        // delay_us(500);
    }
    
}




