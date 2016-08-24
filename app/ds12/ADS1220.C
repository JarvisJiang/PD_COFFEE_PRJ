
#include "DS_delay.h"
#include "bsp_usart1.h"
#include "stdio.h"
#include "ads1220.h"
u8 results1,results2,results3;

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_ADS1220_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  /*!< SPI_FLASH_SPI_CS_GPIO, SPI_FLASH_SPI_MOSI_GPIO, 
       SPI_FLASH_SPI_MISO_GPIO, SPI_FLASH_SPI_DETECT_GPIO 
       and SPI_FLASH_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);

  /*!< SPI_FLASH_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  /*!< AFIO Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  
  /*!< Configure SPI_FLASH_SPI pins: SCK */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                    			//
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						// �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  /*!< Configure SPI_FLASH_SPI pins: DIN */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;                    //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;							// �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  /*!< Configure SPI_FLASH_SPI pins: DOUT */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;                    			//
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;							//��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */

  /* SPI1 configuration */
  // ��SCLK�½��أ�ϵͳͨ��DIN��1220�������� 
  // ��SCLK�����أ�ϵͳͨ��DOUT ��ȡ1220����
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	  //����ȫ˫��
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							  //����ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						  //8λģʽ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							  //SCLK����ʱλ�͵�ƽ
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;							  //SCLK���½��زɼ�����
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								  //�ӻ�ѡ���źţ�����ģʽ��������GPIOѡ��ӻ�
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	  //APB2����APB1����Ƶ�ʵ�1/2~1/256��Ƶ
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					  //���ݴ����MSBλ��ʼ
  SPI_InitStructure.SPI_CRCPolynomial = 7;								  //CRCֵ����Ķ���ʽ
  SPI_Init(SPI1, &SPI_InitStructure);


  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}


/*******************************************************************
����������ʼ��DAC�ӿ�
����ԭ�ͣ�void ADC_gpio_init()
����˵������
���أ�  ��
********************************************************************/
void ADS1220_GPIO_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/////////CS  reset ��������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_4 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;	//--�������ģʽ
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_10MHz; 	//--GPIO��ת�ٶ�Ϊ50MHz

	GPIO_Init(GPIOA , &GPIO_InitStructure);	//--������д���Ӧ��GPIO�Ĵ�����	


//	GPIO_InitTypeDef GPIO_InitStructure;
    ///////// DRDY��������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU;	//--��������ģʽ
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 	//--GPIO��ת�ٶ�Ϊ50MHz
	GPIO_Init(GPIOA , &GPIO_InitStructure);	//--������д���Ӧ��GPIO�Ĵ�����

}

//-----------------------------------------------------------------//
//	��    �ܣ�ADS1220 д����
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: ��ADS1220�е�ַΪregaddr�ļĴ���д��һ���ֽ�databyte
//-----------------------------------------------------------------//

void ADS1220WREG(unsigned char regaddr,unsigned char databyte)
{
    ADS_CS_LOW();
	while(ADS_DRDY);//��ADS1220_DRDYΪ��ʱ����д�Ĵ���
	//��Ĵ���д�����ݵ�ַ
    SPI_SendByte(ADS1220_WREG_CMD | (regaddr & 0xF));
    //д�����ݵĸ���n-1
    SPI_SendByte(0);
    //��regaddr��ַָ��ļĴ���д������databyte
  	DS_delay_us(5);
    SPI_SendByte(databyte);
	ADS_CS_HIGH();
}

//-----------------------------------------------------------------//
//	��    �ܣ�ADS1220 ���Ĵ�������
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: ��ADS1220�е�ַΪregaddr�ļĴ�������һ���ֽ�databyte
//-----------------------------------------------------------------//

unsigned char ADS1220RREG(unsigned char regaddr)
{
    //��ADS1220�е�ַΪregaddr�ļĴ�������һ���ֽ�
    unsigned char r=0, len =0;
    ADS_CS_LOW();
//	DS_delay_us(10);
	while(ADS_DRDY);//��ADS1220_DRDYΪ��ʱ����д�Ĵ���
	//д���ַ
	
    SPI_SendByte(ADS1220_RREG_CMD | ((regaddr<<2|(len&0x0)) & 0xF));
    //д���ȡ���ݵĸ���n-1
   // SPI_SendByte(0);
	  DS_delay_us(10);  //min=50*(1/fCLKIN)=50*(1/7.68MHZ)=6500ns;max=whatever
    //����regaddr��ַָ��ļĴ���������
    r=SPI_SendByte(0);
	  ADS_CS_HIGH();

    return r;//��������
}

//-----------------------------------------------------------------//
//	��    �ܣ�ADS1220��ʼ���ӳ���
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: /
//-----------------------------------------------------------------//
void ADS1220_Init(void)
{ 

	unsigned char config[4];
	//config[0] = ADS1220_MUX_AIN0_AIN1 + ADS1220_GAIN_16 + ADS1220_USE_PGA;
	config[0] = ADS1220_MUX_AIN1_AIN0 + ADS1220_GAIN_8 + ADS1220_USE_PGA;
	config[1] = ADS1220_DATA_RATE_20SPS + ADS1220_OP_MODE_NORMAL + ADS1220_CONVERSION_CONTINUOUS + ADS1220_TEMP_SENSOR_OFF + ADS1220_BURN_OUT_CURRENT_OFF;

	config[2] = ADS1220_FIR_50_60 + ADS1220_VREF_EXT_REF0_PINS + ADS1220_LOW_SIDE_POWER_OPEN + ADS1220_IDAC_CURRENT_500_UA;
	config[3] = ADS1220_IDAC1_AIN2 + ADS1220_IDAC2_AIN3 + ADS1220_DRDY_ON_DRDY_ONLY;
	printf("config[0] = %x\r\n",config[0]);
	printf("config[1] = %x\r\n",config[1]);
	printf("config[2] = %x\r\n",config[2]);
	printf("config[3] = %x\r\n",config[3]);
	ADS_CS_LOW();
	RESET_HIGH();
	DS_delay_us(1000);

	SPI_SendByte(ADS1220_RESET_CMD);
	DS_delay_ms(10);
	while(ADS_DRDY);                                //��ADS1220_DRDYΪ��ʱ����д�Ĵ���
	SPI_SendByte(ADS1220_START_CMD);                 //ͬ������
	SPI_SendByte(ADS1220_START_CMD);               //ͬ������
	
	while(ADS_DRDY);
	SPI_SendByte(ADS1220_WREG_CMD | 3);//����д��4���Ĵ���
	SPI_SendByte(config[0]);
	SPI_SendByte(config[1]);                             //��λ��ǰ��ʹ���ڲ�У׼����ʹ�û���
	SPI_SendByte(config[2]);//�˿�����A2Ϊ����A3λ��
	SPI_SendByte(config[3]);                   //�Ŵ�������
	//SPI_SendByte(ADS1220_DRATE_2_5SPS);             //�ɼ��ٶ�����
	DS_delay_us(100);
	
	while(ADS_DRDY);                                //��ADS1220_DRDYΪ��ʱ����д�Ĵ���
//	SPI_SendByte(ADS1220_CMD_SELFCAL);              //ƫ�ƺ������Զ�У׼
	
	
    ADS_CS_HIGH();
	DS_delay_ms(100);
	
	ADS1220RREG(0);
	printf("read config[0] = %x\r\n",ADS1220RREG(0));
	printf("read config[0] = %x\r\n",ADS1220RREG(1));
	printf("read config[0] = %x\r\n",ADS1220RREG(2));
	printf("read config[0] = %x\r\n",ADS1220RREG(3));
 
}


//-----------------------------------------------------------------//
//	��    �ܣ�
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע:
//-----------------------------------------------------------------//

unsigned long ADS1220ReadData(void)  
{
    unsigned char i=0;
    unsigned long sum=0;
		unsigned long r=0;
		ADS_CS_LOW();

  	SPI_SendByte(ADS1220_START_CMD);
		SPI_SendByte(ADS1220_START_CMD);
		SPI_SendByte(ADS1220_RDATA_CMD);

		while(ADS_DRDY);               //��ADS1220_DRDYΪ��ʱ����д�Ĵ��� 	
		DS_delay_us(1000);                  //min=50*(1/fCLKIN)=50*(1/7.68MHZ)=6500ns;max=whatever
			for(i=0;i<3;i++)
			{
				sum = sum << 8;
			r = SPI_SendByte(0);  
				sum |= r;
			}
		//	SPI_SendByte(ADS1220_POWERDOWN_CMD);
		ADS_CS_HIGH();		 
    return sum;
}


#define DS1220_REFER_RESISTOR   1651
#define DS1220_ADC1_FCURRENT				(0.0005) 
#define DS1220_USER_PGA				(8) 
#define DS1220_REFER_VOLTAGE    (DS1220_REFER_RESISTOR*((float)(DS1220_ADC1_FCURRENT*2)))   //  500UA *2 

#define DS1220_FSR              ((float)(DS1220_REFER_VOLTAGE/DS1220_USER_PGA))
	
#define DS1220_MAX_RESISTOR    	((float)(DS1220_FSR/DS1220_ADC1_FCURRENT))
float DS1220ResConvertToTem(short code)
{
	float rtnTemp;
	float rate;
	float resi;
	float voltage;
	if(code<0)
	{
		printf("code < 0 \r\n");
		return 0;
	}
	rate 		= (float)code/32768;	
	voltage = rate*DS1220_FSR;
	resi    = voltage/DS1220_ADC1_FCURRENT;
	#if 0
	printf("MAX resistor: %f\r\n", DS1220_MAX_RESISTOR);
	printf("ds1220 rate: = %f\r\n",rate);
	printf("ds1220 voltage: = %f\r\n",voltage);
	printf("ds1220 resi: = %f\r\n",resi);
	#endif
	rtnTemp = CalculateTemperatureAlg(resi);
	printf("ds1220 rtnTemp: = %f\r\n",rtnTemp);
	return rtnTemp;	
}




