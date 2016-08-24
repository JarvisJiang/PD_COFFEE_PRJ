
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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						// 推免输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  /*!< Configure SPI_FLASH_SPI pins: DIN */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;                    //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;							// 复用推免输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  /*!< Configure SPI_FLASH_SPI pins: DOUT */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;                    			//
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;							//上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */

  /* SPI1 configuration */
  // 在SCLK下降沿，系统通过DIN向1220发送数据 
  // 在SCLK上升沿，系统通过DOUT 读取1220数据
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	  //两线全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							  //主机模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						  //8位模式
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							  //SCLK空闲时位低电平
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;							  //SCLK的下降沿采集数据
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								  //从机选择信号，软件模式，就是用GPIO选择从机
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	  //APB2或者APB1总线频率的1/2~1/256分频
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					  //数据传输从MSB位开始
  SPI_InitStructure.SPI_CRCPolynomial = 7;								  //CRC值计算的多项式
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
函数名：初始化DAC接口
函数原型：void ADC_gpio_init()
参数说明：无
返回：  无
********************************************************************/
void ADS1220_GPIO_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/////////CS  reset 引脚设置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_4 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;	//--推免输出模式
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_10MHz; 	//--GPIO翻转速度为50MHz

	GPIO_Init(GPIOA , &GPIO_InitStructure);	//--将配置写入对应的GPIO寄存器中	


//	GPIO_InitTypeDef GPIO_InitStructure;
    ///////// DRDY引脚设置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU;	//--浮点输入模式
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 	//--GPIO翻转速度为50MHz
	GPIO_Init(GPIOA , &GPIO_InitStructure);	//--将配置写入对应的GPIO寄存器中

}

//-----------------------------------------------------------------//
//	功    能：ADS1220 写数据
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: 向ADS1220中地址为regaddr的寄存器写入一个字节databyte
//-----------------------------------------------------------------//

void ADS1220WREG(unsigned char regaddr,unsigned char databyte)
{
    ADS_CS_LOW();
	while(ADS_DRDY);//当ADS1220_DRDY为低时才能写寄存器
	//向寄存器写入数据地址
    SPI_SendByte(ADS1220_WREG_CMD | (regaddr & 0xF));
    //写入数据的个数n-1
    SPI_SendByte(0);
    //向regaddr地址指向的寄存器写入数据databyte
  	DS_delay_us(5);
    SPI_SendByte(databyte);
	ADS_CS_HIGH();
}

//-----------------------------------------------------------------//
//	功    能：ADS1220 读寄存器数据
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: 从ADS1220中地址为regaddr的寄存器读出一个字节databyte
//-----------------------------------------------------------------//

unsigned char ADS1220RREG(unsigned char regaddr)
{
    //从ADS1220中地址为regaddr的寄存器读出一个字节
    unsigned char r=0, len =0;
    ADS_CS_LOW();
//	DS_delay_us(10);
	while(ADS_DRDY);//当ADS1220_DRDY为低时才能写寄存器
	//写入地址
	
    SPI_SendByte(ADS1220_RREG_CMD | ((regaddr<<2|(len&0x0)) & 0xF));
    //写入读取数据的个数n-1
   // SPI_SendByte(0);
	  DS_delay_us(10);  //min=50*(1/fCLKIN)=50*(1/7.68MHZ)=6500ns;max=whatever
    //读出regaddr地址指向的寄存器的数据
    r=SPI_SendByte(0);
	  ADS_CS_HIGH();

    return r;//返回数据
}

//-----------------------------------------------------------------//
//	功    能：ADS1220初始化子程序
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: /
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
	while(ADS_DRDY);                                //当ADS1220_DRDY为低时才能写寄存器
	SPI_SendByte(ADS1220_START_CMD);                 //同步命令
	SPI_SendByte(ADS1220_START_CMD);               //同步唤醒
	
	while(ADS_DRDY);
	SPI_SendByte(ADS1220_WREG_CMD | 3);//连续写入4个寄存器
	SPI_SendByte(config[0]);
	SPI_SendByte(config[1]);                             //高位在前，使用内部校准，不使用缓存
	SPI_SendByte(config[2]);//端口输入A2为正，A3位负
	SPI_SendByte(config[3]);                   //放大倍数设置
	//SPI_SendByte(ADS1220_DRATE_2_5SPS);             //采集速度设置
	DS_delay_us(100);
	
	while(ADS_DRDY);                                //当ADS1220_DRDY为低时才能写寄存器
//	SPI_SendByte(ADS1220_CMD_SELFCAL);              //偏移和增益自动校准
	
	
    ADS_CS_HIGH();
	DS_delay_ms(100);
	
	ADS1220RREG(0);
	printf("read config[0] = %x\r\n",ADS1220RREG(0));
	printf("read config[0] = %x\r\n",ADS1220RREG(1));
	printf("read config[0] = %x\r\n",ADS1220RREG(2));
	printf("read config[0] = %x\r\n",ADS1220RREG(3));
 
}


//-----------------------------------------------------------------//
//	功    能：
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注:
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

		while(ADS_DRDY);               //当ADS1220_DRDY为低时才能写寄存器 	
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





