#ifndef _CC2541_H_
#define _CC2541_H_

/*
**
cc2541.h


*/


#include "stm32f10x.h"
#include "common.h"
#include <stdio.h>
#include <stdbool.h>



#if defined ( __CC_ARM   )
#pragma anon_unions
#endif



/******************************* CC2541 数据类型定义 ***************************/



/******************************* CC2541 外部全局变量声明 ***************************/
#define RX_BUF_MAX_LEN     1024                                     //最大接收缓存字节数

extern struct  STRUCT_USARTx_Fram_CC41                                  //串口数据帧的处理结构体
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];

  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0
		  __IO u16 FramFinishFlag   :1;                                // 15
	  } InfBit;
  };

} strCC2541_Fram_Record;






/******************************** CC2541 连接引脚定义 ***********************************/

/*  选择  片选引脚CH */
#define      macCC2541_CH_PD_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define      macCC2541_CH_PD_CLK                             RCC_APB2Periph_GPIOC
#define      macCC2541_CH_PD_PORT                            GPIOA
#define      macCC2541_CH_PD_PIN                             GPIO_Pin_0
/** 复位引脚*/
#define      macCC2541_RST_APBxClock_FUN                     RCC_APB2PeriphClockCmd
#define      macCC2541_RST_CLK                               RCC_APB2Periph_GPIOA
#define      macCC2541_RST_PORT                              GPIOA
#define      macCC2541_RST_PIN                               GPIO_Pin_1



#define      macCC2541_UART_BAUD_RATE                       9600

#define      macCC2541_UARTx                                UART4
#define      macCC2541_UART_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define      macCC2541_UART_CLK                             RCC_APB1Periph_UART4
#define      macCC2541_UART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define      macCC2541_UART_GPIO_CLK                        RCC_APB2Periph_GPIOC
#define      macCC2541_UART_TX_PORT                         GPIOC
#define      macCC2541_UART_TX_PIN                          GPIO_Pin_10
#define      macCC2541_UART_RX_PORT                         GPIOC
#define      macCC2541_UART_RX_PIN                          GPIO_Pin_11
#define      macCC2541_UART_IRQ                             UART4_IRQn
#define      macCC2541_UART_INT_FUN                         UART4_IRQHandler



/*********************************************** CC2541 函数宏定义 *******************************************/
#define     macCC2541_UART( fmt, ... )           USART_printf ( macCC2541_UARTx, fmt, ##__VA_ARGS__ )


#define     macCC2541_CH_ENABLE()                 //GPIO_SetBits ( macCC2541_CH_PD_PORT, macCC2541_CH_PD_PIN )
#define     macCC2541_CH_DISABLE()                //GPIO_ResetBits ( macCC2541_CH_PD_PORT, macCC2541_CH_PD_PIN )

#define     macCC2541_RST_HIGH_LEVEL()            //GPIO_SetBits ( macCC2541_RST_PORT, macCC2541_RST_PIN )
#define     macCC2541_RST_LOW_LEVEL()             //GPIO_ResetBits ( macCC2541_RST_PORT, macCC2541_RST_PIN )



/****************************************** CC2541 函数声明 ***********************************************/
void                     CC2541_Init                        (void);
void                     CC2541_Rst                         (void);
bool                     CC2541_Cmd                         (char * cmd, char * reply1, char * reply2, u32 waittime );
void                     CC2541_AT_Test                     (void);
void 										 CC2541_UART_Config									(void);
void 										 CC2541_GPIO_Config									(void);
void 										 CC2541_USART_NVIC_Configuration    (void);
char * 									 CC2541_ReceiveString               (void);
void 										 CC2541_SendingStringConnectless    (char* txBuf);
bool 										 CC2541_BuildLinkByPhone						(void);
#endif



