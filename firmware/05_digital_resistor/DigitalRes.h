#ifndef __DIGITALRES_H__
#define __DIGITALRES_H__

void DigitalRes_init(void);
void DigitalRes_SendData(uint8_t data);
void DigitalRes_Set(float Res);
void DigitalRes_Amplifier_Set(float Gain);

#endif