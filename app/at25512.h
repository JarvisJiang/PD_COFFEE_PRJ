#ifndef _AT25512_H_
#define _AT25512_H_

//Name    Instruction Format   Operation
#define WREN    0X06   // Set Write Enable Latch
#define WRDI    0X04   // Reset Write Enable Latch
#define RDSR    0X05   // Read Status Register
#define WRSR    0X01   // Write Status Register
#define READ    0X03   // Read Data from Memory Array
#define WRITE   0X02   // Write Data to Memory Array
#define ERASE   0XC7   // Erase whole eeprm

#define EEPROM_PAGE_SIZE 128  // page size for each page 

#define AT25512_Enable 	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
#define AT25512_Disable 	GPIO_SetBits(GPIOA,GPIO_Pin_4);

unsigned char AT25512_Read(unsigned long address);
void AT25512_Write(unsigned long address,unsigned char Byte);
void AT25512_init(void);

void AT25512_Write_buffer(unsigned long address,unsigned char *pHeader,unsigned int len);
void AT25512_Read_buffer(unsigned long address, unsigned char *pHeader,unsigned int len);

#endif

