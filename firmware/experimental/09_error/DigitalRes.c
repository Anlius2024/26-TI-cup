#include "ti_msp_dl_config.h"
#include "DigitalRes.h"
#include "delay.h"


void MyI2C_Start(void)
{
	DIG_I2C_SET_SDA();							//释放SDA，确保SDA为高电平
	DIG_I2C_SET_SCL();							//释放SCL，确保SCL为高电平
	DIG_I2C_CLR_SDA();							//在SCL高电平期间，拉低SDA，产生起始信号
	DIG_I2C_CLR_SCL();							//起始后把SCL也拉低，即为了占用总线，也为了方便总线时序的拼接
}


void MyI2C_Stop(void)
{
	DIG_I2C_CLR_SDA();							//拉低SDA，确保SDA为低电平
	DIG_I2C_SET_SCL();							//释放SCL，使SCL呈现高电平
	DIG_I2C_SET_SDA();							//在SCL高电平期间，释放SDA，产生终止信号
}


void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i, temp;
	for (i = 0; i < 8; i ++)				//循环8次，主机依次发送数据的每一位
	{
		/*两个!可以对数据进行两次逻辑取反，作用是把非0值统一转换为1，即：!!(0) = 0，!!(非0) = 1*/
        temp = !!(Byte & (0x80 >> i));
        if(temp == 0){
            DIG_I2C_CLR_SDA();
        }else{
            DIG_I2C_SET_SDA();
        }
		DIG_I2C_SET_SCL();						//释放SCL，从机在SCL高电平期间读取SDA
		DIG_I2C_CLR_SCL();						//拉低SCL，主机开始发送下一位数据
	}
}



void MyI2C_SendAck(uint8_t AckBit)
{
    if(AckBit == 1) DIG_I2C_SET_SDA();
    else            DIG_I2C_CLR_SDA();
	DIG_I2C_SET_SCL();							//释放SCL，从机在SCL高电平期间，读取应答位
	DIG_I2C_CLR_SCL();							//拉低SCL，开始下一个时序模块
}

uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit = 1;							//定义应答位变量
	DIG_I2C_SET_SDA();							//接收前，主机先确保释放SDA，避免干扰从机的数据发送
	DIG_I2C_SET_SCL();							//释放SCL，主机机在SCL高电平期间读取SDA
	// AckBit = MyI2C_R_SDA();					//将应答位存储到变量里
	AckBit = DL_GPIO_readPins(GPIO_I2C_PORT, GPIO_I2C_SDA_PIN);		//读取SDA电平
	delay_us(10);
	DIG_I2C_CLR_SCL();							//拉低SCL，开始下一个时序模块
	return AckBit;							//返回定义应答位变量
}



void DigitalRes_SendData(uint8_t data)
{
	uint8_t AckBit = 0;
	MyI2C_Start();						//I2C起始
	MyI2C_SendByte(0x5C);	//发送从机地址，读写位为0，表示即将写入
	AckBit = 1;
	AckBit = MyI2C_ReceiveAck();					//接收应答
	__BKPT(0);
	MyI2C_SendByte(0x00);			//发送写命令
	AckBit = 1;
	AckBit = MyI2C_ReceiveAck();					//接收应答
	if(AckBit == 0){
		__BKPT(0);
	}
	__BKPT(0);
	MyI2C_SendByte(data);				//发送要写入寄存器的数据
	AckBit = 1;
	AckBit = MyI2C_ReceiveAck();					//接收应答
	__BKPT(0);
	MyI2C_Stop();						//I2C终止
}






void DigitalRes_init(void)
{
    // NVIC_EnableIRQ(I2C_INST_INT_IRQN);
}


// 步进10k/128
void DigitalRes_Set(float Res)
{
    if (Res < 0) {
        Res = 0;
    } else if (Res > 10000) {
        Res = 10000;
    }

    uint8_t data = (uint8_t)(Res * 128.f / 10000.f);
    if (data > 127) {
        data = 127;
    }
    DigitalRes_SendData(data);
}

// 0-3.175，步进0.025
void DigitalRes_Amplifier_Set(float Gain)
{
    if (Gain < 0) {
        Gain = 0;
    } else if (Gain > 3.175) {
        Gain = 3.175;
    }

    // 补偿误差
    if(Gain >= 1 && Gain < 2)   Gain -= 0.1;
    else if(Gain >= 2 && Gain <= 3.175)     Gain -= 0.125;

    float data = Gain * 128.f / 3.2f;
    DigitalRes_SendData(data);

}






