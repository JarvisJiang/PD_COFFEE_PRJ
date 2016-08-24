/**
  ******************************************************************************
  * @file USART/Interrupt/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and 
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_usart.h"
#include "actol_bluetooth.h"
#include "stm32f10x_tim.h"
#include "a4988_driver.h"

#include "cc2541.h"


#include <stdio.h>
#include <string.h> 
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "test.h"
#include "uart_slave.h"
#include "water_heater.h"
#define EXTI_UP  1
#define EXTI_DW  0
#define DCS_BIT_COUNTER 81
#define BYTE_DURATION 500 * 2


extern uint8_t g_Send_wifi_flag;
extern uint8_t g_Send_ble_flag;
extern uint8_t g_exti_flag;
extern uint8_t g_tp_motor_counter;
extern unsigned int g_Timer2Counter;
extern unsigned int g_Timer4Counter;
extern unsigned int g_Timer4Counter;
extern uint8_t g_keyscan_flag;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval : None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval : None
  */
__asm void wait()
{
	BX lr
}
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
	//wait();
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval : None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval : None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval : None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval : None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval : None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval : None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval : None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval : None
  */
void USART1_IRQHandler(void)      //串口1 中断服务程序
{
  unsigned int i;
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	   //判断读寄存器是否非空
  {	
    
    RxBuffer1[RxCounter1++] = USART_ReceiveData(USART1);   //将读寄存器的数据缓存到接收缓冲区里
	           
		
		if(RxBuffer1[0] != 0xAA)    // Header for 0xAA
		{
			RxCounter1=0;
		}
		
    if((RxCounter1 > 2) && (RxBuffer1[0] == 0xAA) && (RxCounter1 == RxBuffer1[1]+2))   
    {
			for(i=0; i< RxCounter1; i++) Rx1_Buff1[i]	= RxBuffer1[i]; 	     //将接收缓冲器的数据转到发送缓冲区，准备转发
			rec_f1=1;															 //接收成功标志
			Rx1_Buff1[RxCounter1]=0;		                                     //发送缓冲区结束符    
			RxCounter1=0;
	  
    }
		if(RxCounter1 > 32)   // make the totoal received by to  less 32 bytes
		{
			RxCounter1 = 0;
		}			
  }
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)                   //这段是为了避免STM32 USART 第一个字节发不出去的BUG 
  { 
     USART_ITConfig(USART1, USART_IT_TXE, DISABLE);					     //禁止发缓冲器空中断， 
  }	
  
}



/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles macESP8266_USARTx Handler.
  * @param  None
  * @retval None
  */
#if 0
void macESP8266_USART_INT_FUN ( void )
{	
	uint8_t ucCh;
	
	
	if ( USART_GetITStatus ( macESP8266_USARTx, USART_IT_RXNE ) == SET )
	{
		ucCh  = USART_ReceiveData( macESP8266_USARTx );
		
		if ( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ucCh;

	}
	 	 
	if (  (USART_GetITStatus( macESP8266_USARTx, USART_IT_IDLE ) == SET)&&(2<strEsp8266_Fram_Record .InfBit .FramLength))                                         //数据帧接收完毕
	{
    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( macESP8266_USARTx );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
		
  }	

}
#endif
/**
  * @brief  This function handles macUART_SLAVE_USARTx Handler.
  * @param  None
  * @retval None
  */

void macUART_SLAVE_USART_INT_FUN ( void )
{	
	uint8_t ucCh;
	
	
	if ( USART_GetITStatus ( macUART_SLAVE_USARTx, USART_IT_RXNE ) == SET )
	{
		ucCh  = USART_ReceiveData( macUART_SLAVE_USARTx );
		
		if ( strUART_SLAVE_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
			strUART_SLAVE_Fram_Record .Data_RX_BUF [ strUART_SLAVE_Fram_Record .InfBit .FramLength ++ ]  = ucCh;

	}
	 	 
	if (  (USART_GetITStatus( macUART_SLAVE_USARTx, USART_IT_IDLE ) == SET)&&(2<strUART_SLAVE_Fram_Record .InfBit .FramLength))                                         //数据帧接收完毕
	{
    strUART_SLAVE_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( macUART_SLAVE_USARTx );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
		
  }	

}

#if 0
void macCC2541_UART_INT_FUN(void) 
{
	uint8_t ucCh;
	
	
	if ( USART_GetITStatus ( macCC2541_UARTx, USART_IT_RXNE ) == SET )
	{
		ucCh  = USART_ReceiveData( macCC2541_UARTx );
		
		if ( strCC2541_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
			strCC2541_Fram_Record .Data_RX_BUF [ strCC2541_Fram_Record .InfBit .FramLength ++ ]  = ucCh;

	}
	 	 
	if ( (USART_GetITStatus( macCC2541_UARTx, USART_IT_IDLE ) == SET)&&(2<strCC2541_Fram_Record .InfBit .FramLength) )                                         //数据帧接收完毕
	{
    strCC2541_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( macCC2541_UARTx );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
		
  }	
}
#endif
void TIM2_IRQHandler(void)						  
{
	//unsigned char itemp = 0;
	
	if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update)!=RESET)
	   {		
			GPIO_SetBits(GPIOC, GPIO_Pin_3);			 
			 
			if(g_Enable_A4988) // enable the a4988 stepper moter driver
			{
					if(g_Timer2Counter%2)
					{
						GPIO_ResetBits(GPIOB , GPIO_Pin_0);		
						A4988_STEP_L; // stepper low 
					}
					else
					{
						GPIO_SetBits(GPIOB, GPIO_Pin_0);	
						A4988_STEP_H;   //stepper high
					}					
					g_Timer2Counter++;				 
			}
			else
			{
					g_Timer2Counter = 0;
			}
			
			GPIO_ResetBits(GPIOC, GPIO_Pin_3);
			
			TIM_ClearFlag(TIM2,TIM_FLAG_Update);  
	   }		
}

void TIM4_IRQHandler(void)						  
{	
	if(TIM_GetFlagStatus(TIM4,TIM_FLAG_Update)!=RESET)
	   {	
			 
			g_Timer4Counter++;	
			
			if(g_Timer4Counter>= 100)
			{
				g_keyscan_flag = 1;
				g_Timer4Counter = 0;
			}
	   }	
		 
	TIM_ClearFlag(TIM4,TIM_FLAG_Update); 	 
}
#if 0
#define PRINTF(fmt,...)   USART_OUT(USART1,fmt,##__VA_ARGS__)

void PID_ADJUST_USART_INT_FUN ( void )
{	
	uint8_t ch;
	static char RI_Count,RI_SD_flag = 0x88;

	//PRINTF("INT\n");
	if ( USART_GetITStatus ( PID_ADJUST_USART, USART_IT_RXNE ) == SET )
	{
		ch  = USART_ReceiveData( PID_ADJUST_USART );
		PRINTF("rev: %x\n",ch);
		
	}
	 	 
	if ((USART_GetITStatus( PID_ADJUST_USART, USART_IT_IDLE ) == SET))                                         //数据帧接收完毕
	{
		ch = USART_ReceiveData( PID_ADJUST_USART );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
  }	
	if ((ch == 'S')||(ch == 's'))
		{
			RI_Count= 0 ;
			RI_SD_flag = 0 ;
	}
   	if ((ch == 'P')||(ch == 'p'))
		{
			RI_Count= 0 ;
			RI_SD_flag = 1 ;
	}
	if ((ch == 'I')||(ch == 'i'))
		{
			RI_Count= 0 ;
			RI_SD_flag = 2 ;
	}
	if ((ch == 'D')||(ch == 'd'))
		{
			RI_Count= 0 ;
			RI_SD_flag = 3 ;
	}
	if ((ch == 'T')||(ch == 't'))
		{
			RI_Count= 0 ;
			RI_SD_flag = 4 ;
	}


   switch (RI_SD_flag)
	{
		case 0: 
			{
		
				RI_Buffer[RI_Count++] = ch - '0' ; 
				if(RI_Count == 3)
				{
					g_set_temper = (RI_Buffer[1]*16 +RI_Buffer[2])*10;//*100 +RI_Buffer[3]*10 + RI_Buffer[4] ;
					PRINTF("g_set_temper = %d\n",g_set_temper);
					PRINTF("[1] = %d, [2] = %d, [3] = %d,[4] = %d",RI_Buffer[1],RI_Buffer[2],RI_Buffer[3],RI_Buffer[4]);
					RI_SD_flag = 0x88;
				}
			} 
			break;
		case 1:
			{
				RI_Buffer[RI_Count++] = ch - '0' ; 
				if(RI_Count == 4)
				{
					spid.Proportion = RI_Buffer[1]*100 +RI_Buffer[2]*10 + RI_Buffer[3] ;	
				}
			} 
			break;
		case 2:
			{
				RI_Buffer[RI_Count++] = ch - '0' ; 
				if(RI_Count == 4)
				{
					spid.Integral = RI_Buffer[1]*100 +RI_Buffer[2]*10 + RI_Buffer[3] ;	
				}
			} 
			break;
		case 3:
			{
				RI_Buffer[RI_Count++] = ch - '0' ; 
				if(RI_Count == 4)
				{
					spid.Derivative = RI_Buffer[1]*100 +RI_Buffer[2]*10 + RI_Buffer[3] ;	
				}
			} 
			break;
		case 4:
			{
				RI_Buffer[RI_Count++] = ch - '0' ; 
				if(RI_Count == 4)
				{
					g_temp_offset = RI_Buffer[1]*100 +RI_Buffer[2]*10 + RI_Buffer[3] ;	
				}
			} 
			break;
		default:
			;
			break;
	}
}
#else
extern unsigned char g_comm;
void PID_ADJUST_USART_INT_FUN ( void )
{	
	 if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)    //?????????
  {     
    g_comm = USART_ReceiveData(USART1);   //?????:?USART_DR????????comm????,????????,???USART_DR???????RXNE????[??]????????,????????"??????"DMA1_FLAG_TCx(?:?DMA_IFCR??CTCIFx=1),??????????????,????????????????
    
	if(g_comm == 0x63)                    //??????????????
	{  			
			g_comm = 0;      
      send_char_com(0x63);              //??CP?ASCII?
      send_char_com(0x70);                              			  
	}  
  }
}
#endif
