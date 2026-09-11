#include "ti_msp_dl_config.h"
#include "ad9850.h"

void ad9850_reset_serial()
{
AD_CLK_CLR() ;
AD_FQUP_CLR();
//rest信号
AD_RST_CLR();
AD_RST_SET();
AD_RST_CLR();
//w_clk信号
AD_CLK_CLR() ;
AD_CLK_SET();
AD_CLK_CLR() ;
//fq_up信号
AD_FQUP_CLR();
AD_FQUP_SET();
AD_FQUP_CLR();
}


void ad9850_wr_serial(unsigned char w0,double frequence)
{
unsigned char i,w;
long int y;
double x;
uint8_t ad9850_bit_data = 0;

//计算频率的HEX值
x=4294967295/125;//适合125M晶振
//如果时钟频率不为180MHZ，修改该处的频率值，单位MHz  ！！！
frequence=frequence/1000000;
frequence=frequence*x;
frequence=frequence*4.16667;
y=frequence;
//写w4数据
w=(y>>=0);
for(i=0;i<8;i++)
{
ad9850_bit_data=(w>>i)&0x01;
if(ad9850_bit_data == 1){
    AD_DATA_SET();
}
else{ 
    AD_DATA_CLR();
}
AD_CLK_SET();
AD_CLK_CLR();
}
//写w3数据
w=(y>>8);
for(i=0;i<8;i++)
{
ad9850_bit_data=(w>>i)&0x01;
if(ad9850_bit_data == 1){
    AD_DATA_SET();
}
else{ 
    AD_DATA_CLR();
}
AD_CLK_SET();
AD_CLK_CLR();
}
//写w2数据
w=(y>>16);
for(i=0;i<8;i++)
{
ad9850_bit_data=(w>>i)&0x01;
if(ad9850_bit_data == 1){
    AD_DATA_SET();
}
else{ 
    AD_DATA_CLR();
}
AD_CLK_SET();
AD_CLK_CLR();
}
//写w1数据
w=(y>>24);
for(i=0;i<8;i++)
{
ad9850_bit_data=(w>>i)&0x01;
if(ad9850_bit_data == 1){
    AD_DATA_SET();
}
else{ 
    AD_DATA_CLR();
}
AD_CLK_SET();
AD_CLK_CLR();
}
//写w0数据
w=w0;   
for(i=0;i<8;i++)
{
ad9850_bit_data=(w>>i)&0x01;
if(ad9850_bit_data == 1){
    AD_DATA_SET();
}
else{ 
    AD_DATA_CLR();
}
AD_CLK_SET();
AD_CLK_CLR();
}
//移入始能
AD_FQUP_SET();
AD_FQUP_CLR();
}

