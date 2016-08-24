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
  * @brief  ESP8266��ʼ������
  * @param  ��
  * @retval ��
  */
void ESP8266_Init ( void )
{
	ESP8266_GPIO_Config (); 
	
	ESP8266_USART_Config (); 
	
	
	macESP8266_RST_HIGH_LEVEL();

	macESP8266_CH_DISABLE();
	
	
}


/**
  * @brief  ��ʼ��ESP8266�õ���GPIO����
  * @param  ��
  * @retval ��
  */
static void ESP8266_GPIO_Config ( void )
{
	#if 0

	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;


	/* ���� CH_PD ����*/
	macESP8266_CH_PD_APBxClock_FUN ( macESP8266_CH_PD_CLK, ENABLE ); 
											   
	GPIO_InitStructure.GPIO_Pin = macESP8266_CH_PD_PIN;	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_Init ( macESP8266_CH_PD_PORT, & GPIO_InitStructure );	 

	
	/* ���� RST ����*/
	macESP8266_RST_APBxClock_FUN ( macESP8266_RST_CLK, ENABLE ); 
											   
	GPIO_InitStructure.GPIO_Pin = macESP8266_RST_PIN;	

	GPIO_Init ( macESP8266_RST_PORT, & GPIO_InitStructure );	 
	#endif

}



/**
  * @brief  ��ʼ��ESP8266�õ��� USART
  * @param  ��
  * @retval ��
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
	
	
	/* �ж����� */
	USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж� 
	USART_ITConfig ( macESP8266_USARTx, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 	
	USART_ITConfig(macESP8266_USARTx, USART_IT_TXE, DISABLE);

	ESP8266_USART_NVIC_Configuration ();
	
	
	USART_Cmd(macESP8266_USARTx, ENABLE);
	
	
}


/**
  * @brief  ���� ESP8266 USART �� NVIC �ж�
  * @param  ��
  * @retval ��
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
 * @name��ESP8266_Rst
 * @brief ������WF-ESP8266ģ��
 * @input  ����
 * @return  : ��
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
 * @name��RmoveNullByte
 * @brief  �ڲ��Թ����� ����  esp ���ݰ���һ���ַ� ����Ϊ'\0'������  ���� �޷���ӡ 
 * @input  ����
 * @return  : ��
 */
static void RmoveNullByte(char * byte, int len)
{
	int i;
	for(i = 0; i<len; i++)
	{
		if(0==byte[i])
		{
			byte[i] = 0x0d;//����Ϊ�س���
			return;
		}
	}
}

/*
 * @name��ESP8266_Cmd
 * @brief  ����WF-ESP8266ģ�鷢��ATָ��
 * @input  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * @return   : 1��ָ��ͳɹ�
 *         		 0��ָ���ʧ��

 */

bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{   
//	int byteCount;
          
	
	macESP8266_Usart ( "%s\r\n", cmd );
	strEsp8266_Fram_Record .InfBit .FramLength = 0;     //���¿�ʼ�����µ����ݰ�
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
		return true;
	
	Delay_ms ( waittime );                 //��ʱ
	
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
 * @name��ESP8266_AT_Test
 * @brief  ����WF-ESP8266ģ�����AT��������

 */
void ESP8266_AT_Test ( void )
{
	macESP8266_RST_HIGH_LEVEL();
	
	Delay_ms ( 100 ); 
	
	while ( ! ESP8266_Cmd ( "AT", "OK", NULL, 500 ) ) printf("ESP not ok\r\n");  	

}


/*
 * @name     ��ESP8266_Net_Mode_Choose
 * @brief ��ѡ��WF-ESP8266ģ��Ĺ���ģʽ
 * @input  ��enumMode������ģʽ
 * @return  : 1��ѡ��ɹ�
 *            0��ѡ��ʧ��
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
 * name��ESP8266_JoinAP
 * brief  ��WF-ESP8266ģ�������ⲿWiFi
 * input  ��pSSID��WiFi�����ַ���
 *        ��pPassWord��WiFi�����ַ���
 * reurn  : 1�����ӳɹ�
 *         0������ʧ��
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 5000 );
	
}

/*
 * @name				 ��ESP8266_Get_LinkStatus
 * @description  ����ȡ WF-ESP8266 ������״̬�����ʺϵ��˿�ʱʹ��
 * @input  			 ����
 * @return       : 	
 *         				 	2����������
 *         					3��ʧȥ����
 *         					4����ȡ״̬ʧ��
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
 * name��ESP8266_ReceiveString
 * description  ��WF-ESP8266ģ������ַ���
 * input  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 * output  : ���յ����ַ����׵�ַ
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
 * name��ESP8266_CLD_SendString
 * description  ��WF-ESP8266ģ�鷢���ַ���
 * input  ��pStr �������ַ���   ����
 * output  :    0�� ʧ��
                1�� �ɹ�
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



