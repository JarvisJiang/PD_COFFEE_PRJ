#include <stm32f10x.h>
#include "AT25512.h"
#include "ws_spi.h"
#include "stm32f10x_spi.h"

void AT25512_SPI1_WriteByte(unsigned char data)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,data);	
}

unsigned char AT25512_SPI1_GetByte(void)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,0x00);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI1);	
		
}

unsigned char AT25512_SPI1_RW(unsigned char data)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,data);

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI1);
	
}

unsigned char AT25512_SPI1_Read_Reg(unsigned char reg)
{
	unsigned char reg_val;
  	AT25512_SPI1_RW(reg);                 
  	reg_val = AT25512_SPI1_RW(0);         
  	return(reg_val);            
}


void AT25512_Wrsr(unsigned char Byte)
{
	AT25512_Enable;
	AT25512_SPI1_WriteByte(WRSR);
	AT25512_SPI1_WriteByte(Byte);
	AT25512_Disable;	
}

unsigned char AT25512_Rdsr()
{
	unsigned char temp;
	AT25512_Enable;
	AT25512_SPI1_WriteByte(RDSR);
	temp = AT25512_SPI1_GetByte();
	AT25512_Disable;	
	return temp;
}

void AT25512_init(void)
{	
	//SPI1 GPIO INIT
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI1_Configuration();

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA,&GPIO_InitStruct);	
	AT25512_Disable;	
	
	// AT25512 INIT
	AT25512_Enable;
	AT25512_SPI1_WriteByte(WRDI);
	AT25512_Disable;	
	AT25512_Enable;
	AT25512_SPI1_WriteByte(WREN);
	AT25512_Disable;	
}

void AT25512_Write(unsigned long address,unsigned char Byte)
{
	
	while(AT25512_Rdsr()&0x01);		
	AT25512_Enable;
	AT25512_SPI1_WriteByte(WRDI);
	AT25512_SPI1_WriteByte(WREN);
	AT25512_Disable;		

	AT25512_Enable;
	AT25512_SPI1_WriteByte(WRITE);
	AT25512_SPI1_WriteByte((unsigned char)(address>>8));
	AT25512_SPI1_WriteByte((unsigned char)address);
	AT25512_SPI1_WriteByte(Byte);
	AT25512_Disable;		
		
}

unsigned char AT25512_Read(unsigned long address)
{
	unsigned char temp;
	
	AT25512_Enable;
	AT25512_SPI1_WriteByte(READ);
	AT25512_SPI1_WriteByte((unsigned char)(address>>8));
	AT25512_SPI1_WriteByte((unsigned char)address);
	temp = AT25512_SPI1_GetByte();
	AT25512_Disable;	
	return temp;
	
}
				
void AT25512_Write_buffer(unsigned long address,unsigned char *pHeader,unsigned int len)
{
	unsigned int i=0; 
	
	while(AT25512_Rdsr()&0x01);		
	AT25512_Enable;
	AT25512_SPI1_WriteByte(WRDI);
	AT25512_SPI1_WriteByte(WREN);
	AT25512_Disable;		

	AT25512_Enable;
	AT25512_SPI1_WriteByte(WRITE);
	AT25512_SPI1_WriteByte((unsigned char)(address>>8));
	AT25512_SPI1_WriteByte((unsigned char)address);
	
	for(i=0;i<len;i++)
	{
		AT25512_SPI1_WriteByte(pHeader[i]);
	}  
	
	AT25512_Disable;		
		
}

void AT25512_Read_buffer(unsigned long address, unsigned char *pHeader,unsigned int len)
{
	unsigned int i;
	
	AT25512_Enable;
	AT25512_SPI1_WriteByte(READ);
	AT25512_SPI1_WriteByte((unsigned char)(address>>8));
	AT25512_SPI1_WriteByte((unsigned char)address);
	
	for(i=0;i<4;i++){SPI1_Send_byte(0x00);}  
		
	for(i=0;i<len;i++){pHeader[i]=AT25512_SPI1_GetByte();}  
	
	AT25512_Disable;	
	
}

