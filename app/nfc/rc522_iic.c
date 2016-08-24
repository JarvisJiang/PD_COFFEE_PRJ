#include "stm32f10x.h"
#include "rc522_iic.h"
#include "rc522.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±?3ìDò??1??§?°ê1ó?￡??′?-×÷??Dí?é￡?2?μ?ó?óú???üè?o?ó?í?
//ALIENTEK???￠STM32?a·￠°?
//IIC?y?ˉ ′ú??	   
//?yμ??-×ó@ALIENTEK
//??ê???ì3:www.openedv.com
//DT??è??ú:2012/9/9
//°?±?￡oV1.0
//°?è¨?ùóD￡?μá°?±????￡
//Copyright(C) 1??YêDD?òíμ?×ó????óD?T1??? 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//3?ê??ˉIIC
//IO·??òéè??

#if NFC_IIC

#define SLA_ADDR 0x50
static u16 OK=0;//?ì2aoˉêy×′ì? ??1￡o?á?￠2￡oD′?￡
static u8 DAA;
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}



#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x4001100C 


#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40011008 

//IO口操作,只对单一的IO口!
//确保n的值小于16!

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

/*

PB10  PB11




*/
//IO2ù×÷oˉêy	 
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //ê?è?SDA 
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

//2úéúIIC?eê?D?o?
static void IIC_Start(void)
{
	SDA_OUT();     //sda??ê?3?
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(40);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(40);
	IIC_SCL=0;//?ˉ×?I2C×ü??￡?×?±?·￠?í?ò?óê?êy?Y 
}	  
//2úéúIICí￡?1D?o?
static void IIC_Stop(void)
{
	SDA_OUT();//sda??ê?3?
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(40);
	IIC_SCL=1; 
	IIC_SDA=1;//·￠?íI2C×ü???áê?D?o?
	delay_us(40);							   	
}
//μè′yó|′eD?o?μ?à′
//·μ???μ￡o1￡??óê?ó|′eê§°ü
//        0￡??óê?ó|′e3é1|
static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDAéè???aê?è?  
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
	IIC_SCL=0;//ê±?óê?3?0 	   
	return 0;  
} 
//2úéúACKó|′e
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
//2?2úéúACKó|′e		    
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
//IIC·￠?íò???×??ú
//·μ??′ó?úóD?Tó|′e
//1￡?óDó|′e
//0￡??Tó|′e	

static void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//à-μíê±?ó?aê?êy?Y′?ê?
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(20);   //??TEA5767?aèy???óê±??ê?±?D?μ?
		IIC_SCL=1;
		delay_us(20); 
		IIC_SCL=0;	
		delay_us(20);
    }	 
} 	    
//?á1??×??ú￡?ack=1ê±￡?·￠?íACK￡?ack=0￡?·￠?ínACK   
static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDAéè???aê?è?
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
        IIC_NAck();//·￠?ínACK
    else
        IIC_Ack(); //·￠?íACK   
    return receive;
}





void RC522_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//à-μíê±?ó?aê?êy?Y′?ê?
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(20);   //??TEA5767?aèy???óê±??ê?±?D?μ?
		IIC_SCL=1;
		delay_us(20); 
		IIC_SCL=0;	
		delay_us(20);
    }	 
} 	    
//?á1??×??ú￡?ack=1ê±￡?·￠?íACK￡?ack=0￡?·￠?ínACK   
u8 RC522_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDAéè???aê?è?
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
        IIC_NAck();//·￠?ínACK
    else
        IIC_Ack(); //·￠?íACK   
    return receive;
}





//?áRC522??′??÷
//DZ:??′??÷μ??·
//·μ??￡o?á3?μ??μ
static void RC522_WR_Reg(u8 RCsla,u8 addr,u8 val) 
{
	IIC_Start();  				 
	IIC_Send_Byte(RCsla);     	//·￠?íD′?÷?t??á?	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   			//·￠?í??′??÷μ??·
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(val);     		//·￠?í?μ					   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();						//2úéúò???í￡?1ì??t 	   
}
//?á??′??÷
//addr:??′??÷μ??·
//·μ???μ:?áμ?μ??μ
static u8 RC522_RD_Reg(u8 RCsla,u8 addr) 		
{
	u8 temp=0;		 
	IIC_Start();  				 
	IIC_Send_Byte(RCsla);	//·￠?íD′?÷?t??á?	 
	temp=IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   		//·￠?í??′??÷μ??·
	temp=IIC_Wait_Ack(); 	 										  		   
	IIC_Start();  	 	   		//??D????ˉ
	IIC_Send_Byte(RCsla+1);	//·￠?í?á?÷?t??á?	 
	temp=IIC_Wait_Ack();	   
    temp=IIC_Read_Byte(0);		//?áè?ò???×??ú,2??ìD??ù?á,·￠?íNAK 	    	   
    IIC_Stop();					//2úéúò???í￡?1ì??t 	    
	return temp;				//·μ???áμ?μ??μ
} 
u8 IIC_R_RC522(u8 DZ)
{						   
	u8 DAT=0;	
    DAT = RC522_RD_Reg(SLA_ADDR,  DZ);
	return DAT;          		//·μ??ê?μ?μ?êy?Y
}

//1|    ?ü￡oD′RC522??′??÷
//2?êy?μ?÷￡oDZ:??′??÷μ??·
//          DATA:D′è?μ??μ

void IIC_W_RC522(u8 DZ,u8 DATA)
{
    RC522_WR_Reg(SLA_ADDR,  DZ, DATA);
}

void rc_522_IIC_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;                    //
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;							// 复用推免输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
//			RCC->APB2ENR|=1<<3;//先使能外设IO PORTB时钟 							 
//			GPIOB->CRH&=0XFFF000FF;//PB1/11 推挽输出
//			GPIOB->CRH|=0X00033300;	   
//			GPIOB->ODR|=3<<10;     //PB10,11 输出高
}

 #endif










