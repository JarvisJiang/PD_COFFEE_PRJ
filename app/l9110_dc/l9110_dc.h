#ifndef _L9910_DC_H__
#define _L9910_DC_H__
#include "stm32f10x.h"

#define REVERSE_FAST_DECAY_MODE   				0X10  //
#define FORWORD_FAST_DECAY_MODE   				0X20  //
#define LOW_SIDE_SLOW_DECAY_MODE   				0X30
#define HIGH_SIDE_SLOW_DECAY_MODE   			0X40
/*
PC10 ~ PC13
*/

#define L9910_SET_M1_A_HIGH()  GPIO_SetBits (GPIOC, GPIO_Pin_10)   
#define L9910_SET_M1_A_LOW()   GPIO_ResetBits (GPIOC, GPIO_Pin_10)

#define L9910_SET_M1_B_HIGH()  GPIO_SetBits (GPIOC, GPIO_Pin_11)   
#define L9910_SET_M1_B_LOW()   GPIO_ResetBits (GPIOC, GPIO_Pin_11)

#define L9910_SET_M2_A_HIGH()  GPIO_SetBits (GPIOC, GPIO_Pin_12)   
#define L9910_SET_M2_A_LOW()   GPIO_ResetBits (GPIOC, GPIO_Pin_12)

#define L9910_SET_M2_B_HIGH()  GPIO_SetBits (GPIOC, GPIO_Pin_13)   
#define L9910_SET_M2_B_LOW()   GPIO_ResetBits (GPIOC, GPIO_Pin_13)
/*function */
/*
LOW LEVEL ACTIVE
*/
void l9910_gpio_config(void);
void l9910_dc_motor1_forward(void);
void l9910_dc_motor1_reverse(void);
void l9910_dc_motor1_stop(void);

void l9910_dc_motor2_forward(void);
void l9910_dc_motor2_reverse(void);
void l9910_dc_motor2_stop(void);

extern void l9910_dc_test(void);


#endif







