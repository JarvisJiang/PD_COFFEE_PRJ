#ifndef __RFM69H_H
#define __RFM69H_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#define NSS_L 	GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define NSS_H 	GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define SCK_L 	GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define SCK_H 	GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define MOSI_L 	GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define MOSI_H 	GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define MISO_R	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)	

#define Addr_RF_FIFO 0x00

#define RF_315MHz 0
#define RF_434MHz 1
#define RF_868MHz 2
#define RF_915MHz 3

#define RF_Rate1K2 0
#define RF_Rate2K4 1
#define RF_Rate4K8 2
#define RF_Rate9K6 3

#define RF_Power13dBm 0
#define RF_Power10dBm 1
#define RF_Power7dBm  2
#define RF_Power4dBm  3

#define RF_RxIndicatePin GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_2)
#define RF_TxIndicatePin GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_3)
#define RF_ResetDisable	GPIO_ResetBits(GPIOB, GPIO_Pin_10); 
#define RF_ResetEnable	GPIO_SetBits(GPIOB, GPIO_Pin_10); 

enum RF_ModeTable {RxMode,TxMode,StandBy,Sleep};	//RFM69 mode define

void SPI_WriteByte(u8 WrPara);
u8   SPI_ReadByte(void);

void RFM69H_Config(void);
u8 RFM69H_ModeSelect(u8 mode);
void RFM69H_ClearFIFO(void);
u8   RFM69H_ReadRSSI(void);
u8   RFM69H_ReadByte(u8 adr);
void RFM69H_WriteByte(u16 WrPara);
void RFM69H_BurstRead(u8 adr, u8 *ptr, u8 length);
void RFM69H_BurstWrite(u8 adr, u8 *ptr, u8 length);
u8   RFM69H_RxData(u8 *dat);
u8 	 RFM69H_TxData(u8 *dat, u8 length);

extern unsigned char Tx_rfm69_Buffer[32];
extern void Delay(__IO uint32_t nCount);
extern unsigned char RF_DataBuf[32];
extern void Delay_Ms(unsigned int ms);

#endif /*__RFM69_H*/
