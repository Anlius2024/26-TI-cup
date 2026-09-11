#include "ti_msp_dl_config.h"
#include "bsp.h"

uint16_t adc_buff[FFT_LENGTH];
volatile uint16_t ADCConvEnd;

int main()
{
    SYSCFG_DL_init();

    DL_DMA_setSrcAddr(DMA,DMA_CH0_CHAN_ID,(uint32_t)0x40556280);
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)adc_buff);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);

    while(1)
    {
        
        //while(!ADCConvEnd);
        if(ADCConvEnd==1){

        data_fft();
        show_pic();
        anlysize_figure(200000);
        
        ADCConvEnd=0;                    
        temp = Freq;

        DL_TimerA_startCounter(TIMER_0_INST);
    }

 


}   
}    
     




void ADC12_0_INST_IRQHandler(void)
{
    switch(DL_ADC12_getPendingInterrupt(ADC12_0_INST))
    {
        case DL_ADC12_IIDX_DMA_DONE:
            DL_TimerA_stopCounter(TIMER_0_INST);
           //__BKPT();
            ADCConvEnd = 1;
            break;
        default:
            break;
    }
}




