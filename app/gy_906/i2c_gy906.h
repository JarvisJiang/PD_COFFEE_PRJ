#ifndef __I2C_EE_H
#define __I2C_EE_H
#include "stm32f10x.h"

#define EEPROM_Block0_ADDRESS 0xA4
#define EEP_Firstpage 0x00
#define I2C_Speed 400000
#define I2C2_OWN_ADDRESS7 0x4000
#define I2C_PageSize 16

void I2C_EE_Init(void);
void I2C_Test(void);
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);
void I2C_EE_WaitEepromStandbyState(void);
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
static void I2C_GPIO_Config(void);
static void I2C_Mode_Configu(void);
void gy906_read_temp(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
extern u8 Debug_value;
void test_gy906(void);
#endif

