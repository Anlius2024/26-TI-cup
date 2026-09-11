#ifndef __DIGITALRES_H__
#define __DIGITALRES_H__


#define DIG_I2C_SET_SCL()   DL_GPIO_setPins(GPIO_I2C_PORT, GPIO_I2C_SCK_PIN)
#define DIG_I2C_CLR_SCL()   DL_GPIO_clearPins(GPIO_I2C_PORT, GPIO_I2C_SCK_PIN)

#define DIG_I2C_SET_SDA()   DL_GPIO_setPins(GPIO_I2C_PORT, GPIO_I2C_SDA_PIN)
#define DIG_I2C_CLR_SDA()   DL_GPIO_clearPins(GPIO_I2C_PORT, GPIO_I2C_SDA_PIN)

void MyI2C_Start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
void MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);

void DigitalRes_init(void);
void DigitalRes_SendData(uint8_t data);
void DigitalRes_Set(float Res);
void DigitalRes_Amplifier_Set(float Gain);

#endif