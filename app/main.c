/****************************************************************************

  Defined UART pins
	TXD1----- PA9-US1-TX
	RXD1----- PA10-US1-RX
	BoardRate 115200,n,8,1 
	
 	TXD2----- PA2-US2-TX
	RXD2----- PA3-US2-RX
	BordRate 15200,n,8,1 	
	

* v0.10    2014-6-21 sun68  Created this file for stadio lighting
*
*/
/* Includes ------------------------------------------------------------------*/



#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "test.h"
#include "bsp_usart1.h"
//#include "bsp_gpio.h"  
#include "bsp_led.h"   
#include "cc2541.h"

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
#include "at25512.h"
#include "a4988_driver.h"

#include "ads1220.h"
#include "DS_delay.h"


#include "uart_slave.h"
#include "drv8810a.h"
#include "water_heater.h"

#include "l9110_dc.h"

#include "gy906.h"


/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;
uint8_t TxBuffer1[32] = "This is USART1 Test";  
uint8_t RxBuffer1[32],rec_f1,tx_flag1;
uint8_t Rx1_Buff1[32];
//uint8_t Buff11_flag, Buff12_flag, Buff13_flag;
__IO uint8_t TxCounter1 = 0x00;
__IO uint8_t RxCounter1 = 0x00; 
uint32_t Rec_Len1;
uint8_t TxBuffer2[32] = "This is USART2 Test";  
uint8_t RxBuffer2[32],rec_f2,tx_flag2;
uint8_t Rx2_Buff1[32];
//uint8_t Buff21_flag, Buff22_flag, Buff23_flag;
__IO uint8_t TxCounter2 = 0x00;
__IO uint8_t RxCounter2 = 0x00; 
uint32_t Rec_Len2;

__IO uint16_t Rx2_Current_pointer = 0x00;
__IO uint16_t Rx2_Tail_pointer = 0x00; 

uint8_t i_forword_priod = 0;
uint8_t i_invered_priod = 0;
uint8_t ivalue = 0;
uint8_t klength = 0;
uint8_t vLength, Rx2_lenght;

uint8_t g_Send_wifi_flag;
uint8_t g_Send_ble_flag;
uint8_t g_exti_flag;
uint8_t g_tp_motor_counter;
unsigned int g_Timer2Counter = 0;
unsigned int g_Timer4Counter = 0;

__IO uint8_t g_keyscan_flag = 0;

//struct Receive_data
//{
//	unsigned int num;
//	unsigned char name[20];

//};

__IO struct Receive_data RX2_rec_data[500];


unsigned char Tx_rfm69_Buffer[32] = {
	0x14, 0x55, 0x11, 0x04, 0x01, 0x00, 0x01, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xa0
};  

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void UART1_NVIC_Configuration(void);
void UART2_NVIC_Configuration(void);
void UART4_NVIC_Configuration(void);
void NVIC_TIM2_Config(void);  
void Delay(__IO uint32_t nCount);
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
char *itoa(int value, char *string, int radix);
void USART1_Config(USART_TypeDef* USARTx);
void USART2_Config(USART_TypeDef* USARTx);
void USART4_Config(USART_TypeDef* USARTx);
void Timer_2_Config(void);
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
void GPIO_Button_Configuration(void);
void test_nfc(void);
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStruct;
USART_ClockInitTypeDef USART_ClockInitStruct;


const unsigned char Company_Name[] = "PerceptionDigital Coffee Machine \r\n";

typedef struct
{
	uint16_t  Device_ID;
	uint16_t  Group_ID;	
	uint8_t   MAC_Addres[6];
	uint8_t   Brightness[3];
	uint8_t   Status;	
} Device_Structure;


typedef struct
{
	uint16_t  Device_ID;
	uint16_t  Group_ID;	
	uint8_t   Brightness[3];
	
} Control_Structure;

unsigned char Database_info[32];
unsigned char rfm69_control[32];
unsigned char m_crc;



/****************************************************************************
* 名    称：void USART_Config(USART_TypeDef* USARTx)
* 功    能：奋斗板串口初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART1_Config(USART_TypeDef* USARTx){
  USART_InitStructure.USART_BaudRate = BT_BAUDRATE_UART115200;    //9600						//速率115200bps
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
  USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
  /* Configure USART1 */
  USART_Init(USARTx, &USART_InitStructure);							//配置串口参数函数   
  /* Enable USART1 Receive and Transmit interrupts */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                    //使能接收中断
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);						//使能发送缓冲空中断   
  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);	
}


/****************************************************************************
* 名    称：void USART2_Config(USART_TypeDef* USARTx)
* 功    能：奋斗板串口初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART2_Config(USART_TypeDef* USARTx){
  USART_InitStructure.USART_BaudRate = BT_BAUDRATE_UART115200;   // 115200;						//速率115200bps
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
  USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
  /* Configure USART2 */
  USART_Init(USARTx, &USART_InitStructure);							//配置串口参数函数  
  /* Enable USART2 Receive and Transmit interrupts */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                    //使能接收中断
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);						//使能发送缓冲空中断   
  /* Enable the USART2 */
  USART_Cmd(USART2, ENABLE);	
}


/****************************************************************************
* 名    称：void USART4_Config(USART_TypeDef* USARTx)
* 功    能：奋斗板串口初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART4_Config(USART_TypeDef* USARTx){
  USART_InitStructure.USART_BaudRate = BT_BAUDRATE_UART115200;   // 115200;						//速率115200bps
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
  USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
  /* Configure USART4 */
  USART_Init(USARTx, &USART_InitStructure);							//配置串口参数函数  
	USART_ClearFlag(UART4, USART_IT_RXNE);     // CLEAR FLAG
  /* Enable USART4 Receive and Transmit interrupts */
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);                    //使能接收中断
  USART_ITConfig(UART4, USART_IT_TXE, ENABLE);						//使能发送缓冲空中断   
  /* Enable the USART4 */
  USART_Cmd(UART4, ENABLE);	
}


uint8_t ScanKey(void)
{
		if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0) ||
			(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) == 0))
			{			

				 Delay_Ms(20);
				
				 if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)  
				 {
						return  1;
				 }
				 else if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) == 0)  
				 {		 
						return  2;
				 }
				 else
				 {		 
						return 0;
				 }
			}
			else
			{
					return 0;
			}
}


void NVIC_TIM2_Config(void)                                  
{		
  NVIC_InitTypeDef NVIC_InitStructure;                         															
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);             	 
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;              
 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;     
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;           
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;               
	NVIC_Init(&NVIC_InitStructure);                              
}


void Timer_2_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;				 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);	         
	TIM_DeInit(TIM2);                                          
	TIM_TimeBaseStructure.TIM_Period=215;						 // 215 - 6 ms,  107 - 3ms
	TIM_TimeBaseStructure.TIM_Prescaler=1999;					 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		  
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	  
 									 
//  T=(TIM_Period+1)*(TIM_Prescaler+1)/TIMxCLK=(35999+1)*(1999+1)/72MHz=1s
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure); 		 		  
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);  				  		  
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                      
	TIM_Cmd(TIM2,ENABLE); 
		
}

void Timer_2_Motor_Config(uint16_t period)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;				 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);	         
	TIM_DeInit(TIM2);                                          
	TIM_TimeBaseStructure.TIM_Period=period;						 //
	TIM_TimeBaseStructure.TIM_Prescaler=999;					 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		  
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	  
 									 
//  T=(TIM_Period+1)*(TIM_Prescaler+1)/TIMxCLK=(35999+1)*(1999+1)/72MHz=1s
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure); 		 		  
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);  				  		  
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                      
	TIM_Cmd(TIM2,ENABLE); 
	
}

void NVIC_TIM4_Config(void)                                  
{		
  NVIC_InitTypeDef NVIC_InitStructure;                         															
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);             	 
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;              
 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;     
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;           
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;               
	NVIC_Init(&NVIC_InitStructure);                              
}


void Timer_4_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;				 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	         
	TIM_DeInit(TIM4);                                          
	TIM_TimeBaseStructure.TIM_Period=35;		// 215 - 6 ms,  107 - 3ms  35 - 1ms  17 - 0.5ms
	TIM_TimeBaseStructure.TIM_Prescaler=1999;					 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		  
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	  
 									 
//  T=(TIM_Period+1)*(TIM_Prescaler+1)/TIMxCLK=(35999+1)*(1999+1)/72MHz=1s
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure); 		 		  
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);  				  		  
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);                      
	TIM_Cmd(TIM4,ENABLE); 	
	
}


/****************************************************************************
* 名    称：int main(void)
* 功    能：
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/

int main(void)
{
		/* 初始化 */
//	int timeCount;
//	int byteCount;
//	char *buf;
	long ads1200Data, i;
//	timeCount = 5; 
  USARTx_Config ();                                                              //初始化串口1
//	SysTick_Init ();                                                               //配置 SysTick 为 1ms 中断一次 
//	ESP8266_Init ();      //初始化WiFi模块使用的接口和外设
//   CC2541_Init();      //初始化蓝牙                                                      
//  LED_Init ();
	printf ( "\r\n WF-ESP8266 WiFi模块测试例程\r\n" );                          //打印测试例程提示信息
	
	//test_uart_slave();
	test_nfc();
	
//	test_pid_version2();
	
	
    Gy906_Init();
  	Test_GY906();
	water_heater_test_pid();
	drv8801_test();
	l9910_dc_test();
	test_heat_up();
	
	test_uart_slave();
	
	
		DS_delay_init(72);	                    //???????
		SPI_ADS1220_Init();								  //SPI???
		ADS1220_GPIO_init();							  //?????
		ADS1220_Init();
	
	//
	//ESP8266_AT_Test();
	printf("ads1220 test !\r\n");
	i = 1;
	while(1)
	{
		ads1200Data = ADS1220ReadData();
		printf("orig: hex[%d]:%x\r\n", i,ads1200Data);
		ads1200Data = ads1200Data>>8;
		printf("deci: data[%d]: %d\r\n", i,(short)ads1200Data);
		DS1220ResConvertToTem(ads1200Data);//
		i++;
	//	DS_delay_ms(500);
	}
	
#if 0
	while(1)
	{
		ESP8266_Rst();
		//printf("string: %s\r\n",  ESP8266_ReceiveString(1));
		Delay_ms(1000);
		for(byteCount = 0; byteCount <1024;byteCount++)
		{
			printf("%c",strEsp8266_Fram_Record .Data_RX_BUF[byteCount]);	
		}
		printf("\r\n rev =  %s",strEsp8266_Fram_Record .Data_RX_BUF);	
		Delay_ms ( 200 ); 
	}
	//ESP8266_Sta_ConnectAPPTest();

		macESP8266_Usart("AT\r\n");
		strEsp8266_Fram_Record .InfBit .FramLength = 0;
		strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
		
		Delay_ms(200);
	//	macLED1_TOGGLE();
	//	macLED2_TOGGLE();
	//	macLED3_TOGGLE();

		Delay_ms(200);
		//while(0 == strEsp8266_Fram_Record .InfBit .FramFinishFlag);
		
			//strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
		
		
		printf("\r\n rev =  %s",strEsp8266_Fram_Record .Data_RX_BUF);				
		

		Delay_ms(1000);

		if( 0 == timeCount)
		{
	//		break;
		}
	}while(1);
#endif

	
}


/****************************************************************************
* 名    称：void Delay(__IO uint32_t nCount)
* 功    能：延时函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


void UARTx_Send_Command(USART_TypeDef* USARTx, uint8_t *Data, uint8_t length)
{
	uint8_t i;
	for(i = 0; i < length; i++)
	{
		USART_SendData(USARTx, Data[i]);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
	
}

/****************************************************************************
* 名    称：void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
* 功    能：格式化串口输出函数
* 入口参数：USARTx:  指定串口
			Data：   发送数组
			...:     不定参数
* 出口参数：无
* 说    明：格式化串口输出函数
        	"\r"	回车符	   USART_OUT(USART1, "abcdefg\r")   
			"\n"	换行符	   USART_OUT(USART1, "abcdefg\r\n")
			"%s"	字符串	   USART_OUT(USART1, "字符串是：%s","abcdefg")
			"%d"	十进制	   USART_OUT(USART1, "a=%d",10)
* 调用方法：无 
****************************************************************************/
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...){ 
	const char *s;
    int d;
    char buf[16];
    va_list ap;
    va_start(ap, Data);

	while(*Data!=0){				                          //判断是否到达字符串结束符
		if(*Data==0x5c){									  //'\'
			switch (*++Data){
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);	   

					Data++;
					break;
				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data++;
					break;
				
				default:
					Data++;
				    break;
			}
			
			 
		}
		else if(*Data=='%'){									  //
			switch (*++Data){				
				case 's':										  //字符串
                	s = va_arg(ap, const char *);
                	for ( ; *s; s++) {
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
            	case 'd':										  //十进制
                	d = va_arg(ap, int);
                	itoa(d, buf, 10);
                	for (s = buf; *s; s++) {
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
				default:
					Data++;
				    break;
			}		 
		}
		else USART_SendData(USARTx, *Data++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
}



/******************************************************
		整形数据转字符串函数
        char *itoa(int value, char *string, int radix)
		radix=10 标示是10进制	非十进制，转换结果为0;  

	    例：d=-379;
		执行	itoa(d, buf, 10); 后
		
		buf="-379"							   			  
**********************************************************/
char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */



/****************************************************************************
* 名    称：void RCC_Configuration(void)
* 功    能：系统时钟配置为72MHZ， 外设时钟配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void RCC_Configuration(void)
{
   SystemInit();   
   // UART 1 RC config   
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO  , ENABLE);     
   // UART 2 RCC config   
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO  , ENABLE); 
   RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE); 
   RCC_APB1PeriphClockCmd( RCC_APB1Periph_UART4, ENABLE); 
	 
}



/****************************************************************************
* 名    称：void GPIO_Configuration(void)
* 功    能：通用IO口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/  
void GPIO_Configuration(void)
{

  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 						 
/**
 *  LED1 -> PC3 , LED2 -> PC4 , LED3 -> PB0
 */					 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);		
			 
// For rfm69 SPI GPIO config
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;	//RF Reset
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;	//SPI: NSS/SCK/MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);		

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;	//SPI MISO
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
		
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3;		//RF data received/send
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 //USART1 TX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 //复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 //A端口 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 //USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //复用开漏输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 //A端口 
  
  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         		 //USART2 TX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 //复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 //A端口 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	         	 	 //USART2 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //复用开漏输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 //A端口 
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         		 //USART4 TX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 //复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure);		    		 //A端口 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	         	 //USART4 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //复用开漏输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
}



/****************************************************************************
* 名    称：void NVIC_Configuration(void)
* 功    能：中断源配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void UART1_NVIC_Configuration(void)
{
  /*  结构声明*/
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/****************************************************************************
* 名    称：void NVIC_Configuration(void)
* 功    能：中断源配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void UART2_NVIC_Configuration(void)
{
  /*  结构声明*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;                
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

/****************************************************************************
* 名    称：void NVIC_Configuration(void)
* 功    能：中断源配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void UART4_NVIC_Configuration(void)
{
  /*  结构声明*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;                
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}




/* GPIO Button  */
void GPIO_Button_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); 	
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
	
  /* Buttons */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}


void Delay_Ms(unsigned int ms)
{
unsigned int i;
unsigned int t = 71995;	
	
	for(i=0; i<ms; i++)
	{
	for(; t != 0; t--);
	}
}

#if 0 // for testing
void SPI1_Init(void)
{
    RCC->APB2ENR |= 1<<12;   // Enable SPI1 clock
    RCC->APB2ENR |= 1<<2;
    // Config the gpio functions
    GPIOA->CRL&=0X000FFFFF;  
    GPIOA->CRL|=0XBBB00000;   //PA5.6.7 output 50M  
    GPIOA->ODR|=0X7<<5; 
    SPI1->CR1|=0<<11;   //8bit data formate 
    SPI1->CR1|=0<<10;   //full dupix
    SPI1->CR1|=1<<9;    //soft NSS
    SPI1->CR1|=1<<8;
    SPI1->CR1|=0<<7;   //MSBfirst  
    SPI1->CR1|=7<<3;    //SET Fsck=Fcpu/256 
    SPI1->CR1|=1<<2;    //SPI master 
    SPI1->CR1|=1<<1;    //idle SCK? 1 CPOL=1 
    SPI1->CR1|=1<<0;    //sample second
    SPI1->CR1|=1<<6;    //enable  SPI 
}



u8 SPI1_ReadWriteByte(u8 data)
{
    //while((SPI1->SR && 1<<7) == 0);  //??SPI1??
    while((SPI1->SR && 1<<1)==0);     //waiting tx empty
    SPI1->DR = data;     // write data
    while((SPI1->SR && 1<<0)==0);     
    return SPI1->DR;     // read data
}
#endif 

/******************* (C) COPYRIGHT 2011 奋斗STM32 *****END OF FILE****/






