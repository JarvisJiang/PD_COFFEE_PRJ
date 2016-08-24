/*********************************************************************************************************
*
* File                : ws_AT45DBXX.c
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
#include "ws_AT45DBXX.h"

uint16_t Get_Add_Page(uint16_t ID_number)
{
	uint16_t Page;
		Page = 2 + ID_number/11;
	return (Page);
}



uint16_t Get_Add_Offser(uint16_t ID_number)
{
	uint16_t Offset;
		Offset = (ID_number % 11)* 24;
	return (Offset);
}

void AT45DBXX_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI1_Configuration();

	/*RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);*/

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	AT45DBXX_Disable;
}

static void AT45DBXX_BUSY(void)
{
	u8 AT45DBXXStruct;
	AT45DBXX_Enable;
	SPI1_Send_byte(READ_STATE_REGISTER);
	do
	{AT45DBXXStruct = SPI1_Receive_byte();}
	while(!(AT45DBXXStruct & 0x80));
	AT45DBXX_Disable;	
}

void AT45DBXX_Read_ID(u8 *IData)
{
	u8 i;
	AT45DBXX_BUSY();
	AT45DBXX_Enable;
	SPI1_Send_byte(Read_ID); //÷¥––∂¡»°id√¸¡Ó		  	
  	for(i=0;i<4;i++)
  	{
  		IData[i] = SPI1_Receive_byte();
  	}
  	AT45DBXX_Disable;
}

void AT45DBXX_write_buffer(u16 BufferOffset,u8 Data)
{			
	AT45DBXX_Enable;
	SPI1_Send_byte(0x84);			
	SPI1_Send_byte(0xff);
	SPI1_Send_byte((u8)BufferOffset>>8);
	SPI1_Send_byte((u8)BufferOffset);		
	SPI1_Send_byte(Data);
	AT45DBXX_Disable;
}

u8 AT45DBXX_read_buffer(u16 BufferOffset)
{		
	u8 temp;
	AT45DBXX_Enable;
 	SPI1_Send_byte(0xd4);
	SPI1_Send_byte(0xff);
	SPI1_Send_byte((u8)BufferOffset>>8);
	SPI1_Send_byte((u8)BufferOffset);
	temp=SPI1_Receive_byte();
	AT45DBXX_Disable;
	return temp;		
}


/******************************************************************************/  
/*??:                                                                      */  
/*    When the last bit in the main memory array has been read,the device will*/  
/*    continue reading back at the beginning of the first page of memory.As w-*/  
/*    ith crossing over page boundaries,no delays will be incurred when wrapp-*/  
/*    ing around from the end of the array to the beginning of the array.     */  
/*??:                                                                      */  
/*    PA      - page number 0~2047                                                */  
/*    BFA     - BUFFER start address                                   */  
/*    pHeader - Array pointer                                              */  
/*    len     - Read length                                                */  
/******************************************************************************/  
void AT45DB041B_ContinuousArrayRead(u16 PA,u16 BFA,unsigned char *pHeader,u16 len){         
  unsigned int i;        
      
	AT45DBXX_BUSY();
	AT45DBXX_Enable;
	
	SPI1_Send_byte(0xe8);      
	SPI1_Send_byte((unsigned char)(PA>>7));      
	SPI1_Send_byte((unsigned char)((PA<<1)|(BFA>>8)));  
	SPI1_Send_byte((unsigned char)BFA);  
	for(i=0;i<4;i++){SPI1_Send_byte(0x00);}  
		
	for(i=0;i<len;i++){pHeader[i]=SPI1_Receive_byte();}  
		
	AT45DBXX_Disable;
}


/******************************************************************************/  
/*??:                                                                      */  
/*    page number(0~263)???BUFFER                                          */  
/*??:                                                                      */  
/*    buffer  - 0x01  - BUFFER 1, 0x02  - BUFFER 2                           */  
/*              buffer1 0x84 BUFFER2 0x87                               */  
/*    BFA     - BUFFER start number,0~263                                     */  
/*    pHeader - data pointer                                              */  
/*    len     - write data lengh 1~264                                           */  
/******************************************************************************/  
void AT45DB041B_BufferWrite(u8 buffer,u16 BFA, u8 *pHeader,u16 len){  
		unsigned int i;  
				
		AT45DBXX_BUSY();
		AT45DBXX_Enable; 
			
		switch(buffer){  
				case 1:SPI1_Send_byte(0x84);break;  
				case 2:SPI1_Send_byte(0x87);break;  
		}  
		SPI1_Send_byte(0x00);  
		SPI1_Send_byte((unsigned char)(BFA>>8));  
		SPI1_Send_byte((unsigned char)BFA);  
			
		for(i=0;i<len;i++){SPI1_Send_byte(pHeader[i]);}  
			
		AT45DBXX_Disable;       
}

/******************************************************************************/  
/*??:                                                                      */  
/*    write(0~263) to SPI nand flash */  
/*    write BUFFER 1 or BUFFER 2        */  
/*??:                                                                      */  
/*    buffer  - 0x01 to BUFFER 1,  0x02 to BUFFER 2                  */  
/*    PA      - page number,0~2047                                                */  
/*    BFA     - start BUFFER                                     */  
/*    pHeader - write buffer                                              */  
/*    len     - write length                                                */  
/******************************************************************************/  
void AT45DB041B_BufferToMainMemoryPageProgramWithBuilt_inErase(u8 buffer,u16 PA,u16 BFA,u8 *pHeader,u16 len){  

		AT45DB041B_BufferWrite(buffer,BFA,pHeader,len);  

		AT45DBXX_BUSY();
		AT45DBXX_Enable;  

		switch(buffer){  
			case 1:SPI1_Send_byte(0x83);break;  
			case 2:SPI1_Send_byte(0x86);break;  
		}  
		SPI1_Send_byte((unsigned char)(PA>>7));  
		SPI1_Send_byte((unsigned char)(PA<<1));  
		SPI1_Send_byte(0x00); 

		AT45DBXX_Disable;  
} 



/******************************************************************************/  
/*??:                                                                      */  
/*    without Erease function                                   */  
/******************************************************************************/  
void AT45DB041B_BufferToMainMemoryPageProgramWithoutBuilt_inErase(u8 buffer,u16 PA,u16 BFA,u8 *pHeader,u16 len){  
		unsigned int i=0;  

		AT45DB041B_BufferWrite(buffer,BFA,pHeader,len); 

		AT45DBXX_BUSY();
		AT45DBXX_Enable;  

		SPI1_Send_byte(0x88+buffer);  
		SPI1_Send_byte((unsigned char)(PA>>7));  
		SPI1_Send_byte((unsigned char)(PA<<1));  
		SPI1_Send_byte(0x00);  
			
		for(i=0;i<len;i++){SPI1_Send_byte(pHeader[i]);}  

		AT45DBXX_Disable; 
}  

