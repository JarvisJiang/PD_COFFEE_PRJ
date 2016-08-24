/*********************************************************************************************************
*
* File                : sw_spi.c
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

#include <stm32f10x.h>
#include "ws_spi.h"
#include "stm32f10x_spi.h"

static void SPI1_Rcc_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
}

static void GPIO_SPI1_Configuration(void)
{
	/*********************************************************/
	/**** gpio config for SPI 1 ***********/
	
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
}

void SPI1_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStruct;

	SPI1_Rcc_Configuration();

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStruct);
	
	GPIO_SPI1_Configuration();

	SPI_Cmd(SPI1, ENABLE);
}

void SPI1_Send_byte(u16 data)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,data);

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);
	SPI_I2S_ReceiveData(SPI1);
}

u16 SPI1_Receive_byte(void)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,0x00);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI1);
}



