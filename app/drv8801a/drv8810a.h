#ifndef _DRV8810A_H__
#define _DRV8810A_H__
#include "stm32f10x.h"

#define REVERSE_FAST_DECAY_MODE   				0X10  //
#define FORWORD_FAST_DECAY_MODE   				0X20  //
#define LOW_SIDE_SLOW_DECAY_MODE   				0X30
#define HIGH_SIDE_SLOW_DECAY_MODE   			0X40


#define DRV8801_SET_SLEEP_PIN_HIGH()  GPIO_SetBits (GPIOE, GPIO_Pin_5)
#define DRV8801_SET_SLEEP_PIN_LOW()   GPIO_ResetBits (GPIOE, GPIO_Pin_5)

#define DRV8801_SET_ENABLE_PIN_HIGH()  GPIO_SetBits (GPIOE, GPIO_Pin_6)
#define DRV8801_SET_ENABLE_PIN_LOW()   GPIO_ResetBits (GPIOE, GPIO_Pin_6)


#define DRV8801_SET_PHASE_PIN_HIGH()  GPIO_SetBits (GPIOE, GPIO_Pin_2)
#define DRV8801_SET_PHASE_PIN_LOW()   GPIO_ResetBits (GPIOE, GPIO_Pin_2)

#define DRV8801_SET_MODE1_PIN_HIGH()  GPIO_SetBits (GPIOE, GPIO_Pin_3)
#define DRV8801_SET_MODE1_PIN_LOW()   GPIO_ResetBits (GPIOE, GPIO_Pin_3)

#define DRV8801_SET_MODE2_PIN_HIGH()  GPIO_SetBits (GPIOE, GPIO_Pin_4)
#define DRV8801_SET_MODE2_PIN_LOW()   GPIO_ResetBits (GPIOE, GPIO_Pin_4)
/*function */
/*
LOW LEVEL ACTIVE
*/
void drv8801_enable(void);
void drv8801_disable(void);
void drv8801_enter_sleep_mode(void);
void drv8801_enter_reverse_mode(void);
void drv8801_enter_forward_mode(void);
void drv8801_enter_decay_mode(int decay_m);
void drv8801_gpio_config(void);
void drv8801_test(void);
void drv8801_enter_fast_stop(void);

#endif







