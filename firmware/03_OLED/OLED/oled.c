#include "oled.h"
#include "oledfont.h"  	 
#include "ti/driverlib/m0p/dl_core.h"

enum I2cControllerStatus {
    I2C_STATUS_IDLE = 0,
    I2C_STATUS_TX_STARTED,
    I2C_STATUS_TX_INPROGRESS,
    I2C_STATUS_TX_COMPLETE,
    I2C_STATUS_RX_STARTED,
    I2C_STATUS_RX_INPROGRESS,
    I2C_STATUS_RX_COMPLETE,
    I2C_STATUS_ERROR,
} volatile gI2cControllerStatus;

volatile uint32_t gTxLen, gTxCount, gRxCount, gRxLen;
uint8_t gTxPacket[16];
uint8_t gRxPacket[16];

/* OLED 显存缓冲（8 页 × 128 列），所有绘制先写显存，再调用 OLED_Refresh() 刷屏 */
uint8_t OLED_GRAM[8][128];

/* 等待 I2C 完成的最长轮询次数（带超时防止卡死） */
#define I2C_BUSY_TIMEOUT  1000000

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
void delay_ms(unsigned long ms) 
{
    while(ms--)
	    delay_cycles(CPUCLK_FREQ/1000);
}

//反显函数
void OLED_ColorTurn(uint8_t i)
{
    if(i==0)
    {
        OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
    }
    if(i==1)
    {
        OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
    }
}

//屏幕旋转180度
void OLED_DisplayTurn(uint8_t i)
{
if(i==0)
    {
        OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
        OLED_WR_Byte(0xA1,OLED_CMD);
    }
    if(i==1)
    {
        OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
        OLED_WR_Byte(0xA0,OLED_CMD);
    }
}

//发送一个字节
//向SSD1306写入一个字节。
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat,uint8_t mode)
{
    volatile uint32_t timeout;

    gI2cControllerStatus = I2C_STATUS_IDLE;
    gTxLen = 2;

    if(mode)
        gTxPacket[0] = 0x40;
    else
        gTxPacket[0] = 0x00;
    gTxPacket[1] = dat;

    gTxCount = DL_I2C_fillControllerTXFIFO(I2C_OLED_INST, &gTxPacket[0], gTxLen);

    DL_I2C_disableInterrupt(I2C_OLED_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);

    gI2cControllerStatus = I2C_STATUS_TX_STARTED;

    /* 等待总线空闲（带超时，避免卡死） */
    timeout = I2C_BUSY_TIMEOUT;
    while ((!(DL_I2C_getControllerStatus(I2C_OLED_INST) &
              DL_I2C_CONTROLLER_STATUS_IDLE)) &&
           (timeout-- > 0))
        ;

    if (timeout > 0) {
        DL_I2C_startControllerTransfer(I2C_OLED_INST, 0x3C,
            DL_I2C_CONTROLLER_DIRECTION_TX, gTxLen);

        /* 等待发送完成或出错（带超时） */
        timeout = I2C_BUSY_TIMEOUT;
        while ((gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) &&
               (gI2cControllerStatus != I2C_STATUS_ERROR) &&
               (timeout-- > 0))
            ;

        timeout = I2C_BUSY_TIMEOUT;
        while ((DL_I2C_getControllerStatus(I2C_OLED_INST) &
                DL_I2C_CONTROLLER_STATUS_BUSY_BUS) &&
               (timeout-- > 0))
            ;

        timeout = I2C_BUSY_TIMEOUT;
        while ((!(DL_I2C_getControllerStatus(I2C_OLED_INST) &
                  DL_I2C_CONTROLLER_STATUS_IDLE)) &&
               (timeout-- > 0))
            ;
    }
}

//坐标设置

void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 
    OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f),OLED_CMD);
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//刷新显存到OLED
void OLED_Refresh(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)
    {
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址
        for(n=0;n<128;n++)
            OLED_WR_Byte(OLED_GRAM[i][n],OLED_DATA);
    }
}

//在指定坐标画一个点（写入显存，调用 OLED_Refresh() 后生效）
//x:0~127 列; y:0~63 行; t:1 点亮, 0 熄灭
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
    uint8_t page, bit, mask;
    if((x>127) || (y>63)) return;   //越界保护
    page = y/8;
    bit  = y%8;
    mask = (uint8_t)(1<<bit);
    if(t)
        OLED_GRAM[page][x] |= mask;              //点亮
    else
        OLED_GRAM[page][x] &= (uint8_t)~mask;    //熄灭
}

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)
        for(n=0;n<128;n++)
            OLED_GRAM[i][n]=0;
    OLED_Refresh();
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63				 
//sizey:选择字体 6x8  8x16
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t sizey)
{      	
    uint8_t c=0,sizex=sizey/2;
    uint16_t i=0,size1;
    if(sizey==8)size1=6;
    else size1=(sizey/8+((sizey%8)?1:0))*(sizey/2);
    c=chr-' ';//得到偏移后的值
    for(i=0;i<size1;i++)
    {
        if(i%sizex==0&&sizey!=8) y+=1;   //8x16 字号：第二页
        if(sizey==8) OLED_GRAM[y][x+i]=asc2_0806[c][i];        //6X8字号
        else if(sizey==16) OLED_GRAM[y][x+i%sizex]=asc2_1608[c][i];//8x16字号
        //		else if(sizey==xx) OLED_GRAM[y][x+i]=asc2_xxxx[c][i];//用户添加字号
        else return;
    }
}
//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;	 
    while(n--)result*=m;    
    return result;
}				  
//显示数字
//x,y :起点坐标
//num:要显示的数字
//len :数字的位数
//sizey:字体大小		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t sizey)
{         	
    uint8_t t,temp,m=0;
    uint8_t enshow=0;
    if(sizey==8)m=2;
    for(t=0;t<len;t++)
    {
        temp=(num/oled_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(sizey/2+m)*t,y,' ',sizey);
                continue;
            }else enshow=1;
        }
        OLED_ShowChar(x+(sizey/2+m)*t,y,temp+'0',sizey);
    }
}
//显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t sizey)
{
    uint8_t j=0;
    while (chr[j]!='\0')
    {		
        OLED_ShowChar(x,y,chr[j++],sizey);
        if(sizey==8)x+=6;
        else x+=sizey/2;
    }
}
//显示汉字
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t no,uint8_t sizey)
{
    uint16_t i,size1=(sizey/8+((sizey%8)?1:0))*sizey;
    for(i=0;i<size1;i++)
    {
        if(i%sizey==0) y+=1;
        if(sizey==16) OLED_GRAM[y][x+i%16]=Hzk[no][i];//16x16字号
        //		else if(sizey==xx) OLED_GRAM[y][x+i]=xxx[c][i];//用户添加字号
        else return;
    }				
}


//显示图片
//x,y显示坐标
//sizex,sizey,图片长宽
//BMP：要显示的图片
void OLED_DrawBMP(uint8_t x,uint8_t y,uint8_t sizex, uint8_t sizey,uint8_t BMP[])
{ 	
    uint16_t j=0;
    uint8_t i,m;
    sizey=sizey/8+((sizey%8)?1:0);
    for(i=0;i<sizey;i++)
    {
        for(m=0;m<sizex;m++)
        {      
            OLED_GRAM[i+y][x+m]=BMP[j++];
        }
    }
} 


//画波形（128 列采样点）
//wave[i] 为第 i 列对应的像素行高度(0~63)，自动清屏并刷屏
void OLED_Wave(uint16_t *wave)
{
    uint8_t i, p;
    uint16_t j;

    /* 清显存（不立即刷屏，最后一次性刷新） */
    for (p = 0; p < 8; p++)
        for (i = 0; i < 128; i++)
            OLED_GRAM[p][i] = 0;

    /* 逐列画点 */
    for (i = 0; i < 128; i++) {
        j = wave[i];
        if (j >= 64) j = 63;                         /* 限幅到 0~63 */
        OLED_GRAM[j / 8][i] |= (uint8_t)(1u << (j % 8));
    }
    OLED_Refresh();
}

//画频谱（128 个幅度值 0~63，逻辑与波形相同）
void OLED_Spectrum(uint16_t *spectrum)
{
    OLED_Wave(spectrum);
}

//浮点转字符串（固定 4 位小数），返回静态缓冲区，可重复调用
char* OLED_float2str(float fnum)
{
    static char buf[20];
    uint32_t zhengshu, xiaoshu;
    char tmp[12];
    int8_t n = 0;
    uint8_t i;
    char *p = buf;

    /* 符号处理 */
    if (fnum < 0.0f) { *p++ = '-'; fnum = -fnum; }

    /* 整数部分 + 小数部分（乘 10000 取整） */
    zhengshu = (uint32_t)fnum;
    xiaoshu  = (uint32_t)((fnum - (float)zhengshu) * 10000.0f + 0.5f);

    /* 进位处理：1.99995 -> 2.0000 */
    if (xiaoshu >= 10000) { zhengshu++; xiaoshu = 0; }

    /* 写整数部分（倒序） */
    if (zhengshu == 0) tmp[n++] = '0';
    while (zhengshu > 0) { tmp[n++] = (char)('0' + zhengshu % 10); zhengshu /= 10; }
    while (n > 0) *p++ = tmp[--n];

    /* 写小数部分，固定 4 位 */
    *p++ = '.';
    for (i = 0; i < 4; i++) {
        *p++ = (char)('0' + xiaoshu / 1000);
        xiaoshu = (xiaoshu % 1000) * 10;
    }

    *p = '\0';
    return buf;
}

/* 把标签字符串和数值字符串拼接进 dst（不用 string.h） */
static void oled_cat_label_val(char *dst, const char *label, const char *val)
{
    while (*label) *dst++ = *label++;
    while (*val)   *dst++ = *val++;
    *dst = '\0';
}

//显示 Vpp / Vrms（单位 mV）
void OLED_VppVrms(float vpp, float vrms)
{
    char buf1[32];
    char buf2[32];

    OLED_Clear();

    oled_cat_label_val(buf1, "Vpp:  ", OLED_float2str(vpp));
    oled_cat_label_val(buf2, buf1, " mV");
    OLED_ShowString(1, 0, (uint8_t*)buf2, 16);      //第 1 行（页 0）

    oled_cat_label_val(buf1, "Vrms: ", OLED_float2str(vrms));
    oled_cat_label_val(buf2, buf1, " mV");
    OLED_ShowString(1, 2, (uint8_t*)buf2, 16);      //第 2 行（页 2）
}

//显示相位/幅度；index=0 显示在第 1/3 行，index=1 显示在第 5/7 行
//phase 单位 kHz，mag 单位 mV（对应 LCD 原版 1+32*index / 17+32*index 换算为页）
void OLED_Phase_Mag(float phase, float mag, uint8_t index)
{
    char buf1[32];
    char buf2[32];

    oled_cat_label_val(buf1, "Phase: ", OLED_float2str(phase));
    oled_cat_label_val(buf2, buf1, " kHz");
    OLED_ShowString(1, (uint8_t)(index * 4), (uint8_t*)buf2, 16);      //页 0 / 4

    oled_cat_label_val(buf1, "Mag:   ", OLED_float2str(mag));
    oled_cat_label_val(buf2, buf1, " mV");
    OLED_ShowString(1, (uint8_t)(index * 4 + 2), (uint8_t*)buf2, 16);  //页 2 / 6
}


//初始化SSD1306					    
void OLED_Init(void)
{
    delay_ms(200);
    NVIC_EnableIRQ(I2C_OLED_INST_INT_IRQN);
    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
    OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_CMD);//-not offset
    OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
    OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(0x02,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
    OLED_Clear();
    OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
}  

void I2C_OLED_INST_IRQHandler(void)
{
    switch (DL_I2C_getPendingInterrupt(I2C_OLED_INST)) {
        case DL_I2C_IIDX_CONTROLLER_RX_DONE:
            gI2cControllerStatus = I2C_STATUS_RX_COMPLETE;
            break;
        case DL_I2C_IIDX_CONTROLLER_TX_DONE:
            DL_I2C_disableInterrupt(
                I2C_OLED_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
            gI2cControllerStatus = I2C_STATUS_TX_COMPLETE;
            break;
        case DL_I2C_IIDX_CONTROLLER_RXFIFO_TRIGGER:
            gI2cControllerStatus = I2C_STATUS_RX_INPROGRESS;
            /* Receive all bytes from target */
            while (DL_I2C_isControllerRXFIFOEmpty(I2C_OLED_INST) != true) {
                if (gRxCount < gRxLen) {
                    gRxPacket[gRxCount++] =
                        DL_I2C_receiveControllerData(I2C_OLED_INST);
                } else {
                    /* Ignore and remove from FIFO if the buffer is full */
                    DL_I2C_receiveControllerData(I2C_OLED_INST);
                }
            }
            break;
        case DL_I2C_IIDX_CONTROLLER_TXFIFO_TRIGGER:
            gI2cControllerStatus = I2C_STATUS_TX_INPROGRESS;
            /* Fill TX FIFO with next bytes to send */
            if (gTxCount < gTxLen) {
                gTxCount += DL_I2C_fillControllerTXFIFO(
                    I2C_OLED_INST, &gTxPacket[gTxCount], gTxLen - gTxCount);
            }
            break;
            /* Not used for this example */
        case DL_I2C_IIDX_CONTROLLER_ARBITRATION_LOST:
        case DL_I2C_IIDX_CONTROLLER_NACK:
            if ((gI2cControllerStatus == I2C_STATUS_RX_STARTED) ||
                (gI2cControllerStatus == I2C_STATUS_TX_STARTED)) {
                /* NACK interrupt if I2C Target is disconnected */
                gI2cControllerStatus = I2C_STATUS_ERROR;
            }
        case DL_I2C_IIDX_CONTROLLER_RXFIFO_FULL:
        case DL_I2C_IIDX_CONTROLLER_TXFIFO_EMPTY:
        case DL_I2C_IIDX_CONTROLLER_START:
        case DL_I2C_IIDX_CONTROLLER_STOP:
        case DL_I2C_IIDX_CONTROLLER_EVENT1_DMA_DONE:
        case DL_I2C_IIDX_CONTROLLER_EVENT2_DMA_DONE:
        default:
            break;
    }
}