/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "platform_config.h" 
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stdarg.h"
#include <string.h>
#include <stdio.h>
#include "water_heater.h"

extern void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
/****************************************************************


PA6  PA7 
***************************************************************/ 

#define PRINTF(fmt,...) USART_OUT(USART1,fmt,##__VA_ARGS__)

void GPIO_PWM_Config(void)  
{ 
GPIO_InitTypeDef GPIO_InitStructure; 

/* GPIOA and GPIOB clock enable */ 
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);  

/*GPIOA Configuration: TIM3 channel 1 and 2 as alternate function push-pull */ 
GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;//| GPIO_Pin_7; // 
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           // ?????? 
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
GPIO_Init(GPIOB, &GPIO_InitStructure); 
} 

/**************************************************************** 

***************************************************************/  
#define M_TIM_PERIOD  0XEA5F
void TIM3_PWM_Config(void)  
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
	TIM_OCInitTypeDef  TIM_OCInitStructure;  
	/* PWM??????? */  
	u16 CCR1= 32768;          
//	u16 CCR2= 36000;  
	/*PCLK1??2?????TIM3??????72MHz*/  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
	/* Time base configuration */                                            
	TIM_TimeBaseStructure.TIM_Period =	M_TIM_PERIOD;  // ÖÜÆÚ  
	TIM_TimeBaseStructure.TIM_Prescaler = 2;                                    //?????:???=2,??72/3=24MHz  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                                //????????:???  
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 //????????  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
	/* PWM1 Mode configuration: Channel1 */  
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           //???PWM??1  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
	TIM_OCInitStructure.TIM_Pulse = CCR1;                                       //?????,???????????,??????  
	TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;                    //?????????CCR1?????  
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);                                    //????1      
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  
//	/* PWM1 Mode configuration: Channel2 */  
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
//	TIM_OCInitStructure.TIM_Pulse = CCR2;                                       //????2??????,??????????PWM  
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;                    //?????????CCR2????? 
//	TIM_OC2Init(TIM3, &TIM_OCInitStructure);                                    //????2  
//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  
//	TIM_ARRPreloadConfig(TIM3, ENABLE);                                         //??TIM3?????ARR  
	/* TIM3 enable counter */  
	TIM_Cmd(TIM3, ENABLE);                                                      //??TIM3   
} 


void init_heat_up(void)
{
	GPIO_PWM_Config(); 
//	TIM3_PWM_Config(); 
}

/*
suorce clk = 24MHZ  //  72 /  3
1/24MHZ  *  TIM_Period  = T 
1/24MHZ  *  0XEA5F(59999)  = 2.5ms  (400hz)
adjust duty cycle
*/
void config_heat_up_pwm(float ratio)
{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
//	TIM_OCInitTypeDef  TIM_OCInitStructure;  
	/* PWM??????? */  
	float f_percent = (float)M_TIM_PERIOD/100.0;
	u16 CCR1= 0;

	if(0.0>ratio)
	{
	//	stop_water_heat_up();
	//	return;
		ratio = 0;
	}
	else if(ratio>100||ratio==100)
	{
		ratio = 100;
		//all_speed_heat_up();
	}
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  	
	f_percent *= ratio;
	CCR1   		 = (unsigned short)f_percent;
	//USART_OUT(USART1,"CCR1 = %d\n",CCR1);
	set_pwm_ch3_output(1);
	TIM_SetCompare3(TIM3,CCR1);
	#if 0
	/*PCLK1??2?????TIM3??????72MHz*/  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
	/* Time base configuration */                                            
	TIM_TimeBaseStructure.TIM_Period =0xEA5F;  
	TIM_TimeBaseStructure.TIM_Prescaler = 2;                                    //72/3=24MHz  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                                //????????:???  
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 //????????  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
	/* PWM1 Mode configuration: Channel1 */  
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           //???PWM??1  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
	TIM_OCInitStructure.TIM_Pulse = CCR1;                                       //?????,???????????,??????  
	TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;                    //?????????CCR1?????  
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);                                    //????1      
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable); 
	/* TIM3 enable counter */  
	TIM_Cmd(TIM3, ENABLE);
	#endif
}



/*
suorce clk = 24MHZ  //  72 /  3
1/24MHZ  *  TIM_Period     = T 
1/24MHZ  *  0XEA5F(59999)  = 2.5ms  (400hz)
1/24MHZ  *  0X5dbf(23999)  = 1.0ms  (1000hz)
1/24MHZ  *  0X2edf(11999)  = 0.5ms  (2000hz)
frq   =  
*/
/*
500 hz  ~   10 khz
**/
void config_frq_pwm(int  frq)
{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
//	TIM_OCInitTypeDef  TIM_OCInitStructure;  

//	u16 CCR1= 32768;
  u16 set_period;
	u16 set_1k_value = 0x5dbf;
	float ftemp;
	if(frq<400||frq>10000)
	{
		USART_OUT(USART1,"frq  parameter error\n");
	}
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3,ENABLE);
	ftemp = (float)(1000.0/frq);
	set_period = (u16)(set_1k_value*ftemp);
  USART_OUT(USART1,"set_period = %d\n",set_period);
	TIM_SetAutoreload(TIM3,set_period);//change  FRQ HZ
	TIM_SetCompare3(TIM3,set_period/2);//CCR1=(TIM_Period+1)* D=36000   dutycycle  = 50%
    
}


void all_speed_heat_up(void)
{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); 
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
		set_pwm_ch3_output(1);
//	GPIO_SetBits(GPIOA,GPIO_Pin_7)
//	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
//	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
}

void stop_water_heat_up(void)
{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); 

//	GPIO_SetBits(GPIOA,GPIO_Pin_7)
    GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		set_pwm_ch3_output(0);
//	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
}

void test_heat_up(void)
{
		
  init_heat_up();//PA6
  TIM3_PWM_Config();
	#if 1
	config_heat_up_pwm(5);
//	stop_water_heat_up();
	config_heat_up_pwm(15);
	config_heat_up_pwm(35);
  	config_heat_up_pwm(45);
	config_heat_up_pwm(65);
    config_heat_up_pwm(85);
	config_heat_up_pwm(95);
	config_heat_up_pwm(100);
	#endif
	set_pwm_ch3_output(0);
	set_pwm_ch3_output(1);
	set_pwm_ch3_output(0);
	set_pwm_ch3_output(1);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);

	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
		GPIO_SetBits(GPIOB,GPIO_Pin_0);

	GPIO_ResetBits(GPIOB,GPIO_Pin_0);

}
void set_pwm_ch3_output(char bit)
{
	short mask = (1<<8);
	if(bit)
	TIM3->CCER |= mask;
	else
	TIM3->CCER &= ~mask;
}

void test_pwm_frq(void)
{
	TIM3_PWM_Config();
	config_frq_pwm(500);

	config_frq_pwm(800);
 
	config_frq_pwm(1000);

	config_frq_pwm(1050);

	config_frq_pwm(1200);

	config_frq_pwm(1400);

	config_frq_pwm(1500);

	config_frq_pwm(1600);
	config_frq_pwm(1800);
	config_frq_pwm(2000);
}

