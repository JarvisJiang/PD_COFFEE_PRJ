#include "stm32f10x.h"
#include "platform_config.h" 
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stdarg.h"
#include "ws_AT45DBXX.h"
#include <string.h>
#include <stdio.h>
#include "actol_bluetooth.h"
#include "actol_rfm69.h"
#include "rfm69h.h"

/* User the Send data and command to rfm69 communication port */

unsigned char Caculat_rfm69_CRC (unsigned char *rBuffer, unsigned char rd_size)
{
	unsigned char ii;
	unsigned char r_CRC;	
	
	r_CRC = 0;
		
	for(ii=1; ii< rd_size; ii++)
	{
			r_CRC = (r_CRC + rBuffer[ii]) & 0xFF;

	}
	return ((0x100 - r_CRC) & 0xff);
}


void Hander_MCU_rfm69(uint8_t *bdata, uint8_t Command, uint8_t lenght)
{
	uint8_t i;
	
	TxBuffer2[0] = 0x55;
	TxBuffer2[1] = lenght;
	TxBuffer2[2] = Command;
	
	for(i = 0; i < lenght-1; i++)
	{
		TxBuffer2[3+i] = bdata[i];		
	}
	
	TxBuffer2[lenght+1] = Caculat_CRC(TxBuffer2, lenght);
	
	Tx_rfm69_Buffer[0] = lenght + 3;	
	
	for(i = 1; i < lenght+3; i++)
	{
		Tx_rfm69_Buffer[i] = TxBuffer2[i-1];
	}	
	

	RFM69H_TxData(Tx_rfm69_Buffer, lenght+3);
		
}



