#ifndef _GY960_H_
#define _GY960_H_

#include "stm32f10x.h"
#define SAW 0x00
#define SAR 0x01

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 


#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 

//IO口操作,只对单一的IO口!
//确保n的值小于16!

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 


//u8 SMbus_GetCrc8(u8 previous,u8 data);
u16 Gy906_Read(u8 addr);
void Gy906_Write(u8 addr,u16 dat);
float Gy906_ReadTemp(void);
u8 PEC_Calculation(u8 pec[]);

//IO·??òéè??
#define SDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
#define SDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}

//IO2ù×÷oˉêy	 
#define GY906_IIC__SCL    PCout(12) //SCL  PC12
#define GY906_IIC__SDA    PCout(11) //SDA	 PC11
#define READ_SDA   PCin(11)  //ê?è?SDA 
void Gy906_Init(void);
void GY906_IIC_GPIO_Init(void);
void GY906_IIC__Start(void);
void GY906_IIC__Stop(void);
unsigned char Wait_Ack(void);
void Ack(void);
void NAck(void);
void GY906_IIC__Send(u8 dat);
u8 GY906_IIC__Read(u8 ack);
void Test_GY906(void);
#endif
