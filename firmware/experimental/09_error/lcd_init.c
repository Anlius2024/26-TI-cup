#include "lcd_init.h"

/******************************************************************************
 * Function: LCD_GPIO_Init
 * Description: Initializes the GPIO pins for the LCD.
 ******************************************************************************/
void LCD_GPIO_Init(void) 
{
    
}

// /******************************************************************************
//  * Function: LCD_Writ_Bus
//  * Description: Writes data to the LCD bus.
//  * Parameters:
//  *   dat: The data to be written to the LCD bus.
//  ******************************************************************************/
// void LCD_Writ_Bus(u8 dat) 
// {
//     u8 i;
//     LCD_CS_Clr(); //Clear the chip select line
//     for (i = 0; i < 8; i++) 
// 	{
//         LCD_SCLK_Clr(); //Clear the clock line
//         if (dat & 0x80) 
// 		{
//             LCD_MOSI_Set(); //Set the MOSI line if the bit is 1
//         }
// 		else 
// 		{
//             LCD_MOSI_Clr(); //Clear the MOSI line if the bit is 0
//         }
//         LCD_SCLK_Set(); //Set the clock line
//         dat <<= 1; //Shift the data to the left
//     }
//     LCD_CS_Set(); //Set the chip select line
// }

/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{
    LCD_CS_Clr();

    //发送数据
    DL_SPI_transmitData8(SPI_LCD_INST, dat);
    //等待SPI总线空闲
    while(DL_SPI_isBusy(SPI_LCD_INST));

    LCD_CS_Set();
}

/******************************************************************************
 * Function: LCD_WR_DATA8
 * Description: Writes an 8-bit data to the LCD.
 * Parameters:
 *   dat: The 8-bit data to be written.
 ******************************************************************************/
void LCD_WR_DATA8(u8 dat) 
{
    LCD_Writ_Bus(dat);
}

/******************************************************************************
 * Function: LCD_WR_DATA
 * Description: Writes a 16-bit data to the LCD.
 * Parameters:
 *   dat: The 16-bit data to be written.
 ******************************************************************************/
void LCD_WR_DATA(u16 dat) 
{
    LCD_Writ_Bus(dat >> 8); //Write the high byte
    LCD_Writ_Bus(dat); //Write the low byte
}

/******************************************************************************
 * Function: LCD_WR_REG
 * Description: Writes a command to the LCD register.
 * Parameters:
 *   dat: The command to be written.
 ******************************************************************************/
void LCD_WR_REG(u8 dat) 
{
    LCD_DC_Clr(); //Set DC line low for command mode
    LCD_Writ_Bus(dat);
    LCD_DC_Set(); //Set DC line high for data mode
}

/******************************************************************************
 * Function: LCD_Address_Set
 * Description: Sets the address window for the LCD.
 * Parameters:
 *   x1, y1: Starting coordinates of the window.
 *   x2, y2: Ending coordinates of the window.
 ******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+0);
		LCD_WR_DATA(x2+0);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+0);
		LCD_WR_DATA(y2+0);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//储存器写
	}
}
/******************************************************************************
 * Function: delayLCD_ms
 * Description: Delays the execution for a specified number of milliseconds.
 * Parameters:
 *   ms: The number of milliseconds to delay.
 ******************************************************************************/
void delayLCD_ms(unsigned long ms) 
{
    delay_cycles(ms * (CPUCLK_FREQ / 1000));
}

/******************************************************************************
 * Function: LCD_Init
 * Description: Initializes the LCD with the required sequence of commands.
 ******************************************************************************/
void LCD_Init(void)
{
	LCD_GPIO_Init();//初始化GPIO
	// SPI1_Init();    //初始化SPI1
	LCD_RES_Clr();  //复位
	delayLCD_ms(100);
	LCD_RES_Set();
	delayLCD_ms(100);
	
	LCD_BLK_Set();//打开背光
    delayLCD_ms(100);
	
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); //Sleep out 
	delayLCD_ms(120);              //Delay 120ms 
	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
	LCD_WR_REG(0xB1); 
	LCD_WR_DATA8(0x05); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_REG(0xB2); 
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_REG(0xB3); 
	LCD_WR_DATA8(0x05); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_DATA8(0x05); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_DATA8(0x3C); 
	//------------------------------------End ST7735S Frame Rate---------------------------------// 
	LCD_WR_REG(0xB4); //Dot inversion 
	LCD_WR_DATA8(0x03); 
	//------------------------------------ST7735S Power Sequence---------------------------------// 
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA8(0x28); 
	LCD_WR_DATA8(0x08); 
	LCD_WR_DATA8(0x04); 
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA8(0XC0); 
	LCD_WR_REG(0xC2); 
	LCD_WR_DATA8(0x0D); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_REG(0xC3); 
	LCD_WR_DATA8(0x8D); 
	LCD_WR_DATA8(0x2A); 
	LCD_WR_REG(0xC4); 
	LCD_WR_DATA8(0x8D); 
	LCD_WR_DATA8(0xEE); 
	//---------------------------------End ST7735S Power Sequence-------------------------------------// 
	LCD_WR_REG(0xC5); //VCOM 
	LCD_WR_DATA8(0x1A); 
	LCD_WR_REG(0x36); //MX, MY, RGB mode 
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
	else LCD_WR_DATA8(0xA0); 
	//------------------------------------ST7735S Gamma Sequence---------------------------------// 
	LCD_WR_REG(0xE0); 
	LCD_WR_DATA8(0x04); 
	LCD_WR_DATA8(0x22); 
	LCD_WR_DATA8(0x07); 
	LCD_WR_DATA8(0x0A); 
	LCD_WR_DATA8(0x2E); 
	LCD_WR_DATA8(0x30); 
	LCD_WR_DATA8(0x25); 
	LCD_WR_DATA8(0x2A); 
	LCD_WR_DATA8(0x28); 
	LCD_WR_DATA8(0x26); 
	LCD_WR_DATA8(0x2E); 
	LCD_WR_DATA8(0x3A); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x03); 
	LCD_WR_DATA8(0x13); 
	LCD_WR_REG(0xE1); 
	LCD_WR_DATA8(0x04); 
	LCD_WR_DATA8(0x16); 
	LCD_WR_DATA8(0x06); 
	LCD_WR_DATA8(0x0D); 
	LCD_WR_DATA8(0x2D); 
	LCD_WR_DATA8(0x26); 
	LCD_WR_DATA8(0x23); 
	LCD_WR_DATA8(0x27); 
	LCD_WR_DATA8(0x27); 
	LCD_WR_DATA8(0x25); 
	LCD_WR_DATA8(0x2D); 
	LCD_WR_DATA8(0x3B); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x04); 
	LCD_WR_DATA8(0x13); 
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	LCD_WR_REG(0x3A); //65k mode 
	LCD_WR_DATA8(0x05); 
	LCD_WR_REG(0x29); //Display on 
}