#include"protocolToAPP.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "LinkMode.h"

struct  STRUCT_APPToDev_Packet  strAppToDev_Packet_Record;
struct  STRUCT_DevToAPP_Packet  strDevToApp_Packet_Record;


/*
初始化数据
*/
void init_APPtoDev_Packet()
{
	memset((unsigned char*)&strAppToDev_Packet_Record,0,APPToDev_RX_BUF_MAX_LEN);//strDevToApp_Packet_Record
	memset(&strDevToApp_Packet_Record,0,DevToAPP_TX_BUF_MAX_LEN);	
}

bool process_APPtoDev_Packet(char* temBuf, short length)
{
	int i;
	memcpy(&strAppToDev_Packet_Record, temBuf,length);
	// test 
	printf("app to dev string::");
	for(i = 0; i<length; i++)
	{
		printf("HEX:	%x",strAppToDev_Packet_Record.Data_RX_BUF[i]);
	}
	
	/*
	checksum 校验
	*/
	if(strAppToDev_Packet_Record.str_Packet.u8checkSum!=CheckSum((unsigned char*)&strAppToDev_Packet_Record,length))
	{
		//process err  please fill it
		return 0;
	}
	else
	{
		// process cmd 
		
		//respond  cmd
		
		
	}
	
	strAppToDev_Packet_Record.flag = 1;
	return 0;
}
/*

处理相关命令
*/

bool process_APPtoDev_Cmd(unsigned char cmd)
{
	switch(strAppToDev_Packet_Record.str_Packet.u8cmd)
	{
		
		case 1:
		{
			break;
		}
			
		default:
			break;
	}
	return 0;
}
/*

应答相关命令
*/
bool respond_DevtoAPP_Cmd(unsigned char cmd)
{
	
	return true;
}
/*
校验和
*/
unsigned char CheckSum(unsigned char *buf, int len)
{
	int i;
	unsigned char sum;
	if((0==buf)||(len<=0))
		return 0;
	for(i = 0 ; i < len ; i++)
	{
		sum += buf[i];
	}
	return sum;
}

/**
*
*
*
*
**/


