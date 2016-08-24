#ifndef __DELAY_H
#define __DELAY_H
 			   

static unsigned char  fac_us=0;//us延时倍乘数
static unsigned short fac_ms=0;//ms延时倍乘数

void DS_delay_init(unsigned char SYSCLK);
void DS_delay_mms(unsigned short nms);
void DS_delay_us(unsigned int Nus);
void DS_delay_ms(unsigned short time_cn);
#endif
