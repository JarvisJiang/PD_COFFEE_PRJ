#include "stm32f10x.h"
#include "rc522.h"
#include <string.h>
#include <stdio.h>


static u8 USART_RX_BUF[64];
static u8 USART_RX_STA;
/*******************************
*连线说明：
*1--SS(SDA)  <----->PB0
*2--SCK <----->PB13
*3--MOSI<----->PB15
*4--MISO<----->PB14
*5--悬空
*6--GND <----->GND
*7--RST <----->PB1
*8--VCC <----->VCC

#if NFC_IIC

*1--SCK <--->  PB10
*2--SDA <--->  PB11
*3--RST <--->  PB1


#endif
************************************/

/************************************/

static void delay_us(unsigned int nus)
{
	  unsigned int i = 0; 
		while(nus--)
		{
			i = 10;
			while(i--);
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
static void test_delay(void)
{
	while(1)
	{
		
		CLR_SPI_CS;
		delay_ms(10);
		SET_SPI_CS;
		delay_ms(10);
	}
	
}
static u8 ReadData(u8   addr,u8 *pKey,u8 *pSnr,u8 *dataout)
{
	u8 status,k;
	status=0x02;//
	k=5;

	do
    {
		status=PcdAuthState(PICC_AUTHENT1A,addr,pKey,pSnr);
		k--;
		//printf("AuthState is wrong\n");						      
    }while(status!=MI_OK && k>0);

	status=0x02;//
	k=5;
	do
    {
		status=PcdRead(addr,dataout);
		k--;
		//printf("ReadData is wrong\n");							      
    }while(status!=MI_OK && k>0);
	return status;
}
static u8 WriteData(u8   addr,u8 *pKey,u8 *pSnr,u8 *datain)
{
	u8 status,k;
	status=0x02;//
	k=5;
	do
    {
		status=PcdAuthState(PICC_AUTHENT1A,addr,pKey,pSnr);
		k--;
		//printf("AuthState is wrong\n");						      
    }while(status!=MI_OK && k>0);

	status=0x02;//
	k=5;
	do
    {
		status=PcdWrite(addr,datain);
		k--;
		//printf("ReadData is wrong\n");							      
    }while(status!=MI_OK && k>0);
	return status;
}
/*全局变量*/
static unsigned char CT[2];//卡类型
static unsigned char SN[4]; //卡号
u8 KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};
/*函数声明*/
void ShowID(u16 x,u16 y, u8 *p, u16 charColor, u16 bkColor);	 //显示卡的卡号，以十六进制显示
void PutNum(u16 x,u16 y, u32 n1,u8 n0, u16 charColor, u16 bkColor);	//显示余额函数
void Store(u8 *p,u8 store,u8 cash);//最重要的一个函数
#define  	DEBUG_NFC(fmt,args...)    printf(fmt, ##args)
/*
can not continuous  read large than one block
test  can not pass
*/
u8 read_sector_data(u8   addr,u8 *pKey,u8 *pSnr, u8 *success_flag)
{
	    u8 status = 0x02;//error
	    u8 data[16];
	    u8 sector_data[64];
			u32 i = 0;
			memset(sector_data, 1, 64);
		  for(i = 0 ; i < 4; i++)
			{
				status = ReadData(addr+i,pKey,pSnr,data);
				if(status==MI_OK)
				{
					memcpy(sector_data+(i<<4),data,16);
				}
				else
				{
					printf("read sector error block %d \r\n",i);
				}
			}
			
			printf("addr = %d--ReadData:\n",addr);
			for(i=0;i<64;i++)
			{
				printf("%02x ",sector_data[i]);
				if((i%16)==15)
				{
					printf("\n");
				}
			}
			

}
void test_read_data(void)
{
	u8 b_key_arry[6] 				= {0xff,0xff,0xff,0xff,0xff,0xff};
	u8 a_key_arry[6] 			  = 				{0xff,0xff,0xff,0xff,0xff,0xff};
	u8 access_bit_front[4]  = {0,0,0,0};
	u8 access_bit_last[4]   = {0,0,0,0};
	u8 status, k;
	u8 addr=0;
	u8 loop = 0;
	u8 data[32];
	u32 i;
	static u8 repeat_flag = 0;
	static u8 success_flag = 0;
  static u8 backup_sn[4];
	static u8 first_scan_flag = 0;
	u8 offset = 0;
	u8 error = 0;

	while(1)
	{
			DEBUG_NFC("-------------address-%d---------------",addr);
			status=0x02;//设置初始值
		
			
//			if(repeat_flag)
//				continue;
			
		 if(64==addr)
			 addr = 0;

			k=5;		 //设置重复次数，有事读卡一次并不能成功
			do
		    {
				status=PcdRequest(PICC_REQALL,CT);/*扫描卡*/
				k--;						      
		    }while(status!=MI_OK );//&& k>0
			if(MI_OK!=status)
			{
				error = 3;
				DEBUG_NFC("err0 scanner\r\n");
				continue;
				
			//	continue;
			}
			status=0x02;//
			k=5;
			do
				{
				status=PcdAnticoll(SN);/*防冲撞*/
				k--;						      
				}while(status!=MI_OK && k>0);
			repeat_flag = 0;
			if (status==MI_OK)
			{
					if((1==first_scan_flag)&&(1==success_flag))
					{
						repeat_flag = 1;
						for(i = 0 ; i < 3 ; i++)
						{
							
							if((SN[i]!=backup_sn[i]))
							repeat_flag = 0;
						}
					}
					else
					{
						first_scan_flag = 1;
						repeat_flag     = 0;
					}
			}
			else
			{
				DEBUG_NFC("PcdAnticoll\r\n");
				error = 2;
				
				continue;
			}
			if(repeat_flag==1) //previous accessed is true， and the card(the same SN) accessed again
			{
				//do something
			}
			status=0x02;//
			k=5;
			do
		    {
				status=PcdSelect(SN);//选卡
				k--;						      
		    }while(status!=MI_OK && k>0);
			if(MI_OK!=status)
			{
				DEBUG_NFC("err :PcdSelect \r\n");
				error = 5;
				continue;
			}

				success_flag = 0;

				#if 1
				status = ReadData(addr+0,a_key_arry,SN,data);
				
				if(status==MI_OK)
				{
				success_flag = 1;
				printf("addr = %d--ReadData:",addr);
				for(i=0;i<16;i++)
				{
				printf("%02x ",data[i]);
				}
				printf("\n\n");
			//	ShowID(0,100,SN,0,0); //在液晶屏上显示卡的ID号
			//	PcdHalt();
				addr++;
				delay_ms(1000);
				}
				else
				{
					DEBUG_NFC("ReadData is wrong\n");
				}
				#endif

}
}


void pin_reset(void)
{
		SET_RC522RST;
    delay_us(10);
	//PORTD&=~(1<<RC522RST);
		CLR_RC522RST;
    delay_us(10);
	//PORTD|=(1<<RC522RST);
		SET_RC522RST;
}

void test_write_data(void)
{
	u8 b_key_arry[6] 				= {0xff,0xff,0xff,0xff,0xff,0xff};
	u8 a_key_arry[6] 			  = 				{0xff,0xff,0xff,0xff,0xff,0xff};
	u8 access_bit_front[4]  = {0,0,0,0};
	u8 access_bit_last[4]   = {0,0,0,0};
	u8 status, k;
	u8 addr=4;
	u8 loop = 0;
	u8 data[32];
	u8 write_data[16] = {
						   0x12,0x34,0x56,0x78,
						   0x78,0x56,0x34,0x12,
						   0x21,0x22,0x23,0x24,
						   0x31,0x41,0x51,0x61
						};
	u8 write_data_last_blk[16] ={0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
							  0xff, 0x07, 0x80, 0x69,
							  0xff, 0xff, 0xff, 0xff, 0xff, 0xff
                                               };
	u32 i;
	static u8 repeat_flag = 0;
	static u8 success_flag = 0;
  static u8 backup_sn[4];
	static u8 first_scan_flag = 0;
	u8 offset = 0;
	u8 error = 0;
	
	while(1)
	{
			DEBUG_NFC("-----wrt-----address-%d---------------",addr);
			status=0x02;//设置初始值
		
		//	InitRc522();
//			if(repeat_flag)
//				continue;
		//	pin_reset();
		//delay_ms(500);
		 if(64==addr)
			 addr = 4;

			k=5;		 //设置重复次数，有事读卡一次并不能成功
			do
		    {
				status=PcdRequest(PICC_REQALL,CT);/*扫描卡*/
				k--;						      
		    }while((status!=MI_OK )&& (k>0));//
			if(MI_OK!=status)
			{
				error = 3;
				DEBUG_NFC("err0 scanner\r\n");
				continue;
				
			//	continue;
			}
			status=0x02;//
			k=20;
			#if 1
			do
				{
				status=PcdAnticoll(SN);/*防冲撞*/
				k--;						      
				}while(status!=MI_OK && k>0);
			repeat_flag = 0;
			if (status==MI_OK)
			{
					if((1==first_scan_flag)&&(1==success_flag))
					{
						repeat_flag = 1;
						for(i = 0 ; i < 3 ; i++)
						{
							
							if((SN[i]!=backup_sn[i]))
							repeat_flag = 0;
						}
					}
					else
					{
						first_scan_flag = 1;
						repeat_flag     = 0;
					}
			}
			else
			{
				DEBUG_NFC("PcdAnticoll\r\n");
				error = 2;
				
				continue;
			}
			#endif
			if(repeat_flag==1) //previous accessed is true， and the card(the same SN) accessed again
			{
				//do something
			}
			status=0x02;//
			k=5;
			do
		    {
				status=PcdSelect(SN);//选卡
				k--;						      
		    }while(status!=MI_OK && k>0);
			if(MI_OK!=status)
			{
				DEBUG_NFC("err :PcdSelect \r\n");
				error = 5;
				continue;
			}

				success_flag = 0;

				#if 1
				if((addr%4)!=3)
				status = WriteData(addr,a_key_arry,SN,write_data);
				else
				status = WriteData(addr,a_key_arry,SN,write_data_last_blk);
				if(status == MI_OK)
				{
					//DEBUG_NFC("write ok\r\n");
				}
				else
				{
					DEBUG_NFC("write not ok\r\n");
					continue;
				}
				status = ReadData(addr+0,a_key_arry,SN,data);
				if(status==MI_OK)
				{
				success_flag = 1;
				printf("addr = %d--ReadData:",addr);
				for(i=0;i<16;i++)
				{
				printf("%02x ",data[i]);
				}
				printf("\n\n");
			//	ShowID(0,100,SN,0,0); //在液晶屏上显示卡的ID号
			//	PcdHalt();
				addr++;
			//	delay_ms(1000);
				}
				else
				{
					DEBUG_NFC("read is wrong\n");
				}
				#endif

}
}

void test_nfc(void)
{	
	unsigned char status  = 0;
	u8 Data[16];
	u8 i;
	u8 k;//读写错误重试次数


	printf("test_nfc is ok\n");//串口输出数据，提示单片机及串口工作正常
	//rc_522_IIC_Init();
	//rc522_test_iic_gpio_delay();
	InitRc522();				//初始化射频卡模块
	
//	test_read_data();
	test_write_data();
	while(1)
	{
		status = PcdRequest(PICC_REQALL,CT);/*扫描卡*/
		status = PcdAnticoll(SN);/*防冲撞*/
		if (status==MI_OK)
		{
		//	GPIOC->BRR=0x06;   //三个led全部点亮
		ShowID(0,100,SN,0,0); //在液晶屏上显示卡的ID号
		//printf("ID:%02x %02x %02x %02x\n",sn[0],sn[1],sn[2],sn[3]);//发送卡号
		}	
	}


	while(1)													 
	{		
		
		if(!(USART_RX_STA&0x80))//串口发送的数据全部保存在USART_RX_BUF中，利用中断实现，每次数据完成后USART_RX_STA最高位置1
		{
			status = PcdRequest(PICC_REQALL,CT);/*扫描卡*/
			status = PcdAnticoll(SN);/*防冲撞*/
			if (status==MI_OK)
			{
				GPIOC->BRR=0x06;   //三个led全部点亮
				ShowID(0,100,SN,0,0); //在液晶屏上显示卡的ID号
				//printf("ID:%02x %02x %02x %02x\n",sn[0],sn[1],sn[2],sn[3]);//发送卡号
			}	
			else
			{
				GPIOC->BSRR=0X06;//只点亮led0
			}
		}
		else
		{
			status=0x02;//设置初始值
			k=5;		 //设置重复次数，有事读卡一次并不能成功
			do
		    {
				status=PcdRequest(PICC_REQALL,CT);/*扫描卡*/
				k--;						      
		    }while(status!=MI_OK && k>0);

			status=0x02;//
			k=5;
			do
		    {
				status=PcdAnticoll(SN);/*防冲撞*/
				k--;						      
		    }while(status!=MI_OK && k>0);

			status=0x02;//
			k=20;
			do
		    {
				status=PcdSelect(SN);//选卡
				k--;						      
		    }while(status!=MI_OK && k>0);

			if(status!=MI_OK)
			{
				printf("PcdSelect is wrong\n");
			}
			switch (USART_RX_BUF[0])
			{
				case 0xa0:  printf("ID:%02x %02x %02x %02x\n",SN[0],SN[1],SN[2],SN[3]);//发送卡号					  
							break;
				case 0xa1://读数据，一般不用，仅在测试以及对卡的特殊操作时使用
							status=ReadData(USART_RX_BUF[7],&USART_RX_BUF[1],SN,Data);
							if(status==MI_OK)
						    {
								printf("ReadData:");
								for(i=0;i<16;i++)
							    {
							      printf("%02x ",Data[i]);
							    }
								printf("\n\n");
							    PcdHalt();
							}
							else
							{
								printf("ReadData is wrong\n");
							}	
							break;
				case 0xa2://写数据，一般不用
							status=WriteData(USART_RX_BUF[7],&USART_RX_BUF[1],SN,&USART_RX_BUF[8]);
						    if(status!=MI_OK)
						    {
								printf("WriteData is wrong\n");						      
						    }
							else
							printf("WriteData is ok\n");
							break;
				case 0xa3://修改密码
							break;
				case '+' ://增加money
							Store(&USART_RX_BUF[1],1,1);
							break;
				case '-' ://减少money
							Store(&USART_RX_BUF[1],0,1);
							break;
				case '=' ://查看money
							Store(&USART_RX_BUF[1],1,0);
							break;
				default:
							break;
			}
			USART_RX_STA=0;//串口接收完成标志清零
		}
	}
}/*************************************
*函数功能：显示卡的卡号，以十六进制显示
*参数：x，y 坐标
*		p 卡号的地址
*		charcolor 字符的颜色
*		bkcolor   背景的颜色
***************************************/
void ShowID(u16 x,u16 y, u8 *p, u16 charColor, u16 bkColor)	 //显示卡的卡号，以十六进制显示
{
	u8 i;
	i = 0;
	printf("Serial Number:");
	while(i!=4)
	{
		printf(" %x",p[i]);
		i++;
	}
	printf("\n");
	
//	DisplayString(x,y,"The Card ID is:",charColor,bkColor);
//	DisplayString(x,y+16,num,charColor,bkColor);
}
/********************************
*函数功能：求p的n次幂
*/
int power(u8 p,u8 n)
{
	int pow=1;
	u8 i;
	for(i=0;i<n;i++)
	{
		pow*=p;	
	}
	return pow;
}
/******************************************
*函数功能：实现卡内金额的查询，充值以及扣费
*参数：	p    --金额
*		store--store为1，为充值，
*				store为0，为扣费
*		cash --cash为1，则为充值或扣费
*				cash为0，则查询余额
*******************************************/
void Store(u8 *p,u8 store,u8 cash)
{
	u8 data[16];
	u8 len=0;
	u8 i=0;
	u32 dec=0;
	u8 status;
	u8 tmp;

	status=ReadData(0x04,KEY,SN,data);
	dec=data[11];
	dec<<=8;
	dec+=data[12];
	dec<<=8;
	dec+=data[13];
	dec<<=8;
	dec+=data[14];

	if(!cash)
	{
//		LCD_Fill(0,148,240,16,WHITE);
//		DisplayString(0,148,"The Balance is:",RED,WHITE);
		PutNum(120,148,dec,data[15],0,0);
	}
	else
	{
		while(p[len]!='.')
		{
			len++;
		}

		if(store)
		{
			data[15]+=p[len+1]-'0';
			if(data[15]>9)
			{
				data[15]-=10;
				dec++;
			}
			while(len--)
			{
				dec+=(p[len]-'0')*power(10,i);
				i++;
					
			}
		}
		else
		{
			if(data[15]<p[len+1]-'0')
			{
				data[15]+=10;
				dec--;
			}
			data[15]-=p[len+1]-'0';
			
			while(len--)
			{
				if(dec<(p[len]-'0')*power(10,i))
				{
//					DisplayString(0,164,"You do not have enough money!",RED,WHITE);
					goto End;	
				}
				dec-=(p[len]-'0')*power(10,i);
				i++;
					
			}
		}
	
//		LCD_Fill(0,164,240,16,WHITE);
//		DisplayString(0,164,"The Balance is:",RED,WHITE);
//		PutNum(120,164,dec,data[15],RED,WHITE);
	
		data[14]=dec&0xff;
		data[13]=(dec>>8)&0xff;
		data[12]=(dec>>16)&0xff;
		data[11]=(dec>>24)&0xff;
	
End:	status=WriteData(0X04,KEY,SN,data);

	
	}
}


void PutNum(u16 x,u16 y, u32 n1,u8 n0, u16 charColor, u16 bkColor)
{
	u8 tmp[13];
	u8 i;

//	LCD_SetRegion(0,0,239,319,FALSE);
	tmp[0]=n1/1000000000+'0';
	for(i=1;i<10;i++)
	{
		tmp[i]=n1/(1000000000/power(10,i))%10+'0';
	}
	tmp[10]='.';
	tmp[11]=n0+'0';
	tmp[12]=0;
//	DisplayString(x,y,tmp,charColor,bkColor);


	
}
