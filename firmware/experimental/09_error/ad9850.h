#ifndef __AD9850_H__
#define __AD9850_H__


#define AD_CLK_SET()   DL_GPIO_setPins(GPIO_DDS_PORT, GPIO_DDS_PIN_CLK_PIN);
#define AD_CLK_CLR()   DL_GPIO_clearPins(GPIO_DDS_PORT, GPIO_DDS_PIN_CLK_PIN);

#define AD_FQUP_SET()   DL_GPIO_setPins(GPIO_DDS_PORT, GPIO_DDS_PIN_FQUD_PIN);
#define AD_FQUP_CLR()   DL_GPIO_clearPins(GPIO_DDS_PORT, GPIO_DDS_PIN_FQUD_PIN);

#define AD_DATA_SET()   DL_GPIO_setPins(GPIO_DDS_PORT, GPIO_DDS_PIN_DATA_PIN);
#define AD_DATA_CLR()   DL_GPIO_clearPins(GPIO_DDS_PORT, GPIO_DDS_PIN_DATA_PIN);

#define AD_RST_SET()   DL_GPIO_setPins(GPIO_DDS_PORT, GPIO_DDS_PIN_RST_PIN);
#define AD_RST_CLR()   DL_GPIO_clearPins(GPIO_DDS_PORT, GPIO_DDS_PIN_RST_PIN);

void ad9850_reset_serial();
void ad9850_wr_serial(unsigned char w0,double frequence);


#endif