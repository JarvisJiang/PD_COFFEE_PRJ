#include "i2c_gy906.h"
//#include "usart.h"

void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
#define printf(fmt,...)   USART_OUT(USART1,fmt,##__VA_ARGS__)

unsigned char EEPROM_ADDRESS;
unsigned char I2c_Buf_Write[256];
unsigned char I2c_Buf_Read[256];

u8 Debug_value;
//==================================
//��������I2C_EE_Init
//���� ��I2C ����(EEPROM)��ʼ��
//���� ����
//��� ����
//���� ���ⲿ����
//==================================
void I2C_EE_Init(void)
{

	I2C_GPIO_Config();

	I2C_Mode_Configu();

	//����ͷ�ļ� i2c_ee.h �еĶ�����ѡ�� EEPROM Ҫд��ĵ�ַ
	#ifdef EEPROM_Block0_ADDRESS

	//ѡ�� EEPROM Block0 ��д��
	EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
	#endif


	#ifdef EEPROM_Block1_ADDRESS
	//ѡ�� EEPROM Block1 ��д��
 	EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
	#endif

	#ifdef EEPROM_Block2_ADDRESS
	//ѡ�� EEPROM Block2 ��д��
	EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
	#endif

	#ifdef EEPROM_Block3_ADDRESS
	//ѡ�� EEPROM Block3 ��д��
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
//��������I2C_EE_Test
//���� ��I2C(AT24C02)��д����
//���� ����
//��� ����
//���� ����
//===============================
void I2C_Test(void)
{
	u16 i;
	I2C_EE_Init();
	printf("д�������\n\r");
  
	while(1)
	{
	gy906_read_temp(I2c_Buf_Read, 0x07, 3);
	for (i=0; i<3; i++)
	{

		printf("0x%02X ", I2c_Buf_Read[i]);

	}
	DelayMs(300);
	}
	
		for ( i=0; i<=255; i++ ) //��仺��
	{
		I2c_Buf_Write[i] = i;

		printf("0x%02X ", I2c_Buf_Write[i]);
		if(i%16 == 15)
			printf("\n\r");
	}
		
	//�� I2c_Buf_Write ��˳�����������д�� EERPOM ��
	I2C_EE_BufferWrite( I2c_Buf_Write, EEP_Firstpage, 256);

	printf("\n\r ����д��ɹ�\n\r");	

	
	printf("\n\r ����������\n\r");
	//�� EEPROM ��������˳�򱣳ֵ� I2c_Buf_Read ��
	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256);

	//�� I2c_Buf_Read �е�����ͨ�����ڴ�ӡ
	for (i=0; i<256; i++)
	{
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			printf("0x%02X ", I2c_Buf_Read[i]);
			printf("����:I2C EEPROM д������������ݲ�һ��\n\r");
			return;
		}
	printf("0x%02X ", I2c_Buf_Read[i]);
	if(i%16 == 15)
		printf("\n\r");

	}
	printf("I2C(AT24C02)��д���Գɹ�\n\r");
}


//===============================
//��������I2C_EE_BufferWrite
//���� �����������е�����д�� I2C EEPROM ��
//���� ��-pBuffer ������ָ��	-WriteAddr �������ݵ� EEPROM �ĵ�ַ	-NumByteToWrite Ҫд�� EEPROM ���ֽ���
//��� ����
//���� ����
//���� ���ⲿ����
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
//��������I2C_EE_PageWrite
//���� ���� EEPROM ��һ��дѭ���п���д����ֽڣ���һ��д����ֽ���
//���ܳ��� EEPROM ҳ�Ĵ�С��AT24C02 ÿҳ�� 8 ���ֽڡ�
//���� ��-pBuffer ������ָ��
//-WriteAddr �������ݵ� EEPROM �ĵ�ַ	-NumByteToWrite Ҫд�� EEPROM ���ֽ���
//��� ����
//���� ����
//���� ���ⲿ����
//===================================
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua

	//Send START condition 

	I2C_GenerateSTART(I2C2, ENABLE);	// ���� I2Cx ���� START ����

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
//��������I2C_EE_WaitEepromStandbyState
//���� ��Wait for EEPROM Standby state
//���� ����
//��� ����
//���� ����
//���� ��
//============================================
void I2C_EE_WaitEepromStandbyState(void)
{
	vu16 SR1_Tmp = 0;

	do
	{
		//Send START condition
		I2C_GenerateSTART(I2C2, ENABLE);	//���� I2Cx ���� START ����

		//Read I2C2 SR1 register
		SR1_Tmp = I2C_ReadRegister(I2C2, I2C_Register_SR1);	//��ȡָ���� I2C �Ĵ�����������ֵ

		//Send EEPROM address for write
		I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	}
	while(!(I2C_ReadRegister(I2C2, I2C_Register_SR1) & 0x0002));	//��ַ�ѱ����ͣ���ģʽ��7λ��ַģʽʱ�����յ���ַ��ACK���λ����λ
																// 0: ��ַ����û�н���	1: ��ַ���ͽ���
	//Clear AF flag
	I2C_ClearFlag(I2C2, I2C_FLAG_AF);								//��� I2Cx �Ĵ�������־λ

	//STOP condition
	I2C_GenerateSTOP(I2C2, ENABLE); // Added by Najoua 27/08/2008
}


//====================================================
//��������I2C_EE_BufferRead
//���� ���� EEPROM �����ȡһ�����ݡ�
//���� ��-pBuffer ��Ŵ� EEPROM ��ȡ�����ݵĻ�����ָ��	-WriteAddr �������ݵ� EEPROM �ĵ�ַ	-NumByteToWrite Ҫ�� EEPROM ��ȡ���ֽ�����
//��� ����
//���� ����
//���� ���ⲿ����
//===================================================
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{
	//*((u8 *)0x4001080c) |=0x80;
	//Debug_value = I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY);
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua		����æ��־λ


	//Send START condition
	I2C_GenerateSTART(I2C2, ENABLE);	//���� I2Cx ���� START ����
 	//*((u8 *)0x4001080c) &=~0x80;

	//Test on EV5 and clear it
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));		

	//Send EEPROM address for write
	I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);

	//Test on EV6 and clear it
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	//Clear EV6 by setting again the PE bit
	I2C_Cmd(I2C2, ENABLE);	//ʹ��I2C2

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
			I2C_AcknowledgeConfig(I2C2, DISABLE);	//ʹ�ܻ���ʧ��ָ�� I2C ��Ӧ����

			//Send STOP Condition
			I2C_GenerateSTOP(I2C2, ENABLE);		//���� I2Cx ���� STOP ����
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
//��������I2C_EE_BufferRead
//���� ���� EEPROM �����ȡһ�����ݡ�
//���� ��-pBuffer ��Ŵ� EEPROM ��ȡ�����ݵĻ�����ָ��	-WriteAddr �������ݵ� EEPROM �ĵ�ַ	-NumByteToWrite Ҫ�� EEPROM ��ȡ���ֽ�����
//��� ����
//���� ����
//���� ���ⲿ����
//===================================================
void gy906_read_temp(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{
	//*((u8 *)0x4001080c) |=0x80;
	//Debug_value = I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY);
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua		����æ��־λ


	//Send START condition
	I2C_GenerateSTART(I2C2, ENABLE);	//���� I2Cx ���� START ����
 	//*((u8 *)0x4001080c) &=~0x80;

	//Test on EV5 and clear it
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));		

	//Send EEPROM address for write
	I2C_Send7bitAddress(I2C2, 0x00, I2C_Direction_Transmitter);

	//Test on EV6 and clear it
	//while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	//Clear EV6 by setting again the PE bit
	I2C_Cmd(I2C2, ENABLE);	//ʹ��I2C2

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
			I2C_AcknowledgeConfig(I2C2, DISABLE);	//ʹ�ܻ���ʧ��ָ�� I2C ��Ӧ����

			//Send STOP Condition
			I2C_GenerateSTOP(I2C2, ENABLE);		//���� I2Cx ���� STOP ����
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
//��������I2C_GPIO_Config
//���� ��I2C1 I/O ����
//���� ����
//��� ����
//���� ���ڲ�����
//===================================
static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// ʹ���� I2C2 �йص�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);

	// PB6-I2C1_SCL��PB7-I2C1_SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // ��©���
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//====================================
//��������I2C_Configuration
//���� ��I2C ����ģʽ����
//���� ����
//��� ����
// ���� ���ڲ�����
//====================================
static void I2C_Mode_Configu(void)
{
	I2C_InitTypeDef I2C_InitStructure;

	//I2C ����
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 =I2C2_OWN_ADDRESS7;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

	//I2C2 ��ʼ�� 
	I2C_Init(I2C2, &I2C_InitStructure);
	// ʹ�� I2C2 
	I2C_Cmd(I2C2, ENABLE);
}