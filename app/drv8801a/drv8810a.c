#include "drv8810a.h"

#define DIR_STOP     0X74
#define DIR_FORWARD  0X75
#define DIR_REVERSE  0X76

void drv_8801_delay_ms(int ms)
{
	int i;
	int j = 60000;
	for(i = 0 ;  i < ms ; i++)
	{
		do
		{
			__asm("NOP");
		}while(j--);
		j = 2200; // 72 MHZ
	}
}
/*
direction
*/
static char s_dir_flag = DIR_STOP;
void drv8801_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_6 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;	 
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 	 

	GPIO_Init(GPIOE , &GPIO_InitStructure);	 
}

void drv8801_enable(void)
{
	DRV8801_SET_ENABLE_PIN_HIGH();
}
void drv8801_disable(void)
{
	DRV8801_SET_ENABLE_PIN_LOW();
}
/*
LOW LEVEL ACTIVEW
*/
void  drv8801_enter_sleep_mode(void)
{
	DRV8801_SET_SLEEP_PIN_LOW();
	
}

void drv8801_enter_reverse_mode(void)
{
	
	if(DIR_FORWARD ==s_dir_flag)
	{
		drv8801_enter_fast_stop();
		
	}	
	DRV8801_SET_SLEEP_PIN_HIGH();
	drv8801_enable();
	DRV8801_SET_PHASE_PIN_LOW();
	s_dir_flag = DIR_REVERSE;
	
}

void drv8801_enter_forward_mode(void)
{
	if(DIR_REVERSE ==s_dir_flag)
	{
		
		drv8801_enter_fast_stop();
	}
	DRV8801_SET_SLEEP_PIN_HIGH();
	drv8801_enable();
	DRV8801_SET_PHASE_PIN_HIGH();
	s_dir_flag = DIR_FORWARD;
}

void drv8801_enter_decay_mode(int decay_m)
{
	DRV8801_SET_SLEEP_PIN_HIGH();
	drv8801_disable();
	switch(decay_m)
	{
		case REVERSE_FAST_DECAY_MODE: // DIRCTION??	
		DRV8801_SET_PHASE_PIN_LOW();
		DRV8801_SET_MODE1_PIN_LOW();

		break;
		case FORWORD_FAST_DECAY_MODE: // DIRCTION??	
		DRV8801_SET_PHASE_PIN_HIGH();
		DRV8801_SET_MODE1_PIN_LOW();

		break;
		case LOW_SIDE_SLOW_DECAY_MODE:
		DRV8801_SET_MODE1_PIN_HIGH();
		DRV8801_SET_MODE2_PIN_LOW();

		break;
		case HIGH_SIDE_SLOW_DECAY_MODE:
		DRV8801_SET_MODE1_PIN_HIGH();
		DRV8801_SET_MODE2_PIN_HIGH();
		
		break;
		default:
		break;
		
	}
	s_dir_flag = DIR_STOP;
}

void drv8801_enter_fast_stop(void)
{
	if(DIR_REVERSE ==s_dir_flag)
	{
		drv8801_enter_decay_mode(REVERSE_FAST_DECAY_MODE);
		drv_8801_delay_ms(500);
	}
	if(DIR_FORWARD ==s_dir_flag)
	{
		drv8801_enter_decay_mode(FORWORD_FAST_DECAY_MODE);
		drv_8801_delay_ms(500);
	}
	s_dir_flag = DIR_STOP;
}

void drv8801_test(void)
{
	drv8801_gpio_config();
	drv8801_enter_reverse_mode();
	drv8801_enter_forward_mode();
	drv8801_enter_fast_stop();
	drv8801_enter_sleep_mode();
	
//while(1)
//{
//	DRV8801_SET_SLEEP_PIN_LOW(); 
//	DRV8801_SET_ENABLE_PIN_LOW(); 
//	DRV8801_SET_PHASE_PIN_LOW();
//	DRV8801_SET_MODE1_PIN_LOW(); 
//	DRV8801_SET_MODE2_PIN_LOW(); 
//	drv_8801_delay_ms(10);
//	DRV8801_SET_SLEEP_PIN_HIGH();
//	DRV8801_SET_ENABLE_PIN_HIGH();  
//	DRV8801_SET_PHASE_PIN_HIGH();
//	DRV8801_SET_MODE1_PIN_HIGH();
//	DRV8801_SET_MODE2_PIN_HIGH();
//	drv_8801_delay_ms(10);	
//}

}
