#ifndef  __BSP_UART_SLAVE_H
#define	 __BSP_UART_SLAVE_H



#include "stm32f10x.h"
#include "common.h"
#include <stdio.h>
#include <stdbool.h>



#if defined ( __CC_ARM   )
#pragma anon_unions
#endif





/******************************* UART_SLAVE 外部全局变量声明 ***************************/
#define RX_BUF_MAX_LEN     1024                                     //最大接收缓存字节数

extern struct  STRUCT_USARTx_Fram_UART_SLAVE                                  //串口数据帧的处理结构体
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
	
  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 
	
} strUART_SLAVE_Fram_Record;



/******************************** UART_SLAVE 连接引脚定义 ***********************************/
//#define      macUART_SLAVE_CH_PD_APBxClock_FUN                   RCC_APB2PeriphClockCmd
//#define      macUART_SLAVE_CH_PD_CLK                             RCC_APB2Periph_GPIOG  
//#define      macUART_SLAVE_CH_PD_PORT                            GPIOG
//#define      macUART_SLAVE_CH_PD_PIN                             GPIO_Pin_13

//#define      macUART_SLAVE_RST_APBxClock_FUN                     RCC_APB2PeriphClockCmd
//#define      macUART_SLAVE_RST_CLK                               RCC_APB2Periph_GPIOG
//#define      macUART_SLAVE_RST_PORT                              GPIOG
//#define      macUART_SLAVE_RST_PIN                               GPIO_Pin_14

// 

//#define      macUART_SLAVE_USART_BAUD_RATE                       115200

//#define      macUART_SLAVE_USARTx                                USART3
//#define      macUART_SLAVE_USART_APBxClock_FUN                   RCC_APB1PeriphClockCmd
//#define      macUART_SLAVE_USART_CLK                             RCC_APB1Periph_USART3
//#define      macUART_SLAVE_USART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
//#define      macUART_SLAVE_USART_GPIO_CLK                        RCC_APB2Periph_GPIOB     
//#define      macUART_SLAVE_USART_TX_PORT                         GPIOB   
//#define      macUART_SLAVE_USART_TX_PIN                          GPIO_Pin_10
//#define      macUART_SLAVE_USART_RX_PORT                         GPIOB
//#define      macUART_SLAVE_USART_RX_PIN                          GPIO_Pin_11
//#define      macUART_SLAVE_USART_IRQ                             USART3_IRQn
//#define      macUART_SLAVE_USART_INT_FUN                         USART3_IRQHandler



/******************************** UART_SLAVE 连接引脚定义 ***********************************/
//PA0
#define      macUART_SLAVE_CH_PD_APBxClock_FUN                   RCC_APB2PeriphClockCmd
#define      macUART_SLAVE_CH_PD_CLK                             RCC_APB2Periph_GPIOA 
#define      macUART_SLAVE_CH_PD_PORT                            GPIOA
#define      macUART_SLAVE_CH_PD_PIN                             GPIO_Pin_0
//PA1
#define      macUART_SLAVE_RST_APBxClock_FUN                     RCC_APB2PeriphClockCmd
#define      macUART_SLAVE_RST_CLK                               RCC_APB2Periph_GPIOA
#define      macUART_SLAVE_RST_PORT                              GPIOA
#define      macUART_SLAVE_RST_PIN                               GPIO_Pin_1

 

#define      macUART_SLAVE_USART_BAUD_RATE                       9600

#define      macUART_SLAVE_USARTx                                USART2
#define      macUART_SLAVE_USART_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define      macUART_SLAVE_USART_CLK                             RCC_APB1Periph_USART2
#define      macUART_SLAVE_USART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define      macUART_SLAVE_USART_GPIO_CLK                        RCC_APB2Periph_GPIOA    
#define      macUART_SLAVE_USART_TX_PORT                         GPIOA
#define      macUART_SLAVE_USART_TX_PIN                          GPIO_Pin_2
#define      macUART_SLAVE_USART_RX_PORT                         GPIOA
#define      macUART_SLAVE_USART_RX_PIN                          GPIO_Pin_3
#define      macUART_SLAVE_USART_IRQ                             USART2_IRQn
#define      macUART_SLAVE_USART_INT_FUN                         USART2_IRQHandler



/*********************************************** ESP8266 函数宏定义 *******************************************/
#define     macUART_SLAVE_Usart( fmt, ... )           USART_printf ( macUART_SLAVE_USARTx, fmt, ##__VA_ARGS__ ) 
#define     macPC_Usart( fmt, ... )                printf ( fmt, ##__VA_ARGS__ )    

#define     macUART_SLAVE_CH_ENABLE()               //  GPIO_SetBits ( macESP8266_CH_PD_PORT, macESP8266_CH_PD_PIN )
#define     macUART_SLAVE_CH_DISABLE()             //   GPIO_ResetBits ( macESP8266_CH_PD_PORT, macESP8266_CH_PD_PIN )

#define     macUART_SLAVE_RST_HIGH_LEVEL()           GPIO_SetBits ( macUART_SLAVE_RST_PORT, macUART_SLAVE_RST_PIN )
#define     macUART_SLAVE_RST_LOW_LEVEL()            GPIO_ResetBits ( macUART_SLAVE_RST_PORT, macUART_SLAVE_RST_PIN )



/****************************************** ESP8266 函数声明 ***********************************************/
void                     UART_SLAVE_Init                        ( void );
void                     UART_SLAVE_Rst                         ( void );
bool                     UART_SLAVE_Cmd                         ( char * cmd, char * reply1, char * reply2, u32 waittime );





extern void test_uart_slave(void);
extern struct  STRUCT_USARTx_Fram_UART_SLAVE strUART_SLAVE_Fram_Record;



void process_sys_cmd(char *arg);
void process_boil_cmd(char *arg);
void process_brewer_cmd(char *arg);
void process_grind_cmd(char *arg);
void process_milk_cmd(char *arg);//0x50 ~0x60

char verify_chksum(char * buf);
#endif


