#include "i2c_gy906.h"
//#include "usart.h"

void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
#define printf(fmt,...)   USART_OUT(USART1,fmt,##__VA_ARGS__)

unsigned char EEPROM_ADDRESS;
unsigned char I2c_Buf_Write[256];
unsigned char I2c_Buf_Read[256];

u8 Debug_value;
//==================================
//函数名：I2C_EE_Init
//描述 ：I2C 外设(EEPROM)初始化
//输入 ：无
//输出 ：无
//调用 ：外部调用
//==================================
void I2C_EE_Init(void)
{

	I2C_GPIO_Config();

	I2C_Mode_Configu();

	//根据头文件 i2c_ee.h 中的定义来选择 EEPROM 要写入的地址
	#ifdef EEPROM_Block0_ADDRESS

	//选择 EEPROM Block0 来写入
	EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
	#endif


	#ifdef EEPROM_Block1_ADDRESS
	//选择 EEPROM Block1 来写入
 	EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
	#endif

	#ifdef EEPROM_Block2_ADDRESS
	//选择 EEPROM Block2 来写入
	EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
	#endif

	#ifdef EEPROM_Block3_ADDRESS
	//选择 EEPROM Block3 来写入
	EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
	#endif 
}

static void DelayMs(int ms)
{
	int i;
	int j = 2200;
	for(i = 0 ;  i < ms ; i++)
	{
		do
		{
			__asm("NOP");
		}while(j--);
		j = 2200; // 72 MHZ
	}
}	
//===============================
//函数名：I2C_EE_Test
//描述 ：I2C(AT24C02)读写测试
//输入 ：无
//输出 ：无
//返回 ：无
//===============================
void I2C_Test(void)
{
	u16 i;
	I2C_EE_Init();
	printf("写入的数据\n\r");
  
	while(1)
	{
	gy906_read_temp(I2c_Buf_Read, 0x07, 3);
	for (i=0; i<3; i++)
	{

		printf("0x%02X ", I2c_Buf_Read[i]);

	}
	DelayMs(300);
	}
	
		for ( i=0; i<=255; i++ ) //填充缓冲
	{
		I2c_Buf_Write[i] = i;

		printf("0x%02X ", I2c_Buf_Write[i]);
		if(i%16 == 15)
			printf("\n\r");
	}
		
	//将 I2c_Buf_Write 中顺序递增的数据写入 EERPOM 中
	I2C_EE_BufferWrite( I2c_Buf_Write, EEP_Firstpage, 256);

	printf("\n\r 数据写入成功\n\r");	

	
	printf("\n\r 读出的数据\n\r");
	//将 EEPROM 读出数据顺序保持到 I2c_Buf_Read 中
	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256);

	//将 I2c_Buf_Read 中的数据通过串口打印
	for (i=0; i<256; i++)
	{
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			printf("0x%02X ", I2c_Buf_Read[i]);
			printf("错误:I2C EEPROM 写入与读出的数据不一致\n\r");
			return;
		}
	printf("0x%02X ", I2c_Buf_Read[i]);
	if(i%16 == 15)
		printf("\n\r");

	}
	printf("I2C(AT24C02)读写测试成功\n\r");
}


//===============================
//函数名：I2C_EE_BufferWrite
//描述 ：将缓冲区中的数据写到 I2C EEPROM 中
//输入 ：-pBuffer 缓冲区指针	-WriteAddr 接收数据的 EEPROM 的地址	-NumByteToWrite 要写入 EEPROM 的字节数
//输出 ：无
//返回 ：无
//调用 ：外部调用
//===============================
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite)
{
	u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

	Addr = WriteAddr % I2C_PageSize;
	count = I2C_PageSize - Addr;
	NumOfPage = NumByteToWrite / I2C_PageSize;
	NumOfSingle = NumByteToWrite % I2C_PageSize;

 	//If WriteAddr is I2C_PageSize aligned 
 	if(Addr == 0)
	{
		//If NumByteToWrite < I2C_PageSize
		if(NumOfPage == 0)
		{
			I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			I2C_EE_WaitEepromStandbyState();
		}
		//If NumByteToWrite > I2C_PageSize
		else
		{
			while(NumOfPage--)
			{
				I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
				I2C_EE_WaitEepromStandbyState();
				WriteAddr += I2C_PageSize;
				pBuffer += I2C_PageSize;
			}

			if(NumOfSingle!=0)
			{
				I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
				I2C_EE_WaitEepromStandbyState();
			}
		}
	}
	//If WriteAddr is not I2C_PageSize aligned
 	else
	{
		//If NumByteToWrite < I2C_PageSize

		if(NumOfPage== 0)
		{
			I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			I2C_EE_WaitEepromStandbyState();
		}
		//If NumByteToWrite > I2C_PageSize
		else
		{
			NumByteToWrite -= count;
			NumOfPage = NumByteToWrite / I2C_PageSize;
			NumOfSingle = NumByteToWrite % I2C_PageSize;

			if(count != 0)
			{
				I2C_EE_PageWrite(pBuffer, WriteAddr, count);
				I2C_EE_WaitEepromStandbyState();
				WriteAddr += count;
				pBuffer += count;
			}

			while(NumOfPage--)
			{
				I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
 				I2C_EE_WaitEepromStandbyState();
				WriteAddr += I2C_PageSize;
				pBuffer += I2C_PageSize;
			}
			if(NumOfSingle != 0)
			{
				I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
				I2C_EE_WaitEepromStandbyState();
 			}
		}
	}
}


//===================================
//函数名：I2C_EE_PageWrite
//描述 ：在 EEPROM 的一个写循环中可以写多个字节，但一次写入的字节数
//不能超过 EEPROM 页的大小。AT24C02 每页有 8 个字节。
//输入 ：-pBuffer 缓冲区指针
//-WriteAddr 接收数据的 EEPROM 的地址	-NumByteToWrite 要写入 EEPROM 的字节数
//输出 ：无
//返回 ：无
//调用 ：外部调用
//===================================
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua

	//Send START condition 

	I2C_GenerateSTART(I2C2, ENABLE);	// 产生 I2Cx 传输 START 条件

	//Test on EV5 and clear it 
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

	//Send EEPROM address for write
	I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	//I2C_Send7bitAddress(I2C2, 0xA6, I2C_Direction_Transmitter);

	//Test on EV6 and clear it
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	//Send the EEPROM's internal address to write to
	I2C_SendData(I2C2, WriteAddr);

	//Test on EV8 and clear it
	while(! I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	//While there is data to be written
	while(NumByteToWrite--)
	{
		//Send the current byte
		I2C_SendData(I2C2, *pBuffer);

		//Point to the next byte to be written
		pBuffer++;

		//Test on EV8 and clear it
		while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	/* Send STOP condition */
	I2C_GenerateSTOP(I2C2, ENABLE);
}


//===========================================
//函数名：I2C_EE_WaitEepromStandbyState
//描述 ：Wait for EEPROM Standby state
//输入 ：无
//输出 ：无
//返回 ：无
//调用 ：
//============================================
void I2C_EE_WaitEepromStandbyState(void)
{
	vu16 SR1_Tmp = 0;

	do
	{
		//Send START condition
		I2C_GenerateSTART(I2C2, ENABLE);	//产生 I2Cx 传输 START 条件

		//Read I2C2 SR1 register
		SR1_Tmp = I2C_ReadRegister(I2C2, I2C_Register_SR1);	//读取指定的 I2C 寄存器并返回其值

		//Send EEPROM address for write
		I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	}
	while(!(I2C_ReadRegister(I2C2, I2C_Register_SR1) & 0x0002));	//地址已被发送（主模式）7位地址模式时，当收到地址的ACK后该位被置位
																// 0: 地址发送没有结束	1: 地址发送结束
	//Clear AF flag
	I2C_ClearFlag(I2C2, I2C_FLAG_AF);								//清除 I2Cx 的待处理标志位

	//STOP condition
	I2C_GenerateSTOP(I2C2, ENABLE); // Added by Najoua 27/08/2008
}


//====================================================
//函数名：I2C_EE_BufferRead
//描述 ：从 EEPROM 里面读取一块数据。
//输入 ：-pBuffer 存放从 EEPROM 读取的数据的缓冲区指针	-WriteAddr 接收数据的 EEPROM 的地址	-NumByteToWrite 要从 EEPROM 读取的字节数。
//输出 ：无
//返回 ：无
//调用 ：外部调用
//===================================================
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{
	//*((u8 *)0x4001080c) |=0x80;
	//Debug_value = I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY);
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua		总线忙标志位


	//Send START condition
	I2C_GenerateSTART(I2C2, ENABLE);	//产生 I2Cx 传输 START 条件
 	//*((u8 *)0x4001080c) &=~0x80;

	//Test on EV5 and clear it
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));		

	//Send EEPROM address for write
	I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);

	//Test on EV6 and clear it
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	//Clear EV6 by setting again the PE bit
	I2C_Cmd(I2C2, ENABLE);	//使能I2C2

	//Send the EEPROM's internal address to write to
	I2C_SendData(I2C2, ReadAddr);

	//Test on EV8 and clear it
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	//Send STRAT condition a second time
	I2C_GenerateSTART(I2C2, ENABLE);

	//Test on EV5 and clear it
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

	//Send EEPROM address for read
	I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Receiver);
	

	//Test on EV6 and clear it
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	//While there is data to be read
	while(NumByteToRead)
	{
		if(NumByteToRead == 1)
		{
			//Disable Acknowledgement
			I2C_AcknowledgeConfig(I2C2, DISABLE);	//使能或者失能指定 I2C 的应答功能

			//Send STOP Condition
			I2C_GenerateSTOP(I2C2, ENABLE);		//产生 I2Cx 传输 STOP 条件
		}


		//Test on EV7 and clear it
		if(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
		{
			//Read a byte from the EEPROM
			*pBuffer = I2C_ReceiveData(I2C2);

			//Point to the next location where the byte read will be saved

			 pBuffer++;

			//Decrement the read bytes counter
			NumByteToRead--;
		}
	}

	//Enable Acknowledgement to be ready for another reception
	I2C_AcknowledgeConfig(I2C2, ENABLE);
}


//====================================================
//函数名：I2C_EE_BufferRead
//描述 ：从 EEPROM 里面读取一块数据。
//输入 ：-pBuffer 存放从 EEPROM 读取的数据的缓冲区指针	-WriteAddr 接收数据的 EEPROM 的地址	-NumByteToWrite 要从 EEPROM 读取的字节数。
//输出 ：无
//返回 ：无
//调用 ：外部调用
//===================================================
void gy906_read_temp(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{
	//*((u8 *)0x4001080c) |=0x80;
	//Debug_value = I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY);
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua		总线忙标志位


	//Send START condition
	I2C_GenerateSTART(I2C2, ENABLE);	//产生 I2Cx 传输 START 条件
 	//*((u8 *)0x4001080c) &=~0x80;

	//Test on EV5 and clear it
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));		

	//Send EEPROM address for write
	I2C_Send7bitAddress(I2C2, 0x00, I2C_Direction_Transmitter);

	//Test on EV6 and clear it
	//while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	//Clear EV6 by setting again the PE bit
	I2C_Cmd(I2C2, ENABLE);	//使能I2C2

	//Send the EEPROM's internal address to write to
	I2C_SendData(I2C2, ReadAddr);

	//Test on EV8 and clear it
	//while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	//Send STRAT condition a second time
	//I2C_GenerateSTART(I2C2, ENABLE);

	//Test on EV5 and clear it
//	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

	//Send EEPROM address for read
//	I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Receiver);
	

	//Test on EV6 and clear it
//	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	//While there is data to be read
	while(NumByteToRead)
	{
		if(NumByteToRead == 1)
		{
			//Disable Acknowledgement
			I2C_AcknowledgeConfig(I2C2, DISABLE);	//使能或者失能指定 I2C 的应答功能

			//Send STOP Condition
			I2C_GenerateSTOP(I2C2, ENABLE);		//产生 I2Cx 传输 STOP 条件
		}


		//Test on EV7 and clear it
		if(1)//I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)
		{
			//Read a byte from the EEPROM
			*pBuffer = I2C_ReceiveData(I2C2);

			//Point to the next location where the byte read will be saved

			 pBuffer++;

			//Decrement the read bytes counter
			NumByteToRead--;
		}
	}

	//Enable Acknowledgement to be ready for another reception
	I2C_AcknowledgeConfig(I2C2, ENABLE);
}

//====================================
//函数名：I2C_GPIO_Config
//描述 ：I2C1 I/O 配置
//输入 ：无
//输出 ：无
//调用 ：内部调用
//===================================
static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// 使能与 I2C2 有关的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);

	// PB6-I2C1_SCL、PB7-I2C1_SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 开漏输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//====================================
//函数名：I2C_Configuration
//描述 ：I2C 工作模式配置
//输入 ：无
//输出 ：无
// 调用 ：内部调用
//====================================
static void I2C_Mode_Configu(void)
{
	I2C_InitTypeDef I2C_InitStructure;

	//I2C 配置
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 =I2C2_OWN_ADDRESS7;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

	//I2C2 初始化 
	I2C_Init(I2C2, &I2C_InitStructure);
	// 使能 I2C2 
	I2C_Cmd(I2C2, ENABLE);
}
