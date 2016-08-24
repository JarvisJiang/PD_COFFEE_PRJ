#include "l9110_dc.h"

void l9910_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;	 
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 	 

	GPIO_Init(GPIOC , &GPIO_InitStructure);	
}
void l9910_dc_motor1_forward(void)
{
	L9910_SET_M1_A_HIGH();
	L9910_SET_M1_B_LOW();
}
void l9910_dc_motor1_reverse(void)
{
	L9910_SET_M1_A_LOW();
	L9910_SET_M1_B_HIGH();
}
void l9910_dc_motor1_stop(void)
{
	L9910_SET_M1_A_LOW();
	L9910_SET_M1_B_LOW();
}

void l9910_dc_motor2_forward(void)
{
	L9910_SET_M2_A_HIGH();
	L9910_SET_M2_B_LOW();
}
void l9910_dc_motor2_reverse(void)
{
	L9910_SET_M2_A_LOW();
	L9910_SET_M2_B_HIGH();
}
void l9910_dc_motor2_stop(void)
{
	L9910_SET_M2_A_LOW();
	L9910_SET_M2_B_LOW();
}
void l9910_dc_test(void)
{
	l9910_gpio_config();
	l9910_dc_motor1_forward();
	l9910_dc_motor1_reverse();
	l9910_dc_motor1_stop();
	l9910_dc_motor2_forward();
	l9910_dc_motor2_reverse();
	l9910_dc_motor2_stop();

}



