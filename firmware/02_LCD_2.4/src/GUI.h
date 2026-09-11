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
#ifndef __GUI_H__
#define __GUI_H__

void GUI_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void Draw_Circle(u16 x0,u16 y0,u16 fc,u8 r);
void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len,u8 size,u8 mode);
void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode);
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode);
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p);
void gui_circle(int xc, int yc,u16 c,int r, int fill);
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode);
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2);


void LCD_Wave_320_240(u16 *wave, u16 color);
void LCD_Spectrum(u16 *spectrum, u16 color);
char* LCD_float2str(float fnum);
static void lcd_cat_label_val(char *dst, const char *label, const char *val);
void LCD_VppVrms(float vpp, float vrms);
void LCD_Phase_Mag(float phase, float mag, uint8_t index);












#endif

