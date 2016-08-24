
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
PID函数
*************************************************/ 
/*************PID**********************************/

struct PID_WATER_HEATER spid; // PID Control Structure



//unsigned char key1,i,k;
static unsigned int g_cur_temperature;
// unsigned int temp , last_temp;
//unsigned char shu[4]={0,0,0,0}, key_count1;
//unsigned char counter;
 int g_set_temper ,g_temp_offset = 40;  // 温 度 补  偿
char RI_Buffer[64];
//unsigned char RI_SD_flag ; 		//uart receive  buf  flag
				
//char  tp_flag , key_dot_flag;


//unsigned char   RI_Count , 
//				RI_Buffer[50] ;

//专家控制表  保温					   	  1  2  3  4  5  6  7  8  9	  室温 14.7度 建 立
static const unsigned char  keep_warm[101] = {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,// 0 ~10 度
											   8, 9 , 20, 32, 33, 34, 35, 36, 37, 38, // 11~20 度
											  39, 40, 41, 42, 43, 45, 46, 48, 50, 55, // 31~30 度
											  59, 60, 61, 62, 63, 64, 65, 66, 67, 70, // 31~40 度
											  70, 70, 70, 70, 70, 70, 71, 72, 74, 80, // 41~50 度
											  77, 78, 78, 79, 80, 81, 81, 82, 82, 83, // 51~60 度
											  83, 84, 85, 86, 87, 88, 89, 90, 91, 92, // 61~70 度
											  90, 90, 92, 94, 96, 98,100,101,104,108, // 71~80 度
											 108,109,110,111,112,112,113,114,118,120, // 81~90 度
											 101,102,103,104,105,106,107,108,109,110, // 91~100 度
								};
////专家控制表  减小超调				   1  2  3  4  5  6  7  8  9	  室温 14.7度 建 立
//static const unsigned char  pid_off[101]  =	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,// 0 ~10 度
//											   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 11~20 度
//											   0, 0, 0, 0, 0, 2, 3, 3, 4, 4, // 21~30 度
//											   5, 5, 5, 6, 6, 6, 6, 6, 6, 6, // 31~40 度
//											   6, 6, 6, 6, 6, 6, 6, 6, 6, 6, // 41~50 度
//											   6, 6, 6, 7, 7, 7, 7, 7, 7, 8, // 51~60 度
//											   8, 9, 9, 9, 9, 9, 9, 9, 9, 9, // 61~70 度
//											   9, 9, 9, 9, 9,10,10,10,10,10, // 71~80 度
//											  10,11,11,11,12,12,13,13,13,13, // 81~90 度
//											  14,14,14,15,15,15,15,15,15,15, // 91~100 度
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
									
									
/*********************************PID函数**********************************************/
              
static void water_heater_PIDInit (struct PID_WATER_HEATER *pp) 
{ 
 	 memset ( pp,0,sizeof(struct PID_WATER_HEATER)); 
} 


/**
  * @brief  初始化ESP8266用到的 USART
  * @param  无
  * @retval 无
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
	
	
	/* 中断配置 */
	USART_ITConfig ( USART3, USART_IT_RXNE, ENABLE ); //使能串口接收中断 
	//USART_ITConfig ( USART3, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 	
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
x10 倍处理
*************************************************/
static unsigned int water_heater_PIDCalc( struct PID_WATER_HEATER *pp, unsigned int NextPoint ) 
{ 
	  unsigned int dError,Error,pError; 
	  //增量法计算公式：
	  //Pdt=Kp*[E(t)-E(t-1)]+Ki*E(t)+Kd*[E(t)-2*E(t-1)+E(t-2)]
	  Error = g_set_temper - NextPoint;       // 偏差E(t) 
	  pError=Error-pp->LastError;	 //E(t)-E(t-1)
	  dError=Error-2*pp->LastError+pp->PrevError; //E(t)-2*E(t-1)+E(t-2) 
	  pp->PrevError = pp->LastError; 
	  pp->LastError = Error; 
	  return ( 
	            pp->Proportion * pError        //比例 
	            + pp->Integral *Error  //积分项 
	            + pp->Derivative * dError	  // 微分项
				);  
} 

/************************************************
				PID函数初始化
*************************************************/
static void PIDBEGIN() 
{ 
	  water_heater_PIDInit(&spid); // Initialize Structure 
	  spid.Proportion = 10; // Set PID Coefficients 10
	  spid.Integral = 5; 	//5
	  spid.Derivative = 2; 	// 4
}
/*********************************PID函数OVER******************************************/


/************************************************
			 实时温度读取函数
从DS18b20中读取实时温度
返回值放大10倍便于PID计算
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
	向串口uart1发送一个字符串，strlen为该字符串长度
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
             PID温度控制做动函数
***********************************************************/ 
void compare_temper() 		//PID温度控制输出函数
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
    if((dir) > 150)//如果控制目标温度温与实时温度差大于8度，（放大10倍）是80
    {
    
	   PWM0_set(5);  //PWM 输出高电平占空245/255比最大。即全速加温

	   g_pwm_value = 250 ; 
	   temp = ReadTemperature();
	   
    }
	else 	//pid控制范围内
    {
    PRINTF("IN PID__________________ \n ");
	  for(i=0;i<20;i++) //8度范围内PID增量控制，10次周期 即PID积分式中T=10   change i < 20
	  { 
	      temp = ReadTemperature();//PID输入实时温度采样值
			   PRINTF("temp = %d \n", temp);
		  rin = temp ;
	    rout = water_heater_PIDCalc ( &spid,rin ); // PID增量输出
			PRINTF("rout = %d\n",rout);
		  DelayMs(5); 
	  }//PID增量输出范围（0-255）配合pwm取值范围 	
		 
		 if(rout >=240) rout=240;	//保障PWM输入值10-240 防止pwm出现失调。stc单片机特性。
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
   
  else if(g_set_temper < g_cur_temperature)      //目标温度小于实时温度pwm输出低电平最高占空比，关闭加热。
  {
 
     if((g_cur_temperature - g_set_temper) <= 50)
	 {	 //目标温度小于实时温度 在5度之内
 
		 PRINTF("SMALL 5 OOOOOOO\n");
	   	g_pwm_value =  (g_set_temper/10 + g_temp_offset- 60 +10*(3-g_cur_temperature+g_set_temper)) ;//(unsigned char)(
		  PRINTF("g_pwm_value = %d\n",(255 - g_pwm_value));
		  PWM0_set(255 - g_pwm_value); 			     //PWM 5 温度保持
      
		 temp = ReadTemperature();
		 PRINTF("SMALL 5 OOOOOOO\n");
	 }
	 else if(g_cur_temperature - g_set_temper > 5)
	 {	 // 目标温度小于实时温度 在0.5度之外

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

	 send_string_com("ST "); //串口输出监视数据 
	 send_char_com(zhi[g_set_temper/1000]); //设定温度值
	 send_char_com(zhi[g_set_temper/100]);
	 send_char_com(zhi[g_set_temper%100/10]);
	 send_char_com(zhi[g_set_temper%10]);	  //小数点后一位
     send_string_com("  T "); //串口输出监视数据 
	 send_char_com(zhi[temp/1000]); //实时温度值
	 send_char_com(zhi[temp/100]);
	 send_char_com(zhi[temp%100/10]);
	 send_char_com(zhi[temp%10]);	  //小数点后一位
	 send_string_com("  PWM "); //串口输出监视数据
	 PRINTF("PWM = %d\n",g_pwm_value);
	 send_char_com(zhi[g_pwm_value/100]); //PID增量输出值
	 send_char_com(zhi[g_pwm_value%100/10]);
	 send_char_com(zhi[g_pwm_value%10]);
	 send_string_com("/255") ;
	 send_string_com(" P"); //串口输出监视数据
	 send_char_com(zhi[spid.Proportion/100]); //PID增量输出值
	 send_char_com(zhi[spid.Proportion%100/10]);
	 send_char_com(zhi[spid.Proportion%10]);
	 send_string_com(" I"); //串口输出监视数据
	 send_char_com(zhi[spid.Integral/100]); //PID增量输出值
	 send_char_com(zhi[spid.Integral%100/10]);
	 send_char_com(zhi[spid.Integral%10]);
	 send_string_com(" D"); //串口输出监视数据
	 send_char_com(zhi[spid.Derivative/100]); //PID增量输出值
	 send_char_com(zhi[spid.Derivative%100/10]);
	 send_char_com(zhi[spid.Derivative%10]);
	 TO =g_temp_offset- 40  + keep_warm[g_set_temper/10];
	 send_string_com(" TO"); //串口输出监视数据
	 send_char_com(zhi[TO/100]); //PID增量输出值
	 send_char_com(zhi[TO%100/10]);
	 send_char_com(zhi[TO%10]);
	 send_char_com(0x0d);//输出回车，换行间隔控制字符
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
		if(g_set_temper-last_set_temper >= 100)//长距离与短距离加热 
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
		if( g_set_temper-g_cur_temperature < 10 )   //积分死区时间设定
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
                  主函数
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
	 PWM0_set(250);		//初始化PWM两路输出最高低电平占空比，不要加热
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
	
//		Set_Tempera[0] = g_set_temper/1000 + 0x30;//百位
//		Set_Tempera[1] = g_set_temper%1000/100 + 0x30;
//		Set_Tempera[2] = g_set_temper%100/10 + 0x30;
//		Set_Tempera[3] = '.';
//		Set_Tempera[4] = g_set_temper%10 + 0x30;
//		Set_Tempera[5] = '\0';
//    PRINTF("set tempra: %s\n",Set_Tempera);
	    		
//		if(counter-- == 0) //温度连续读5次 
//		{
//		  temper = ReadTemperature();  			
//		  counter =5;
//	 	}
		g_cur_temperature = ReadTemperature();
//		Display_Temperature(g_cur_temperature);    //温度显示；
		if(!g_set_temper==0) compare_temper();//只有设置目标温度后才启动PID温控作动函数	
	//	PRINTF("PID ADJUST OVER\n");
		DelayMs(500);
	} 
	 PRINTF("LOOP OVER\n");
}





 void COM_Recieve( ) //interrupt 4 		 //串口中断
{
	#if 0
	if (RI) //RI==开始接收
	{
		unsigned char ch;
		RI = 0 ; 		//软件RI=0
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
//温度显示 与设置

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











