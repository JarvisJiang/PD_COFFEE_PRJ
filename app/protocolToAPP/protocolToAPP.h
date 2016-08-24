#ifndef _PROTOCOL_TO_APP_H_
#define _PROTOCOL_TO_APP_H_

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#include "stm32f10x.h"
#include "common.h"
#include <stdio.h>
#include <stdbool.h>

/**

Make Coffee   																							30ml					 94.5c 						12.5g 			10 level 			20s 						87.5c    
App - Coffee_Machine Header  Packet_length   Command   Water_Capacity Water_Temperature Coffee_weight Coffee_scale  Brew time   Coffee_Temperature   Reserved    CRC  
Example							  0xAA         12          0xD1         0x05          0x5E, 0x05    0x0C, 0x05       0x02         0x14            0x57, 0x05        0xFF     0xNN  
Byte                    1          1            1           1                 2            2              1           1                  2               1        1  
             
             
   0x81: receive command successful          
Device - App       Hearder         Packet length       Command     Status         CRC        
Example             0xAA               3                 0x81      Fail : 1
                                                                  Success: 0      0xNN        
Byte                 1                  1                  1          1             1        
             
   0x82: brew coffee successful          
Device - App       Hearder Packet length     Command        Status        CRC        
Example             0xAA        3             0x82         Fail : 1
                                                          Success: 0     0xNN        
Byte                 1          3              1             1             1        
      


**/

 
#define  APPToDev_RX_BUF_MAX_LEN 15
#define  DevToAPP_TX_BUF_MAX_LEN 5
extern struct  STRUCT_APPToDev_Packet                                  //串口数据帧的处理结构体
{
  union
	{
		char  Data_RX_BUF [ APPToDev_RX_BUF_MAX_LEN ];
		struct
		{
		unsigned	char  u8header;
		unsigned	char  u8packetLength;
		unsigned	char  u8cmd;
		unsigned	char  u8waterCap;
		unsigned	short u16waterTemp;
		unsigned	short u16coffeeWeight;
		unsigned	char  u8coffeeScale;
		unsigned  char  u8brewTime;
		unsigned	short u16coffeeTemp;
		unsigned	char  Reserved1;
		unsigned	char  u8checkSum;	
		unsigned	char  Reserved2;
		}str_Packet;
	};
	unsigned  char  flag; // flag:0  无效     flag:1 有效
} strAppToDev_Packet_Record;

extern struct  STRUCT_DevToAPP_Packet                                  //串口数据帧的处理结构体
{
  union
	{
		char  Data_RX_BUF [ DevToAPP_TX_BUF_MAX_LEN ];
		struct
		{
		unsigned	char  u8header;
		unsigned	char  u8packetLength;
		unsigned	char  u8cmd;
		unsigned	char  u8errState;// success:0  fail:!0
		unsigned	char  u8checkSum;	
		}str_Packet;
	};
	unsigned  char  flag; // flag:0  无效     flag:1 有效
} strDevToApp_Packet_Record;

unsigned char 						CheckSum										(unsigned char *buf, int len);


#endif


