#include "fft.h"

//char str[30];


arm_cfft_radix4_instance_f32  scfft;//定义scfft结构体
float FFT_InputBuf[FFT_LENGTH*2];	//FFT输入数组
float FFT_OutputBuf[FFT_LENGTH];	//FFT输出数组


__IO int maxindex;
float maxvalue;


float Freq;
float temp;
float boxing;                      
uint16_t flag;                     //波形标识
uint16_t flag_temp;
float show_buf[FFT_LENGTH];


void data_fft(void)
{
     for(int i=0; i < FFT_LENGTH; i++)
        {
            FFT_InputBuf[2*i]=adc_buff[i]*3.3/4095; //实部
            FFT_InputBuf[2*i+1]=0; 
        }    
     
    arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);//初始化
    arm_cfft_radix4_f32(&scfft,FFT_InputBuf);//傅里叶计算
    arm_cmplx_mag_f32(FFT_InputBuf,FFT_OutputBuf,FFT_LENGTH);//取模得幅值

}

        
void show_pic(void)
{
    show_buf[0]=FFT_OutputBuf[0]*1000/1024;
    for(int i=1;i<FFT_LENGTH/2;i++)
    {
        show_buf[i]=FFT_OutputBuf[i]*2000/1024;
        maxindex = i;
        
    }
}

void anlysize_figure(uint32_t tim_freq)
{
    
    maxvalue = 0;
    for(int i=2;i<FFT_LENGTH/2;i++)
    {
        if(FFT_OutputBuf[i]>maxvalue)
        {
            maxvalue=FFT_OutputBuf[i];
            maxindex = i;
        }
    }
    //判断波形
    boxing = FFT_OutputBuf[maxindex] / FFT_OutputBuf[3*maxindex];
    if(boxing < 5 && boxing >2)                                             //方波                          
    {
        flag = 1;
    }    
    else                  
    if(boxing < 30 && boxing >8)                                            //三角波                          
    {
        flag = 2;
    }   
    else                                           //正弦波                          
    {
        flag = 3;
    }   
    if(flag !=  flag_temp)
    {
        if(flag == 1) printf("fangbo\n\r");
        if(flag == 2) printf("sanjiaobo\n\r");
        if(flag == 3) printf("sinbo\n\r");
    }
    flag_temp = flag;
    //++maxindex;
    Freq = maxindex*(float)tim_freq/FFT_LENGTH;
    if(fabs(Freq - temp)>200)
    {
        printf("Freq:%.2f\n\r",Freq)   ;                       //频率计算
        printf("A:%f\n\r",FFT_OutputBuf[0]/(FFT_LENGTH));              //计算幅度
    }
}
        
     





