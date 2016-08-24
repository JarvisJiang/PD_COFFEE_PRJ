#include "bsp_esp8266.h"
#include "common.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "bsp_SysTick.h"
#include "misc.h"


static void                   ESP8266_GPIO_Config                 ( void );
static void                   ESP8266_USART_Config                ( void );
static void                   ESP8266_USART_NVIC_Configuration    ( void );



struct  STRUCT_USARTx_Fram_Esp strEsp8266_Fram_Record = { 0 };



/**
  * @brief  ESP8266初始化函数
  * @param  无
  * @retval 无
  */
void ESP8266_Init ( void )
{
	ESP8266_GPIO_Config (); 
	
	ESP8266_USART_Config (); 
	
	
	macESP8266_RST_HIGH_LEVEL();

	macESP8266_CH_DISABLE();
	
	
}


/**
  * @brief  初始化ESP8266用到的GPIO引脚
  * @param  无
  * @retval 无
  */
static void ESP8266_GPIO_Config ( void )
{
	#if 0

	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;


	/* 配置 CH_PD 引脚*/
	macESP8266_CH_PD_APBxClock_FUN ( macESP8266_CH_PD_CLK, ENABLE ); 
											   
	GPIO_InitStructure.GPIO_Pin = macESP8266_CH_PD_PIN;	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_Init ( macESP8266_CH_PD_PORT, & GPIO_InitStructure );	 

	
	/* 配置 RST 引脚*/
	macESP8266_RST_APBxClock_FUN ( macESP8266_RST_CLK, ENABLE ); 
											   
	GPIO_InitStructure.GPIO_Pin = macESP8266_RST_PIN;	

	GPIO_Init ( macESP8266_RST_PORT, & GPIO_InitStructure );	 
	#endif

}



/**
  * @brief  初始化ESP8266用到的 USART
  * @param  无
  * @retval 无
  */
static void ESP8266_USART_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	
	/* config USART clock */
	macESP8266_USART_APBxClock_FUN ( macESP8266_USART_CLK, ENABLE );
	macESP8266_USART_GPIO_APBxClock_FUN ( macESP8266_USART_GPIO_CLK, ENABLE );
	
	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  macESP8266_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macESP8266_USART_TX_PORT, &GPIO_InitStructure);  
  
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = macESP8266_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macESP8266_USART_RX_PORT, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = macESP8266_USART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(macESP8266_USARTx, &USART_InitStructure);
	
	
	/* 中断配置 */
	USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断 
	USART_ITConfig ( macESP8266_USARTx, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 	
	USART_ITConfig(macESP8266_USARTx, USART_IT_TXE, DISABLE);

	ESP8266_USART_NVIC_Configuration ();
	
	
	USART_Cmd(macESP8266_USARTx, ENABLE);
	
	
}


/**
  * @brief  配置 ESP8266 USART 的 NVIC 中断
  * @param  无
  * @retval 无
  */
static void ESP8266_USART_NVIC_Configuration ( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig ( macNVIC_PriorityGroup_x );

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = macESP8266_USART_IRQ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


/*
 * @name：ESP8266_Rst
 * @brief ：重启WF-ESP8266模块
 * @input  ：无
 * @return  : 无
 */
void ESP8266_Rst ( void )
{
	#if 0
	 ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	
	
	#else
	macESP8266_RST_LOW_LEVEL();
	Delay_ms ( 200 ); 
	macESP8266_RST_HIGH_LEVEL();
	 
	#endif

}

/*
 * @name：RmoveNullByte
 * @brief  在测试过程中 发现  esp 数据包第一个字符 可能为'\0'结束符  导致 无法打印 
 * @input  ：无
 * @return  : 无
 */
static void RmoveNullByte(char * byte, int len)
{
	int i;
	for(i = 0; i<len; i++)
	{
		if(0==byte[i])
		{
			byte[i] = 0x0d;//更换为回车符
			return;
		}
	}
}

/*
 * @name：ESP8266_Cmd
 * @brief  ：对WF-ESP8266模块发送AT指令
 * @input  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * @return   : 1，指令发送成功
 *         		 0，指令发送失败

 */

bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{   
//	int byteCount;
          
	
	macESP8266_Usart ( "%s\r\n", cmd );
	strEsp8266_Fram_Record .InfBit .FramLength = 0;     //从新开始接收新的数据包
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
	
	Delay_ms ( waittime );                 //延时
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
	RmoveNullByte(strEsp8266_Fram_Record .Data_RX_BUF,strEsp8266_Fram_Record .InfBit .FramLength);
	printf("Length = %d\r\n",strEsp8266_Fram_Record .InfBit .FramLength );
	/*
	if('H'==*cmd)
	{
	for(byteCount = 0; byteCount <1024;byteCount++)
	{
		printf("%c",strEsp8266_Fram_Record .Data_RX_BUF[byteCount]);	
	}
	}
*/
	 printf( "CMDEack= %s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}


/*
 * @name：ESP8266_AT_Test
 * @brief  ：对WF-ESP8266模块进行AT测试启动

 */
void ESP8266_AT_Test ( void )
{
	macESP8266_RST_HIGH_LEVEL();
	
	Delay_ms ( 100 ); 
	
	while ( ! ESP8266_Cmd ( "AT", "OK", NULL, 500 ) ) printf("ESP not ok\r\n");  	

}


/*
 * @name     ：ESP8266_Net_Mode_Choose
 * @brief ：选择WF-ESP8266模块的工作模式
 * @input  ：enumMode，工作模式
 * @return  : 1，选择成功
 *            0，选择失败
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 
		
	  case AP:
		  return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 
		
		case STA_AP:
		  return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 
		
	  default:
		  return false;
  }
	
}


/*
 * name：ESP8266_JoinAP
 * brief  ：WF-ESP8266模块连接外部WiFi
 * input  ：pSSID，WiFi名称字符串
 *        ：pPassWord，WiFi密码字符串
 * reurn  : 1，连接成功
 *         0，连接失败
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 5000 );
	
}

/*
 * @name				 ：ESP8266_Get_LinkStatus
 * @description  ：获取 WF-ESP8266 的连接状态，较适合单端口时使用
 * @input  			 ：无
 * @return       : 	
 *         				 	2，建立连接
 *         					3，失去连接
 *         					4，获取状态失败
 */
uint8_t ESP8266_Get_LinkStatus ( void )
{
	if ( ESP8266_Cmd ( "AT+CIPSTATUS", "OK", 0, 500 ) )
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:2\r\n" ) )
			return 2;
		
		else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:3\r\n" ) )
			return 3;
		
		else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:4\r\n" ) )
			return 4;		

	}
	
	return 0;
	
}


/*
 * name：ESP8266_ReceiveString
 * description  ：WF-ESP8266模块接收字符串
 * input  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 * output  : 接收到的字符串首地址
 */
char * ESP8266_ReceiveString ()
{
	char * pRecStr = 0;
	
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	
	while ( ! strEsp8266_Fram_Record .InfBit .FramFinishFlag );
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
	
	pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;
	


	return pRecStr;
	
}

/*
 * name：ESP8266_CLD_SendString
 * description  ：WF-ESP8266模块发送字符串
 * input  ：pStr 待发送字符串   长度
 * output  :    0， 失败
                1， 成功
 */

bool ESP8266_CLD_SendString (char * pStr, u32 ulStrLength)
{
	char cStr [20];
	bool bRet = false;
	
    sprintf ( cStr, "AT+CLDSEND=%d", ulStrLength+2);
	
	bRet = ESP8266_Cmd ( cStr, "> ", 0, 100 );
  printf("bRet1 = %d\r\n",bRet);
	bRet = ESP8266_Cmd (pStr, "SEND OK", 0, 500 );
  printf("bRet2 = %d\r\n",bRet);
	
	return bRet;

}



