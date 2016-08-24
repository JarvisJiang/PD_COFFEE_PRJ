

#ifndef __ACTOL_UI_H
#define __ACTOL_UI_H

#include <stm32f10x.h>
#include "ws_spi.h"
#include "stm32f10x_spi.h"

#define BT_BAUDRATE_UART9600  	9600

#define BT_BAUDRATE_UART115200  115200

#define Scuess  0
#define Fail    1

#define ADD_DEVICE2SCENE	0x61
#define ADD_DEVICE2SCENE_ACK 0x8F

#define REM_DEVICE4SCENE	0x62
#define REM_DEVICE4SCENE_ACK 0x8E

#define SYNC_SIGNAL_DEVICE	0x63
#define SYNC_SIGNAL_DEVICE_ACK	0x8D

#define INQUIRE_DEVICE_INFO		0x64
#define INQUIRE_DEVICE_INFO_ACK		0x8C

#define SYNC_SCENE_DATA		0x65
#define SYNC_SCENE_DATA_ACK		0x8B

#define BRIGHTNESS_SCENE_CONTROL		0x68
#define BRIGHTNESS_SCENE_CONTROL_ACK		0x88

#define DELETE_A_SCENE		0x69
#define DELETE_A_SCENE_ACK		0x87

#define MAC_TRACK_CONTROL		0x6B
#define MAC_TRACK_CONTROL_ACK		0x85



/* Private typedef -----------------------------------------------------------*/
extern uint8_t TxBuffer1[32]; 
extern uint8_t TxBuffer2[32]; 
extern uint8_t RxBuffer1[32];
extern uint8_t RxBuffer2[32];
extern uint8_t Rx2_Buff1[32];
extern uint8_t Rx1_Buff1[32];

extern __IO uint8_t TxCounter1;
extern __IO uint8_t TxCounter2;
extern __IO uint8_t RxCounter1; 
extern __IO uint8_t RxCounter2;

extern __IO uint16_t Rx2_Current_pointer;
extern __IO uint16_t Rx2_Tail_pointer; 

extern uint8_t rec_f1,tx_flag1;
extern uint8_t rec_f2,tx_flag2;

extern uint32_t Rec_Len1;
extern uint32_t Rec_Len2;

extern void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
extern unsigned char Caculat_CRC (unsigned char *rBuffer, unsigned char rd_size);
extern void BT_UART2_ACK(uint8_t status,  uint8_t length, uint8_t Command, uint16_t Device_id);
extern void UARTx_Send_Command(USART_TypeDef* USARTx, uint8_t *Data, uint8_t length);
#endif /*__ACTOL_UI_H*/
