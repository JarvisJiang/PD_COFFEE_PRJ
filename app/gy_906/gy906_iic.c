#include "gy906.h"
#include "misc.h"



//延时nus
//nus为要延时的us数.		    								   
static void delay_us(unsigned int nus)
{		
		int i;
		int j = 20;
		for(i = 0 ;  i < nus ; i++)
		{
		do
		{
			__asm("NOP");
		}while(j--);
		j = 20; // 72 MHZ
		}
							    
}

void GY906_IIC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//gy906_delay_init();
	
	//RCC->APB2ENR|=1<<4;//?èê1?üíaéèIO PORTCê±?ó 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//?a??ê?3?
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
 
	GY906_IIC__SCL=1;
	GY906_IIC__SDA=1;
}

void GY906_IIC__Start(void)
{
	
	SDA_OUT();
	GY906_IIC__SDA=1;
	delay_us(1);
	GY906_IIC__SCL=1;
	delay_us(5);
	GY906_IIC__SDA=0;
	delay_us(5);
	GY906_IIC__SCL=0;
	delay_us(2);

}

void GY906_IIC__Stop(void)
{
	SDA_OUT();
	GY906_IIC__SDA=0;
	GY906_IIC__SCL=0;
	delay_us(1);
	GY906_IIC__SCL=1;
	delay_us(5);
//	GY906_IIC__SCL=1;
	GY906_IIC__SDA=1;
	delay_us(10);
	GY906_IIC__SCL=0;
	delay_us(2);
	

}


//??óDó|′e?íê?1￡?ó|′e?íê?0
unsigned char Wait_Ack(void)
{
	unsigned char times=0;
	SDA_IN();
	GY906_IIC__SCL=0;delay_us(1);
	GY906_IIC__SDA=1;delay_us(1);
//	delay_us(2);
	GY906_IIC__SCL=1;
	delay_us(2);
	while(READ_SDA)
	{
		times++;
		if(times>20)
		{
			GY906_IIC__Stop();
			return 1;
		}
	}
	delay_us(1);
	GY906_IIC__SCL=0;
	return 0;
}

void Ack(void)
{
	SDA_OUT();
	GY906_IIC__SCL=0;
	GY906_IIC__SDA=0;
	delay_us(2);
	GY906_IIC__SCL=1;
	delay_us(5);
	GY906_IIC__SCL=0;
	delay_us(1);
}

void NAck(void)
{
	SDA_OUT();
	GY906_IIC__SCL=0;
	GY906_IIC__SDA=1;
	delay_us(5);
	GY906_IIC__SCL=1;
	delay_us(5);
	GY906_IIC__SCL=0;
	delay_us(1);
}

void GY906_IIC__Send(unsigned char dat)
{
	unsigned char i;
	SDA_OUT();
	GY906_IIC__SCL=0;
	for(i=0;i<8;i++)
	{
		GY906_IIC__SDA=(dat&0x80)>>7;
		dat<<=1;
		delay_us(5);
		GY906_IIC__SCL=1;
		delay_us(5);
		GY906_IIC__SCL=0;
		delay_us(5);
	}
}

unsigned char GY906_IIC__Read(unsigned char ack)
{
	unsigned char receive=0;
	unsigned char k;
	SDA_IN();
	for(k=0;k<8;k++)
	{
		GY906_IIC__SCL=0;
		delay_us(5);
		receive<<=1;
		GY906_IIC__SCL=1;
		delay_us(2);
		if(READ_SDA)
			receive++;
		delay_us(5);
		GY906_IIC__SCL=0;
//		delay_us(2);
	}
	if(!ack)
		NAck();//ê?0 2?ó|′e
	else
		Ack();//ê?1 ó|′e
	return receive;
}
