#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "platform_config.h" 
#include "platform_config.h" 
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stdarg.h"
#include "a4988_driver.h"

extern void Delay_Ms(unsigned int ms);

uint8_t 	g_Enable_A4988 = DISABLE;  // Disable the A4988 in timer 2
uint8_t 	g_Direct_A4988;  // control the a4988 direction
uint8_t 	g_Angle_A4988;   // control the a4988 turn angle
uint16_t 	g_Period_A4988;  // control the a4988 turn period
uint32_t 	g_Steps_A4988;   // control the a4988 steppers

void a4988_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE); 	
	
/**
 *  PE8 -> DIR , PE9 -> STEP , PE10 -> SLEEP
 *  PE11 -> RESET	, PE12 -> MS3  , PE13 -> MS2
 *  PE14 -> MS1, PE15 -> ENABLE
 */					 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	A4988_SLEEP_H;   // WakeUp the A4988
	A4988_ENABLE_H;  // Disable the A4988
	
  /* Set for 1.8/4 degrees */
	GPIO_ResetBits(GPIOE, GPIO_Pin_14);     // MS1 - L	 
	GPIO_SetBits(GPIOE, GPIO_Pin_13);    // MS2 - H
	GPIO_ResetBits(GPIOE, GPIO_Pin_12);     // MS3 - L	 
	
	A4988_DIR_H;    // Foreward  // reversed
	A4988_STEP_L;   // Defaul is low step
	A4988_RESET_L;  // RESET A4988 LOW
	Delay_Ms(100);
	A4988_RESET_H; // RESET A4988 HIGHT
		
}

	float aiy;
	float cycle;
	uint16_t turn_per_min;
	uint16_t timer2period;


uint16_t motor_turn_frequency(uint16_t roll)
{
	

	aiy = (float)roll/60;    // round turn per minute
	turn_per_min = (uint16_t) 200 * 4 * aiy;
	cycle = (float)1000 / turn_per_min;   // calculate the ms
	timer2period = (uint16_t)(72 * cycle) - 1;
	
	Timer_2_Motor_Config(timer2period);   // config the timer 2
		
	return 0;
}


void a4988_step_Controller(uint8_t dir, uint16_t roll, uint32_t steps)
{
	
	A4988_ENABLE_L;  // Enable the A4988
	
	if(g_Direct_A4988)
	{
		A4988_DIR_H;    // Foreward
	}
	else
	{
		A4988_DIR_L;    // reversed
	}
	

	g_Direct_A4988 = dir;
	g_Period_A4988 = roll;
	g_Steps_A4988 =  steps;	
	g_Enable_A4988 = STEP_ENABLE;
	motor_turn_frequency(roll);

}	


void a4988_stop(void)
{
	A4988_ENABLE_H;  // Disable the A4988
	g_Direct_A4988 = HIGH_H;
	A4988_DIR_H;    // Foreward
	g_Enable_A4988 = STEP_DISABLE;
}
