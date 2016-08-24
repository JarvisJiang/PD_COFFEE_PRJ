#include "stm32f10x.h"
#include "platform_config.h" 
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stdarg.h"
#include "ws_AT45DBXX.h"
#include <string.h>
#include <stdio.h>
#include "actol_bluetooth.h"


unsigned char Caculat_CRC (unsigned char *rBuffer, unsigned char rd_size)
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


void BT_UART2_ACK(uint8_t status,  uint8_t length, uint8_t Command, uint16_t Device_id)
{
	TxBuffer2[0] = 0xAA;
	TxBuffer2[1] = length & 0xFF;	
	TxBuffer2[2] = Command;
	//TxBuffer2[3] = Device_id >>8;
	//TxBuffer2[4] = Device_id & 0xFF;
	TxBuffer2[3] = status;
	TxBuffer2[4] = Caculat_CRC(TxBuffer2, length+1);
		
}

