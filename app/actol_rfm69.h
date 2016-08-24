#ifndef __ACTOL_RFM69_H
#define __ACTOL_RFM69_H

#include <stm32f10x.h>
#include "ws_spi.h"
#include "stm32f10x_spi.h"


#define  MCU_ADD_SCENE   0xD1
#define  MCU_REM_SCENE	 0xD3
#define  MCU_BRI_CONTROL	0xD5
#define  MCU_INQ_NET	0xD7
#define  MCU_GET_MAC  0xD9
#define  MCU_MAC_CONTROL 0xDA


#define LIT_ADD_SCENE_ACK		0x92
#define LTI_REM_SCENE_ACK		0x94
#define  LIT_INQ_NET_ACK	0x96
#define  LIT_GET_MAC_ACK  0x98


extern void Hander_MCU_rfm69(uint8_t *bdata, uint8_t Command, uint8_t lenght);

#endif /*__ACTOL_RFM69_H*/
