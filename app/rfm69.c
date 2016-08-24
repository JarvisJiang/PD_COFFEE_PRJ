#include "rfm69h.h"

const u16 RFM69HFreqTbl[4][3] = 
{ 
  {0x074e, 0x08c0, 0x0900}, //315MHz
  {0x076c, 0x0880, 0x0900}, //434MHz
  {0x07d9, 0x0800, 0x0900}, //868MHz
  {0x07e4, 0x08c0, 0x0900}, //915MHz
};

const u16 RFM69HRateTbl[4][2] = 
{
  {0x0368, 0x042B},         //BR=1.2K BW=83.333K
  {0x0334, 0x0415},         //BR=2.4K BW=83.333K  
  {0x031A, 0x040B},         //BR=4.8K BW=83.333K
  {0x030D, 0x0405},         //BR=9.6K BW=83.333K
};

const u16 RFM69HPowerTbl[4][1] = 
{
  {0x117F},                 //20dbm
  {0x117C},                 //17dbm
  {0x1179},                 //14dbm  
  {0x1176},                 //11dbm
};

const u16 RFM69HConfigTbl[17] = 
{ 
  0x0202,                   //RegDataModul, FSK Packet, Gaussian filter, BT = 0.5
  0x0502,                   //RegFdevMsb, 241*61Hz = 35KHz  
  0x0641,                   //RegFdevLsb
//  0x130F,                   //RegOcp, Disable OCP
  0x1952,                   //RegRxBw , RxBW, 83KHz
//	0x1E00,										//Disable
  
  0x2C00,                   //RegPreambleMsb  
  0x2D05,                   //RegPreambleLsb, 3Byte Preamble
  0x2E90,                   //enable Sync.Word, 2+1=3bytes
  0x2FAA,                   //0xAA, SyncWord = aa2dd4
  0x302D,                   //0x2D
  0x31D4,                   //0xD4  
  0x3790,                   //RegPacketConfig1, Variable length,DC free off, Enable CRC,NRZ encode
  0x3840,                   //RegPayloadLength, maximum 64 bytes data
  0x3C01,                   //RegFiFoThresh   Tx start at FifoNotEmpty, interrupt at 1 datta
//  0x3700,                   //RegPacketConfig1, Disable CRC,NRZ encode
//  0x3815,                   //RegPayloadLength, 21bytes for length & Fixed length	
//	0x3C95,                   //RegFiFoThresh   	

  
  0x1888,                   //RegLNA, 200R  
  0x581B,                   //RegTestLna, Normal sensitivity
//  0x582D,                   //RegTestLna, increase sensitivity with LNA (Note: consumption also increase!)
  0x6F30,                   //RegTestDAGC, Improved DAGC
//  0x6F00,                   //RegTestDAGC, Normal DAGC
  0x0104,                   //Enter standby mode    
};

const u16 RFM69HRxTbl[5] = 
{       
  0x2504,                   //DIO Mapping for Rx
  0x1310,                   //OCP Disabled
  0x5A55,                   //Normal and Rx
  0x5C70,                   //Normal and Rx   
  0x0110                    //Enter Rx mode
};         

const u16 RFM69HTxTbl[5] = 
{
  0x2504,                   //DIO Mapping for Tx
  0x130F,                   //OCP Disabled
//  0x5A55,                   //Normal and Rx 13dbm
//  0x5C70,                   //Normal and Rx 
  0x5A5D,                   //Normal and Rx 20dbm
  0x5C7C,                   //Normal and Rx 	
  0x010C,                   //Enter Tx mode
};

enum RF_ModeTable RF_Mode;
extern void UARTx_Send_Command(USART_TypeDef* USARTx, uint8_t *Data, uint8_t length);

/**********************************************************
**Name:     RFM69_Config
**Function: Set RFM69 entry Rx_mode
**Input:    mode -> working mode
**Output:   RFM69_ModeSelect -> selected mode
**********************************************************/
u8 RFM69H_ModeSelect(u8 mode)
{
u8 temp;	
	
	switch(mode)
	{
		case RxMode:
		{
		for(temp=0; temp<5; temp++)   //Define to Rx mode  
			RFM69H_WriteByte(RFM69HRxTbl[temp]);	
			
			temp = RxMode;
		}break;
		case TxMode:
		{
		for(temp=0; temp<5; temp++)   //Define to Rx mode  
			RFM69H_WriteByte(RFM69HTxTbl[temp]);		
			
			temp = TxMode;
		}break;
		case StandBy:
		{
			RFM69H_WriteByte(0x0104); 		//Define to standby mode	

			temp = StandBy;			
		}break;		
		case Sleep:
		{
			RFM69H_WriteByte(0x0100);		//Define to sleep mode
			
			temp = Sleep;
		}break;			
		default:
		{
			RFM69H_WriteByte(0x0104); 		//Define to standby mode	

			temp = StandBy;			
		}break;
	}
	
	return temp;
}
/**********************************************************
**Name:     RFM69_ClearFIFO
**Function: Change to RxMode from StandbyMode, can clear FIFO buffer
**Input:    None
**Output:   None
**********************************************************/
void RFM69H_ClearFIFO(void)
{
  RFM69H_WriteByte(0x0104);        //Standby
  RFM69H_WriteByte(0x0110);        //entry RxMode
}

/**********************************************************
**Name:     RFM69_ReadRSSI
**Function: Read the RSSI value
**Input:    none
**Output:   RFM69_ReadRSSI -> RSSI value
**********************************************************/
u8 RFM69H_ReadRSSI(void)
{
u16 temp=0xff;
	
  if((RFM69H_ReadByte(0x24)&0x02)==0x02)
  {
    temp = RFM69H_ReadByte(0x24);
    temp = 0xff-temp;
    temp >>= 1;
    temp &= 0xff;
  }
  return (u8)temp;
}

/**********************************************************
**Name:     RFM69_RxData
**Function: Check for receive one packet
**Input:    dat -> data buffer
**Output:   RFM69_RxData -> Receive data length
**********************************************************/
u8 RFM69H_RxData(u8 *dat)
{
u8 temp = 0;	
u8 Rx_Status = 0;		
	
	RFM69H_ModeSelect(RxMode);		//set module to tx mode to tx data	
	
	while(!((Rx_Status&0xC0)==0xC0 && Rx_Status!=0xff))	//wait for rx status stable
	{
		Rx_Status = RFM69H_ReadByte(0x27);
		Delay_Ms(10);
	}
	
	Rx_Status = 0;
	while((Rx_Status&0x04) != 0x04)		//wait for data receive
	{
		Rx_Status = RFM69H_ReadByte(0x28);
		Delay_Ms(10);	
	}
	
	temp = RFM69H_ReadByte(Addr_RF_FIFO);	//read length of data
	RFM69H_BurstRead(Addr_RF_FIFO, dat, temp-1);	//read data
	
	RFM69H_ModeSelect(StandBy);	//clear fifo
	RFM69H_ModeSelect(RxMode);
	
	UARTx_Send_Command(USART1,&temp,1);
	UARTx_Send_Command(USART1,dat,temp-1);
	
	return(temp);
}

/**********************************************************
**Name:     RFM69_TxData(u8 *dat, u8 length)
**Function: Check RFM69 send over & send next packet
**Input:    dat -> data; length -> data length
**Output:   RFM69_TxData Tx_Status 0:fail,1:success
**********************************************************/
u8 RFM69H_TxData(u8 *dat, u8 length)
{
u8 Tx_Status = 0;	
	
	RFM69H_ModeSelect(TxMode);		//set module to tx mode to tx data	
	
	Tx_Status = 0;
	while(!((Tx_Status&0xA0)==0xA0 && Tx_Status!=0xff))		//wait for tx status stable
	{
		Tx_Status = RFM69H_ReadByte(0x27);
		Delay_Ms(2);   // 10
	}

	RFM69H_BurstWrite(Addr_RF_FIFO, dat, length);    		//send data to fifo to tx

	while((Tx_Status&=0x08) != 0x08)		//wait for tx done
	{
	Tx_Status = RFM69H_ReadByte(0x28);
	Delay_Ms(2);   // 10
	}
	
	RFM69H_ModeSelect(StandBy);
	return(Tx_Status);
}
/**********************************************************
**Name:     RFM69_Config
**Function: Set RFM69 entry Rx_mode
**Input:    None
**Output:   "0" for Error Status
**********************************************************/
void RFM69H_Config(void)
{
u8 temp; 
	
	RFM69H_ModeSelect(StandBy);  	//RFM69 enter standby mode
	
  for(temp=0; temp<3; temp++)                      
  RFM69H_WriteByte(RFM69HFreqTbl[RF_434MHz][temp]);     //setting frequency
	
  for(temp=0; temp<2; temp++)
  RFM69H_WriteByte(RFM69HRateTbl[RF_Rate9K6][temp]);    //setting bitrate  RF_Rate1K2
  
  RFM69H_WriteByte(RFM69HPowerTbl[RF_Power13dBm][0]);   //Setting output power
	
  for(temp=0; temp<17; temp++)
  RFM69H_WriteByte(RFM69HConfigTbl[temp]);              //setting base	
}
/**********************************************************
**Name:     SPI_WriteByte
**Function: SPI Write one byte
**Input:    WrPara
**Output:   none
**note:     use for burst mode
**********************************************************/
void SPI_WriteByte(u8 WrPara)
{
  u8 bitcnt;  
  NSS_L;
  SCK_L;
  
  for(bitcnt=8; bitcnt!=0; bitcnt--)
  {
    SCK_L;
    if(WrPara&0x80)
      MOSI_H;
    else
      MOSI_L;
    SCK_H;
    WrPara <<= 1;
  }
  SCK_L;
  MOSI_H;
}

/**********************************************************
**Name:     SPI_ReadByte
**Function: SPI Read one byte
**Input:    None
**Output:   result byte
**Note:     use for burst mode
**********************************************************/
u8 SPI_ReadByte(void)
{
 u8 RdPara = 0;
 u8 bitcnt;
 
  NSS_L;
  MOSI_H;                                                 //Read one byte data from FIFO, MOSI hold to High
  for(bitcnt=8; bitcnt!=0; bitcnt--)
  {
    SCK_L;
    RdPara <<= 1;
    SCK_H;
    if(MISO_R)
      RdPara |= 0x01;
    else
      RdPara |= 0x00;
  }
  SCK_L;
  return(RdPara);
}

/**********************************************************
**Name:     RFM69_ReadByte
**Function: RFM69 Read CMD
**Input:    adr -> address for read
**Output:   None
**********************************************************/
u8 RFM69H_ReadByte(u8 adr)
{
  u8 tmp; 
  SPI_WriteByte(adr);                                         //Send address first
  tmp = SPI_ReadByte();  
  NSS_H;
  return(tmp);
}

/**********************************************************
**Name:     RFM69_WriteByte
**Function: RFM69 Write CMD
**Input:    WrPara -> address & data
**Output:   None
**********************************************************/
void RFM69H_WriteByte(u16 WrPara)                
{                                                       
  u8 bitcnt;    
  
  SCK_L;
  NSS_L;
  
  WrPara |= 0x8000;                                        //MSB must be "1" for write 
  
  for(bitcnt=16; bitcnt!=0; bitcnt--)
  {
    SCK_L;
    if(WrPara&0x8000)
      MOSI_H;
    else
      MOSI_L;
    SCK_H;
    WrPara <<= 1;
  }
  SCK_L;
  MOSI_H;
  NSS_H;
}

/**********************************************************
**Name:     RFM69_BurstRead
**Function: RFM69 burst read mode
**Input:    adr-----address for read
**          ptr-----data buffer point for read
**          length--how many bytes for read
**Output:   None
**********************************************************/
void RFM69H_BurstRead(u8 adr, u8 *ptr, u8 length)
{
  u8 i;
  if(length<=1)                                            //length must more than one
    return;
  else
  {
    SCK_L; 
    NSS_L;
    SPI_WriteByte(adr); 
    for(i=0;i<length;i++)
    ptr[i] = SPI_ReadByte();
    NSS_H;  
  }
}

/**********************************************************
**Name:     RFM69_BurstWrite
**Function: RFM69 burst write mode
**Input:    adr-----address for write
**          ptr-----data buffer point for write
**          length--how many bytes for write
**Output:   none
**********************************************************/
void RFM69H_BurstWrite(u8 adr, u8 *ptr, u8 length)
{ 
  u8 i;

  if(length<=1)                                            //length must more than one
    return;
  else  
  {   
    SCK_L;
    NSS_L;        
    SPI_WriteByte(adr|0x80);
    for(i=0;i<length;i++)
    SPI_WriteByte(ptr[i]);
    NSS_H;  
  }
}
