#include "uart_slave.h"
#include "common.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "bsp_SysTick.h"
#include "misc.h"

#define DBGcmd(X,...)  printf(X,##__VA_ARGS_)
static void                   UART_SLAVE_GPIO_Config                 ( void );
static void                   UART_SLAVE_USART_Config                ( void );
static void                   UART_SLAVE_USART_NVIC_Configuration    ( void );



struct  STRUCT_USARTx_Fram_UART_SLAVE strUART_SLAVE_Fram_Record = { 0 };



/**
  * @brief  UART_SLAVE3?¨º??¡¥o¡¥¨ºy
  * @param  ?T
  * @retval ?T
  */
void UART_SLAVE_Init ( void )
{
	UART_SLAVE_GPIO_Config (); 
	
	UART_SLAVE_USART_Config (); 
	
	
	macUART_SLAVE_RST_HIGH_LEVEL();

	macUART_SLAVE_CH_DISABLE();
	
	
}


/**
  * @brief  3?¨º??¡¥UART_SLAVE¨®?¦Ì?¦Ì?GPIO¨°y??
  * @param  ?T
  * @retval ?T
  */
static void UART_SLAVE_GPIO_Config ( void )
{
	#if 0

	/*?¡§¨°?¨°???GPIO_InitTypeDef¨¤¨¤D¨ª¦Ì??¨¢11¨¬?*/
	GPIO_InitTypeDef GPIO_InitStructure;


	/* ???? CH_PD ¨°y??*/
	macUART_SLAVE_CH_PD_APBxClock_FUN ( macUART_SLAVE_CH_PD_CLK, ENABLE ); 
											   
	GPIO_InitStructure.GPIO_Pin = macUART_SLAVE_CH_PD_PIN;	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_Init ( macUART_SLAVE_CH_PD_PORT, & GPIO_InitStructure );	 

	
	/* ???? RST ¨°y??*/
	macUART_SLAVE_RST_APBxClock_FUN ( macUART_SLAVE_RST_CLK, ENABLE ); 
											   
	GPIO_InitStructure.GPIO_Pin = macUART_SLAVE_RST_PIN;	

	GPIO_Init ( macUART_SLAVE_RST_PORT, & GPIO_InitStructure );	 
	#endif

}



/**
  * @brief  3?¨º??¡¥UART_SLAVE¨®?¦Ì?¦Ì? USART
  * @param  ?T
  * @retval ?T
  */
static void UART_SLAVE_USART_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	
	/* config USART clock */
	macUART_SLAVE_USART_APBxClock_FUN ( macUART_SLAVE_USART_CLK, ENABLE );
	macUART_SLAVE_USART_GPIO_APBxClock_FUN ( macUART_SLAVE_USART_GPIO_CLK, ENABLE );
	
	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  macUART_SLAVE_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macUART_SLAVE_USART_TX_PORT, &GPIO_InitStructure);  
  
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = macUART_SLAVE_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macUART_SLAVE_USART_RX_PORT, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = macUART_SLAVE_USART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(macUART_SLAVE_USARTx, &USART_InitStructure);
	
	
	/* ?D?????? */
	USART_ITConfig ( macUART_SLAVE_USARTx, USART_IT_RXNE, ENABLE ); //¨º1?¨¹¡ä??¨²?¨®¨º??D?? 
	USART_ITConfig ( macUART_SLAVE_USARTx, USART_IT_IDLE, ENABLE ); //¨º1?¨¹¡ä??¨²¡Á¨¹?????D?D?? 	
	USART_ITConfig(macUART_SLAVE_USARTx, USART_IT_TXE, DISABLE);

	UART_SLAVE_USART_NVIC_Configuration ();
	
	
	USART_Cmd(macUART_SLAVE_USARTx, ENABLE);
	
	
}


/**
  * @brief  ???? UART_SLAVE USART ¦Ì? NVIC ?D??
  * @param  ?T
  * @retval ?T
  */
static void UART_SLAVE_USART_NVIC_Configuration ( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig ( macNVIC_PriorityGroup_x );

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = macUART_SLAVE_USART_IRQ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


/*
 * @name¡êoUART_SLAVE_Rst
 * @brief ¡êo????WF-UART_SLAVE?¡ê?¨¦
 * @input  ¡êo?T
 * @return  : ?T
 */
void UART_SLAVE_Rst ( void )
{

	macUART_SLAVE_RST_LOW_LEVEL();
	Delay_ms ( 200 ); 
	macUART_SLAVE_RST_HIGH_LEVEL();
	 


}

/*
 * @name¡êoRmoveNullByte
 * @brief  ?¨²2a¨º?1y3¨¬?D ¡¤¡é??  esp ¨ºy?Y¡ã¨¹¦Ì¨²¨°???¡Á?¡¤? ?¨¦?¨¹?a'\0'?¨¢¨º?¡¤?  ¦Ì??? ?T¡¤¡§¡ä¨°¨®? 
 * @input  ¡êo?T
 * @return  : ?T
 */
static void RmoveNullByte(char * byte, int len)
{
	int i;
	for(i = 0; i<len; i++)
	{
		if(0==byte[i])
		{
			byte[i] = 0x0d;//?¨¹???a??3¦Ì¡¤?
			return;
		}
	}
}

/*
 * @name¡êoUART_SLAVE_Cmd
 * @brief  ¡êo??WF-UART_SLAVE?¡ê?¨¦¡¤¡é?¨ªAT??¨¢?
 * @input  ¡êocmd¡ê?¡äy¡¤¡é?¨ª¦Ì???¨¢?
 *         reply1¡ê?reply2¡ê??¨²¡äy¦Ì??¨¬¨®|¡ê??aNULL¡À¨ª2?D¨¨?¨¬¨®|¡ê?¨¢????a?¨°???-1??¦Ì
 *         waittime¡ê?¦Ì¨¨¡äy?¨¬¨®|¦Ì?¨º¡À??
 * @return   : 1¡ê???¨¢?¡¤¡é?¨ª3¨¦1|
 *         		 0¡ê???¨¢?¡¤¡é?¨ª¨º¡ì¡ã¨¹

 */

bool UART_SLAVE_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{   
//	int byteCount;
          
	
	macUART_SLAVE_Usart ( "%s\r\n", cmd);
	strUART_SLAVE_Fram_Record .InfBit .FramLength = 0;     //¡ä¨®D??a¨º??¨®¨º?D?¦Ì?¨ºy?Y¡ã¨¹
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //2?D¨¨¨°a?¨®¨º?¨ºy?Y
		return true;
	
	Delay_ms ( waittime );                 //?¨®¨º¡À
	
	strUART_SLAVE_Fram_Record .Data_RX_BUF [ strUART_SLAVE_Fram_Record .InfBit .FramLength ]  = '\0';
	RmoveNullByte(strUART_SLAVE_Fram_Record .Data_RX_BUF,strUART_SLAVE_Fram_Record .InfBit .FramLength);
	printf("Length = %d\r\n",strUART_SLAVE_Fram_Record .InfBit .FramLength );
	/*
	if('H'==*cmd)
	{
	for(byteCount = 0; byteCount <1024;byteCount++)
	{
		printf("%c",strUART_SLAVE_Fram_Record .Data_RX_BUF[byteCount]);	
	}
	}
*/
	 printf( "CMDEack= %s\r\n", strUART_SLAVE_Fram_Record .Data_RX_BUF );
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strUART_SLAVE_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strUART_SLAVE_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strUART_SLAVE_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strUART_SLAVE_Fram_Record .Data_RX_BUF, reply2 ) );
	
}









#define IF_CHK_SUM  0

char * Slave_UART_ReceiveHex (char *buf)
{
	char * pRecStr = 0;
	int i;
	
	strUART_SLAVE_Fram_Record .InfBit .FramLength = 0;
	strUART_SLAVE_Fram_Record .InfBit .FramFinishFlag = 0;
	
	while ( ! strUART_SLAVE_Fram_Record .InfBit .FramFinishFlag );

	pRecStr = strUART_SLAVE_Fram_Record.Data_RX_BUF;
	for(i = 0; (i < strUART_SLAVE_Fram_Record .InfBit .FramLength)&&(i < 32); i++)
	{
		buf[i]     = strUART_SLAVE_Fram_Record.Data_RX_BUF[i];
		
	}
	
	
	
#if IF_CHK_SUM
	if(0==verify_chksum(buf))
		return NULL;//check sum  error
#endif
	return pRecStr;
	
}



char verify_chksum(char * buf)
{
	int i;
	char chksum;
	char len;
	char cmd_chksum;
	char cmd_id_offset = 2;
	char chksum_offset;
	len = buf[1];
	chksum_offset = cmd_id_offset + len;
	cmd_chksum = buf[chksum_offset];
	if(0xaa==buf[0])
	{
		for(i = cmd_id_offset; i < (cmd_id_offset + len); i++)
		chksum 		 |=  (~buf[i]);
	}
	if(cmd_chksum == chksum)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
NEED change
*/
int cmd_reply(char *buf,int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		USART_SendData(USART2,buf[i]);	
		while( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );	
	}
	return i;
}
#define CMD_HEADER  0XAA
#define VERSION_H  0X78
#define VERSION_L  0X45
int process_cmd(char *arg, int size)
{
	
//	char master_len  =	 arg[1];
	char comment_set = 	 arg[2];
//	char comment_num =   arg[3];
//	char buf[32];
	
	if(CMD_HEADER != arg[0])
	{
		printf("arg[0] = %x\n",arg[0]);
		printf("header err\n");
	}

	if(size < 4)
	{
		printf("cmd err\n");
		return 0;
	}
	comment_set = comment_set&0xf0;
	switch(comment_set)
	{
		case 0x10:  //version
			process_sys_cmd(arg);
			break;
		case 0x20://boiler
			process_boil_cmd(arg);
			break;
		case 0x40://grind
			process_grind_cmd(arg);
			break;
		case 0x50:
		case 0x60:
			process_milk_cmd(arg);
			break;
		default:
			printf("cmd out range\n");
			break;
	}
	return 1;
}

struct _rev_packet_{
	char start_head;
	char length;
	char cmd_id;
	char payload[16];
	
	char chksum;
};
typedef struct _rev_packet_ T_REV_PACKET;
struct _send_packet_{
	char start_head;
	char length;
	char cmd_id;
	char payload[16];
	char ack;
	char chksum;
};
typedef struct _send_packet_ T_SEND_PACKET;
int mymemcpy(char * des, char * src, int size)
{
	int i = size;
	if((NULL==des)||(NULL==src))
		return -1;
	for(i = 0; i<size; i++)
	{
		des[i] = src[i];
	}
	return 0;
}
char check_sum(char *arg,char size)
{
	int i;
	char sum;
	for(i = 0; i < size; i++)
	{
			sum |= ~(arg[i]);
	}
	return sum;
}

#define FW_MAJOR_VERSION  0x26
#define FW_MINOR_VERSION  0x74

#define HW_MAJOR_VERSION  0X78
#define HW_MINOR_VERSION  0X25



struct _struc_sys_sta
{
	char dev_cur_sta;
	char dev_coffee_bean_tank_sta;
	char water_tank_sta;
	char cofe_cake_tank_sta;
	char waste_water_tank_sta;

};
typedef struct _struc_sys_sta T_SYS_STA;
struct _struc_sys_ 
{
	char set_reset;
	char set_restore_faty_setting;
	char set_enter_deep_sleep;
	
};

typedef  struct _struc_sys_ T_SYS_CTL;
void process_sys_cmd(char *arg)
{
	T_SEND_PACKET send;
	char buf[16];
//	char offset = 0;
//	char ack;
//	char send_size;
	switch(arg[2])
	{
		case 0x10:
			printf("send version\n");
			send.start_head    = 0xaa;
			send.length        = 0x7;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			send.payload[1]    = FW_MAJOR_VERSION;
			send.payload[2]    = FW_MINOR_VERSION;
			send.payload[3]    = HW_MAJOR_VERSION;
			send.payload[4]    = HW_MINOR_VERSION;
			send.payload[5]    = 0;
			send.chksum        = check_sum((&(send.length)+1),send.length);
			break;
		case 0x11://device reset
			printf("device reset\n");
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			send.payload[1]    = 0x00;
			send.chksum        = check_sum((&(send.length)+1),send.length);
			break;
		case 0x12: //restore to factory setting
			printf("restore to factory setting\n");
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			send.payload[1]    = 0x00;
			send.chksum        = check_sum((&(send.length)+1),send.length);
			break;
		
		case 0x13://enter deep sleep
			printf("enter deep sleep\n");
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			send.payload[1]    = 0x00;//ack
			send.chksum        = check_sum((&(send.length)+1),send.length);
			break;
		case 0x14: //current state
			printf("enter current state\n");
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			/*
			0x00 idle
			0x01 claibartion
			0x02 brewing coffee
			0x03 unit fault
			*/
			send.payload[1]    = 0x00;//state
			send.chksum        = check_sum((&(send.length)+1),send.length);
			break;
		case 0x15: //get cofe bean tank sta
			printf("enter current state\n");
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			/*
			0x00 Normal
			0x01 Empty
			0x02 tank removed
			*/
			send.payload[1]    = 0x00;//state
			send.chksum        = check_sum((&(send.length)+1),send.length);
			break;
		case 0x16: //get water  tank sta
			printf("enter current state\n");
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			/*
			0x00 Normal
			0x01 FULL
			0X02 empty
			0xff tank removed
			*/
			send.payload[1]    = 0x00;//state
			send.chksum        = check_sum((&(send.length)+1),send.length);
			break;
		case 0x17: //get cofe cake  tank sta
			printf("enter current state\n");
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			/*
			0x00 Normal
			0x01 FULL
			0X02 empty
			0xff tank removed
			*/
			send.payload[1]    = 0x00;//state
			send.chksum        = check_sum((&(send.length)+1),send.length);
			break;
		case 0x18: //get waste water  tank sta
			printf("enter current state\n");
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			/*
			0x00 Normal
			0x01 FULL
			0X02 empty
			0xff tank removed
			*/
			send.payload[1]    = 0x00;//state
			send.chksum        = check_sum((&(send.length)+1),send.length);
			break;
		default:
			printf("********************************\n");
			printf("cmd1x error\n");
			break;
	}
	mymemcpy(buf,(char*)&send,2+send.length);
	mymemcpy(&buf[2+send.length],(char*)(&send.chksum),1);
	cmd_reply(buf,3+send.length);
}
struct _sturct_pid_
{
		short prop;
		short inte;
		short deri;
};
typedef struct _sturct_pid_ T_PID;
struct _stru_boil_state_
{
	char valve_sta;
	char step_sta;
	char boiler_sta;
	char position_sw_sta;
};
typedef struct _stru_boil_state_ T_BOILER_STATE;
struct _struct_boiler_
{
	T_BOILER_STATE state;
	char set_in_valve_ctl;
	char set_out_valve_ctl;
	unsigned short set_bt_temperature;
	unsigned short get_cur_temperature;
	unsigned short get_bt_temperature;
	unsigned char set_compres_value;
	char get_step_limitation;
	char set_step_dir;
	char set_out_water_flow_value;
	char set_in_water_flow_value;
	char set_speed;
	char set_turn_if_heater;
	T_PID get_pid;
};
typedef struct _struct_boiler_  T_BOIL_CTL; 


/*
boiler
*/
T_BOIL_CTL  g_boil_ctl;
void process_boil_cmd(char *arg)
{
	T_SEND_PACKET send;
	char buf[16];
//	char offset = 0;
//	char ack;
//	char send_size;
	short temp;


	

	switch(arg[2])
	{
		case 0x20:  // boiler state
		printf("send boiler state\n");
		send.start_head    = 0xaa;
		send.length        = 0x7;
		send.cmd_id        = arg[2];
		send.payload[0]    = (arg[2]&0x0f) | 0x80;
		send.payload[1]    = g_boil_ctl.state.valve_sta;
		send.payload[2]    = g_boil_ctl.state.step_sta;
		send.payload[3]    = g_boil_ctl.state.boiler_sta;
		send.payload[4]    = g_boil_ctl.state.position_sw_sta;
		send.payload[5]    = 0;
		send.chksum        = check_sum((&send.cmd_id ),send.length);
		break;
		case 0x21://  valve input control
		printf("valve input control\n");
		g_boil_ctl.set_in_valve_ctl  =  arg[3];  // off on
		send.start_head    = 0xaa;
		send.length        = 0x3;
		send.cmd_id        = arg[2];
		/* 
		off_ack   0x81
		on_ack    0x82
		*/
		send.payload[0]    = (arg[2]&0x0f) | 0x80;
		send.payload[1]    = 0x00;
		send.chksum        = check_sum((&(send.cmd_id) ),send.length);
		break;
		case 0x22: //valve output control
			printf("valve output control\n");
			g_boil_ctl.set_out_valve_ctl  =  arg[3];
			send.start_head    = 0xaa;
		  send.length        = 0x3;
			send.cmd_id        = arg[2];
		  send.payload[0]    = (arg[2]&0x0f) | 0x80;
		  send.payload[1]    = 0x00;
			send.chksum        = check_sum((&(send.cmd_id)),send.length);
			break;
		
		case 0x23://boiler target temperature
			printf("boiler target temperature\n");
			temp = arg[3];
		  temp = (temp<<8)|(arg[4]);
		  g_boil_ctl.set_bt_temperature = temp;
			send.start_head    = 0xaa;
		  send.length        = 0x3;
			send.cmd_id        = arg[2];
		  send.payload[0]    = (arg[2]&0x0f) | 0x80;
		  send.payload[1]    = 0x00;//ack
			send.chksum        = check_sum(&(send.cmd_id),send.length);
			break;
		case 0x24: //get current temperature
			printf("get current temperature\n");
			send.start_head    = 0xaa;
		  send.length        = 0x4;
			send.cmd_id        = arg[2];
		/*
		 cur_temperature  =  get_curtemp(); 
		*/
		  
			send.payload[0]    =  (g_boil_ctl.get_cur_temperature>>8);
			send.payload[1]    = 	 g_boil_ctl.get_cur_temperature&0xff;
			send.payload[2]    = (arg[2]&0x0f) | 0x80;
			send.chksum        = check_sum(&(send.cmd_id),send.length);

			break;
		case 0x25: //set boiler compressor value
			printf("set boiler compressor value\n");
			g_boil_ctl.set_compres_value      = arg[3];
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;
			send.payload[1]    = 0;		  
		/*
		 cur_temperature  =  get_curtemp(); 
		*/
			send.chksum        = check_sum(&(send.cmd_id),send.length);
		 case 0x26: //set step motor move to limitation
			 printf("set step motor move to limitation\n");
			g_boil_ctl.set_step_dir       = arg[3];
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = g_boil_ctl.get_step_limitation;//¨¦??T???T  
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x27: //set step motor dir and speed
			printf("set step motor dir and speed\n");
			g_boil_ctl.set_step_dir       = arg[3];
			g_boil_ctl.set_speed          = arg[4];
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x28: //set_out_water_flow_value   
			printf("set_out_water_flow_value\n");
			g_boil_ctl.set_out_water_flow_value   = arg[3];
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x29: //set_in_water_flow_value
			printf("set_in_water_flow_value\n");
			g_boil_ctl.set_in_water_flow_value   = arg[3]; //0~250
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;  //set successful
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x2A: //HEATER  ON OFF  
			printf("HEATER  ON OFF\n");
			g_boil_ctl.set_turn_if_heater   = arg[3]; //0~250
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;  //set successful
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x2B: //set hearter temperature  PID 
			printf("set hearter temperature  PID \n");
			g_boil_ctl.get_pid.prop 	   = arg[3];
			g_boil_ctl.get_pid.prop       = (g_boil_ctl.get_pid.prop<<8)|arg[4];
			g_boil_ctl.get_pid.inte 	   = arg[5];
			g_boil_ctl.get_pid.inte       = (g_boil_ctl.get_pid.inte <<8)|arg[6];
			g_boil_ctl.get_pid.deri 	   = arg[7];
			g_boil_ctl.get_pid.deri       = (g_boil_ctl.get_pid.deri<<8)|arg[8];
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;  //set successful
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		default:
			printf("********************************\n");
			printf("cmd2x error\n");
			break;
	}
	mymemcpy(buf,(char*)&send,2+send.length);
	mymemcpy(&buf[2+send.length],(char*)(&send.chksum),1);
	cmd_reply(buf,3+send.length);
}

struct _stru_brewer_state_
{
	char get_solenoid_sta;
	char get_tamper_dc_motor_sta; //error spell
	char get_tamper_location;
	char get_position_sw_sta;
	
};
typedef struct _stru_brewer_state_  T_BREWER_STATE;
struct _stur_brewer_
{
	T_BREWER_STATE state;
	char set_tamper_to_target_pos;
	char get_dc_motor_tamper_sta;
	char set_cake_rm_solenoid_on;
	char set_cake_rm_solenoid_off;
	char get_cake_rm_sloenoid_sate;
	
};
typedef struct _stur_brewer_ T_BREWER_CTL;

T_BREWER_CTL  g_brewer_ctl;

void process_brewer_cmd(char *arg)
{
	T_SEND_PACKET send;
//	char buf[16];
//	char offset = 0;
//	char ack;
//	char send_size;
//	short temp;
	switch(arg[2])
	{
		case 0x30:  // brewer state
		printf("send brewer state\n");
		send.start_head    = 0xaa;
		send.length        = 0x7;
		send.cmd_id        = arg[2];
		send.payload[0]    = (arg[2]&0x0f) | 0x80;
		send.payload[1]    = g_brewer_ctl.state.get_solenoid_sta;
		send.payload[2]    = g_brewer_ctl.state.get_tamper_dc_motor_sta;
		send.payload[3]    = g_brewer_ctl.state.get_tamper_location;
		send.payload[4]    = g_brewer_ctl.state.get_position_sw_sta;
		send.payload[5]    = 0;
		send.chksum        = check_sum((&send.cmd_id ),send.length);
		break;
		case 0x31://  set_tamper_to_target_pos
		printf("set_tamper_to_target_pos\n");
		g_brewer_ctl.set_tamper_to_target_pos =  arg[3];  // off on
		send.start_head    = 0xaa;
		send.length        = 0x3;
		send.cmd_id        = arg[2];
		send.payload[0]    = (arg[2]&0x0f) | 0x80;
		send.payload[1]    = 0x00;
		send.chksum        = check_sum((&(send.cmd_id) ),send.length);
		break;
		case 0x32://  get_dc_motor_tamper_sta
		printf("get_dc_motor_tamper_sta\n");
		send.start_head    = 0xaa;
		send.length        = 0x3;
		send.cmd_id        = arg[2];
		send.payload[0]    = g_brewer_ctl.get_dc_motor_tamper_sta;
		send.payload[1]    = 0x00;
		send.chksum        = check_sum((&(send.cmd_id) ),send.length);
		break;
		case 0x33://  set_cake_rm_solenoid_on
		printf("set_cake_rm_solenoid_on\n");
		g_brewer_ctl.set_cake_rm_solenoid_on=  arg[3];  // off on
		send.start_head    = 0xaa;
		send.length        = 0x3;
		send.cmd_id        = arg[2];
		send.payload[0]    = 0x00;//fault
		send.payload[1]    = 0x81;
		send.chksum        = check_sum((&(send.cmd_id) ),send.length);
		break;
		case 0x34://  set_cake_rm_solenoid_off
		printf("set_cake_rm_solenoid_off\n");
		g_brewer_ctl.set_cake_rm_solenoid_off=  arg[3];  // off on
		send.start_head    = 0xaa;
		send.length        = 0x3;
		send.cmd_id        = arg[2];
		send.payload[0]    = 0x00;//fault
		send.payload[1]    = 0x81;
		send.chksum        = check_sum((&(send.cmd_id) ),send.length);
		break;
		case 0x35://  get_cake_rm_sloenoid_sate
		printf("get_cake_rm_sloenoid_sate\n");
		
		send.start_head    = 0xaa;
		send.length        = 0x3;
		send.cmd_id        = arg[2];
		send.payload[0]    = g_brewer_ctl.get_cake_rm_sloenoid_sate;//fault
		send.payload[1]    = 0x81;
		send.chksum        = check_sum((&(send.cmd_id) ),send.length);
		break;
		default:
		printf("\n***********************************\n");
		printf("cmd 0x3  error\n");
	
};
	
}
struct _stru_grind_state_
{
	char dc_mot_sta;
	char step_mot_sta;
	char weight_sta;
	char posi_sw_sta;
};
typedef struct _stru_grind_state_  T_GRIND_STATE;
struct _stur_grind_
{
	T_GRIND_STATE state;
	char get_distance;
	char set_location;
	char get_step_cer_location;
	char get_step_cer_sta;
	unsigned short get_cur_temperature;
	short set_coffee_weight;
	short set_sample_num;
	short get_sample_num;
	short get_cer_coffee_weight;
	short get_cur_coffee_weight;
	char  set_step_dir;
	char set_motor_stop;
	char set_cer_time_on;
	
};
typedef struct _stur_grind_ T_GRIND_CTL;
/*
grind
*/
T_GRIND_CTL  g_grind_ctl;
void process_grind_cmd(char *arg)
{
	T_SEND_PACKET send;
	char buf[16];
//	char offset = 0;
//  char ack;
//	char send_size;
//	short temp;

	switch(arg[2])
	{
		case 0x40:  // grind state
			printf("send grind state\n");
			send.start_head    = 0xaa;
			send.length        = 0x7;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;
			send.payload[1]    = g_grind_ctl.state.dc_mot_sta;
			send.payload[2]    = g_grind_ctl.state.posi_sw_sta;
			send.payload[3]    = g_grind_ctl.state.step_mot_sta;
			send.payload[4]    = g_grind_ctl.state.weight_sta;
			send.payload[5]    = 0;
			send.chksum        = check_sum((&send.cmd_id ),send.length);
			break;
		case 0x41:// set calibrate grind size step motor
			printf("set calibrate grind size step motor\n");
			send.start_head    = 0xaa;
		  send.length        = 0x3;
			send.cmd_id        = arg[2];
		/* 
			off_ack   0x81
		  on_ack    0x82
    */
		  send.payload[0]    = g_grind_ctl.get_distance;
		  send.payload[1]    = 0x00;
			send.chksum        = check_sum((&(send.cmd_id) ),send.length);
			break;
		case 0x42: //set step motor to certain location
			printf("Set step motor to certain location\n");
			g_grind_ctl.set_location = arg[3];
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			send.payload[1]    = 0x00;
			send.chksum        = check_sum((&(send.cmd_id)),send.length);
			break;
		
		case 0x43://get step motor to certain location
			printf("Get step motor to certain location]\n");
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = g_grind_ctl.get_step_cer_location;

			send.payload[1]    = g_grind_ctl.get_step_cer_sta;//ack
			send.chksum        = check_sum(&(send.cmd_id),send.length);
			break;
		case 0x44: //get current temperature
			printf("Get current temperature\n");
		      g_grind_ctl.set_coffee_weight = arg[3];
			g_grind_ctl.set_coffee_weight =( g_grind_ctl.set_coffee_weight <<8) |g_grind_ctl.set_coffee_weight; 
			send.start_head    = 0xaa;
		  send.length        = 0x3;
			send.cmd_id        = arg[2];
		/*
		 cur_temperature  =  get_curtemp(); 
		*/
		  
			send.payload[0]    =  0x81;
			send.payload[1]    = 	 g_grind_ctl.get_cur_temperature&0xff;
			
			send.chksum        = check_sum(&(send.cmd_id),send.length);

			break;
		case 0x45: //set coffe certain weight value 
			printf("Set coffe certain weight value \n");
			g_grind_ctl.set_sample_num    = arg[3];
			g_grind_ctl.set_sample_num    = (g_grind_ctl.set_sample_num <<8)|g_grind_ctl.set_sample_num;
			send.length        = 0x6;
			send.cmd_id        = arg[2];
			send.payload[0]    = (g_grind_ctl.get_sample_num>>8)&0xff;
			send.payload[1]    =  g_grind_ctl.get_sample_num&&0xff;	
			send.payload[2]    = ( g_grind_ctl.get_cer_coffee_weight>>8)&0xff;
			send.payload[3]    = g_grind_ctl.get_cer_coffee_weight&0xff;
			send.payload[4]    = 0x00;
		/*
		 cur_temperature  =  get_curtemp(); 
		*/
			send.chksum        = check_sum(&(send.cmd_id),send.length);
			break;
		 case 0x46: //set step motor move to limitation
			printf("Set step motor move to limitation \n");
			g_grind_ctl.set_step_dir       = arg[3];
			send.start_head    = 0xaa;
			send.length        = 0x4;
			send.cmd_id        = arg[2];
			send.payload[0]    = (g_grind_ctl.get_cur_coffee_weight>>8)&0xff;//¨¦??T???T
			send.payload[1]    = g_grind_ctl.get_cur_coffee_weight&0xff;
			send.payload[2]    = 0x00;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x47: //set grind size DC motor stop ´íÎó
			printf("Set grind size DC motor stop \n");
			g_grind_ctl.set_motor_stop   = arg[3];
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x48: //set grind motor certian time on 
			printf("Set grind motor certian time on  \n");			
			g_grind_ctl.set_cer_time_on   = arg[3];
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		default:
			printf("********************************\n");
			printf("cmd4x error\n");
			break;
	}
	mymemcpy(buf,(char*)&send,2+send.length);
	mymemcpy(&buf[2+send.length],(char*)&send.chksum,1);
	cmd_reply(buf,3+send.length);
}

struct _stru_milk_state_
{
	char get_valve_sta;
	char get_solenoid_sta;
	char get_forther_motor_sta;
	char get_milk_heater_sta;
	char get_water_pump_state;
	char get_air_pump_sta;
	
};
typedef struct _stru_milk_state_  T_MILK_STATE;
struct _stur_milk_
{
	T_MILK_STATE state;
	T_PID set_steamer_temp_pid;
	char set_if_steamer_heater;
	short set_steamer_temp;
	short get_cur_steamer_temp;
	char set_water_pump_on_duration;
	char set_water_pump_off;
	char get_steamer_boiler_sta;
	char get_water_pump_sta;
	short set_cer_valves_solenoid_sta_if;
	short get_cer_valves_solenoid_sta_if;
	char set_air_pump_on_duration;
	char set_air_in_pump_off;
	char get_air_in_pump_sta;
	char set_frother_move_to_certian_pos;
	char set_frother_to_top_pos;
	char set_frother_to_bottom_pos;
	char get_frother_cur_pos;
	char get_cur_ir_milk_temp;
	char set_milk_target_temp;
	char set_milk_heater_on_duration;
	char set_milk_heater_off;
	char get_milk_temp_raise_rate;
	
      
};
typedef struct _stur_milk_ T_MILK_CTL;
/*
grind
*/
T_MILK_CTL  g_milk_ctl;
void process_milk_cmd(char *arg)
{
	T_SEND_PACKET send;
	char buf[16];
//	char offset = 0;
//  char ack;
//	char send_size;
//	short temp;

	switch(arg[2])
	{
		case 0x50:  // grind state
			printf("send milk ctl state\n");
			send.start_head    = 0xaa;
			send.length        = 0x9;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;
			send.payload[1]    = g_milk_ctl.state.get_valve_sta;
			send.payload[2]    = g_milk_ctl.state.get_solenoid_sta;
			send.payload[3]    = g_milk_ctl.state.get_forther_motor_sta;
			send.payload[4]    = g_milk_ctl.state.get_milk_heater_sta;
			send.payload[5]    = g_milk_ctl.state.get_water_pump_state;
			send.payload[6]    = g_milk_ctl.state.get_air_pump_sta;
			send.payload[7]    = 00;
			send.chksum        = check_sum((&send.cmd_id ),send.length);
			break;
		case 0x52:// set calibrate grind size step motor
			printf("set steamer temp pid\n");
			g_milk_ctl.set_steamer_temp_pid.prop = arg[3];
			g_milk_ctl.set_steamer_temp_pid.prop = (g_milk_ctl.set_steamer_temp_pid.prop<<8)|arg[4] ;
			g_milk_ctl.set_steamer_temp_pid.inte= arg[5];
			g_milk_ctl.set_steamer_temp_pid.inte= (g_milk_ctl.set_steamer_temp_pid.inte<<8)|arg[6] ;
			g_milk_ctl.set_steamer_temp_pid.deri= arg[7];
			g_milk_ctl.set_steamer_temp_pid.deri= (g_milk_ctl.set_steamer_temp_pid.deri<<8)|arg[8] ;
			
			/* 
			off_ack   0x81
			on_ack    0x82
			*/
			send.payload[0]    = 0;//ack_active
			send.payload[1]    = 0x00;
			send.chksum        = check_sum((&(send.cmd_id) ),send.length);
			break;
		case 0x53: //set steam heater on off
			printf("set steam heater on off\n");
			g_milk_ctl.set_if_steamer_heater = arg[3];
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (arg[2]&0x0f) | 0x80;
			send.payload[1]    = 0x00; //state
			send.chksum        = check_sum((&(send.cmd_id)),send.length);
			break;
		
		case 0x54://set steamer target temp
			printf("set steamer target temp\n");
			g_milk_ctl.set_steamer_temp = arg[3];
			g_milk_ctl.set_steamer_temp = (g_milk_ctl.set_steamer_temp<<8)| arg[4];
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;

			send.payload[1]    = 0;//ack
			send.chksum        = check_sum(&(send.cmd_id),send.length);
			break;
		case 0x55: //get steamer current temp
			printf("Get  steamer current temperature\n");
			send.start_head    = 0xaa;
			send.length           =    0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (g_milk_ctl.get_cur_steamer_temp>>8)&0xff;
			send.payload[1]    = g_milk_ctl.get_cur_steamer_temp&0xff;
			send.payload[2]    =  0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);

			break;
		case 0x56: //set water pump on and duration 
			printf("set water pump on and duration \n");
		      g_milk_ctl.set_water_pump_on_duration  = arg[3];
			
			send.length          = 0x3;
			send.cmd_id         = arg[2];
			send.payload[0]    = 0x81;
			send.payload[1]    =  0;//ack	
	
	
			send.chksum        = check_sum(&(send.cmd_id),send.length);
			break;
		 case 0x57: //set_water_pump_off  
			printf("set_water_pump_off\n");
			g_milk_ctl.set_water_pump_off= arg[3];
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = (g_grind_ctl.get_cur_coffee_weight>>8)&0xff;//¨¦??T???T
			send.payload[1]    = g_grind_ctl.get_cur_coffee_weight&0xff;
			send.payload[2]    = 0x00;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x58: //get_steamer_boi ´íÎó
			printf("get_steamer_boiler status \n");
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = g_milk_ctl.get_steamer_boiler_sta ;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x59: //get_water_pump_sta 
			printf("get_water_pump_sta  \n");			
			
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = g_milk_ctl.get_water_pump_sta;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x5A: //set_cer_valves_solenoid_sta_if  
			printf("set_cer_valves_solenoid_sta_if   \n");			
			g_milk_ctl.set_cer_valves_solenoid_sta_if  = arg[3];
			g_milk_ctl.set_cer_valves_solenoid_sta_if  = (g_milk_ctl.set_cer_valves_solenoid_sta_if<<8)|arg[4];
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x5B: //  get_cer_valves_solenoid_sta_if
			printf("get_cer_valves_solenoid_sta_if   \n");			
		
			
			send.start_head    = 0xaa;
			send.length        = 0x4;
			send.cmd_id        = arg[2];
			send.payload[0]    = (g_milk_ctl.get_cer_valves_solenoid_sta_if>>8)&0xff;//¨¦??T???T
			send.payload[1]    = (g_milk_ctl.get_cer_valves_solenoid_sta_if)&0xff ;
			send.payload[2]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x5c://set_air_pump_on_duration
			printf("set_air_pump_on_duration   \n");			
			g_milk_ctl.set_air_pump_on_duration= arg[3];
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x5D://set_air_in_pump_off
			printf("set_air_in_pump_off   \n");			
			g_milk_ctl.set_air_in_pump_off = arg[3];
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x5E://get_air_in_pump_sta
			printf("get_air_in_pump_sta   \n");			
			
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = g_milk_ctl.get_air_in_pump_sta;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x60://set_frother_move_to_certian_pos
			printf("set_frother_move_to_certian_pos   \n");			
			g_milk_ctl.set_frother_move_to_certian_pos = arg[3];
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x61://set_frother_to_top_pos
			printf("set_frother_to_top_pos   \n");			
			g_milk_ctl.set_frother_to_top_pos = arg[3];
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x62://set_frother_to_bottom_pos
			printf("set_frother_to_bottom_pos   \n");			
			g_milk_ctl.set_frother_to_bottom_pos= arg[3];
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x63://get_frother_cur_pos
			printf("get_frother_cur_pos   \n");			
			
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = g_milk_ctl.get_frother_cur_pos;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x64://get_cur_ir_milk_temp
			printf("get_cur_ir_milk_temp   \n");			
			
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = g_milk_ctl.get_cur_ir_milk_temp;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x65://set_milk_target_temp
			printf("set_milk_target_temp   \n");			
			g_milk_ctl.set_milk_target_temp = arg[3];
			g_milk_ctl.set_milk_target_temp = (g_milk_ctl.set_milk_target_temp <<8)|g_milk_ctl.set_milk_target_temp;
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x66://set_milk_heater_on_duration
			printf("set_milk_heater_on_duration   \n");			
			g_milk_ctl.set_milk_heater_on_duration= arg[3];
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x67://set_milk_heater_off
			printf("set_milk_heater_off   \n");			
			g_milk_ctl.set_milk_heater_off= arg[3];
			
			send.start_head    = 0xaa;
			send.length        = 0x3;
			send.cmd_id        = arg[2];
			send.payload[0]    = 0x81;//¨¦??T???T
			send.payload[1]    = 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;
		case 0x68://get_cur_ir_milk_temp
			printf("get_cur_ir_milk_temp   \n");			
			
			
			send.start_head    = 0xaa;
			send.length        = 0x4;
			send.cmd_id        = arg[2];
			send.payload[0]    = (g_milk_ctl.get_milk_temp_raise_rate>>8)&0xff;//¨¦??T???T
			send.payload[1]    = g_milk_ctl.get_milk_temp_raise_rate&0xff;
			send.payload[2] 	= 0;
			send.chksum        = check_sum(&(send.cmd_id),send.length);	
			break;	
		default:
			printf("********************************\n");
			printf("cmd5x error\n");
			break;
	}
	mymemcpy(buf,(char*)&send,2+send.length);
	mymemcpy(&buf[2+send.length],(char*)&send.chksum,1);
	cmd_reply(buf,3+send.length);
}


void test_uart_slave(void)
{
	char cmd_buf[32];
	int len;
	int i;
	UART_SLAVE_Init();
	UART_SLAVE_Cmd("hello\n",NULL,NULL,100);
	printf("test cmd of coffee machine start.....\n");
	while(1)
	{
		Slave_UART_ReceiveHex(cmd_buf);
		len = strUART_SLAVE_Fram_Record .InfBit .FramLength;
		for(i = 0; i < len; i++)
		{
		printf("cmd_buf[%d] = 0x%x \n",i,cmd_buf[i]);
		}	
		printf("\n");
		//process_cmd(cmd_buf,len);
	}

}






