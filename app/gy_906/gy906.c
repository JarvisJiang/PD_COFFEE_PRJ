#include "gy906.h"

//u8 Table[16]={0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D};
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
#define PRINTF(fmt,...)   USART_OUT(USART1,fmt,##__VA_ARGS__)
void Gy906_Init(void)
{
	GY906_IIC_GPIO_Init();

}

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

u16 Gy906_Read(u8 addr)
{
	unsigned  short temp;
	unsigned  char  PEC1;
	unsigned  char  PEC2;
	unsigned  char  error=0;
	unsigned  char  arr[6];
//	u8 ak=0;
	u8 tempH=0,tempL=0;
	do
	{
repeat:
		GY906_IIC__Stop();
		--error;
		if(!error)
		{break;}
		GY906_IIC__Start();
		GY906_IIC__Send(SAW);//?那足a米??迄?∩
//		ak=Wait_Ack();// 1車|∩e那∫～邦 0車|∩e3谷1|
		if(Wait_Ack())
		{
			goto	repeat;
		}
		GY906_IIC__Send(addr);
		if(Wait_Ack())
		{
			goto	repeat;
		}
		GY906_IIC__Start();
		GY906_IIC__Send(SAR);//
//		Wait_Ack();
		if(Wait_Ack())
		{
			goto	repeat;
		}
		tempL=GY906_IIC__Read(1);
		tempH=GY906_IIC__Read(1);
		PEC1=GY906_IIC__Read(0);
		GY906_IIC__Stop();
		arr[5] = SAW;                //
        arr[4] = addr;                        //
        arr[3] = SAR;        //Load array arr
        arr[2] = tempL;                                //
        arr[1] = tempH;                                //
        arr[0] = 0;                                        //
        PEC2=PEC_Calculation(arr);//Calculate CRC
		
	}
	while(PEC1!=PEC2);

	temp=(tempH<<8)|tempL;
	return temp;	
}

u8 PEC_Calculation(u8 pec[])
{
    unsigned  char        crc[6];
    unsigned  char         BitPosition=47;
    unsigned  char         shift;
    unsigned  char         i;
    unsigned  char         j;
    unsigned  char         temp;

    do
    {
        /*Load pattern value 0x000000000107*/
        crc[5]=0;
        crc[4]=0;
        crc[3]=0;
        crc[2]=0;
        crc[1]=0x01;
        crc[0]=0x07;

        /*Set maximum bit position at 47 ( six bytes byte5...byte0,MSbit=47)*/
        BitPosition=47;

        /*Set shift position at 0*/
        shift=0;

        /*Find first "1" in the transmited message beginning from the MSByte byte5*/
        i=5;
        j=0;
        while((pec[i]&(0x80>>j))==0 && i>0)
        {
            BitPosition--;
            if(j<7)
            {
                j++;
            }
            else
            {
                j=0x00;
                i--;
            }
        }/*End of while */

        /*Get shift value for pattern value*/
        shift=BitPosition-8;

        /*Shift pattern value */
        while(shift)
        {
            for(i=5; i<0xFF; i--)
            {
                if((crc[i-1]&0x80) && (i>0))
                {
                    temp=1;
                }
                else
                {
                    temp=0;
                }
                crc[i]<<=1;
                crc[i]+=temp;
            }/*End of for*/
            shift--;
        }/*End of while*/

        /*Exclusive OR between pec and crc*/
        for(i=0; i<=5; i++)
        {
            pec[i] ^=crc[i];
        }/*End of for*/
    }
    while(BitPosition>8); /*End of do-while*/

    return pec[0];
}


float Gy906_ReadTemp(void)
{
	unsigned short temp;
	float real;
	temp=Gy906_Read(0x07);
	real=temp*0.02-273.15;
	return real;
}

void Test_GY906(void)
{
	float Temp;
  int   temp_x10;
	float decimal;
	
	Gy906_Init();
	while(1)
	{
		Temp=Gy906_ReadTemp();
		temp_x10 = Temp *10;
		decimal = temp_x10%10;
		PRINTF("%d\r\n",Temp);
		PRINTF("temp = %f\r\n",Temp);
		PRINTF("temp_x10 = %d\r\n",temp_x10);
		temp_x10 = temp_x10/10;
		PRINTF("real temp = %d.%d\r\n",temp_x10,decimal);
		DelayMs(1000);
	}
}
