#include "key.h"



uint8_t key1_get(void)
{
	uint8_t KeyNum=0;
	if(DL_GPIO_readPins(GPIO_SWITCHES_PORT,GPIO_SWITCHES_USER_SWITCH_1_PIN) >0  )
	{
		delay_ms(15);
		if(DL_GPIO_readPins(GPIO_SWITCHES_PORT,GPIO_SWITCHES_USER_SWITCH_1_PIN) >0 )
		{
			while(DL_GPIO_readPins(GPIO_SWITCHES_PORT,GPIO_SWITCHES_USER_SWITCH_1_PIN));
			KeyNum=1;
		}
		else
			KeyNum=0;
	}
	return KeyNum;
}


// uint8_t key2_get(void)
// {
// 	uint8_t KeyNum=0;
// 	if(DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_21) == 0)
// 	{
// 		delay_ms(15);
// 		if(DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_21) == 0)
// 		{
// //			while(!DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_21));
// 			KeyNum=1;
// 		}
// 		else
// 			KeyNum=0;
// 	}
// 	return KeyNum;
// }

