#include "test.h"
#include "bsp_esp8266.h"
#include "bsp_SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>

#include "bsp_led.h"
#include "cc2541.h"

#include "ads1220.h"
#include "DS_delay.h"
char buf1[]="abcdefghijklmq";
char buf2[]="abcffgssgaga";
char buf3[]="TFGgsagafstg";
char buf4[]="12345rtsgst";
char buf5[]="JiaNgJiejjj01222";
char *databuf[5] = {buf1,buf2,buf3,buf4,buf5};
/*
function:bool ESP8266_Sta_SmartLinkByPhone(void)
brief:
    1.实现smartlink 功能   需要esp8266 与手机在 同一局域网中
		2.APP 输入 路由wifi密码     
		3.smartlink成功后 需要 APP 扫描控制 并发送开始命令， 次函数功能才完成
*/
bool ESP8266_Sta_SmartLinkByPhone(void)
{
	char rtn;
  int bufCnt = 0;
	int waitTimeCount;
	char *appBuf;
	int tryCount = 0;
	trySmartLinkAgain:
	tryCount++;
	if(10==tryCount)
	{
		printf("\r\nit can not SmartLink please check your device\r\n");
		return 0;
	}
	ESP8266_Rst();
	
	if(0==ESP8266_Cmd ( "AT+RESTORE", "OK", NULL, 500 ))
	{
		printf("\r\n restore failure!");
		goto trySmartLinkAgain;
	}
	
	Delay_ms (500);
	printf("\r\nESP8266 Station  working start .......");


	
	if(0==ESP8266_Net_Mode_Choose (STA ))
	{
		printf("\r\n set STmode failure");
		goto trySmartLinkAgain;
	}
	rtn = ESP8266_Cmd( "AT+CWSMARTSTART=1", "OK", NULL, 500 );
	if(!rtn)
	{
		do
		{
			rtn = ESP8266_Cmd( "AT+CWSMARTSTOP", "OK", NULL, 500 );
			rtn = ESP8266_Cmd( "AT+CWSMARTSTART=1", "OK", NULL, 500 );
			printf("\r\n set SMART LINK Fail");
		}while(!rtn);
	}

// in here   waiting wifi code from app
	printf("\r\n please set wifi code:\r\n");
	do
	{
		Delay_ms(100);
		waitTimeCount++;

		if(1000==waitTimeCount)
		{
			printf("\r\n waiting ... phone's ID CODE");
			goto trySmartLinkAgain;

		}

	}while( !( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "WIFI GOT" ));
	Delay_ms(200);
	
	if(0==ESP8266_Cmd ( "AT+CLDSTART", "OK", NULL, 500 ))
	{
		printf("\r\n CLDSTART failure");
		goto trySmartLinkAgain;
	}
	if(0==ESP8266_Cmd ( "AT+LANSTART", "OK", NULL, 500 ))
	{
		printf("\r\n	LANSTART failure");
		goto trySmartLinkAgain;
	}
	if(0==ESP8266_Cmd ( "AT+CLDUNBIND", "OK", NULL, 500 ))
	{
		printf("\r\n CLDUNBIND failure");
		goto trySmartLinkAgain;
	}
	//in here need app bind  the device(esp8266)  and start control,  it need app's start data(otherwise it will waiting forever) , then it can start intertranslating;
	Delay_ms(200);
	printf("**********V******V*********************\r\n");
	printf("**********@******@*******************\r\n");
	printf("*************V**************************\r\n");	
	printf("please sent a start data to me by you app !!\r\n");
	printf("**************************************\r\n");
	appBuf = ESP8266_ReceiveString();
  printf("\r\n appStartBuf = %s\r\n",appBuf);
	printf("start CLD data translate start \r\n");
	while(1)
	{

		if(bufCnt==5)
		bufCnt = 0;
		rtn = ESP8266_CLD_SendString(databuf[bufCnt],5);
		bufCnt++;
		if(NULL==rtn)
		{
		printf("send fail!\r\n");
		}
		Delay_ms(200);
		printf("**********V******V*********************\r\n");
		printf("**********@******@*******************\r\n");
		printf("*************V**************************\r\n");	
		printf("please sent a data to me by you app  send  ESC string for quiting!!\r\n");
		appBuf = ESP8266_ReceiveString();
		printf("\r\n appBuf = %s\r\n",appBuf);
		
		if(0!=strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ESC" ))
		{
			printf("\r\nquit link cld\r\n");
			return 1;
		}
		

		Delay_ms(1000);
	}
	


	
}
/*
*bool ESP8266_Sta_LinkCLD(void)
*description:  when esp8266 have connected with ap, and bind a app from phone,
and saved the ssid  and code into flash. this function will be used when it be 
reset or unconnect with CLD, and it will link CLD;
*/
bool ESP8266_Sta_LinkCLD(void)
{
	char rtn;
  int bufCnt = 0;
	int tryCount;
//	int waitTimeCount;
	char *appBuf;
tryAgainLinkCLD:
	tryCount++;
	if(10==tryCount)
	{
		printf("\r\nit can not LINK CLD please check your device\r\n");
		return 0;
	}
	ESP8266_Rst();
	Delay_ms (1000);
	printf("\r\nESP8266 Station  working start .......");

	if(0==ESP8266_Net_Mode_Choose (STA ))
	{
		printf("\r\n set STmode failure");
		goto tryAgainLinkCLD;
	}
	Delay_ms (100);
	if(0==ESP8266_Cmd ( "AT+CLDSTART", "OK", NULL, 500 ))
	{
		printf("\r\n CLDSTART failure");
		goto tryAgainLinkCLD;
	}
	
	printf("**********V******V*********************\r\n");
	printf("**********@******@*******************\r\n");
	printf("*************V**************************\r\n");	
	printf("please sent a start data to me by you app !!\r\n");
	printf("**************************************\r\n");
	appBuf = ESP8266_ReceiveString();
  printf("\r\n appStartBuf = %s\r\n",appBuf);
	printf("start CLD data translate start \r\n");
	while(1)
	{

		if(bufCnt==5)
		bufCnt = 0;
		rtn = ESP8266_CLD_SendString(databuf[bufCnt],5);
		bufCnt++;
		if(NULL==rtn)
		{
		printf("send fail!\r\n");
		}
		Delay_ms(200);
		printf("**********V******V*********************\r\n");
		printf("**********@******@*******************\r\n");
		printf("*************V**************************\r\n");	
		printf("please sent a data to me by you app  quit!!\r\n");
		appBuf = ESP8266_ReceiveString();

		printf("\r\n appBuf = %s\r\n",appBuf);
		if(0!=strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ESC" ))
		{
			printf("\r\nquit link cld\r\n");
			return 1;
		}

		Delay_ms(1000);
	}


	
	
}
/*
brief： 此函数一直测试 与APP 透传数据   此过程 AT指令无效
*/
void CC2541_TEST(void)
{
	char *buf;
	while(1)
	{
		
		CC2541_BuildLinkByPhone();
		buf = CC2541_ReceiveString();// 一直等待  rx interupt
		printf("cc2541: %s",buf);
		
		
		CC2541_SendingStringConnectless("AT+NOTI\r\n");// 无连接 发送数据    AT固件本身 没有 应答
		CC2541_SendingStringConnectless("1213456\r\n");
		
	}
}


void DS1220_TEST(void)
{
	int i;
	short ads1200Data;
	DS_delay_init(72);	                    //???????
	SPI_ADS1220_Init();								  //SPI???
	ADS1220_GPIO_init();							  //?????
	ADS1220_Init();

	printf("ads1220 test !\r\n");
	i = 1;
	while(1)
	{
		ads1200Data = ADS1220ReadData();
		printf("orig: hex[%d]:%x\r\n", i,ads1200Data);
		ads1200Data = ads1200Data>>8;
		printf("deci: data[%d]: %d\r\n", i,(short)ads1200Data);
		DS1220ResConvertToTem(ads1200Data);//
		i++;
		DS_delay_ms(500);
	}
}

#if 0

bool ESP8266_Sta_ConnectAPPTest(void)
{
	char rtn;
  int bufCnt = 0;
	int waitTimeCount;
tryAgain:

	ESP8266_Rst();
	Delay_ms (1000);
	printf("\r\nESP8266 Station  working start .......");



	ESP8266_Net_Mode_Choose (STA );
	//????AT??????? ????????
	//rtn = ESP8266_Cmd ( "AT+CWSMARTSTART=1", "OK", NULL, 500 );

	//?? ???????smartlink  ??WIFI ??
	//????????? ???
	//?????????APP ?IP
	do
	{
		Delay_ms(100);
		waitTimeCount++;
		//????????
		if(100==waitTimeCount)
		{
			goto tryAgain;

		}

	}while( !( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "WIFI GOT" ));

	//??????
	ESP8266_Cmd ( "AT+CLDSTART", "OK", NULL, 500 );
	Delay_ms(100);
	while(1)
	{
		if(bufCnt==5)
			bufCnt = 0;
		rtn = ESP8266_CLD_SendString(databuf[bufCnt],15);
		bufCnt++;
		if(NULL==rtn)
		{
		printf("send fail!\r\n");
		}
		Delay_ms(1000);
	}
	
	//???????
	ESP8266_Cmd ( "AT+LANSTART", "OK", NULL, 1000 );
	//??????????????

	ESP8266_Cmd ( "AT+CLDUNBIND", "OK", NULL, 500 );

	// ??????? ??APP ?????

    //  test ??????

	do {
		rtn = ESP8266_CLD_SendString("HELLO",5);
		if(NULL==rtn)
		{
			printf("send fail!\r\n");
		}
		Delay_ms(200);
 		if ( strEsp8266_Fram_Record .InfBit .FramFinishFlag )
		{
			printf ( "\r\n??%s??", strEsp8266_Fram_Record .Data_RX_BUF );//
			//??UART??  ???
			USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, DISABLE ); //????????
			Delay_ms(10);
			USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //????????
		}

	} while (1);

}

#endif


