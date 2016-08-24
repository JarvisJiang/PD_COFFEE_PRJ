#ifndef __A4988_DRIVER_H
#define __A4988_DRIVER_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#define HIGH_H    1
#define LOW_L     0

#define STEP_ENABLE    1
#define STEP_DISABLE   0

#define A4988_MS3_L 		GPIO_ResetBits(GPIOE, GPIO_Pin_12)
#define A4988_MS3_H 		GPIO_SetBits(GPIOE, GPIO_Pin_12)

#define A4988_MS2_L 		GPIO_ResetBits(GPIOE, GPIO_Pin_13)
#define A4988_MS2_H 		GPIO_SetBits(GPIOE, GPIO_Pin_13)

#define A4988_MS1_L 		GPIO_ResetBits(GPIOE, GPIO_Pin_14)
#define A4988_MS1_H 		GPIO_SetBits(GPIOE, GPIO_Pin_14)

#define A4988_DIR_L			GPIO_ResetBits(GPIOE,GPIO_Pin_8)	
#define A4988_DIR_H			GPIO_SetBits(GPIOE,GPIO_Pin_8)	

#define A4988_STEP_L 		GPIO_ResetBits(GPIOE, GPIO_Pin_9)
#define A4988_STEP_H 		GPIO_SetBits(GPIOE, GPIO_Pin_9)

#define A4988_SLEEP_L 	GPIO_ResetBits(GPIOE, GPIO_Pin_10)
#define A4988_SLEEP_H 	GPIO_SetBits(GPIOE, GPIO_Pin_10)

#define A4988_RESET_L 	GPIO_ResetBits(GPIOE, GPIO_Pin_11)
#define A4988_RESET_H 	GPIO_SetBits(GPIOE, GPIO_Pin_11)

#define A4988_ENABLE_L	GPIO_ResetBits(GPIOE,GPIO_Pin_15)	
#define A4988_ENABLE_H	GPIO_SetBits(GPIOE,GPIO_Pin_15)	


extern uint8_t 	g_Enable_A4988;  // Disable the A4988 in timer 2
extern uint8_t 	g_Direct_A4988;  // control the a4988 direction
extern uint8_t 	g_Angle_A4988;   // control the a4988 turn angle
extern uint16_t 	g_Period_A4988;  // control the a4988 turn period
extern uint32_t 	g_Steps_A4988;   // control the a4988 steppers

extern void a4988_init(void);
extern void a4988_step_Controller(uint8_t dir, uint16_t period, uint32_t steps);
extern void a4988_stop(void);
extern void Timer_2_Motor_Config(uint16_t period);

#endif
