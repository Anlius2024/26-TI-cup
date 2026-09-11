#include "ti_msp_dl_config.h"

#include "key.h"
#include "lcd.h"
#include "GUI.h"



#define DAC_BUF_SIZE 64
#define ADC_SAMPLE_SIZE 4096
#define ADC_FIFO_SAMPLES (ADC_SAMPLE_SIZE >> 1)
volatile uint16_t ADC_buf[ADC_SAMPLE_SIZE];
volatile bool ADC_done;



// Output Freq(1khz) = Sample Freq(64khz) / Buf Size(64)
const uint16_t DAC_buf[] = {2048, 2248, 2447, 2642, 2831, 3013,
    3185, 3347, 3496, 3631, 3750, 3854, 3940, 4007, 4056, 4086, 4095, 4086,
    4056, 4007, 3940, 3854, 3750, 3631, 3496, 3347, 3185, 3013, 2831, 2642,
    2447, 2248, 2048, 1847, 1648, 1453, 1264, 1082, 910, 748, 599, 464, 345,
    241, 155, 88, 39, 9, 0, 9, 39, 88, 155, 241, 345, 464, 599, 748, 910, 1082,
    1264, 1453, 1648, 1847};


// int main(void)
// {
//     SYSCFG_DL_init();
    


//     DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) DL_ADC12_getFIFOAddress(ADC12_0_INST));
//     DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC_buf[0]);
//     DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, ADC_FIFO_SAMPLES);
//     DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
//     NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
//     ADC_done = false;
//     // DL_ADC12_startConversion(ADC12_0_INST);
//     // DL_TimerA_startCounter(TIMER_0_INST);



//     // for (uint16_t i = 0; i < DAC_BUF_SIZE; i++) {
//     //     DAC_buf[i] = (uint16_t)(2000.0f * sin(2.0f * M_PI * i / DAC_BUF_SIZE) + 2000);
//     // }
//     DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) & DAC_buf[0]);
//     DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) & (DAC0->DATA0));
//     DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, sizeof(DAC_buf) / sizeof(uint16_t));
//     DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
//     DL_TimerA_startCounter(TIMER_1_INST);
//     NVIC_EnableIRQ(DAC12_INT_IRQN);




//     while (1) {
//         while (ADC_done == false);
//         ADC_done = false;

//         if (KEY_Scan_Debounce()) {
//             DL_GPIO_togglePins(GPIOB, GPIO_LEDS_USER_LED_1_PIN);
//         }
        
//     }
    
// }

int main(void){
    SYSCFG_DL_init(); 

    LCD_Init(); 



    delay_ms(200);

    uint16_t wave[LCD_W];
    for (uint16_t i = 0; i < LCD_W; i++) {
        wave[i] = (uint16_t)((float)(LCD_H / 2) * sin(2.0f * M_PI * i / DAC_BUF_SIZE) + (LCD_H / 2));
    }


    while(1) {

        if(key1_get()){
            // __BKPT(0);
            DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);   // LED 是否还在闪
            // LCD_Wave_320_240(&wave[0], 0xFFFF);
            // LCD_Clear(BLUE);
            LCD_VppVrms(33.33, -66.66);
            // LCD_Spectrum(&wave[0], 0xFFFF);
            // LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
            // LCD_Phase_Mag(10.5, 30.12, 0);
            // LCD_Phase_Mag(21, 66.643, 1);
        }
    }


}

void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            DL_TimerA_stopCounter(TIMER_0_INST);
            DL_ADC12_disableConversions(ADC12_0_INST);
            ADC_done = true;

            // __BKPT(0);

            DL_TimerA_startCounter(TIMER_0_INST);
            DL_ADC12_enableConversions(ADC12_0_INST);
            
            break;
        default:
            break;
    }
}






