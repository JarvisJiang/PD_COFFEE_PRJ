#ifndef  __TEST_H
#define	 __TEST_H



#include "stm32f10x.h"
#include "bsp_esp8266.h"



/********************************** �û���Ҫ���õĲ���**********************************/
#define      macUser_ESP8266_BulitApSsid                  "JRJ"         //Ҫ�������ȵ������
#define      macUser_ESP8266_BulitApEcn                   OPEN               //Ҫ�������ȵ�ļ��ܷ�ʽ
#define      macUser_ESP8266_BulitApPwd                   "JRJ12345"         //Ҫ�������ȵ����Կ

#define      macUser_ESP8266_TcpServer_IP                 "192.168.123.169"  //������������IP��ַ
#define      macUser_ESP8266_TcpServer_Port               "8080"             //�����������Ķ˿�   

#define      macUser_ESP8266_TcpServer_OverTime           "1800"             //��������ʱʱ�䣨��λ���룩



/********************************** ���Ժ������� ***************************************/
bool					 ESP8266_Sta_ConnectAPPTest						(void);
bool 					 ESP8266_Sta_SmartLinkByPhone         (void);
bool 					 ESP8266_Sta_LinkCLD									(void);


#endif

