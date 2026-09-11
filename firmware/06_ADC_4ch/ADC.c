#include "ti_msp_dl_config.h"
#include "ADC.h"

volatile bool gCheckADC;



volatile uint16_t gAdcResult0[RESULT_SIZE];
volatile uint16_t gAdcResult1[RESULT_SIZE];
volatile uint16_t gAdcResult2[RESULT_SIZE];
volatile uint16_t gAdcResult3[RESULT_SIZE];



void ADC_Init()
{
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
}


void ADC_Run()
{
    for(uint8_t j = 0; j<RESULT_SIZE; j++){

        gCheckADC  = false;

        DL_ADC12_startConversion(ADC12_0_INST);
        /* Wait until all data channels have been loaded. */
        while (gCheckADC == false) {
            __WFE();
        }

        /* Store ADC Results into their respective buffer */
        gAdcResult0[j] =
            DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
        gAdcResult1[j] =
            DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
        gAdcResult2[j] =
            DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_2);
        gAdcResult3[j] =
            DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_3);

        DL_ADC12_enableConversions(ADC12_0_INST);
    }
    /* 一轮采集完成，可设置断点检查数组 */
    __BKPT(0);
}










/* Check for the last result to be loaded then change boolean */
void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM3_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}













