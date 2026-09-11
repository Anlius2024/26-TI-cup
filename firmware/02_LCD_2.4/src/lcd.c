//////////////////////////////////////////////////////////////////////////////////
// LCD driver provenance
// ------------------------------------------------------------------------------
// This TFT LCD driver (ILI9341 2.4" / ST7735 1.8" module) originates from the
// QDtech / lcdwiki reference code for STM32, and was ported to the TI MSPM0G3507
// using DriverLib.  See docs/THIRD_PARTY_NOTICES.md for full attribution.
//
// The original header carried the vendor's phone / e-mail / QQ contact details.
// Those have been removed here.  The original Chinese comments in this file were
// destroyed by an earlier mis-encoded save (every non-ASCII byte became U+FFFD)
// and were therefore not recoverable; comments below have been rewritten in
// English based on the surviving code and on the intact copy of the same header
// in FONT.H.
//
// Original reference module wiring (STM32, kept for comparison only -- the
// MSPM0G3507 pin assignment is configured through SysConfig, see the .syscfg):
//
//   LCD module      STM32        function
//   VCC             DC5V/3.3V    power
//   GND             GND          ground
//   SDI(MOSI)       PB15         SPI data write
//   SDO(MISO)       PB14         SPI data read (may be left unconnected)
//   LED             PB9          backlight control (tie to 5V/3.3V if unused)
//   SCK             PB13         SPI clock
//   DC/RS           PB10         data / command select
//   RST             PB12         reset
//   CS              PB11         chip select
//
// Touch panel pins (only for modules that carry a touch controller):
//   T_IRQ           PC10         touch interrupt
//   T_DO            PC2          touch SPI read
//   T_DIN           PC3          touch SPI write
//   T_CS            PC13         touch chip select
//   T_CLK           PC0          touch SPI clock
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 **************************************************************************************************/

 /* @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/	
#include "lcd.h"
#include "stdlib.h"
#include "delay.h"	 
// #include "SPI.h"

	   
_lcd_dev lcddev;

u16 POINT_COLOR = WHITE , BACK_COLOR = BLACK;  
u16 DeviceCode;	 

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(u8 data)
{ 
   LCD_CS_CLR;     
	 LCD_RS_CLR;	  
    DL_SPI_transmitData8(SPI_LCD_INST, data);
    while(DL_SPI_isBusy(SPI_LCD_INST));
   LCD_CS_SET;	
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u8 data)
{
   LCD_CS_CLR;
	 LCD_RS_SET;
   DL_SPI_transmitData8(SPI_LCD_INST, data);
   while(DL_SPI_isBusy(SPI_LCD_INST));
   LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	 
void Lcd_WriteData_16Bit(u16 Data)
{	
   LCD_CS_CLR;
   LCD_RS_SET;  
   DL_SPI_transmitData8(SPI_LCD_INST, Data >> 8);
   while(DL_SPI_isBusy(SPI_LCD_INST));
	DL_SPI_transmitData8(SPI_LCD_INST, Data);
	while(DL_SPI_isBusy(SPI_LCD_INST));
   LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);
	Lcd_WriteData_16Bit(POINT_COLOR); 
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(u16 Color)
{
  unsigned int i,m;  
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
	LCD_CS_CLR;
	LCD_RS_SET;
	for(i=0;i<lcddev.height;i++)
	{
    for(m=0;m<lcddev.width;m++)
    {	
			Lcd_WriteData_16Bit(Color);
		}
	}
	 LCD_CS_SET;
} 

/*****************************************************************************
 * @name       :void LCD_GPIOInit(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen GPIO
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_GPIOInit(void)
{
	// GPIO_InitTypeDef  GPIO_InitStructure;	      
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12; //GPIOB9,10,11,12
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_RESET(void)
{
	LCD_RST_CLR;
	delay_ms(100);	
	LCD_RST_SET;
	delay_ms(50);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{  
	LCD_GPIOInit();
 	LCD_RESET();
	LCD_WR_REG(0xCF);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0xD9); //0xC1 
	LCD_WR_DATA(0X30); 
	LCD_WR_REG(0xED);  
	LCD_WR_DATA(0x64); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0X12); 
	LCD_WR_DATA(0X81); 
	LCD_WR_REG(0xE8);  
	LCD_WR_DATA(0x85); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x7A); 
	LCD_WR_REG(0xCB);  
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x34); 
	LCD_WR_DATA(0x02); 
	LCD_WR_REG(0xF7);  
	LCD_WR_DATA(0x20); 
	LCD_WR_REG(0xEA);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xC0);    //Power control 
	LCD_WR_DATA(0x1B);   //VRH[5:0] 
	LCD_WR_REG(0xC1);    //Power control 
	LCD_WR_DATA(0x12);   //SAP[2:0];BT[3:0] 0x01
	LCD_WR_REG(0xC5);    //VCM control 
	LCD_WR_DATA(0x08); 	 //30
	LCD_WR_DATA(0x26); 	 //30
	LCD_WR_REG(0xC7);    //VCM control2 
	LCD_WR_DATA(0XB7); 
	LCD_WR_REG(0x36);    // Memory Access Control 
	LCD_WR_DATA(0x08); 
	LCD_WR_REG(0x3A);   
	LCD_WR_DATA(0x55); 
	LCD_WR_REG(0xB1);   
	LCD_WR_DATA(0x00);   
	LCD_WR_DATA(0x1A); 
	LCD_WR_REG(0xB6);    // Display Function Control 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0xA2); 
	LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0x26);    //Gamma curve selected 
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xE0);    //Set Gamma 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x1D); 
	LCD_WR_DATA(0x1A); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x0D); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x49); 
	LCD_WR_DATA(0X66); 
	LCD_WR_DATA(0x3B); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x11); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x09); 
	LCD_WR_DATA(0x05); 
	LCD_WR_DATA(0x04); 		 
	LCD_WR_REG(0XE1);    //Set Gamma 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x18); 
	LCD_WR_DATA(0x1D); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x04); 
	LCD_WR_DATA(0x36); 
	LCD_WR_DATA(0x13); 
	LCD_WR_DATA(0x4C); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x13); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x2E); 
	LCD_WR_DATA(0x2F); 
	LCD_WR_DATA(0x05); 
	LCD_WR_REG(0x2B); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);
	LCD_WR_REG(0x2A); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);	 
	LCD_WR_REG(0x11); //Exit Sleep
	delay_ms(120);
	LCD_WR_REG(0x29); //display on		

  LCD_direction(USE_HORIZONTAL);
	LCD_BLK_Set;
	LCD_Clear(WHITE);
}
 
/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar);		
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar);		
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd);

	LCD_WriteRAM_Prepare();
}   

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(u8 direction)
{ 
			lcddev.setxcmd=0x2A;
			lcddev.setycmd=0x2B;
			lcddev.wramcmd=0x2C;
	switch(direction){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x36,(1<<3)|(0<<6)|(0<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(0<<7)|(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x36,(1<<3)|(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;	
		default:break;
	}		
}	 
