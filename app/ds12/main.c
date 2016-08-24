/*********************************************************************************
 * �ļ���  ��main.c
 * ����    ��ADS1256ģ����Գ���         
 * ʵ��ƽ̨��
 * ��汾  ��ST3.0.0
 * ����    ��            
			 ADS_DRDY     PA2
			 ADS_RESET    PA4
			 ADS_CS       PA3
			 ADS_CLK      PA5
			 ADS_DIN	  PA7
			 ADS_DOUT	  PA6
 * ����    ��QQ:152033152  ������� http://sanfengdz.taobao.com/
           //ˮƽ���ޣ�������Щ�ط�д������׼����ӭ�Ὠ�顣
		   //�������ԭ������Ȩ���У�
**********************************************************************************/
#include "stm32f10x.h"
#include "usart1.h"
#include "delay.h"
#include "ads1256.h"

#define MEDIAN_LEN  5                     //��ֱ�˲����ܳ��ȣ�һ��ѡȡ����   
#define MEDIAN      2                     //��ֵ���˲������е�λ��
unsigned long   AD_buffer[MEDIAN_LEN];    //ad�ɼ����黺��
//unsigned long   MED_buffer[MEDIAN_LEN];   //��ֵ�˲����黺��
unsigned char   medleng = 0;          //���뻺������ݸ���

unsigned long medina_filter(unsigned long *MED_buffer);
/**********************************************************/
// ��������main
// ����  ��������
// ����  ����
// ���  ����
/*********************************************************/
int main(void)
{  
//	unsigned long results = 0;
//	unsigned char i = 0;
	SystemInit();	                      //��Ƶʱ�ӳ�ʼ��
	delay_init(72);	                    //��ʱʱ�ӳ�ʼ��
	USART1_Config();								    //���ڳ�ʼ��	  
	SPI_ADS1256_Init();								  //SPI��ʼ��
	ADS1256_GPIO_init();							  //�˿ڳ�ʼ��
	ADS1256_Init();
	
	while(1)
	{
		/*
			ADS_sum(a,b)���������ܣ�������ѹa-b
			��˵��ADS1256_MUXP_AIN��0~7������ͨ��0~7������� P��positive
				  ADS1256_MUXN_AIN��0~7������ͨ��0~7����ָ� N��negative
			
			������ֲ���ͨ��0��1��ADS_sum(ADS1256_MUXP_AIN0 | ADS1256_MUXN_AIN1);    ����ͨ��0-ͨ��1�ĵ�ѹֵ
				���˲���ͨ��0��   ADS_sum(ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);  ����ͨ��0�Եصĵ�ѹֵ
		*/
	  ADS_sum(ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);	//ͨ��0�Եصĵ�ѹֵ
		sendChar(0x0d);	sendChar(0x0a);				  //������ʾ����
		ADS_sum(ADS1256_MUXP_AIN1 | ADS1256_MUXN_AINCOM);	//ͨ��1�Եصĵ�ѹֵ
		sendChar(0x0d);	sendChar(0x0a);	
		ADS_sum(ADS1256_MUXP_AIN2 | ADS1256_MUXN_AINCOM);	//ͨ��2�Եصĵ�ѹֵ
		sendChar(0x0d);	sendChar(0x0a);	
		ADS_sum(ADS1256_MUXP_AIN3 | ADS1256_MUXN_AINCOM);	//ͨ��3�Եصĵ�ѹֵ
		sendChar(0x0d);	sendChar(0x0a);	
		ADS_sum(ADS1256_MUXP_AIN4 | ADS1256_MUXN_AINCOM);	//ͨ��4�Եصĵ�ѹֵ
		sendChar(0x0d);	sendChar(0x0a);	
		ADS_sum(ADS1256_MUXP_AIN5 | ADS1256_MUXN_AINCOM);	//ͨ��5�Եصĵ�ѹֵ
		sendChar(0x0d);	sendChar(0x0a);	
  	ADS_sum(ADS1256_MUXP_AIN6 | ADS1256_MUXN_AINCOM);	//ͨ��6�Եصĵ�ѹֵ
		sendChar(0x0d);	sendChar(0x0a);	
		ADS_sum(ADS1256_MUXP_AIN7 | ADS1256_MUXN_AINCOM);	//ͨ��7�Եصĵ�ѹֵ	
		
		sendChar(0x0d);				  //������ʾ����
		sendChar(0x0a);				  //������ʾ����

		delay_ms(500);
	}
}

/**********************************************************/
// ����������ֵ�˲�����
// ����  ����ȡǰ9�βɼ������ݣ�ȥ����3����ȥ����3����Ȼ���м��
// ����  ��3��������ƽ��ֵ�����㷨�ɾ����ܵ��˵��������ݣ�����Ӱ��ɼ��ٶȡ�
// ����  ��9�����ݵ�����
// ���  ���м�3�����ݵ�ƽ��ֵ
/*********************************************************/
unsigned long medina_filter(unsigned long *MED_buffer)  //xad - ADCת��ֵ   
{   
	
	unsigned char i,j;
	unsigned long xd;
	u32 xxd;
	
	for(i = 0; i < MEDIAN_LEN; i ++)     
	{   
		for(j = 0; j < MEDIAN_LEN - i; j ++)
		{
			
			if( MED_buffer[i] > MED_buffer[i + 1]) // ��ѯ���ĵ�ǰԪ��>ADֵ,�򽻻����ǵ�ֵ   
			{ xd = MED_buffer[i]; MED_buffer[i] = MED_buffer[i + 1]; MED_buffer[i + 1] = xd;} 
		}			
	} 
	xxd = MED_buffer[MEDIAN - 1] + MED_buffer[MEDIAN] + MED_buffer[MEDIAN + 1];
	xd = xxd/3;
	return xd; //��ֵ     
}

/******************* (C) COPYRIGHT 2014 ������ӿ��������� *****END OF FILE****/
