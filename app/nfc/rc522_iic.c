#include "stm32f10x.h"
#include "rc522_iic.h"
#include "rc522.h"
//////////////////////////////////////////////////////////////////////////////////	 
//��?3��D��??1??��?�㨺1��?��??��?-����??D��?����?2?��?��?����???����?o?��?��?
//ALIENTEK???��STM32?a�����?
//IIC?y?�� �䨲??	   
//?y��??-����@ALIENTEK
//??��???��3:www.openedv.com
//DT??��??��:2012/9/9
//��?��?��oV1.0
//��?����?����D��?�̨���?��????��
//Copyright(C) 1??Y��DD?������?����????��D?T1??? 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//3?��??��IIC
//IO��??������??

#if NFC_IIC

#define SLA_ADDR 0x50
static u16 OK=0;//?��2ao����y���䨬? ??1��o?��?��2��oD��?��
static u8 DAA;
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}



#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x4001100C 


#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40011008 

//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

/*

PB10  PB11




*/
//IO2������o����y	 
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //��?��?SDA 
static void delay_us(unsigned int nus)
{
	  unsigned int  i = 0; 
		while(nus--)
		{
			i = 10;
//			__asm("NOP");
//			__asm("NOP");
//			__asm("NOP");
			while(i--);
		}
}
void rc522_test_iic_gpio_delay(void)
{
	while(1)
	{
	PBout(10) = 1;
	PBout(11) = 0;
	delay_us(1);
	PBout(10) = 0;
	PBout(11) = 1;
	delay_us(1);		
	}

}
static void delay_ms(unsigned int nms)
{		
			unsigned int i = 0;
	    while(nms--)
			{
				i = 12000;
				while(i--);
			}
							    
}

//2������IIC?e��?D?o?
static void IIC_Start(void)
{
	SDA_OUT();     //sda??��?3?
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(40);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(40);
	IIC_SCL=0;//?����?I2C����??��?��?��?����?��?��?����?��y?Y 
}	  
//2������IIC����?1D?o?
static void IIC_Stop(void)
{
	SDA_OUT();//sda??��?3?
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(40);
	IIC_SCL=1; 
	IIC_SDA=1;//����?��I2C����???����?D?o?
	delay_us(40);							   	
}
//�̨���y��|��eD?o?��?����
//����???�̡�o1��??����?��|��e����㨹
//        0��??����?��|��e3��1|
static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����???a��?��?  
	IIC_SDA=1;delay_us(10);	   
	IIC_SCL=1;delay_us(10);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//����?����?3?0 	   
	return 0;  
} 
//2������ACK��|��e
static void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(20);
	IIC_SCL=1;
	delay_us(20);
	IIC_SCL=0;
}
//2?2������ACK��|��e		    
static void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(20);
	IIC_SCL=1;
	delay_us(20);
	IIC_SCL=0;
}					 				     
//IIC����?����???��??��
//����??�䨮?����D?T��|��e
//1��?��D��|��e
//0��??T��|��e	

static void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//��-�̨�����?��?a��?��y?Y��?��?
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(20);   //??TEA5767?a��y???������??��?��?D?��?
		IIC_SCL=1;
		delay_us(20); 
		IIC_SCL=0;	
		delay_us(20);
    }	 
} 	    
//?��1??��??����?ack=1������?����?��ACK��?ack=0��?����?��nACK   
static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����???a��?��?
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(20);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(10); 
    }					 
    if (!ack)
        IIC_NAck();//����?��nACK
    else
        IIC_Ack(); //����?��ACK   
    return receive;
}





void RC522_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//��-�̨�����?��?a��?��y?Y��?��?
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(20);   //??TEA5767?a��y???������??��?��?D?��?
		IIC_SCL=1;
		delay_us(20); 
		IIC_SCL=0;	
		delay_us(20);
    }	 
} 	    
//?��1??��??����?ack=1������?����?��ACK��?ack=0��?����?��nACK   
u8 RC522_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����???a��?��?
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(20);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(10); 
    }					 
    if (!ack)
        IIC_NAck();//����?��nACK
    else
        IIC_Ack(); //����?��ACK   
    return receive;
}





//?��RC522??��??��
//DZ:??��??�¦�??��
//����??��o?��3?��??��
static void RC522_WR_Reg(u8 RCsla,u8 addr,u8 val) 
{
	IIC_Start();  				 
	IIC_Send_Byte(RCsla);     	//����?��D��?��?t??��?	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   			//����?��??��??�¦�??��
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(val);     		//����?��?��					   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();						//2��������???����?1��??t 	   
}
//?��??��??��
//addr:??��??�¦�??��
//����???��:?����?��??��
static u8 RC522_RD_Reg(u8 RCsla,u8 addr) 		
{
	u8 temp=0;		 
	IIC_Start();  				 
	IIC_Send_Byte(RCsla);	//����?��D��?��?t??��?	 
	temp=IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   		//����?��??��??�¦�??��
	temp=IIC_Wait_Ack(); 	 										  		   
	IIC_Start();  	 	   		//??D????��
	IIC_Send_Byte(RCsla+1);	//����?��?��?��?t??��?	 
	temp=IIC_Wait_Ack();	   
    temp=IIC_Read_Byte(0);		//?����?��???��??��,2??��D??��?��,����?��NAK 	    	   
    IIC_Stop();					//2��������???����?1��??t 	    
	return temp;				//����???����?��??��
} 
u8 IIC_R_RC522(u8 DZ)
{						   
	u8 DAT=0;	
    DAT = RC522_RD_Reg(SLA_ADDR,  DZ);
	return DAT;          		//����??��?��?��?��y?Y
}

//1|    ?����oD��RC522??��??��
//2?��y?��?�¡�oDZ:??��??�¦�??��
//          DATA:D�䨨?��??��

void IIC_W_RC522(u8 DZ,u8 DATA)
{
    RC522_WR_Reg(SLA_ADDR,  DZ, DATA);
}

void rc_522_IIC_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;                    //
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;							// �����������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
//			RCC->APB2ENR|=1<<3;//��ʹ������IO PORTBʱ�� 							 
//			GPIOB->CRH&=0XFFF000FF;//PB1/11 �������
//			GPIOB->CRH|=0X00033300;	   
//			GPIOB->ODR|=3<<10;     //PB10,11 �����
}

 #endif










