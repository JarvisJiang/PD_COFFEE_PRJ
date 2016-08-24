#include "i2c_gy906.h"
//#include "usart.h"

void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
#define printf(fmt,...)   USART_OUT(USART1,fmt,##__VA_ARGS__)
#define uint unsigned int
#define uchar unsigned char
#define Nack_counter 10

//mlx90614 端口定义
#define SCL_H  GPIO_SetBits (GPIOC, GPIO_Pin_12)
#define SCL_L  GPIO_ResetBits (GPIOC, GPIO_Pin_12)

#define SDA_H  GPIO_SetBits (GPIOC, GPIO_Pin_11)
#define SDA_L  GPIO_ResetBits (GPIOC, GPIO_Pin_11)


#define SDA_READ GPIOC->IDR & GPIO_Pin_11
//************数据定义****************
unsigned char  flag;//可位寻址数据
unsigned char bit_out;//=flag^7;
unsigned char bit_in;//=flag^0;
uchar DataH,DataL,Pecreg;
//************函数声明*****************************************
void start_bit(void); //MLX90614 发起始位子程序
void stop_bit(void); //MLX90614 发结束位子程序
uchar rx_byte(void); //MLX90614 接收字节子程序
void send_bit(void); //MLX90614 发送位子程序
void tx_byte(uchar dat_byte); //MLX90614 接收字节子程序
void receive_bit(void); //MLX90614 接收位子程序
void delay(uint N); //延时程序
uint memread(void); //读温度数据



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

static void Delayus(int us)
{
	int i;
	int j = 20;
	for(i = 0 ;  i < us ; i++)
	{
		do
		{
			__asm("NOP");
		}while(j--);
		j = 20; // 72 MHZ
	}
}

static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// ??? I2C2 ?????
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);


	// PB6-I2C1_SCL?PB7-I2C1_SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // ????
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
//*************主函数*******************************************
void test_gy906()
{
uint Tem;
I2C_GPIO_Config();
//函数部分
SCL_H;SDA_H;
Delayus(2);
SCL_L;
DelayMs(1);
SCL_H;

while(1)
{
Tem=memread();
USART_OUT(USART1,"Tem = %d\n", Tem);
DelayMs(1000);
}
}

//************************************
void start_bit(void)
{
SDA_H;
Delayus(1);
SCL_H;
Delayus(1);
SDA_L;
Delayus(1);
SCL_L;
Delayus(1);
}
//------------------------------
void stop_bit(void)
{
SDA_L;
Delayus(1);
SCL_H;
Delayus(1);
SDA_H;
Delayus(1);
SCL_L;
Delayus(1);
}
//---------发送一个字节---------
void tx_byte(uchar dat_byte)
{
char i,n,dat;
n=Nack_counter;
TX_again:
dat=dat_byte;
for(i=0;i<8;i++)
{
if(dat&0x80)
bit_out=1;
else
bit_out=0;
send_bit();
dat=dat<<1;
}
receive_bit();
if(bit_in==1)
{
stop_bit();
if(n!=0)
{n--;goto Repeat;}
else
goto exit;
}
else
goto exit;
Repeat:
//stop_bit();
start_bit();
goto TX_again;
exit: ;
}
//-----------发送一个位---------
void send_bit(void)
{
if(bit_out==0)
SDA_L;
else
SDA_H;
Delayus(2);
SCL_H;
Delayus(2);
SCL_L;
Delayus(2);
}
//----------接收一个字节--------
uchar rx_byte(void)
{
uchar i,dat;
dat=0;
SDA_H;
for(i=0;i<8;i++)
{
dat=dat<<1;
receive_bit();
if(bit_in==1)
dat=dat+1;
}
SCL_L;
send_bit();
return dat;
}
//----------接收一个位----------
void receive_bit(void)
{
SDA_H;bit_in=1;
SCL_H;
Delayus(1);
if(SDA_READ)
{
	bit_in=1;
}
else
{
	bit_in = 0;
}

Delayus(1);
SCL_L;
Delayus(1);
}
//------------延时--------------
void delay(uint N)
{
uint i;
for(i=0;i<N;i++)
Delayus(1);
}
//------------------------------
uint memread(void)
{
static	char addr;
	addr = 0;
stop_bit();
start_bit();
tx_byte(addr); //Send SlaveAddress
//addr++;
tx_byte(0x07); //Send Command
//------------
start_bit();
tx_byte(0x01);
bit_out=0;
DataL=rx_byte();
bit_out=0;
DataH=rx_byte();
bit_out=1;
Pecreg=rx_byte();
stop_bit();
return(DataH*256+DataL);
}

	#if 0
void display(uint Tem)
{
 unsigned int T,a,b;
T=Tem*2;
	unsigned char buf[10];

if(T>=27315)
{
T=T-27315;
a=T/100;
b=T-a*100;

//---------------------------
if(a>=100)
{
buf[0] = (0x30+a/100);
a=a%100;
buf[1] = (0x30+a/10);
a=a%10;
buf[2] = (0x30+a);
}
else if(a>=10)
{
buf[0] = (0x30+a/10);
a=a%10;
buf[1] =(0x30+a);
}
else
{
dis_dat_wrt(0x30+a);
}
dis_dat_wrt(0x2e);//???
//---------------------------
if(b>=10)
{
dis_dat_wrt(0x30+b/10);
// b=b%10;
// dis_dat_wrt(0x30+b);
}
else
{
dis_dat_wrt(0x30);
// dis_dat_wrt(0x30+b);
}
}
//===========================
else
{
T=27315-T;
a=T/100;
b=T-a*100;
dis_dat_wrt(0x2d);
//--------------------------
if(a>=10)
{
dis_dat_wrt(0x30+a/10);
a=a%10;
dis_dat_wrt(0x30+a);
}
else
{
dis_dat_wrt(0x30+a);
}
dis_dat_wrt(0x2e);//???
//--------------------------
if(b>=10)
{
dis_dat_wrt(0x30+b/10);
b=b%10;
dis_dat_wrt(0x30+b);
}
else
{
dis_dat_wrt(0x30);
dis_dat_wrt(0x30+b);
}
}

}
#endif

