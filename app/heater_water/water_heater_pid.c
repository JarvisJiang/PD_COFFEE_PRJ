
#include "string.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>
#include <math.h>
#include "water_heater.h"
#include "stm32f10x.h"
#include "misc.h"
#include "water_heater.h"
#include "cc2541.h"
#include "stdio.h"
#include "stdlib.h"
#define PRINTF(fmt,...)  
//USART_OUT(USART1,fmt,##__VA_ARGS__)


/************************************************
PID����
*************************************************/ 
/*************PID**********************************/

struct PID_WATER_HEATER spid; // PID Control Structure



//unsigned char key1,i,k;
static unsigned int g_cur_temperature;
// unsigned int temp , last_temp;
//unsigned char shu[4]={0,0,0,0}, key_count1;
//unsigned char counter;
 int g_set_temper ,g_temp_offset = 40;  // �� �� ��  ��
char RI_Buffer[64];
//unsigned char RI_SD_flag ; 		//uart receive  buf  flag
				
//char  tp_flag , key_dot_flag;


//unsigned char   RI_Count , 
//				RI_Buffer[50] ;

//ר�ҿ��Ʊ�  ����					   	  1  2  3  4  5  6  7  8  9	  ���� 14.7�� �� ��
static const unsigned char  keep_warm[101] = {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,// 0 ~10 ��
											   8, 9 , 20, 32, 33, 34, 35, 36, 37, 38, // 11~20 ��
											  39, 40, 41, 42, 43, 45, 46, 48, 50, 55, // 31~30 ��
											  59, 60, 61, 62, 63, 64, 65, 66, 67, 70, // 31~40 ��
											  70, 70, 70, 70, 70, 70, 71, 72, 74, 80, // 41~50 ��
											  77, 78, 78, 79, 80, 81, 81, 82, 82, 83, // 51~60 ��
											  83, 84, 85, 86, 87, 88, 89, 90, 91, 92, // 61~70 ��
											  90, 90, 92, 94, 96, 98,100,101,104,108, // 71~80 ��
											 108,109,110,111,112,112,113,114,118,120, // 81~90 ��
											 101,102,103,104,105,106,107,108,109,110, // 91~100 ��
								};
////ר�ҿ��Ʊ�  ��С����				   1  2  3  4  5  6  7  8  9	  ���� 14.7�� �� ��
//static const unsigned char  pid_off[101]  =	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,// 0 ~10 ��
//											   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 11~20 ��
//											   0, 0, 0, 0, 0, 2, 3, 3, 4, 4, // 21~30 ��
//											   5, 5, 5, 6, 6, 6, 6, 6, 6, 6, // 31~40 ��
//											   6, 6, 6, 6, 6, 6, 6, 6, 6, 6, // 41~50 ��
//											   6, 6, 6, 7, 7, 7, 7, 7, 7, 8, // 51~60 ��
//											   8, 9, 9, 9, 9, 9, 9, 9, 9, 9, // 61~70 ��
//											   9, 9, 9, 9, 9,10,10,10,10,10, // 71~80 ��
//											  10,11,11,11,12,12,13,13,13,13, // 81~90 ��
//											  14,14,14,15,15,15,15,15,15,15, // 91~100 ��
//									
//									};


static void DelayMs(int ms)
{
	int i;
	int j = 2200;
	for(i = 0 ;  i < ms ; i++)
	{
		do
		{
			__asm("NOP");
		}while(j--);
		j = 2200; // 72 MHZ
	}
}									
									
									
/*********************************PID����**********************************************/
              
static void water_heater_PIDInit (struct PID_WATER_HEATER *pp) 
{ 
 	 memset ( pp,0,sizeof(struct PID_WATER_HEATER)); 
} 


/**
  * @brief  ��ʼ��ESP8266�õ��� USART
  * @param  ��
  * @retval ��
  */
#if 0
static void PID_ADJUST_USART_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_10 | GPIO_Pin_11;//TX  RX
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;	 
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 	 

	GPIO_Init(GPIOB , &GPIO_InitStructure);	 
  
	/* config USART clock */
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART3, ENABLE );
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = 0;
	USART_InitStructure.USART_StopBits = 0;
	USART_InitStructure.USART_Parity = 0 ;
	USART_InitStructure.USART_HardwareFlowControl = 0;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	
	
	/* �ж����� */
	USART_ITConfig ( USART3, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж� 
	//USART_ITConfig ( USART3, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 	
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);

	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_2 );

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART3, ENABLE);
	
	
}
#endif
/************************************************
x10 ������
*************************************************/
static unsigned int water_heater_PIDCalc( struct PID_WATER_HEATER *pp, unsigned int NextPoint ) 
{ 
	  unsigned int dError,Error,pError; 
	  //���������㹫ʽ��
	  //Pdt=Kp*[E(t)-E(t-1)]+Ki*E(t)+Kd*[E(t)-2*E(t-1)+E(t-2)]
	  Error = g_set_temper - NextPoint;       // ƫ��E(t) 
	  pError=Error-pp->LastError;	 //E(t)-E(t-1)
	  dError=Error-2*pp->LastError+pp->PrevError; //E(t)-2*E(t-1)+E(t-2) 
	  pp->PrevError = pp->LastError; 
	  pp->LastError = Error; 
	  return ( 
	            pp->Proportion * pError        //���� 
	            + pp->Integral *Error  //������ 
	            + pp->Derivative * dError	  // ΢����
				);  
} 

/************************************************
				PID������ʼ��
*************************************************/
static void PIDBEGIN() 
{ 
	  water_heater_PIDInit(&spid); // Initialize Structure 
	  spid.Proportion = 10; // Set PID Coefficients 10
	  spid.Integral = 5; 	//5
	  spid.Derivative = 2; 	// 4
}
/*********************************PID����OVER******************************************/


/************************************************
			 ʵʱ�¶ȶ�ȡ����
��DS18b20�ж�ȡʵʱ�¶�
����ֵ�Ŵ�10������PID����
*************************************************/
static int ReadTemperature(void)
{

	static unsigned int temperture=0;
	temperture++;
	return temperture;			 
}







void send_char_com(unsigned char  ch)
{
	USART_SendData(UART4,  ch);
	while( USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET );
}
/************************************************************
	�򴮿�uart1����һ���ַ�����strlenΪ���ַ�������
*************************************************************/
static void send_string_com(char *str)
{
    unsigned int k=0;
	
	do
    {
        USART_SendData(UART4,*(str + k));
        k++;
				while( USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET );
    } while(k < strlen(str));
}
/*

*/
static void PWM0_set(unsigned  char duty_cycle)
{
	
	unsigned char  real = 255 - duty_cycle;
	float f_temp;
  if(0==real)
	{
		real = 1;
	}
	f_temp = (float)(real)/2.55;
  real   = ((unsigned char)f_temp);
	config_heat_up_pwm(real);
}

/*********************************************************** 
             PID�¶ȿ�����������
***********************************************************/ 
void compare_temper() 		//PID�¶ȿ����������
{ 
  unsigned char i; 
  const unsigned char  zhi[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  static unsigned  int rout; // PID Response (Output) 
  static unsigned int rin; // PID Feedback (Input)
  static int temp , last_temp;
  static int last_set_temper, TO;
	int dir;
  static unsigned char g_pwm_value,last_PWM;
	PRINTF("COMPARE_______________\n");
  if(g_set_temper > g_cur_temperature) 
  {
		PRINTF("g_set_temper = %d\n",g_set_temper);
		PRINTF("g_cur_temperature = %d\n",g_cur_temperature);
		dir = g_set_temper-g_cur_temperature;
		PRINTF("dir = %d\n",dir);
    if((dir) > 150)//�������Ŀ���¶�����ʵʱ�¶Ȳ����8�ȣ����Ŵ�10������80
    {
    
	   PWM0_set(5);  //PWM ����ߵ�ƽռ��245/255����󡣼�ȫ�ټ���

	   g_pwm_value = 250 ; 
	   temp = ReadTemperature();
	   
    }
	else 	//pid���Ʒ�Χ��
    {
    PRINTF("IN PID__________________ \n ");
	  for(i=0;i<20;i++) //8�ȷ�Χ��PID�������ƣ�10������ ��PID����ʽ��T=10   change i < 20
	  { 
	      temp = ReadTemperature();//PID����ʵʱ�¶Ȳ���ֵ
			   PRINTF("temp = %d \n", temp);
		  rin = temp ;
	    rout = water_heater_PIDCalc ( &spid,rin ); // PID�������
			PRINTF("rout = %d\n",rout);
		  DelayMs(5); 
	  }//PID���������Χ��0-255�����pwmȡֵ��Χ 	
		 
		 if(rout >=240) rout=240;	//����PWM����ֵ10-240 ��ֹpwm����ʧ����stc��Ƭ�����ԡ�
		 if(rout<= 3)  rout=3;

		 g_pwm_value = rout ;//+ stay_warm[(unsigned char)g_set_temper/10];
	 
	 	 //PWM = stay_warm[(unsigned char)g_set_temper/10];
	 		
		if( g_set_temper- temp < 80) 
		{	
				
		   g_pwm_value = rout + g_temp_offset- 40  + keep_warm[g_set_temper/10]; 
			 PRINTF("  small 8 OOOOg_pwm_value = %d\n",g_pwm_value);
		}  //keep_warm[(unsigned char)g_set_temper/10];
		if(g_pwm_value >=240) g_pwm_value = 240;
	    //PWM = rout + stay_warm[(unsigned char)g_set_temper/10];
	 PWM0_set(255-g_pwm_value);	  
    } 
  }
   
  else if(g_set_temper < g_cur_temperature)      //Ŀ���¶�С��ʵʱ�¶�pwm����͵�ƽ���ռ�ձȣ��رռ��ȡ�
  {
 
     if((g_cur_temperature - g_set_temper) <= 50)
	 {	 //Ŀ���¶�С��ʵʱ�¶� ��5��֮��
 
		 PRINTF("SMALL 5 OOOOOOO\n");
	   	g_pwm_value =  (g_set_temper/10 + g_temp_offset- 60 +10*(3-g_cur_temperature+g_set_temper)) ;//(unsigned char)(
		  PRINTF("g_pwm_value = %d\n",(255 - g_pwm_value));
		  PWM0_set(255 - g_pwm_value); 			     //PWM 5 �¶ȱ���
      
		 temp = ReadTemperature();
		 PRINTF("SMALL 5 OOOOOOO\n");
	 }
	 else if(g_cur_temperature - g_set_temper > 5)
	 {	 // Ŀ���¶�С��ʵʱ�¶� ��0.5��֮��

	     PWM0_set(255); 
		 temp = ReadTemperature();
	 }
  }
//  else if( (g_set_temper - temper)==0 )
//  {
//  		Frez = 0;
//		BELL = 0;
//		Frez_LED_OFF;
//		Heat_LED_OFF;
//		PID_LED_ON;
//  }
  // 
	if((last_temp != temp)|(last_PWM != g_pwm_value)|(last_set_temper != g_set_temper))
	
	{
		last_temp = temp;
		last_PWM  = g_pwm_value;
	 PRINTF("send g_set_temper = %d\n",g_set_temper);

	 send_string_com("ST "); //��������������� 
	 send_char_com(zhi[g_set_temper/1000]); //�趨�¶�ֵ
	 send_char_com(zhi[g_set_temper/100]);
	 send_char_com(zhi[g_set_temper%100/10]);
	 send_char_com(zhi[g_set_temper%10]);	  //С�����һλ
     send_string_com("  T "); //��������������� 
	 send_char_com(zhi[temp/1000]); //ʵʱ�¶�ֵ
	 send_char_com(zhi[temp/100]);
	 send_char_com(zhi[temp%100/10]);
	 send_char_com(zhi[temp%10]);	  //С�����һλ
	 send_string_com("  PWM "); //���������������
	 PRINTF("PWM = %d\n",g_pwm_value);
	 send_char_com(zhi[g_pwm_value/100]); //PID�������ֵ
	 send_char_com(zhi[g_pwm_value%100/10]);
	 send_char_com(zhi[g_pwm_value%10]);
	 send_string_com("/255") ;
	 send_string_com(" P"); //���������������
	 send_char_com(zhi[spid.Proportion/100]); //PID�������ֵ
	 send_char_com(zhi[spid.Proportion%100/10]);
	 send_char_com(zhi[spid.Proportion%10]);
	 send_string_com(" I"); //���������������
	 send_char_com(zhi[spid.Integral/100]); //PID�������ֵ
	 send_char_com(zhi[spid.Integral%100/10]);
	 send_char_com(zhi[spid.Integral%10]);
	 send_string_com(" D"); //���������������
	 send_char_com(zhi[spid.Derivative/100]); //PID�������ֵ
	 send_char_com(zhi[spid.Derivative%100/10]);
	 send_char_com(zhi[spid.Derivative%10]);
	 TO =g_temp_offset- 40  + keep_warm[g_set_temper/10];
	 send_string_com(" TO"); //���������������
	 send_char_com(zhi[TO/100]); //PID�������ֵ
	 send_char_com(zhi[TO%100/10]);
	 send_char_com(zhi[TO%10]);
	 send_char_com(0x0d);//����س������м�������ַ�
	 send_char_com(0x0a);//	
	 } 
	else
	{
		  last_temp = temp ;
		  last_PWM  = g_pwm_value ;		  
	}

	if(g_set_temper > last_set_temper)
	{
	    last_set_temper = g_set_temper ;	 
		if(g_set_temper-last_set_temper >= 100)//��������̾������ 
		{ 
			 spid.Integral = 4;
		} 
		else
		{
		  spid.Integral= 5;
		}
	}
	else
	{
		last_set_temper = g_set_temper ;
	}

	if(g_set_temper-g_cur_temperature > 0)
	{
		if( g_set_temper-g_cur_temperature < 10 )   //��������ʱ���趨
		{	
			if(g_set_temper-g_cur_temperature > 2)
			{
				spid.Integral = (g_set_temper-g_cur_temperature)/2+1 ; 
			}
			else
			{
				spid.Integral = 1 ;
			}

			if(temp < last_temp)
			{
		  		spid.Integral += 1;
			}
		}
		else{
		   if(temp < last_temp)
		   {
			  	spid.Proportion += 3;
				if(temp < last_temp)
				{
			  		spid.Integral += 2;
				}
			}
			else
			{
				spid.Proportion = 9 ;
			}
		}

	}	 
}
extern  void CC2541_Init(void);
/*************************************************
                  ������
**************************************************/
void water_heater_test_pid(void)
{ 
//		char  pid_off_flag ;
  // unsigned char Set_Tempera[7];
	 int key_up, key_down;
	 CC2541_Init();
	 PRINTF("water heater pid test!\n");
	 DelayMs(500);
	 PRINTF("TEST START ... \n");
	 PIDBEGIN();
	// PID_ADJUST_USART_Config();
	
//	 USART_SendData(UART4,0xa5);
//	 USART_SendData(UART4,0x5a);
//	 USART_SendData(UART4,0x45);
//	 USART_SendData(UART4,0x54);
	 PWM0_set(250);		//��ʼ��PWM��·�����ߵ͵�ƽռ�ձȣ���Ҫ����
//   Set_Tempera[0] = 1; 
	 g_set_temper = 400;
//	while(1)
//	 {
//		 PRINTF("%x\n",0xa5);
//		 DelayMs(10);
//	 }
	 while(1)
	 {  
		if(key_up == 0)
		{
			DelayMs(10);
			if(key_up == 0)
			{
//				g_set_temper +=10 ;
		//		pid_off_flag = 1; 	
			}
//			while(key_up != 1) ;
		}
		if(key_down == 0)
		{
			DelayMs(10);
			if(key_down == 0)
			{
		//		g_set_temper -=10 ;
		//		pid_off_flag = 1; 
//				while(key_down == 0) ;	
			}
		}
//		if(g_set_temper > 800) g_set_temper = 800;
//		if(g_set_temper < 20) g_set_temper = 20;
	
//		Set_Tempera[0] = g_set_temper/1000 + 0x30;//��λ
//		Set_Tempera[1] = g_set_temper%1000/100 + 0x30;
//		Set_Tempera[2] = g_set_temper%100/10 + 0x30;
//		Set_Tempera[3] = '.';
//		Set_Tempera[4] = g_set_temper%10 + 0x30;
//		Set_Tempera[5] = '\0';
//    PRINTF("set tempra: %s\n",Set_Tempera);
	    		
//		if(counter-- == 0) //�¶�������5�� 
//		{
//		  temper = ReadTemperature();  			
//		  counter =5;
//	 	}
		g_cur_temperature = ReadTemperature();
//		Display_Temperature(g_cur_temperature);    //�¶���ʾ��
		if(!g_set_temper==0) compare_temper();//ֻ������Ŀ���¶Ⱥ������PID�¿���������	
	//	PRINTF("PID ADJUST OVER\n");
		DelayMs(500);
	} 
	 PRINTF("LOOP OVER\n");
}





 void COM_Recieve( ) //interrupt 4 		 //�����ж�
{
	#if 0
	if (RI) //RI==��ʼ����
	{
		unsigned char ch;
		RI = 0 ; 		//���RI=0
		ch = SBUF;
		
	if ((ch == 'S')||(ch == 's'))
		{
			RI_Count= 0 ;
			RI_SD_flag = 0 ;
	}
   	if ((ch == 'P')||(ch == 'p'))
		{
			RI_Count= 0 ;
			RI_SD_flag = 1 ;
	}
	if ((ch == 'I')||(ch == 'i'))
		{
			RI_Count= 0 ;
			RI_SD_flag = 2 ;
	}
	if ((ch == 'D')||(ch == 'd'))
		{
			RI_Count= 0 ;
			RI_SD_flag = 3 ;
	}
	if ((ch == 'T')||(ch == 't'))
		{
			RI_Count= 0 ;
			RI_SD_flag = 4 ;
	}


   	switch (RI_SD_flag)
	{
		case 0: 
			{
				RI_Buffer[RI_Count++] = ch - '0' ; 
				if(RI_Count == 5)
				{
					g_set_temper = RI_Buffer[1]*1000 +RI_Buffer[2]*100 +RI_Buffer[3]*10 + RI_Buffer[4] ;	
				}
			} 
			break;
		case 1:
			{
				RI_Buffer[RI_Count++] = ch - '0' ; 
				if(RI_Count == 4)
				{
					spid.Proportion = RI_Buffer[1]*100 +RI_Buffer[2]*10 + RI_Buffer[3] ;	
				}
			} 
			break;
		case 2:
			{
				RI_Buffer[RI_Count++] = ch - '0' ; 
				if(RI_Count == 4)
				{
					spid.Integral = RI_Buffer[1]*100 +RI_Buffer[2]*10 + RI_Buffer[3] ;	
				}
			} 
			break;
		case 3:
			{
				RI_Buffer[RI_Count++] = ch - '0' ; 
				if(RI_Count == 4)
				{
					spid.Derivative = RI_Buffer[1]*100 +RI_Buffer[2]*10 + RI_Buffer[3] ;	
				}
			} 
			break;
		case 4:
			{
				RI_Buffer[RI_Count++] = ch - '0' ; 
				if(RI_Count == 4)
				{
					g_temp_offset = RI_Buffer[1]*100 +RI_Buffer[2]*10 + RI_Buffer[3] ;	
				}
			} 
			break;
		default:
			;
			break;
	}
  

	#endif
}
	
float g_OutData[4];
unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)

{

    unsigned short CRC_Temp;

    unsigned char i,j;

    CRC_Temp = 0xffff;



    for (i=0;i<CRC_CNT; i++){      

        CRC_Temp ^= Buf[i];

        for (j=0;j<8;j++) {

            if (CRC_Temp & 0x01)

                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;

            else

                CRC_Temp = CRC_Temp >> 1;

        }

    }

    return(CRC_Temp);

}



void OutPut_Data(float OutData)

{

  int temp[4] = {0};

  unsigned int temp1[4] = {0};

  unsigned char databuf[10] = {0};

  unsigned char i;

  unsigned short CRC16 = 0;

  for(i=0;i<4;i++)

   {



    temp[i]  = (int)OutData;

    temp1[i] = (unsigned int)temp[i];



   }



  for(i=0;i<4;i++)

  {

    databuf[i*2]   = (unsigned char)(temp1[i]%256);

    databuf[i*2+1] = (unsigned char)(temp1[i]/256);

  }



  CRC16 = CRC_CHECK(databuf,8);

  databuf[8] = CRC16%256;

  databuf[9] = CRC16/256;



  for(i=0;i<10;i++)
	{
		send_char_com((char)databuf[i]);
	}

   // uart_putchar(databuf);

    //uart_putchar (UART4,(char)databuf);

}

unsigned char g_comm;
void test_labview_waveform(void)
{
	float temp;
	
	char negtive_flag = 0; // +=0  -=1
	float decimal;
	CC2541_Init();
	temp = 25;
	while(1)
	{
	
		temp += 0.1;
		decimal = temp - (int)temp;
		decimal = decimal * 10;		
		if(g_comm == 0x7a)
		{
		send_char_com(negtive_flag);
		send_char_com((unsigned char)temp);
		send_char_com((unsigned char)decimal);			
		}
		DelayMs(200);
	}

}



static double g_tem_cha;
#define PID_P  22.05
#define PID_I  2.15
#define PID_D  11.2
void temp_control(void)
{
  unsigned char n;
  double tt;
   
static double pid_mema[20];

static double pid_ca,pid_out;

//static unsigned short pid_pwma; 
//�¶���ʾ ������

static unsigned short tem_seta;
  for(n=19;n>0;n--)
  {
    pid_mema[n] = pid_mema[n-1];

  }
  pid_mema[0] = ((double)tem_seta - g_tem_cha);
  
  pid_ca = 0;
  for(n = 0;n<=19;n++)
  {
    pid_ca += pid_mema[n];
  }
  
  tt =  PID_D * (pid_mema[0] - pid_mema[1] + pid_mema[0] - pid_mema[2] + pid_mema[0] - pid_mema[3] + pid_mema[0] - pid_mema[4]);
  pid_out = PID_P * pid_mema[0] + PID_I * pid_ca + tt;
  PRINTF("pid_out = %d \n", pid_out);
  if(pid_out > 1000) pid_out = 1000;
  else if(pid_out < -0) pid_out = 0;
  
//  pid_pwma = (unsigned short)pid_out;
  
//	PRINTF("pid_pwma = %d \n", pid_pwma);
  //????PWM
  // TIM2->CCR3 = pid_pwma;
  
  pid_ca = 0;
  
    
}

void test_pid_version2(void)
{
		int i;
	  unsigned int *p;
	  *p = 5;

	  while(1)
		{
		i++;
	//	i = rand();
		PRINTF("RAND = %d \n",i);
		}
	  
	  
}











