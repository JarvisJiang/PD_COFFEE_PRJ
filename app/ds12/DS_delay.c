/*********************************************************************************
 * �ļ���  ��
 * ����    ��         
 * ʵ��ƽ̨��
 * ��汾  ��ST3.0.0
 *
 * ����    ��
**********************************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "DS_delay.h"

void DS_delay_init(u8 SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//ѡ���ڲ�ʱ�� HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}								    
//��ʱNms
//ע��Nms�ķ�Χ
//Nms<=0xffffff*8/SYSCLK
//��72M������,Nms<=1864 
void DS_delay_nms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL=0x01 ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  	    
}   
//��ʱus								   
void DS_delay_us(u32 Nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=Nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	    
}

void  DS_delay_ms(uint16_t time_cn)
{
	uint32_t temp = time_cn*10000;
	while(temp--);	
}
