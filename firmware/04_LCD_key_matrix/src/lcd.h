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
#ifndef __LCD_H
#define __LCD_H		
// #include "sys.h"	 
#include "stdlib.h"

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

#include "ti_msp_dl_config.h"

typedef struct  
{										    
	u16 width;
	u16 height;
	u16 id;				  //LCD ID
	u8  dir;
	u16	 wramcmd;
	u16  setxcmd;
	u16  setycmd;
}_lcd_dev; 	

extern _lcd_dev lcddev;
#define USE_HORIZONTAL  	 1

//////////////////////////////////////////////////////////////////////////////////	  
#define LCD_W 320
#define LCD_H 240

extern u16  POINT_COLOR;
extern u16  BACK_COLOR;

// ////////////////////////////////////////////////////////////////////



#define	LCD_CS_SET  DL_GPIO_setPins(LCD_PORT, LCD_CS_PIN)
#define	LCD_RS_SET	DL_GPIO_setPins(LCD_PORT, LCD_DC_PIN)
#define	LCD_RST_SET	DL_GPIO_setPins(LCD_PORT, LCD_RES_PIN)

#define	LCD_CS_CLR  DL_GPIO_clearPins(LCD_PORT, LCD_CS_PIN)
#define	LCD_RS_CLR	DL_GPIO_clearPins(LCD_PORT, LCD_DC_PIN)
#define	LCD_RST_CLR	DL_GPIO_clearPins(LCD_PORT, LCD_RES_PIN)

#define LCD_BLK_Clr  DL_GPIO_clearPins(LCD_PORT, LCD_BLK_PIN) //BLK
#define LCD_BLK_Set  DL_GPIO_setPins(LCD_PORT, LCD_BLK_PIN)

#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40
#define BRRED 			0XFC07
#define GRAY  			0X8430

#define DARKBLUE      	 0X01CF
#define LIGHTBLUE      	 0X7D7C
#define GRAYBLUE       	 0X5458
 
#define LIGHTGREEN     	0X841F
#define LIGHTGRAY     0XEF5B
#define LGRAY 			 		0XC618

#define LGRAYBLUE      	0XA651
#define LBBLUE          0X2B12
	    															  
void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(u16 Color);	 
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);
u16  LCD_ReadPoint(u16 x,u16 y);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);

u16 LCD_RD_DATA(void);
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
void LCD_WR_DATA(u8 data);
u16 LCD_ReadReg(u8 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
u16 LCD_ReadRAM(void);		   
u16 LCD_BGR2RGB(u16 c);
void LCD_SetParam(void);
void Lcd_WriteData_16Bit(u16 Data);
void LCD_direction(u8 direction );

/*
#if LCD_USE8BIT_MODEL==1
	#define LCD_WR_DATA(data){\
	LCD_RS_SET;\
	LCD_CS_CLR;\
	DATAOUT(data);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	DATAOUT(data<<8);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	LCD_CS_SET;\
	}
	#else
	#define LCD_WR_DATA(data){\
	LCD_RS_SET;\
	LCD_CS_CLR;\
	DATAOUT(data);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	LCD_CS_SET;\
	} 	
#endif
*/
				  		 
#endif  
	 
	 



