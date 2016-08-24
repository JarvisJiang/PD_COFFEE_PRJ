#include "cc2541.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "bsp_SysTick.h"
#include "misc.h"

/**
  * @brief   void CC41_init(void) ����
  * @param  ��
  * @retval ��
	
  */
	
struct  STRUCT_USARTx_Fram_CC41 strCC2541_Fram_Record = { 0 };


 void CC2541_Init(void)
 {
	CC2541_GPIO_Config (); 
	
	CC2541_UART_Config (); 
	
	
	macCC2541_RST_HIGH_LEVEL();

	macCC2541_CH_DISABLE();
 }
/*
 * @name  : C2541_Cmd
 * @brif  ����C2541ģ�鷢��ATָ��
 * @input  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * @return  : 1��ָ��ͳɹ�
 *         		0��ָ���ʧ��
 */

 bool CC2541_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strCC2541_Fram_Record .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�
	
	macCC2541_UART("%s\r\n",cmd);


	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
		return true;
	
	Delay_ms ( waittime );                 //��ʱ
	
	strCC2541_Fram_Record .Data_RX_BUF [ strCC2541_Fram_Record .InfBit .FramLength ]  = '\0';


	printf("CMD's ACK Of CC2541 = %s\r\n",strCC2541_Fram_Record .Data_RX_BUF);
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strCC2541_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strCC2541_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strCC2541_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strCC2541_Fram_Record .Data_RX_BUF, reply2 ) );
	
}

/*
 * @name  : CC2541_AT_Test
 * @brif  ����C2541 ����AT����
 * @input ��
 *        
 * @return  : NULL
 *         		
 */


void CC2541_AT_Test(void)
{

	//char result = 0;


//	CC2541_Rst();
	
	Delay_ms ( 200 ); 
	
	while ( ! CC2541_Cmd ( "AT", "OK", NULL, 500 ) ) printf("CC2541 AT TEST NOT OK\r\n");  	
	

}


/*
 * @name��CC2541_SendingStringConnectless
 * @brief  ����CC2541ģ�鷢������  �������� û�����Ӧ��
 * @input  ��txBuf������������
 */
void CC2541_SendingStringConnectless(char* txBuf)
{

	//char result = 0;


//	CC2541_Rst();
	
	Delay_ms ( 200 ); 
	
	CC2541_Cmd ( txBuf, NULL, NULL, 0 ); 	
	

}





 /**
   * @brief  void CC41_UART_Configure(void) baud rate:9600
   * @param  ��
   * @retval ��
   */
void CC2541_UART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	
	/* config USART clock */
	macCC2541_UART_APBxClock_FUN ( macCC2541_UART_CLK, ENABLE );
	macCC2541_UART_GPIO_APBxClock_FUN ( macCC2541_UART_GPIO_CLK, ENABLE );
	
	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  macCC2541_UART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macCC2541_UART_TX_PORT, &GPIO_InitStructure);  
  
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = macCC2541_UART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macCC2541_UART_RX_PORT, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = macCC2541_UART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(macCC2541_UARTx, &USART_InitStructure);
	
	
	/* �ж����� */
  USART_ITConfig ( macCC2541_UARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж� 
  USART_ITConfig ( macCC2541_UARTx, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 	
	USART_ITConfig(macCC2541_UARTx, USART_IT_TXE, DISABLE);

	CC2541_USART_NVIC_Configuration ();
	
	
	USART_Cmd(macCC2541_UARTx, ENABLE);
	

}
/**
  * @brief  void CC41_GPIO_Configure(void)  reset or en
  * @param  ��
  * @retval ��
  */
void CC2541_GPIO_Config(void)
{
	#if 1
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;


	/* ���� CH_PD ����*/
	macCC2541_CH_PD_APBxClock_FUN ( macCC2541_CH_PD_CLK, ENABLE ); 
											   
	GPIO_InitStructure.GPIO_Pin = macCC2541_CH_PD_PIN;	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_Init ( macCC2541_CH_PD_PORT, & GPIO_InitStructure );	 

	
	/* ���� RST ����*/
	macCC2541_RST_APBxClock_FUN ( macCC2541_RST_CLK, ENABLE ); 
											   
	GPIO_InitStructure.GPIO_Pin = macCC2541_RST_PIN;	

	GPIO_Init ( macCC2541_RST_PORT, & GPIO_InitStructure );	 
	#endif
}
/*
�������gpio rst
*/
void CC2541_Rst(void)
{
	
}


/**
  * @brief  ���� CC2541 USART �� NVIC �ж�
  * @param  ��
  * @retval ��
  */
/*
 
cc2541 ���ȼ� ���� esp 8266
*/
void CC2541_USART_NVIC_Configuration ( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig ( macNVIC_PriorityGroup_x );

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = macCC2541_UART_IRQ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
/**
  * @name   CC2541_ReceiveString
  * @brief  �����Ƶȴ��ж� 	
  * @param  ��
  * @retval ��
  */

/*

*/
char * 	CC2541_ReceiveString(void)
{
	char * pRecStr;
	
	
	strCC2541_Fram_Record .InfBit .FramLength = 0;
	strCC2541_Fram_Record .InfBit .FramFinishFlag = 0;
	
	while ( ! strCC2541_Fram_Record .InfBit .FramFinishFlag );
	strCC2541_Fram_Record .Data_RX_BUF [ strCC2541_Fram_Record .InfBit .FramLength ] = '\0';
	
	pRecStr = strCC2541_Fram_Record .Data_RX_BUF;
	


	return pRecStr;
	
}
/**
  * @name   CC2541_BuildLinkByPhone
  * @brief  ��������   ��Ҫ�ֻ� ����һ���������� 	
  * @param  ��
  * @retval ��
  */

bool CC2541_BuildLinkByPhone(void)
{
	int reptCount = 0;
	while(1)
	{
		reptCount++;
		if(reptCount>10)
		{
			printf("repeat out please check the hardware!\r\n");
			return false;
		}
		CC2541_Rst();
	//����  ģ�鹤������  �ϵ�ȴ� start ����.
	if(!CC2541_Cmd("AT+IMME1\r\n","OK",NULL,200))
	{
		
		printf("IMME ERR\r\n");
		continue;
	}
	// ������ֻ����� device  ��֪ͨ��λ��
	if(!CC2541_Cmd("AT+NOTI1\r\n","OK",NULL,200))
	{
		
		printf("NOTI1 ERR\r\n");
		continue;
	}
	//��������
	if(!CC2541_Cmd("AT+NAMECoffee Machine\r\n","OK",NULL,200))
	{
		printf("NAME ERR\r\n");
		continue;
	}
	if(!CC2541_Cmd("AT+RESET\r\n","OK",NULL,200))
	{
		printf("reset ERR\r\n");
		continue;		
	}
	
	Delay_ms (600);
	//��ʼ ����
	
	if(!CC2541_Cmd("AT+START\r\n","OK",NULL,200))
	{
		printf("start err\r\n");
	}
	else
	{
		break;
	}

		
	}
	printf("CC2541 start!\r\n");
	return true;
}
