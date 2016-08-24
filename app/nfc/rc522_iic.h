#ifndef __RC522_IIC__H__
#define	__RC522_IIC__H__
unsigned char IIC_R_RC522(unsigned char DZ);
void IIC_W_RC522(unsigned char DZ,unsigned char DATA);


void rc_522_IIC_Init(void);

void RC522_IIC_Send_Byte(unsigned char txd);
 
u8 RC522_IIC_Read_Byte(unsigned char ack);

#endif

