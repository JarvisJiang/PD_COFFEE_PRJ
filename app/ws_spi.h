/*********************************************************************************************************
*
* File                : ws_spi.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

struct Receive_data
{
	unsigned int num;
	unsigned char data[20];

};

extern __IO struct Receive_data RX2_rec_data[500];

static void SPI1_Rcc_Configuration(void);
static void GPIO_SPI1_Configuration(void);
void SPI1_Configuration(void);
void SPI1_Send_byte(u16 data);
u16 SPI1_Receive_byte(void);

#endif /*__SPI_H*/
