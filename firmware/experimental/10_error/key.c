#include "key.h"

uint8_t key[4][4];

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


uint8_t keymatrix_get()
{
	uint8_t i, j;
	for(i = 0; i < 4; i++)
	{	
		delay_ms(5);
		if(i == 0)			DL_GPIO_clearPins(GPIO_KEYMATRIX_PIN_R1_PORT, GPIO_KEYMATRIX_PIN_R1_PIN);
		else if(i == 1)		DL_GPIO_clearPins(GPIO_KEYMATRIX_PIN_R2_PORT, GPIO_KEYMATRIX_PIN_R2_PIN);
		else if(i == 2)		DL_GPIO_clearPins(GPIO_KEYMATRIX_PIN_R3_PORT, GPIO_KEYMATRIX_PIN_R3_PIN);
		else if(i == 3)		DL_GPIO_clearPins(GPIO_KEYMATRIX_PIN_R4_PORT, GPIO_KEYMATRIX_PIN_R4_PIN);
		for(j = 0; j < 4; j++)
		{
			delay_ms(5); 
			if(j == 0){
				// __BKPT(0);
				if(DL_GPIO_readPins(GPIO_KEYMATRIX_PIN_C1_PORT, GPIO_KEYMATRIX_PIN_C1_PIN) == 0)	key[i][j] = 1;
				else	key[i][j] = 0;
			}else if(j == 1){
				if(DL_GPIO_readPins(GPIO_KEYMATRIX_PIN_C2_PORT, GPIO_KEYMATRIX_PIN_C2_PIN) == 0)	key[i][j] = 1;
				else	key[i][j] = 0;
			}else if(j == 2){
				if(DL_GPIO_readPins(GPIO_KEYMATRIX_PIN_C3_PORT, GPIO_KEYMATRIX_PIN_C3_PIN) == 0)	key[i][j] = 1;
				else	key[i][j] = 0;
			}else if(j == 3){
				if(DL_GPIO_readPins(GPIO_KEYMATRIX_PIN_C4_PORT, GPIO_KEYMATRIX_PIN_C4_PIN) == 0)	key[i][j] = 1;
				else	key[i][j] = 0;
			}
		}
		if(i == 0)			DL_GPIO_setPins(GPIO_KEYMATRIX_PIN_R1_PORT, GPIO_KEYMATRIX_PIN_R1_PIN);
		else if(i == 1)		DL_GPIO_setPins(GPIO_KEYMATRIX_PIN_R2_PORT, GPIO_KEYMATRIX_PIN_R2_PIN);
		else if(i == 2)		DL_GPIO_setPins(GPIO_KEYMATRIX_PIN_R3_PORT, GPIO_KEYMATRIX_PIN_R3_PIN);
		else if(i == 3)		DL_GPIO_setPins(GPIO_KEYMATRIX_PIN_R4_PORT, GPIO_KEYMATRIX_PIN_R4_PIN);
	}

	if(key[0][0]==1)return 1;
    else if(key[0][1]==1)return 2;
    else if(key[0][2]==1)return 3;
    else if(key[0][3]==1)return 4;
    else if(key[1][0]==1)return 5;
    else if(key[1][1]==1)return 6;
    else if(key[1][2]==1)return 7;
    else if(key[1][3]==1)return 8;
    else if(key[2][0]==1)return 9;
    else if(key[2][1]==1)return 10;
    else if(key[2][2]==1)return 11;
    else if(key[2][3]==1)return 12;
    else if(key[3][0]==1)return 13;
    else if(key[3][1]==1)return 14;
    else if(key[3][2]==1)return 15;
    else if(key[3][3]==1)return 16;
	else return 0;
}