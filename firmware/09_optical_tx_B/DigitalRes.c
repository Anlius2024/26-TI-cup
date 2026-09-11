#include "ti_msp_dl_config.h"
#include "DigitalRes.h"

/* Maximum size of TX packet */
#define I2C_TX_MAX_PACKET_SIZE (16)

/* Number of bytes to send to target device */
#define I2C_TX_PACKET_SIZE (2)

/* Maximum size of RX packet */
#define I2C_RX_MAX_PACKET_SIZE (16)

/* Number of bytes to received from target */
#define I2C_RX_PACKET_SIZE (2)

/* I2C Target address */
#define I2C_TARGET_ADDRESS (0x2E)

/* Data sent to the Target */
uint8_t gTxPacket[I2C_TX_MAX_PACKET_SIZE] = {0x00, 0x2f};
/* Counters for TX length and bytes sent */
uint32_t gTxLen, gTxCount;

/* Data received from Target */
uint8_t gRxPacket[I2C_RX_MAX_PACKET_SIZE];
/* Counters for TX length and bytes sent */
uint32_t gRxLen, gRxCount;
/* Indicates status of I2C */
enum I2cControllerStatus {
    I2C_STATUS_IDLE = 0,
    I2C_STATUS_TX_STARTED,
    I2C_STATUS_TX_INPROGRESS,
    I2C_STATUS_TX_COMPLETE,
    I2C_STATUS_RX_STARTED,
    I2C_STATUS_RX_INPROGRESS,
    I2C_STATUS_RX_COMPLETE,
    I2C_STATUS_ERROR,
} gI2cControllerStatus;




void DigitalRes_init(void)
{
    NVIC_EnableIRQ(I2C_INST_INT_IRQN);
}

void DigitalRes_SendData(uint8_t data)
{
    gI2cControllerStatus = I2C_STATUS_IDLE;
    gTxLen = I2C_TX_PACKET_SIZE;
    gTxPacket[1] = data;
    /*
     * Fill the FIFO
     *  The FIFO is 8-bytes deep, and this function will return number
     *  of bytes written to FIFO */
    gTxCount = DL_I2C_fillControllerTXFIFO(I2C_INST, &gTxPacket[0], gTxLen);
    /* Enable TXFIFO trigger interrupt if there are more bytes to send */
    if (gTxCount < gTxLen) {
        DL_I2C_enableInterrupt(
            I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
    } else {
        DL_I2C_disableInterrupt(
            I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
    }
    /*
     * Send the packet to the controller.
     * This function will send Start + Stop automatically.
     */
    gI2cControllerStatus = I2C_STATUS_TX_STARTED;
    while (!(
        DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
        
    DL_I2C_startControllerTransfer(
        I2C_INST, I2C_TARGET_ADDRESS, DL_I2C_CONTROLLER_DIRECTION_TX, gTxLen);

    /* Wait until the Controller sends all bytes */
    while ((gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) &&
           (gI2cControllerStatus != I2C_STATUS_ERROR)) {
        __WFE();
    }

    while (DL_I2C_getControllerStatus(I2C_INST) &
           DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
        ;

    /* Trap if there was an error */
    if (DL_I2C_getControllerStatus(I2C_INST) &
        DL_I2C_CONTROLLER_STATUS_ERROR) {
        /* LED will remain high if there is an error */
        __BKPT(0);
    }

    while (!(
        DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_IDLE))
        ;
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





void I2C_INST_IRQHandler(void)
{
    switch (DL_I2C_getPendingInterrupt(I2C_INST)) {
        case DL_I2C_IIDX_CONTROLLER_RX_DONE:
            gI2cControllerStatus = I2C_STATUS_RX_COMPLETE;
            break;
        case DL_I2C_IIDX_CONTROLLER_TX_DONE:
            DL_I2C_disableInterrupt(
                I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
            gI2cControllerStatus = I2C_STATUS_TX_COMPLETE;
            break;
        case DL_I2C_IIDX_CONTROLLER_RXFIFO_TRIGGER:
            gI2cControllerStatus = I2C_STATUS_RX_INPROGRESS;
            /* Receive all bytes from target */
            while (DL_I2C_isControllerRXFIFOEmpty(I2C_INST) != true) {
                if (gRxCount < gRxLen) {
                    gRxPacket[gRxCount++] =
                        DL_I2C_receiveControllerData(I2C_INST);
                } else {
                    /* Ignore and remove from FIFO if the buffer is full */
                    DL_I2C_receiveControllerData(I2C_INST);
                }
            }
            break;
        case DL_I2C_IIDX_CONTROLLER_TXFIFO_TRIGGER:
            gI2cControllerStatus = I2C_STATUS_TX_INPROGRESS;
            /* Fill TX FIFO with next bytes to send */
            if (gTxCount < gTxLen) {
                gTxCount += DL_I2C_fillControllerTXFIFO(
                    I2C_INST, &gTxPacket[gTxCount], gTxLen - gTxCount);
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
