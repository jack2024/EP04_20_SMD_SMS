
#include "D:\jobESP\ESPAN-04\Firmware\SPAN-04\ESPAN04_20 - Delay - SP - SMD - SMS\string.h"

#include "D:\jobESP\ESPAN-04\Firmware\SPAN-04\ESPAN04_20 - Delay - SP - SMD - PIC18F252\ESPAN-04.h"
#include "D:\jobESP\ESPAN-04\Firmware\SPAN-04\ESPAN04_20 - Delay - SP - SMD - PIC18F252\23S17.c" // 16 bit I/O Expander

#define OnRelay   1
#define OffRelay   0

#define Pled        PIN_B5
#define I2C_SDA     PIN_C4
#define I2C_SCL     PIN_C3

#define Pack        PIN_C0
#define Preset      PIN_C1
#define Ptest       PIN_C2

#define Pbell       PIN_A3
#define Pbuzzer     PIN_A4

#define P485ctrl    PIN_B3       // ---jj
#define PTxD        PIN_C6
#define PRxD        PIN_C7

//#define PsyncR      PIN_B3
//#define PsyncS      PIN_B4

#define EXP_OUT_ENABLE  PIN_B0
#define EXP_OUT_CLOCK   PIN_B1
#define EXP_OUT_DO      PIN_B2

#define Red       0
#define Green     1
#define Ambian    2

//#define NUMBER_OF_595 8

//#use rs232(baud=9600,parity=N,xmit=PTxD,rcv=PRxD,bits=8,restart_wdt)
/* Config and Enable Hardware UART1(RC6=TX1,RC7=RX1 */
#use rs232(uart1, baud=9600, stream=CH1)

/* Config and Enable Software UART2(RC0=TX2,RC1=RX2 */

#define TX2 PIN_B0                                                            // RC0 = TXD
#define RX2 PIN_B1                                                            // RC1 = RXD
#use rs232(baud=9600, xmit=TX2, rcv=RX2,stream=CH2)

//#USE SPI (  FORCE_HW, BITS=16, ENABLE=IO_CS_PIN, SAMPLE_RISE, stream=SPI_STREAM) 
//#USE SPI (  FORCE_HW, BITS=16, ENABLE=IO_CS_PIN, SAMPLE_RISE, stream=SPI_STREAM2)

//#use spi(MASTER, MODE=0, SPI1, FORCE_HW)
//#use spi(MASTER, MSB_FIRST, BITS=8, MODE=0, BAUD=100000)
//#use spi(DI=PIN_B1, DO=PIN_B0, CLK=PIN_B2, ENABLE=PIN_B4, BITS=8)

void check_ack(void);
void check_test(void);
void check_reset(void);
int1 CheckAutoReset(unsigned char DatType);
void Read_Config(void);
void ForceAllAlarm(void);
void Read_input(void);

void initialIC(void);

int8 StatusTime = 0;         //Hearth beat LED Timer
int1 HearthbeatInd = 0;      //Hearth beat Indicator
int8 FlashingRateTime = 25;
int1 FlashingFlag = 0;
int8 TimeBase1s = 100;
int8 AutoAckDelayTime = 0;

int8 Input1_8_Send;
int8 Input9_16_Send;
int8 Input17_24_Send;
int8 Input25_32_Send;
int8 Input33_40_Send;
int8 Input41_48_Send;
int8 Input49_56_Send;
int8 Input57_64_Send;

int8 Ack1_8_Send;
int8 Ack9_16_Send;
int8 Ack17_24_Send;
int8 Ack25_32_Send;
int8 Ack33_40_Send;
int8 Ack41_48_Send;
int8 Ack49_56_Send;
int8 Ack57_64_Send;

int1 AutoAckFlag = 0;
int1 AutoResetFlag = 0;
int1 AutoTestFlag = 0;

int8 SBUF = 0x00;

int1 Test_fault = 0;
int8 T_test = 0x00;

int1 Test = 0;
int1 Ack_F = 0;
int1 Reset_F = 0;

int1 SyncStatus = 0;   //"0" -> No Sync signal
//int1 SyncFlag = 0;     //use for debounce sync signal
int8 Synctimer = 0x00; //use for debounce sync signal

int8 Output1_8;
int8 Output9_16;

//int8 Output595[4];

//int8 tempdata;
//int8 EEP_Config = 0x00;   //
//int8 NUMBER_OF_595 ;

int8 EEpDat;

volatile int1 RefreshConfigData =0;

volatile int1 functointest_f = 0;

///////// Data from EEProm ///////
//int Device_Addr;

int8 InputType1_8;
int8 InputType9_16;
int8 InputType17_24;

int8 FaultType1_8;
int8 FaultType9_16;
int8 FaultType17_24;


int8 OutputType1_8;
int8 OutputType9_16;
int8 OutputType17_24;

int8 OutputBoth1_8;
int8 OutputBoth9_16;
int8 OutputBoth17_24;

int8 Alarm_Indicator1_8;
int8 Alarm_Indicator9_16;
int8 Alarm_Indicator17_24;

int8 Input1_8;
int8 Input9_10;
int8 Input11_18;
int8 Input19_20;

int8 Input9_16;
int8 Input17_24;
int8 Input25_32;

int8 Red1_8;
int8 Red9_10 ;
int8 Red11_18 ;
int8 Red19_20 ;
int8 Green1_8 ;
int8 Green9_10 ;
int8 Green11_18 ;
int8 Green19_20 ;

/*
int8 OUTRed1_8;
int8 OUTRed9_10 ;
int8 OUTRed11_18 ;
int8 OUTRed19_20 ;
int8 OUTGreen1_8 ;
int8 OUTGreen9_10 ;
int8 OUTGreen11_18 ;
int8 OUTGreen19_20 ;
*/
int8 AutoAck;
int8 AutoAckTime;
int8 FlashingRate;
int8 NoOfPoint;
//int8 MasterSlaveSync;

int1 StartRead = 0;
volatile int8 StartReadCount;

////////////////////////////////
///////// tempolary register //////////
//int8 temp;
//int1 RxDreceive = 0;

//////////////////////////////////////
unsigned char const addr_sq = 0x10,end_sq = 0x11,code_sq = 0x12,start_addr_hi_sq = 0x13,start_addr_lo_sq = 0x14;         //serial sequnce
unsigned char const ubyte_hi_sq = 0x15,ubyte_lo_sq = 0x16,crc_hi_sq = 0x17, second_numofdata = 0x18,byte_count_sq = 0x19,data_sq = 0x20;      //serial sequnce


int1 recieve_completed = 0;
unsigned char sequence;         //keep sequence use for RxD
unsigned char Address;
unsigned char RxD_DataLen = 0x00;
unsigned char TxD_Buff[512];
unsigned char RxD_Buff[1024];
unsigned char CRC_Lo;
unsigned char CRC_Hi;
int16 Send_check_Time = 500; //if no send reset buffer every 5 second

int16 Start_Address = 0x0000;
int16 No_PointCount = 0x0000;
unsigned char Data_ByteCount = 0x00;
unsigned char Data_Buff[1024];
//unsigned char DataTemp;
//unsigned char TxD_DataLen;

int8 MCP23s17_Ip_dat;


//int8 MCP23s17_Op_dat;

unsigned char T_timeout;    //use for calculate RxD timeout
int16 index = 0x00; //use for Loop

int8 outmcp23 = 0;

unsigned char sms_phonenumber[15];

unsigned char SMS_Massage1[32];
unsigned char SMS_Massage2[32];
unsigned char SMS_Massage3[32];
unsigned char SMS_Massage4[32];
unsigned char SMS_Massage5[32];
unsigned char SMS_Massage6[32];
unsigned char SMS_Massage7[32];
unsigned char SMS_Massage8[32];
unsigned char SMS_Massage9[32];
unsigned char SMS_Massage10[32];
unsigned char SMS_Massage11[32];
unsigned char SMS_Massage12[32];
unsigned char SMS_Massage13[32];
unsigned char SMS_Massage14[32];
unsigned char SMS_Massage15[32];
unsigned char SMS_Massage16[32];
unsigned char SMS_Massage17[32];
unsigned char SMS_Massage18[32];
unsigned char SMS_Massage19[32];
unsigned char SMS_Massage20[32];

unsigned char SMS_MassageSUM[160] = ">>";

unsigned char spacestring[] = " ";

volatile int8 send_SMS_period ,send_SMS_count ;

unsigned char const CRC_Table_Hi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
}; 

unsigned  char const CRC_Table_Lo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;

///////////// Define Bit register ////////////////////////////

struct Bit64    // Inputt
{
   int1 B1,B2,B3,B4,B5,B6,B7,B8,B9,B10;
   int1 B11,B12,B13,B14,B15,B16,B17,B18,B19,B20;
   int1 B21,B22,B23,B24,B25,B26,B27,B28,B29,B30;
   //int1 B31,B32,B33,B34,B35,B36,B37,B38,B39,B40;
   //int1 B41,B42,B43,B44,B45,B46,B47,B48,B49,B50;
   //int1 B51,B52,B53,B54,B55,B56,B57,B58,B59,B60;
   //int1 B61,B62,B63,B64;
};

   //Output   (LED Lit = "0")
   //Inputt Type NO/NC (NO = "1" , NC = "0")
   //Fault Type Manual/Auto (Manual = "1" , Auto = "0")
   //Output Type Buz/Bell (Buzzer = "1" , Bell = "0")
   //Output Both (Normal = "1" , Both = "0")
   //Acknowledge flag (Acked = "1" , non Ack = "0")

struct Bit64 Inputt;//,Output,InputType;//,FaultType,OutputType,OutputBoth;
struct Bit64 Output,InputType;
struct Bit64 FaultType,OutputType,OutputBoth;
struct Bit64 AlarmIndicator,Ack,In,In2;
struct Bit64 AckSend,RED_Colour,GREEN_Colour;
struct Bit64 SendSMS;

int1 FaultAgo[30];
int1 FaultNow[30];
int16 ReleaseTime[30];
int16 FaultDelayTime[30];

int1 FaultNCNO[30];
#define NO 1
#define NC 0

//volatile int16 ReadIn_Count =0;
//int1 ReadIn_flag = 0;
/*
int8 DelayTime1 = 0;
int8 DelayTime2 = 0;
int8 DelayTime3 = 0;
int8 DelayTime4 = 0;
int8 DelayTime5 = 0;
int8 DelayTime6 = 0;
int8 DelayTime7 = 0;
int8 DelayTime8 = 0;
int8 DelayTime9 = 0;
int8 DelayTime10 = 0;
int8 DelayTime11 = 0;
int8 DelayTime12 = 0;
int8 DelayTime13 = 0;
int8 DelayTime14 = 0;
int8 DelayTime15 = 0;
int8 DelayTime16 = 0;
int8 DelayTime17 = 0;
int8 DelayTime18 = 0;
int8 DelayTime19 = 0;
int8 DelayTime20 = 0;
*/
char blankSigned[] = ">>";
void sendsum_sms(void)
{
   if(strcmp(SMS_MassageSUM, blankSigned) != 0){
   
      fprintf(CH2,"AT+CMGF=1"); 
      putc('\n',CH2);
      delay_ms(10);
   
      fprintf(CH2,"AT+CMGS=\"");
      fprintf(CH2,sms_phonenumber);
      
      fprintf(CH2,"\"");
      putc('\n',CH2);
      delay_ms(50);
     
     fprintf(CH2,SMS_MassageSUM);
     putc('\n',CH2);
     putc(26,CH2);
     
     strcpy(SMS_MassageSUM, blankSigned);

   }
}
//////////////////////////////--Function--/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////MODBUS Routine//////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Alarmtosend(void)
{
   Input1_8_Send = 0x00;
   Input9_16_Send = 0x00;
   Input17_24_Send = 0x00;
   
   
   Ack1_8_Send = 0x00;
   Ack9_16_Send = 0x00;
   Ack17_24_Send = 0x00;

   
   /////////////////////////////////////////////////////
  
   if((In.B1 == 0) && (Output.B1 ==0))
      AckSend.B1 = 1;
   else
      AckSend.B1 = 0;
      
   if((In.B2 == 0) && (Output.B2 ==0))
      AckSend.B2 = 1;
   else
      AckSend.B2 = 0;
      
   if((In.B3 == 0) && (Output.B3 ==0))
      AckSend.B3 = 1;
   else
      AckSend.B3 = 0;
      
   if((In.B4 == 0) && (Output.B4 ==0))
      AckSend.B4 = 1;
   else
      AckSend.B4 = 0;
      
   if((In.B5 == 0) && (Output.B5 ==0))
      AckSend.B5 = 1;
   else
      AckSend.B5 = 0;
      
   if((In.B6 == 0) && (Output.B6 ==0))
      AckSend.B6 = 1;
   else
      AckSend.B6 = 0;
      
   if((In.B7 == 0) && (Output.B7 ==0))
      AckSend.B7 = 1;
   else
      AckSend.B7 = 0;
      
   if((In.B8 == 0) && (Output.B8 ==0))
      AckSend.B8 = 1;
   else
      AckSend.B8 = 0;
      
   if((In.B9 == 0) && (Output.B9 ==0))
      AckSend.B9 = 1;
   else
      AckSend.B9 = 0;
      
   if((In.B10 == 0) && (Output.B10 ==0))
      AckSend.B10 = 1;
   else
      AckSend.B10 = 0;
      
   if((In.B11 == 0) && (Output.B11 ==0))
      AckSend.B11 = 1;
   else
      AckSend.B11 = 0;
      
   if((In.B12 == 0) && (Output.B12 ==0))
      AckSend.B12 = 1;
   else
      AckSend.B12 = 0;
      
   if((In.B13 == 0) && (Output.B13 ==0))
      AckSend.B13 = 1;
   else
      AckSend.B13 = 0;
      
   if((In.B14 == 0) && (Output.B14 ==0))
      AckSend.B14 = 1;
   else
      AckSend.B14 = 0;
      
   if((In.B15 == 0) && (Output.B15 ==0))
      AckSend.B15 = 1;
   else
      AckSend.B15 = 0;
      
   if((In.B16 == 0) && (Output.B16 ==0))
      AckSend.B16 = 1;
   else
      AckSend.B16 = 0;
      
   if((In.B17 == 0) && (Output.B17 ==0))
      AckSend.B17 = 1;
   else
      AckSend.B17 = 0;
      
   if((In.B18 == 0) && (Output.B18 ==0))
      AckSend.B18 = 1;
   else
      AckSend.B18 = 0;
      
   if((In.B19 == 0) && (Output.B19 ==0))
      AckSend.B19 = 1;
   else
      AckSend.B19 = 0;
      
   if((In.B20 == 0) && (Output.B20 ==0))
      AckSend.B20 = 1;
   else
      AckSend.B20 = 0;
   
   
   
   ///////////////////Ack/////////////////////////////////
   Ack1_8_Send = Ack1_8_Send | AckSend.B8;
   Ack1_8_Send = (Ack1_8_Send << 1) | AckSend.B7;
   Ack1_8_Send = (Ack1_8_Send << 1) | AckSend.B6;
   Ack1_8_Send = (Ack1_8_Send << 1) | AckSend.B5;
   Ack1_8_Send = (Ack1_8_Send << 1) | AckSend.B4;
   Ack1_8_Send = (Ack1_8_Send << 1) | AckSend.B3;
   Ack1_8_Send = (Ack1_8_Send << 1) | AckSend.B2;
   Ack1_8_Send = (Ack1_8_Send << 1) | AckSend.B1;
   
   Ack9_16_Send = Ack9_16_Send | AckSend.B16;
   Ack9_16_Send = (Ack9_16_Send << 1) | AckSend.B15;
   Ack9_16_Send = (Ack9_16_Send << 1) | AckSend.B14;
   Ack9_16_Send = (Ack9_16_Send << 1) | AckSend.B13;
   Ack9_16_Send = (Ack9_16_Send << 1) | AckSend.B12;
   Ack9_16_Send = (Ack9_16_Send << 1) | AckSend.B11;
   Ack9_16_Send = (Ack9_16_Send << 1) | AckSend.B10;
   Ack9_16_Send = (Ack9_16_Send << 1) | AckSend.B9;
   
   Ack17_24_Send = Ack17_24_Send | AckSend.B24;
   Ack17_24_Send = (Ack17_24_Send << 1) | AckSend.B23;
   Ack17_24_Send = (Ack17_24_Send << 1) | AckSend.B22;
   Ack17_24_Send = (Ack17_24_Send << 1) | AckSend.B21;
   Ack17_24_Send = (Ack17_24_Send << 1) | AckSend.B20;
   Ack17_24_Send = (Ack17_24_Send << 1) | AckSend.B19;
   Ack17_24_Send = (Ack17_24_Send << 1) | AckSend.B18;
   Ack17_24_Send = (Ack17_24_Send << 1) | AckSend.B17;
   
   ///////////////////Fault/////////////////////////////////
   Input1_8_Send = Input1_8_Send | In.B8;
   Input1_8_Send = (Input1_8_Send << 1) | In.B7;
   Input1_8_Send = (Input1_8_Send << 1) | In.B6;
   Input1_8_Send = (Input1_8_Send << 1) | In.B5;
   Input1_8_Send = (Input1_8_Send << 1) | In.B4;
   Input1_8_Send = (Input1_8_Send << 1) | In.B3;
   Input1_8_Send = (Input1_8_Send << 1) | In.B2;
   Input1_8_Send = (Input1_8_Send << 1) | In.B1;
   
   Input9_16_Send = Input9_16_Send | In.B16;
   Input9_16_Send = (Input9_16_Send << 1) | In.B15;
   Input9_16_Send = (Input9_16_Send << 1) | In.B14;
   Input9_16_Send = (Input9_16_Send << 1) | In.B13;
   Input9_16_Send = (Input9_16_Send << 1) | In.B12;
   Input9_16_Send = (Input9_16_Send << 1) | In.B11;
   Input9_16_Send = (Input9_16_Send << 1) | In.B10;
   Input9_16_Send = (Input9_16_Send << 1) | In.B9;
   
   Input17_24_Send = Input17_24_Send | In.B24;
   Input17_24_Send = (Input17_24_Send << 1) | In.B23;
   Input17_24_Send = (Input17_24_Send << 1) | In.B22;
   Input17_24_Send = (Input17_24_Send << 1) | In.B21;
   Input17_24_Send = (Input17_24_Send << 1) | In.B20;
   Input17_24_Send = (Input17_24_Send << 1) | In.B19;
   Input17_24_Send = (Input17_24_Send << 1) | In.B18;
   Input17_24_Send = (Input17_24_Send << 1) | In.B17;
}

////////////////////////////////////////////////////////////////////////////////

void StoreReleaseFault()
{
   unsigned char i=0;
   for(i=1;i<=20;i++)
   {
      FaultAgo[i] = FaultNow[i];
   }
}


void CRC(unsigned char *puchMsg , unsigned char usDataLen)
{ 
   unsigned char uIndex ;                   /* fill index into CRC lookup table */
   unsigned char i ;

   CRC_Hi = 0xFF ;                      /* high byte of CRC initialized */
   CRC_Lo = 0xFF ;                      /* low byte of CRC initialized */

   for(i = 0;i < usDataLen;i++)
   {
      restart_wdt();
      uIndex = CRC_Hi ^ (unsigned char) puchMsg[i] ;
      CRC_Hi = CRC_Lo ^ CRC_Table_Hi[uIndex] ;
      CRC_Lo = CRC_Table_Lo[uIndex] ;
   }
}

/********************************6B595 Driver*********************************/
/*
void Driver595()
{
   Signed int8 j=0;
   int8 data_out ;

   restart_wdt();
   
    data_out = (~Input19_20);
    for(j=7;j>=0;j--)
    {
     //output_bit(EXP_OUT_DO,bit_test(Output595[i],j));
     output_bit(EXP_OUT_DO,bit_test(data_out,j));
     output_high(EXP_OUT_CLOCK);
     delay_us(1);
     output_low(EXP_OUT_CLOCK);
    }
    
    data_out = (~Input11_18);
    for(j=7;j>=0;j--)
    {
     //output_bit(EXP_OUT_DO,bit_test(Output595[i],j));
     output_bit(EXP_OUT_DO,bit_test(data_out,j));
     output_high(EXP_OUT_CLOCK);
     delay_us(1);
     output_low(EXP_OUT_CLOCK);
    }
    
    data_out = (~Input9_10);
    for(j=7;j>=0;j--)
    {
     //output_bit(EXP_OUT_DO,bit_test(Output595[i],j));
     output_bit(EXP_OUT_DO,bit_test(data_out,j));
     output_high(EXP_OUT_CLOCK);
     delay_us(1);
     output_low(EXP_OUT_CLOCK);
    }
    
    data_out = (~Input1_8);
    for(j=7;j>=0;j--)
    {
     //output_bit(EXP_OUT_DO,bit_test(Output595[i],j));
     output_bit(EXP_OUT_DO,bit_test(data_out,j));
     output_high(EXP_OUT_CLOCK);
     delay_us(1);
     output_low(EXP_OUT_CLOCK);
    }
 // } 
   output_high(EXP_OUT_ENABLE);
   delay_us(1);
   output_low(EXP_OUT_ENABLE);
}
*/
////////////////////////////////////////////////////////////////////////////////

void checkCommand(void)
{
   restart_wdt();

   //if(sequence == end_sq && Address == SBUF)     //check Address
   if(sequence == end_sq )     //check Address
   {
     RxD_DataLen = 0x00 ;
     RxD_Buff[RxD_DataLen] = SBUF ;      //Byte 1   Address
     restart_wdt();
     RxD_DataLen ++ ;
     sequence = addr_sq;
     T_timeout = 0x14; //200ms
   }
   else if(sequence == addr_sq)
   {
      RxD_Buff[RxD_DataLen] = SBUF ;      //Byte 2   Function Code
      restart_wdt();
      RxD_DataLen ++ ;

       if(RxD_Buff[RxD_DataLen - 1] < 0x17)   //Function Code Must be less than 0x20
       {
           sequence = code_sq;
           T_timeout = 0x14; //200ms
       }
       else if(RxD_Buff[RxD_DataLen - 1] == 0x20)   /////Read Setting//////
       {
          sequence = ubyte_lo_sq;
          T_timeout = 0x14; //200ms
       }
       else if(RxD_Buff[RxD_DataLen - 1] == 0x21)   /////Write setting/////
       {
          sequence = byte_count_sq ;
          T_timeout = 0x14; //200ms
       }
       else if(RxD_Buff[RxD_DataLen - 1] == 0x22)   /////SMS setting/////
       {
          sequence = second_numofdata ;
          T_timeout = 0x14; //200ms
       }
       else                           // Invalid Code
       {
          RxD_DataLen = 0x00;
          sequence = end_sq;
          T_timeout = 0x00;
          output_bit(P485ctrl,0);
       }
   }
   else if(sequence == second_numofdata)
   {
      RxD_Buff[RxD_DataLen] = SBUF ;      //Byte 3   Start address High Byte
      restart_wdt();
      RxD_DataLen ++ ;
      sequence = byte_count_sq;
      T_timeout = 0x14; //200ms
   }
   else if(sequence == byte_count_sq)
   {
      RxD_Buff[RxD_DataLen] = SBUF ;      //Byte 3   Data Byte Count
      restart_wdt();
      RxD_DataLen ++ ;
      if(RxD_Buff[1] == 0x22)   /////SMS setting/////
      {
         index = (RxD_Buff[2] * 0x64) + RxD_Buff[3];
      }
      else{
         index = RxD_Buff[RxD_DataLen - 1] ;    //Data Byte Count
      }
      T_timeout = 0x14; //200ms
      sequence = data_sq ;
   }
   else if(sequence == data_sq)
   {
      RxD_Buff[RxD_DataLen] = SBUF ;      //
      restart_wdt();
      RxD_DataLen ++ ;
      index -- ;                     //Data Byte Count
      if(index == 0x00)
      {
         sequence = ubyte_lo_sq ;      //next CRC
      }
      T_timeout = 0x14; //200ms
   }
   else if(sequence == code_sq)
   {
      RxD_Buff[RxD_DataLen] = SBUF ;      //Byte 3   Start address High Byte
      restart_wdt();
      RxD_DataLen ++ ;
      sequence = start_addr_hi_sq;
      T_timeout = 0x14; //200ms

   }
   else if(sequence == start_addr_hi_sq)
   {
      RxD_Buff[RxD_DataLen] = SBUF ;         //Byte 4   Start address Low Byte
      restart_wdt();
      RxD_DataLen ++ ;
      sequence = start_addr_lo_sq;
      T_timeout = 0x14; //200ms
   }
   else if(sequence == start_addr_lo_sq)
   {
      RxD_Buff[RxD_DataLen] = SBUF ;         //Byte 5   No. of point/force data High Byte
      restart_wdt();
      RxD_DataLen ++ ;
      sequence = ubyte_hi_sq;
      T_timeout = 0x14; //200ms
   }
   else if(sequence == ubyte_hi_sq)
   {
      RxD_Buff[RxD_DataLen] = SBUF ;         //Byte 6   No. of point/force data Low Byte
      restart_wdt();
      RxD_DataLen ++ ;
      sequence = ubyte_lo_sq;
      T_timeout = 0x14; //200ms
   }
   else if(sequence == ubyte_lo_sq)
   {
      RxD_Buff[RxD_DataLen] = SBUF ;         //Byte 7   CRC High Byte
      restart_wdt();
      RxD_DataLen ++ ;
      sequence = crc_hi_sq;
      T_timeout = 0x14; //200ms
   }
   else if(sequence == crc_hi_sq)
   {
      RxD_Buff[RxD_DataLen] = SBUF ;         //Byte 8   CRC Low Byte
      restart_wdt();
      sequence = end_sq;
      T_timeout = 0x00;
      //T_timeout = 0x14; //200ms
      recieve_completed = 1 ;            //Recieve completed then translate
   }
}

///////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
///////////////////// MODBUS FUNCTION //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Modbus_Function(void)
{
   restart_wdt();
   CRC(RxD_Buff , RxD_DataLen - 1);

   if(CRC_Hi == RxD_Buff[RxD_DataLen - 1] && CRC_Lo == RxD_Buff[RxD_DataLen])
   {
   
      if((RxD_Buff[0] == 0xAA)&&(RxD_Buff[1] == 0x20)) //Read Setting//0xAA Is Any Address 
      {
         TxD_Buff[0] = Address ;         //Address
         TxD_Buff[1] = 0x20 ;            //function code
         TxD_Buff[2] = 0x1C ;           //28 Byte  //Data Byte count
         TxD_Buff[3] = InputType1_8 ;    //1
         TxD_Buff[4] = InputType9_16 ;  //2
         TxD_Buff[5] = InputType17_24 ;//3
         
         TxD_Buff[6] = FaultType1_8 ;  //4
         TxD_Buff[7] = FaultType9_16 ; //5
         TxD_Buff[8] = FaultType17_24 ;   //6
        
         TxD_Buff[9] = OutputType1_8 ; //7
         TxD_Buff[10] = OutputType9_16 ;  //8
         TxD_Buff[11] = OutputType17_24 ; //9
        
         TxD_Buff[12] = OutputBoth1_8 ;   //10
         TxD_Buff[13] = OutputBoth9_16 ;  //11
         TxD_Buff[14] = OutputBoth17_24 ; //12
        
         TxD_Buff[15] = Alarm_Indicator1_8 ; //13
         TxD_Buff[16] = Alarm_Indicator9_16 ;   //14
         TxD_Buff[17] = Alarm_Indicator17_24 ;  //15
        
          TxD_Buff[18] = read_eeprom(0x10);//Red1_8;    //16
          TxD_Buff[19] = read_eeprom(0x11);//Red9_10;      //17
          TxD_Buff[20] = read_eeprom(0x12);//Red11_18;     //18
          TxD_Buff[21] = read_eeprom(0x13);//Red19_20;     //19
          
          TxD_Buff[22] = read_eeprom(0x14);//Green1_8;     //20
          TxD_Buff[23] = read_eeprom(0x15);//Green9_10;    //21
          TxD_Buff[24] = read_eeprom(0x16);//Green11_18;      //22
          TxD_Buff[25] = read_eeprom(0x17);//Green19_20;      //23
       
       
         TxD_Buff[26] = AutoAck ;            //24
         TxD_Buff[27] = AutoAckTime ;        //25
         TxD_Buff[28] = FlashingRate ;       //26
         TxD_Buff[29] = NoOfPoint ;          //27
         TxD_Buff[30] = FaultDelayTime[0] ;    //28
         TxD_Buff[31] = Address ;            //29


         TxD_Buff[32] = FaultDelayTime[1] ;
         TxD_Buff[33] = FaultDelayTime[2] ;
         TxD_Buff[34] = FaultDelayTime[3] ;
         TxD_Buff[35] = FaultDelayTime[4] ;
         TxD_Buff[36] = FaultDelayTime[5] ;
         TxD_Buff[37] = FaultDelayTime[6] ;
         TxD_Buff[38] = FaultDelayTime[7] ;
         TxD_Buff[39] = FaultDelayTime[8] ;
         TxD_Buff[40] = FaultDelayTime[9] ;
         TxD_Buff[41] = FaultDelayTime[10] ;
         TxD_Buff[42] = FaultDelayTime[11] ;
         TxD_Buff[43] = FaultDelayTime[12] ;
         TxD_Buff[44] = FaultDelayTime[13] ;
         TxD_Buff[45] = FaultDelayTime[14] ;
         TxD_Buff[46] = FaultDelayTime[15] ;
         TxD_Buff[47] = FaultDelayTime[16] ;
         TxD_Buff[48] = FaultDelayTime[17] ;
         TxD_Buff[49] = FaultDelayTime[18] ;
         TxD_Buff[50] = FaultDelayTime[19] ;
         TxD_Buff[51] = FaultDelayTime[20] ;
         
         
         CRC(TxD_Buff,52)   ; //Cal CRC 49 byte

         TxD_Buff[52] = CRC_Hi ;
         TxD_Buff[53] = CRC_Lo ;

         output_bit(P485ctrl,1);
         delay_ms(10);

         putc(Txd_Buff[0]);
         putc(Txd_Buff[1]);
         putc(Txd_Buff[2]);
         putc(Txd_Buff[3]);
         putc(Txd_Buff[4]);
         putc(Txd_Buff[5]);
         putc(Txd_Buff[6]);
         putc(Txd_Buff[7]);
         putc(Txd_Buff[8]);
         putc(Txd_Buff[9]);
         putc(Txd_Buff[10]);
         putc(Txd_Buff[11]);
         putc(Txd_Buff[12]);
         putc(Txd_Buff[13]);
         putc(Txd_Buff[14]);
         putc(Txd_Buff[15]);
         putc(Txd_Buff[16]);
         putc(Txd_Buff[17]);
         putc(Txd_Buff[18]);
         putc(Txd_Buff[19]);
         putc(Txd_Buff[20]);
         putc(Txd_Buff[21]);
         putc(Txd_Buff[22]);
         putc(Txd_Buff[23]);
         putc(Txd_Buff[24]);
         putc(Txd_Buff[25]);
         putc(Txd_Buff[26]);
         putc(Txd_Buff[27]);
         putc(Txd_Buff[28]);
         putc(Txd_Buff[29]);
         putc(Txd_Buff[30]);
         putc(Txd_Buff[31]);
         
         putc(Txd_Buff[32]);
         putc(Txd_Buff[33]);
         putc(Txd_Buff[34]);
         putc(Txd_Buff[35]);
         putc(Txd_Buff[36]);
         putc(Txd_Buff[37]);
         putc(Txd_Buff[38]);
         putc(Txd_Buff[39]);
         putc(Txd_Buff[40]);
         putc(Txd_Buff[41]);
         
         putc(Txd_Buff[42]);
         putc(Txd_Buff[43]);
         putc(Txd_Buff[44]);
         putc(Txd_Buff[45]);
         putc(Txd_Buff[46]);
         putc(Txd_Buff[47]);
         putc(Txd_Buff[48]);
         putc(Txd_Buff[49]);
         putc(Txd_Buff[50]);
         putc(Txd_Buff[51]);
         
         putc(Txd_Buff[52]);
         putc(Txd_Buff[53]);
       
         delay_ms(3);
         output_bit(P485ctrl,0);
         
      }
     
   ///////////////////////////////////////////////////////////////
   if(RxD_Buff[0] == Address)
   {
         
      if(RxD_Buff[1] == 0x01)///////////// READ COIL /////////////////////
         {
            //Do Read Coil
            Start_Address = RxD_Buff[2] ;
            Start_Address = (Start_Address << 8) | RxD_Buff[3] ;   //Start Address 16 bit
            No_PointCount = RxD_Buff[4] ;
            No_PointCount = (No_PointCount << 8) | RxD_Buff[5] ;      //No. of Point 16 bit

            if(Start_Address < 0x10 && (Start_Address + No_PointCount) <= 0x14)  //Valid point 0-20
            {
               if(No_PointCount < 9) Data_ByteCount = 0x01;
               else if(No_PointCount < 17) Data_ByteCount = 0x02 ;
               else if(No_PointCount < 25) Data_ByteCount = 0x03 ;
               else   Data_ByteCount = 0x01 ; //----Jack

               if(Data_ByteCount == 0x01)
               {

                  if(Start_Address < 0x09)
                  {
                     //Data_Buff[0] = ~output[0] >> Start_Address ;
                     //DataTemp = ~output[1] << (0x08 - Start_Address) ;
                     //Data_Buff[0] = (~output[0] >> Start_Address) | DataTemp;   //Low Byte
                      Data_Buff[0] = Input1_8 >> Start_Address ;
                      //DataTemp = Input9_16 << (0x08 - Start_Address) ;//JJ
                      //Data_Buff[0] = (Input1_8 >> Start_Address) | DataTemp;   //Low Byte//JJ
                  }
                  else if(Start_Address > 0x08 && Start_Address < 0x10)
                  {
                     //Data_Buff[0] = ~output[1] >> (Start_Address - 0x08) ;
                     Data_Buff[0] = Input9_16 >> (Start_Address - 0x08) ;
                  }


                  if(No_PointCount == 0x08)
                  {
                     Data_Buff[0] = Data_Buff[0] & 0xFF ; //8 Point High Byte
                  }
                  else if(No_PointCount == 0x07)
                  {
                     Data_Buff[0] =    Data_Buff[0] & 0x7F ; //7 Point High Byte
                  }
                  else if(No_PointCount == 0x06)
                  {
                     Data_Buff[0] =    Data_Buff[0] & 0x3F ; //6 Point High Byte
                  }
                  else if(No_PointCount == 0x05)
                  {
                     Data_Buff[0] =    Data_Buff[0] & 0x1F ; //5 Point High Byte
                  }
                  else if(No_PointCount == 0x04)
                  {
                     Data_Buff[0] =    Data_Buff[0] & 0x0F ; //4 Point High Byte
                  }
                  else if(No_PointCount == 0x03)
                  {
                     Data_Buff[0] =    Data_Buff[0] & 0x07 ; //3 Point High Byte
                  }
                  else if(No_PointCount == 0x02)
                  {
                     Data_Buff[0] =    Data_Buff[0] & 0x03 ; //2 Point High Byte
                  }
                  else if(No_PointCount == 0x01)
                  {
                     Data_Buff[0] =    Data_Buff[0] & 0x01 ; //1 Point High Byte
                  }

                  TxD_Buff[0] = Address ;         //Address
                  TxD_Buff[1] = 0x01 ;         //Function Code
                  TxD_Buff[2] = Data_ByteCount ;   //Byte Count
                  TxD_Buff[3] = Data_Buff[0] ;   //Data

                  CRC(TxD_Buff,4)   ;            //Cal CRC 5 Byte

                  TxD_Buff[4] = CRC_Hi ;
                  TxD_Buff[5] = CRC_Lo ;

                  //TxD_DataLen = 0x06 ;
                  //rs485_ctrl = 1;
                  //dmsec(4);
                  //send = 1;
                  //TI=1;
                  output_bit(P485ctrl,1);
                  restart_wdt();
                  delay_ms(4);
                  restart_wdt();

                  putc(TxD_Buff[0]) ;               //Address
                  putc(TxD_Buff[1]) ;               //Function Code
                  putc(TxD_Buff[2]) ;               //Byte Count
                  putc(TxD_Buff[3]) ;
                  putc(TxD_Buff[4]) ;
                  putc(TxD_Buff[5]) ;
                  //putc(TxD_Buff[6]) ;
                  //putc(TxD_Buff[7]) ;
                  //putc(TxD_Buff[8]) ;
                  //putc(TxD_Buff[9]) ;
               
                  restart_wdt();
                  delay_ms(3);
                  restart_wdt();
                  output_bit(P485ctrl,0);
              
               }
               else if(Data_ByteCount == 0x02)
               {
                  //Data_Buff[1] = ~output[1] >> Start_Address ;
                  //DataTemp = ~output[1] << (0x08 - Start_Address) ;
                  //Data_Buff[0] = (~output[0] >> Start_Address) | DataTemp;   //Low Byte
                  
                  //Data_Buff[1] = Output9_16 >> Start_Address ;//jj
                  //DataTemp = Output9_16 << (0x08 - Start_Address) ;//jj
                  //Data_Buff[0] = (Output1_8 >> Start_Address) | DataTemp;   //Low Byte//jj
                  Data_Buff[0] = Input1_8 >> Start_Address ;
                  Data_Buff[1] = Input9_16 ;// >> Start_Address ;


                  if((No_PointCount - 0x07) == 0x08)
                  {
                     Data_Buff[1] = Data_Buff[1] & 0xFF ; //16 Point High Byte
                  }
                  else if((No_PointCount - 0x07) == 0x07)
                  {
                     Data_Buff[1] =    Data_Buff[1] & 0x7F ; //15 Point High Byte
                  }
                  else if((No_PointCount - 0x07) == 0x06)
                  {
                     Data_Buff[1] =    Data_Buff[1] & 0x3F ; //14 Point High Byte
                  }
                  else if((No_PointCount - 0x07) == 0x05)
                  {
                     Data_Buff[1] =    Data_Buff[1] & 0x1F ; //13 Point High Byte
                  }
                  else if((No_PointCount - 0x07) == 0x04)
                  {
                     Data_Buff[1] =    Data_Buff[1] & 0x0F ; //12 Point High Byte
                  }
                  else if((No_PointCount - 0x07) == 0x03)
                  {
                     Data_Buff[1] =    Data_Buff[1] & 0x07 ; //11 Point High Byte
                  }
                  else if((No_PointCount - 0x07) == 0x02)
                  {
                     Data_Buff[1] =    Data_Buff[1] & 0x03 ; //10 Point High Byte
                  }
                  else if((No_PointCount - 0x07) == 0x01)
                  {
                     Data_Buff[1] =    Data_Buff[1] & 0x01 ; //9 Point High Byte
                  }

                  TxD_Buff[0] = Address ;         //Address
                  TxD_Buff[1] = 0x01 ;         //Function Code
                  TxD_Buff[2] = Data_ByteCount ;   //Byte Count
                  TxD_Buff[3] = Data_Buff[0] ;      //first byte Data
                  TxD_Buff[4] = Data_Buff[1] ;      //second byte Data

                  CRC(TxD_Buff,5)   ;            //Cal CRC 5 Byte

                  TxD_Buff[5] = CRC_Hi ;
                  TxD_Buff[6] = CRC_Lo ;

                  //TxD_DataLen = 0x07 ;
                  //rs485_ctrl = 1;
                  //dmsec(4);
                  //send = 1;
                  //TI=1;
                  output_bit(P485ctrl,1);
                  restart_wdt();
                  delay_ms(4);
                  restart_wdt();

                  putc(TxD_Buff[0]) ;               //Address
                  putc(TxD_Buff[1]) ;               //Function Code
                  putc(TxD_Buff[2]) ;               //Byte Count
                  putc(TxD_Buff[3]) ;
                  putc(TxD_Buff[4]) ;
                  putc(TxD_Buff[5]) ;
                  putc(TxD_Buff[6]) ;
                  //putc(TxD_Buff[7]) ;
                  //putc(TxD_Buff[8]) ;
                  //putc(TxD_Buff[9]) ;

                  restart_wdt();
                  delay_ms(3);
                  restart_wdt();
                  output_bit(P485ctrl,0);
              
               }
               else if(Data_ByteCount == 0x03)
               {
                  Data_Buff[0] = Input1_8 >> Start_Address ;
                  Data_Buff[1] = Input9_16 ;// >> Start_Address ;
                  Data_Buff[2] = Input17_24 ;// >> Start_Address ;
                  
                  TxD_Buff[0] = Address ;         //Address
                  TxD_Buff[1] = 0x01 ;         //Function Code
                  TxD_Buff[2] = Data_ByteCount ;   //Byte Count
                  TxD_Buff[3] = Data_Buff[0] ;      //first byte Data
                  TxD_Buff[4] = Data_Buff[1] ;      //second byte Data
                  TxD_Buff[5] = Data_Buff[2] ;  

                  CRC(TxD_Buff,6)   ;            //Cal CRC 5 Byte

                  TxD_Buff[6] = CRC_Hi ;
                  TxD_Buff[7] = CRC_Lo ;

                  //TxD_DataLen = 0x07 ;
                  //rs485_ctrl = 1;
                  //dmsec(4);
                  //send = 1;
                  //TI=1;
                  output_bit(P485ctrl,1);
                  restart_wdt();
                  delay_ms(4);
                  restart_wdt();

                  putc(TxD_Buff[0]) ;               //Address
                  putc(TxD_Buff[1]) ;               //Function Code
                  putc(TxD_Buff[2]) ;               //Byte Count
                  putc(TxD_Buff[3]) ;
                  putc(TxD_Buff[4]) ;
                  putc(TxD_Buff[5]) ;
                  putc(TxD_Buff[6]) ;
                  putc(TxD_Buff[7]) ;
                  //putc(TxD_Buff[8]) ;
                  //putc(TxD_Buff[9]) ;

                  restart_wdt();
                  delay_ms(3);
                  restart_wdt();
                  output_bit(P485ctrl,0);
               
               }
            }
            else
            {
               //invalid parameter
               TxD_Buff[0] = Address ;         //Address
               TxD_Buff[1] = 0x81 ;         //Function Code
               TxD_Buff[2] = 0x02 ;         //illegal data address

               CRC(TxD_Buff,3)   ;            //Cal CRC 3 Byte

               TxD_Buff[3] = CRC_Hi ;
               TxD_Buff[4] = CRC_Lo ;

               //TxD_DataLen = 0x05 ;
               //rs485_ctrl = 1;
               //restart_wdt();
               //dmsec(4);
               //restart_wdt();
               //send = 1;
               //TI=1;
               output_bit(P485ctrl,1);
               restart_wdt();
               delay_ms(4);
               restart_wdt();

               putc(TxD_Buff[0]) ;               //Address
               putc(TxD_Buff[1]) ;               //Function Code
               putc(TxD_Buff[2]) ;               //Byte Count
               putc(TxD_Buff[3]) ;
               putc(TxD_Buff[4]) ;

               restart_wdt();
               delay_ms(3);
               restart_wdt();
               output_bit(P485ctrl,0);
               
            }
         }
////\*/
                 /*-------------jack----*/
 
      if(RxD_Buff[1] == 0x07)///////////// READ COIL (esp code)////////////
      {
         Alarmtosend();
         
         disable_interrupts(INT_TIMER2);
         //----------------------------------jj----------------------------//
         
            Data_Buff[0] = Input1_8_Send ; //>> Start_Address ;
            Data_Buff[1] = Input9_16_Send ;// >> Start_Address ;
            Data_Buff[2] = Input17_24_Send ;// >> Start_Address ;
            Data_Buff[3] = Input25_32_Send ;
            Data_Buff[4] = Input33_40_Send ;
            Data_Buff[5] = Input41_48_Send ;
            Data_Buff[6] = Input49_56_Send ;
            Data_Buff[7] = Input57_64_Send ;
           
            Data_Buff[8] = Ack1_8_Send ;
            Data_Buff[9] = Ack9_16_Send ;
            Data_Buff[10] = Ack17_24_Send ;
            Data_Buff[11] = Ack25_32_Send ;
            Data_Buff[12] = Ack33_40_Send ;
            Data_Buff[13] = Ack41_48_Send ;
            Data_Buff[14] = Ack49_56_Send ;
            Data_Buff[15] = Ack57_64_Send ;
   
            
            
            TxD_Buff[0] = Address ;         //Address
            TxD_Buff[1] = 0x07 ;         //Function Code
            TxD_Buff[2] = Data_ByteCount=0x10;   //Byte Count
            TxD_Buff[3] = Data_Buff[0] ;      //first byte Data
            TxD_Buff[4] = Data_Buff[1] ;      //second byte Data
            TxD_Buff[5] = Data_Buff[2] ;  
            TxD_Buff[6] = Data_Buff[3] ;
            TxD_Buff[7] = Data_Buff[4] ;
            TxD_Buff[8] = Data_Buff[5] ;
            TxD_Buff[9] = Data_Buff[6] ;
            TxD_Buff[10] = Data_Buff[7] ;
              
           TxD_Buff[11] = Data_Buff[8] ; //Ack1-8
           TxD_Buff[12] = Data_Buff[9] ;
           TxD_Buff[13] = Data_Buff[10] ;
           TxD_Buff[14] = Data_Buff[11] ;
           TxD_Buff[15] = Data_Buff[12] ;
           TxD_Buff[16] = Data_Buff[13] ;
           TxD_Buff[17] = Data_Buff[14] ;
           TxD_Buff[18] = Data_Buff[15] ; //Ack57-64

           CRC(TxD_Buff,19);            //Cal CRC 5 Byte

          TxD_Buff[19] = CRC_Hi ;
          TxD_Buff[20] = CRC_Lo ;

        
         output_bit(P485ctrl,1);
         restart_wdt();
         delay_ms(4);
         restart_wdt();
               
         putc(TxD_Buff[0]) ;               //Address
         putc(TxD_Buff[1]) ;               //Function Code
         putc(TxD_Buff[2]) ;               //Byte Count
         putc(TxD_Buff[3]) ;
         putc(TxD_Buff[4]) ;
         putc(TxD_Buff[5]) ;
         putc(TxD_Buff[6]) ;
         putc(TxD_Buff[7]) ;
         putc(TxD_Buff[8]) ;
         putc(TxD_Buff[9]) ;
         putc(TxD_Buff[10]) ;
         putc(TxD_Buff[11]) ;
         putc(TxD_Buff[12]) ;
         putc(TxD_Buff[13]) ;
         putc(TxD_Buff[14]) ;
         putc(TxD_Buff[15]) ;
         putc(TxD_Buff[16]) ;
         putc(TxD_Buff[17]) ;
         putc(TxD_Buff[18]) ;
         putc(TxD_Buff[19]) ;
         putc(TxD_Buff[20]) ;

         restart_wdt();
         delay_ms(3);
         restart_wdt();
         output_bit(P485ctrl,0);
           
        
         /////////////////////////////////////////
         
        enable_interrupts(INT_TIMER2); 
     
       }
      /////////////////////////////////////////////////////////////////

       else if(RxD_Buff[1] == 0x05)///////////// FORCE COIL /////////////////////
         {
            //Do Force Coil
            Start_Address = RxD_Buff[2] ;
            Start_Address = (Start_Address << 8) | RxD_Buff[3] ;   //Coil Address 16 bit
            No_PointCount = RxD_Buff[4] ;
            No_PointCount = (No_PointCount << 8) | RxD_Buff[5] ;   //Force Data 16 bit FF00 = ON, 00FF = OFF

            if(Start_Address == 0x00)   ////// Acknowlegde //////
            {
               if(No_PointCount == 0xFF00)   //ON
               {
                  //Modbus_ACK = 1 ;
                  AutoAckFlag = 1;

                  TxD_Buff[0] = Address ;         //Address
                  TxD_Buff[1] = 0x05 ;         //Function Code
                  TxD_Buff[2] = RxD_Buff[2] ;      //Coil Address Hi
                  TxD_Buff[3] = RxD_Buff[3] ;      //Coil Address Lo
                  TxD_Buff[4] = RxD_Buff[4] ;      //Force Data Hi
                  TxD_Buff[5] = RxD_Buff[5] ;      //Force Data Lo

                  CRC(TxD_Buff,6)   ;            //Cal CRC 6 Byte

                  TxD_Buff[6] = CRC_Hi ;
                  TxD_Buff[7] = CRC_Lo ;

                  //TxD_DataLen = 0x08 ;
                  //rs485_ctrl = 1;
                  //dmsec(4);
                  //send = 1;
                  //TI=1;
              output_bit(P485ctrl,1);
                  restart_wdt();
                  delay_ms(4);
                  restart_wdt();

                  putc(TxD_Buff[0]) ;               //Address
                  putc(TxD_Buff[1]) ;               //Function Code
                  putc(TxD_Buff[2]) ;               //Byte Count
                  putc(TxD_Buff[3]) ;
                  putc(TxD_Buff[4]) ;
                  putc(TxD_Buff[5]) ;
                  putc(TxD_Buff[6]) ;
                  putc(TxD_Buff[7]) ;
                  //putc(TxD_Buff[8]) ;
                  //putc(TxD_Buff[9]) ;

                  restart_wdt();
                  delay_ms(3);
                  restart_wdt();
                  output_bit(P485ctrl,0);
              
               }
            }
            else if(Start_Address == 0x01)   ///// Reset //////
            {
               if(No_PointCount == 0xFF00)   //ON
               {
                  //Modbus_RET = 1 ;
                  AutoResetFlag = 1;
                  
                  TxD_Buff[0] = Address ;         //Address
                  TxD_Buff[1] = 0x05 ;         //Function Code
                  TxD_Buff[2] = RxD_Buff[2] ;      //Coil Address Hi
                  TxD_Buff[3] = RxD_Buff[3] ;      //Coil Address Lo
                  TxD_Buff[4] = RxD_Buff[4] ;      //Force Data Hi
                  TxD_Buff[5] = RxD_Buff[5] ;      //Force Data Lo

                  CRC(TxD_Buff,6)   ;            //Cal CRC 6 Byte

                  TxD_Buff[6] = CRC_Hi ;
                  TxD_Buff[7] = CRC_Lo ;

                  //TxD_DataLen = 0x08 ;
                  //rs485_ctrl = 1;
                 //dmsec(4);
                  //send = 1;
                  //TI=1;
              output_bit(P485ctrl,1);
                  restart_wdt();
                  delay_ms(4);
                  restart_wdt();

                  putc(TxD_Buff[0]) ;               //Address
                  putc(TxD_Buff[1]) ;               //Function Code
                  putc(TxD_Buff[2]) ;               //Byte Count
                  putc(TxD_Buff[3]) ;
                  putc(TxD_Buff[4]) ;
                  putc(TxD_Buff[5]) ;
                  putc(TxD_Buff[6]) ;
                  putc(TxD_Buff[7]) ;
                  //putc(TxD_Buff[8]) ;
                  //putc(TxD_Buff[9]) ;

                  restart_wdt();
                  delay_ms(3);
                  restart_wdt();
                  output_bit(P485ctrl,0);
              
               }
            }
            else if(Start_Address == 0x02)   ///// Test //////
            {
               if(No_PointCount == 0xFF00)   //ON
               {
                  //Modbus_Lamp_Test = 1 ;
                  AutoTestFlag = 1;

                  TxD_Buff[0] = Address ;         //Address
                  TxD_Buff[1] = 0x05 ;         //Function Code
                  TxD_Buff[2] = RxD_Buff[2] ;      //Coil Address Hi
                  TxD_Buff[3] = RxD_Buff[3] ;      //Coil Address Lo
                  TxD_Buff[4] = RxD_Buff[4] ;      //Force Data Hi
                  TxD_Buff[5] = RxD_Buff[5] ;      //Force Data Lo

                  CRC(TxD_Buff,6)   ;            //Cal CRC 6 Byte

                  TxD_Buff[6] = CRC_Hi ;
                  TxD_Buff[7] = CRC_Lo ;

                 //TxD_DataLen = 0x08 ;
                 //rs485_ctrl = 1;
                 //dmsec(4);
                 //send = 1;
                 //TI=1;
                  output_bit(P485ctrl,1);
                  restart_wdt();
                  delay_ms(4);
                  restart_wdt();

                  putc(TxD_Buff[0]) ;               //Address
                  putc(TxD_Buff[1]) ;               //Function Code
                  putc(TxD_Buff[2]) ;               //Byte Count
                  putc(TxD_Buff[3]) ;
                  putc(TxD_Buff[4]) ;
                  putc(TxD_Buff[5]) ;
                  putc(TxD_Buff[6]) ;
                  putc(TxD_Buff[7]) ;
                  //putc(TxD_Buff[8]) ;
                  //putc(TxD_Buff[9]) ;

                  restart_wdt();
                  delay_ms(3);
                  restart_wdt();
                  output_bit(P485ctrl,0);
             
               }
               else if(No_PointCount == 0x00)   //OFF
               {
                  //Modbus_Lamp_Test = 0 ;
                  AutoTestFlag = 0;
                  Read_input();
                  AutoAckFlag = 1;
                  check_ack();
                  AutoResetFlag = 1;
                  check_reset();

                  TxD_Buff[0] = Address ;         //Address
                  TxD_Buff[1] = 0x05 ;         //Function Code
                  TxD_Buff[2] = RxD_Buff[2] ;      //Coil Address Hi
                  TxD_Buff[3] = RxD_Buff[3] ;      //Coil Address Lo
                  TxD_Buff[4] = RxD_Buff[4] ;      //Force Data Hi
                  TxD_Buff[5] = RxD_Buff[5] ;      //Force Data Lo

                  CRC(TxD_Buff,6)   ;            //Cal CRC 6 Byte

                  TxD_Buff[6] = CRC_Hi ;
                  TxD_Buff[7] = CRC_Lo ;

                  output_bit(P485ctrl,1);
                  restart_wdt();
                  delay_ms(4);
                  restart_wdt();

                  putc(TxD_Buff[0]) ;               //Address
                  putc(TxD_Buff[1]) ;               //Function Code
                  putc(TxD_Buff[2]) ;               //Byte Count
                  putc(TxD_Buff[3]) ;
                  putc(TxD_Buff[4]) ;
                  putc(TxD_Buff[5]) ;
                  putc(TxD_Buff[6]) ;
                  putc(TxD_Buff[7]) ;
                  //putc(TxD_Buff[8]) ;
                  //putc(TxD_Buff[9]) ;

                  restart_wdt();
                  delay_ms(3);
                  restart_wdt();
                  output_bit(P485ctrl,0);
                  
               }
            }
            else if(Start_Address == 0x03)   ///// Function Test //////
            {
               if(No_PointCount == 0xFF00)   //ON
               {
                  //Modbus_Lamp_Test = 1 ;
                  Test_fault = 1 ;

                  TxD_Buff[0] = Address ;         //Address
                  TxD_Buff[1] = 0x05 ;         //Function Code
                  TxD_Buff[2] = RxD_Buff[2] ;      //Coil Address Hi
                  TxD_Buff[3] = RxD_Buff[3] ;      //Coil Address Lo
                  TxD_Buff[4] = RxD_Buff[4] ;      //Force Data Hi
                  TxD_Buff[5] = RxD_Buff[5] ;      //Force Data Lo

                  CRC(TxD_Buff,6)   ;            //Cal CRC 6 Byte

                  TxD_Buff[6] = CRC_Hi ;
                  TxD_Buff[7] = CRC_Lo ;

                  output_bit(P485ctrl,1);
                  restart_wdt();
                  delay_ms(8);
                  restart_wdt();

                  putc(TxD_Buff[0]) ;               //Address
                  putc(TxD_Buff[1]) ;               //Function Code
                  putc(TxD_Buff[2]) ;               //Byte Count
                  putc(TxD_Buff[3]) ;
                  putc(TxD_Buff[4]) ;
                  putc(TxD_Buff[5]) ;
                  putc(TxD_Buff[6]) ;
                  putc(TxD_Buff[7]) ;
                  //putc(TxD_Buff[8]) ;
                  //putc(TxD_Buff[9]) ;

                  restart_wdt();
                  delay_ms(8);
                  restart_wdt();
                  output_bit(P485ctrl,0);
              
               }
               else if(No_PointCount == 0x00)   //OFF
               {
                  //Modbus_Lamp_Test = 0 ;
                  Test_fault = 0;
                  Read_input();
                  AutoAckFlag = 1;
                  check_ack();
                  AutoResetFlag = 1;
                  check_reset();

                  TxD_Buff[0] = Address ;         //Address
                  TxD_Buff[1] = 0x05 ;         //Function Code
                  TxD_Buff[2] = RxD_Buff[2] ;      //Coil Address Hi
                  TxD_Buff[3] = RxD_Buff[3] ;      //Coil Address Lo
                  TxD_Buff[4] = RxD_Buff[4] ;      //Force Data Hi
                  TxD_Buff[5] = RxD_Buff[5] ;      //Force Data Lo

                  CRC(TxD_Buff,6)   ;            //Cal CRC 6 Byte

                  TxD_Buff[6] = CRC_Hi ;
                  TxD_Buff[7] = CRC_Lo ;

                  //TxD_DataLen = 0x08 ;
                  //rs485_ctrl = 1;
                  //dmsec(4);
                  //send = 1;
                  //TI=1;
                  output_bit(P485ctrl,1);
                  restart_wdt();
                  delay_ms(4);
                  restart_wdt();

                  putc(TxD_Buff[0]) ;               //Address
                  putc(TxD_Buff[1]) ;               //Function Code
                  putc(TxD_Buff[2]) ;               //Byte Count
                  putc(TxD_Buff[3]) ;
                  putc(TxD_Buff[4]) ;
                  putc(TxD_Buff[5]) ;
                  putc(TxD_Buff[6]) ;
                  putc(TxD_Buff[7]) ;

                  restart_wdt();
                  delay_ms(3);
                  restart_wdt();
                  output_bit(P485ctrl,0);
                  
               }
            }
            else if(Start_Address == 0x64)   ///// Change Modbus Addr //////
            {
               Address = No_PointCount;
               write_eeprom(0x1D,Address);   //Communication Address
               
               TxD_Buff[0] = Address ;         //Address
               TxD_Buff[1] = 0x05 ;         //Function Code
               TxD_Buff[2] = RxD_Buff[2] ;      //Coil Address Hi
               TxD_Buff[3] = RxD_Buff[3] ;      //Coil Address Lo
               TxD_Buff[4] = RxD_Buff[4] ;      //Force Data Hi
               TxD_Buff[5] = RxD_Buff[5] ;      //Force Data Lo

               CRC(TxD_Buff,6)   ;            //Cal CRC 6 Byte

               TxD_Buff[6] = CRC_Hi ;
               TxD_Buff[7] = CRC_Lo ;

               output_bit(P485ctrl,1);
               restart_wdt();
               delay_ms(4);
               restart_wdt();

               putc(TxD_Buff[0]) ;               //Address
               putc(TxD_Buff[1]) ;               //Function Code
               putc(TxD_Buff[2]) ;               //Byte Count
               putc(TxD_Buff[3]) ;
               putc(TxD_Buff[4]) ;
               putc(TxD_Buff[5]) ;
               putc(TxD_Buff[6]) ;
               putc(TxD_Buff[7]) ;
               //putc(TxD_Buff[8]) ;
               //putc(TxD_Buff[9]) ;

               restart_wdt();
               delay_ms(3);
               restart_wdt();
               output_bit(P485ctrl,0);


               //save_addr();
            }
         }
         //--------------------------------------------------//
         
         else if(RxD_Buff[1] == 0x21)///////////// WRITE SETTING /////////////////////
         {

            write_eeprom(0x00,0x0F);

            write_eeprom(0x01,RxD_Buff[3]);   //Inputt Type
            write_eeprom(0x02,RxD_Buff[4]);
            write_eeprom(0x03,RxD_Buff[5]);

            write_eeprom(0x04,RxD_Buff[6]);   //Fault Type
            write_eeprom(0x05,RxD_Buff[7]);
            write_eeprom(0x06,RxD_Buff[8]);

            write_eeprom(0x07,RxD_Buff[9]);   //Output Type
            write_eeprom(0x08,RxD_Buff[10]);
            write_eeprom(0x09,RxD_Buff[11]);

            write_eeprom(0x0A,RxD_Buff[12]);   //Output Both
            write_eeprom(0x0B,RxD_Buff[13]);
            write_eeprom(0x0C,RxD_Buff[14]);

            write_eeprom(0x0D,RxD_Buff[15]);   //Alarm / Indicator
            write_eeprom(0x0E,RxD_Buff[16]);
            write_eeprom(0x0F,RxD_Buff[17]);
         
            // LED Colour Config
            write_eeprom(0x10,RxD_Buff[18]); //Red1_8
            write_eeprom(0x11,RxD_Buff[19]); //Red9_10
            write_eeprom(0x12,RxD_Buff[20]); //Red11_18
            write_eeprom(0x13,RxD_Buff[21]); //Red19_20
            write_eeprom(0x14,RxD_Buff[22]); //Green1_8
            write_eeprom(0x15,RxD_Buff[23]); //Green9_10
            write_eeprom(0x16,RxD_Buff[24]); //Green11_18
            write_eeprom(0x17,RxD_Buff[25]); //Green19_20
               
            write_eeprom(0x18,RxD_Buff[26]);   //Auto acknowledge
            write_eeprom(0x19,RxD_Buff[27]);   //Auto acknowledge Time
            write_eeprom(0x1A,RxD_Buff[28]);   //Flashing Rate
            write_eeprom(0x1B,RxD_Buff[29]);   //No of point
            write_eeprom(0x1C,RxD_Buff[30]);   //FaultDelayTime
            write_eeprom(0x1D,RxD_Buff[31]);   //Communication Address
            
            write_eeprom(0x1E,RxD_Buff[32]);   //Communication Address
            write_eeprom(0x1F,RxD_Buff[33]);   //Communication Address
            write_eeprom(0x20,RxD_Buff[34]);   //Communication Address
            write_eeprom(0x21,RxD_Buff[35]);   //Communication Address
            write_eeprom(0x22,RxD_Buff[36]);   //Communication Address
            write_eeprom(0x23,RxD_Buff[37]);   //Communication Address
            write_eeprom(0x24,RxD_Buff[38]);   //Communication Address
            write_eeprom(0x25,RxD_Buff[39]);   //Communication Address
            write_eeprom(0x26,RxD_Buff[40]);   //Communication Address
            write_eeprom(0x27,RxD_Buff[41]);   //Communication Address
            write_eeprom(0x28,RxD_Buff[42]);   //Communication Address
            write_eeprom(0x29,RxD_Buff[43]);   //Communication Address
            write_eeprom(0x2A,RxD_Buff[44]);   //Communication Address
            write_eeprom(0x2B,RxD_Buff[45]);   //Communication Address
            write_eeprom(0x2C,RxD_Buff[46]);   //Communication Address
            write_eeprom(0x2D,RxD_Buff[47]);   //Communication Address
            write_eeprom(0x2E,RxD_Buff[48]);   //Communication Address
            write_eeprom(0x2F,RxD_Buff[49]);   //Communication Address
            write_eeprom(0x30,RxD_Buff[50]);   //Communication Address
            write_eeprom(0x31,RxD_Buff[51]);   //Communication Address
            
            int16  a = 0; 
            unsigned char  phonenum;
            for(; ; a++)
            {
               restart_wdt();
               phonenum = RxD_Buff[52 + a];
               if((phonenum == 0x0D) || (a > 15))
               {
                  sms_phonenumber[a] =  '\0' ;
                  write_eeprom(0x32+a,phonenum);
                  break;
               }
               else
               {
                  sms_phonenumber[a] = phonenum;
                  
                  write_eeprom(0x32+a,phonenum);
               }
            }
            
            TxD_Buff[0] = Address ;         //Address
            TxD_Buff[1] = 0x21 ;            //return function code

            CRC(TxD_Buff,2)   ;            //Cal CRC 2 byte

            TxD_Buff[2] = CRC_Hi ;
            TxD_Buff[3] = CRC_Lo ;

            output_bit(P485ctrl,1);
            delay_ms(10);

            putc(Txd_Buff[0]);
            putc(Txd_Buff[1]);
            putc(Txd_Buff[2]);
            putc(Txd_Buff[3]);

            delay_ms(3);
            output_bit(P485ctrl,0);
            
            reset_cpu();
            //Read_Config(); //jj
         }
         
         else if(RxD_Buff[1] == 0x22)///////////// WRITE Faultname /////////////////////
            {
               
               //SMS_Massage
               int16  i =4,j=0,k=0; //i =4 are first data from RxD_Buff[]
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,0x0D);
                     break;
                  }
                  else
                  {
                     SMS_Massage1[j] = RxD_Buff[i];
                     
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage1[j] = '\0' ; // end string
               
               j=0;
               i++;
               
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {  
                     write_eeprom(0x5D+i,0x0D);
                     break;
                  }
                  else
                  {
                     SMS_Massage2[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage2[j] = '\0' ; // end string
               
               j=0;
               i++;
               
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,0x0D);
                     break;
                  }
                  else
                  {
                     SMS_Massage3[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage3[j] = '\0' ; // end string
               
               j=0;
               i++;
               
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,0x0D);
                     break;
                  }
                  else
                  {
                     SMS_Massage4[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage4[j] = '\0' ; // end string
               
               j=0;
               i++;
               
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,0x0D);
                     break;
                  }
                  else
                  {
                     SMS_Massage5[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage5[j] = '\0' ; // end string
               
               j=0;
               i++;
               
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,0x0D);
                     break;
                  }
                  else
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     SMS_Massage6[j] = RxD_Buff[i];
                  }
               }
               SMS_Massage6[j] = '\0' ; // end string
               
               j=0;
               i++;
               
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,0x0D);
                     break;
                  }
                  else
                  {
                     SMS_Massage7[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage7[j] = '\0' ; // end string
               
               j=0;
               i++;
               
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,0x0D);
                     break;
                  }
                  else
                  {
                     SMS_Massage8[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage8[j] = '\0' ; // end string
               
               /////////////////////////////////////////////
               j=0;
               i++;
               
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage9[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage9[j] = '\0' ; // end string
               j=0;
               i++;
               
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage10[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage10[j] = '\0' ; // end string
               
               j=0;
               i++;
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage11[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage11[j] = '\0' ; // end string
               
               j=0;
               i++;
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage12[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage12[j] = '\0' ; // end string
               
               j=0;
               i++;
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage13[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage13[j] = '\0' ; // end string
               
               j=0;
               i++;
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage14[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage14[j] = '\0' ; // end string
               
               j=0;
               i++;
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage15[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage15[j] = '\0' ; // end string
               
               j=0;
               i++;
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage16[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage16[j] = '\0' ; // end string
               
               j=0;
               i++;
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage17[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage17[j] = '\0' ; // end string
               
               j=0;
               i++;
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage18[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage18[j] = '\0' ; // end string
               
               j=0;
               i++;
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage19[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage19[j] = '\0' ; // end string
               
               j=0;
               i++;
               for(; ; i++,j++)
               {
                  restart_wdt();
                  if((RxD_Buff[i] == 0x0D) || (j > 30))
                  {
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                     break;
                  }
                  else
                  {
                     SMS_Massage20[j] = RxD_Buff[i];
                     write_eeprom(0x5D+i,RxD_Buff[i]);
                  }
               }
               SMS_Massage20[j] = '\0' ; // end string
               
            
               TxD_Buff[0] = Address ;         //Address
               TxD_Buff[1] = 0x22 ;            //return function code
   
               CRC(TxD_Buff,2)   ;            //Cal CRC 2 byte
   
               TxD_Buff[2] = CRC_Hi ;
               TxD_Buff[3] = CRC_Lo ;
   
               output_bit(P485ctrl,1);
               delay_ms(10);
   
               putc(Txd_Buff[0]);
               putc(Txd_Buff[1]);
               putc(Txd_Buff[2]);
               putc(Txd_Buff[3]);
   
               delay_ms(3);
               output_bit(P485ctrl,0);
            }
            ////////////////////////////// WRITE Faultname //////////////////////////

         
                  if(RxD_Buff[1] == 0x03)///////////// READ HOLDING REGGISTER /////////////////////
         {
            Data_Buff[0] = Input9_16; //>> Start_Address ;
            Data_Buff[1] = Input1_8;// >> Start_Address ;
            Data_Buff[2] = Input25_32;
            Data_Buff[3] = Input17_24;// >> Start_Address ;
            
            TxD_Buff[0] = Address ;         //Address
            TxD_Buff[1] = 0x03 ;           //Function Code
            TxD_Buff[2] = 0x04 ;          //Byte Count
            TxD_Buff[3] = Data_Buff[0];   //first byte Data
            TxD_Buff[4] = Data_Buff[1];  //second byte Data
            TxD_Buff[5] = Data_Buff[2]; 
            TxD_Buff[6] = Data_Buff[3]; 

            CRC(TxD_Buff,7)   ;            //Cal CRC 5 Byte

            TxD_Buff[7] = CRC_Hi ;
            TxD_Buff[8] = CRC_Lo ;

            output_bit(P485ctrl,1);
            restart_wdt();
            delay_ms(4);
            restart_wdt();

            putc(TxD_Buff[0]) ;               //Address
            putc(TxD_Buff[1]) ;               //Function Code
            putc(TxD_Buff[2]) ;               //Byte Count
            putc(TxD_Buff[3]) ;
            putc(TxD_Buff[4]) ;
            putc(TxD_Buff[5]) ;
            putc(TxD_Buff[6]) ;
            putc(TxD_Buff[7]) ;
            putc(TxD_Buff[8]) ;
            //putc(TxD_Buff[9]) ;

            restart_wdt();
            delay_ms(3);
            restart_wdt();
            output_bit(P485ctrl,0);

         }
       
      }  

      Send_check_Time = 500; //5 Second
   }

   recieve_completed = 0 ;
   sequence = end_sq ;
   T_timeout = 0x00;
   RxD_DataLen = 0x00 ;
   output_bit(P485ctrl,0);
}


////////////////////////////////////////////////////////////////////

void Read_Input(void)
{

    ///////////////////////--Edit--///////////////////
    //unsigned char MCP23s17_dat1=0;
    //unsigned char MCP23s17_dat2=0;
    //unsigned char MCP23s17_dat3=0;
    //unsigned char MCP23s17_dat4=0;
    
    /*
    MCP23s17_dat1 = IO_INPUT_A(IO_DEVICE_0);
    MCP23s17_dat2 = IO_INPUT_B(IO_DEVICE_0);
    MCP23s17_dat3 = IO_INPUT_A(IO_DEVICE_1);
    MCP23s17_dat4 = IO_INPUT_B(IO_DEVICE_1);
    if((MCP23s17_dat1==0)&&(MCP23s17_dat2==0)&&(MCP23s17_dat3==0)&&(MCP23s17_dat4==0))
     return;
     */
    /*
    do
    {
      MCP23s17_dat1 = IO_INPUT_A(IO_DEVICE_0);
      //MCP23s17_dat2 = IO_INPUT_B(IO_DEVICE_0);
      MCP23s17_dat3 = IO_INPUT_A(IO_DEVICE_1);
      //MCP23s17_dat4 = IO_INPUT_B(IO_DEVICE_1);
      restart_wdt();
    }while((MCP23s17_dat1==0)&&(MCP23s17_dat3==0));
    */
     
      MCP23s17_Ip_dat = IO_INPUT_A(IO_DEVICE_0);
        
      if (AutoTestFlag)  // TEST FROM MODBUS
      {
         MCP23s17_Ip_dat = ~MCP23s17_Ip_dat;         
      }
      
      FaultNow[11] = MCP23s17_Ip_dat;
      FaultNow[12] = MCP23s17_Ip_dat >> 1;
      FaultNow[13] = MCP23s17_Ip_dat >> 2;
      FaultNow[14] = MCP23s17_Ip_dat >> 3;
      FaultNow[15] = MCP23s17_Ip_dat >> 4;
      FaultNow[16] = MCP23s17_Ip_dat >> 5;
      FaultNow[17] = MCP23s17_Ip_dat >> 6;
      FaultNow[18] = MCP23s17_Ip_dat >> 7;
      
      MCP23s17_Ip_dat = IO_INPUT_B(IO_DEVICE_0);
      
      
      if (AutoTestFlag)  // TEST FROM MODBUS
      {
         MCP23s17_Ip_dat = ~MCP23s17_Ip_dat;
      }
      
      FaultNow[19] = MCP23s17_Ip_dat;
      FaultNow[20] = MCP23s17_Ip_dat >> 1;
      
      
      MCP23s17_Ip_dat = IO_INPUT_A(IO_DEVICE_1);
      
          
      if (AutoTestFlag)  // TEST FROM MODBUS
      {
         MCP23s17_Ip_dat = ~MCP23s17_Ip_dat;
      }
      
      FaultNow[1] = MCP23s17_Ip_dat;
      FaultNow[2] = MCP23s17_Ip_dat >> 1;
      FaultNow[3] = MCP23s17_Ip_dat >> 2;
      FaultNow[4] = MCP23s17_Ip_dat >> 3;
      FaultNow[5] = MCP23s17_Ip_dat >> 4;
      FaultNow[6] = MCP23s17_Ip_dat >> 5;
      FaultNow[7] = MCP23s17_Ip_dat >> 6;
      FaultNow[8] = MCP23s17_Ip_dat >> 7;
      
      MCP23s17_Ip_dat = IO_INPUT_B(IO_DEVICE_1);
      
      if (AutoTestFlag)  // TEST FROM MODBUS
      {
         MCP23s17_Ip_dat = ~MCP23s17_Ip_dat;
      }
      
      FaultNow[9] = MCP23s17_Ip_dat;
      FaultNow[10] = MCP23s17_Ip_dat >> 1;
      
      ///////////////---Anal Fault---//////////////////
      unsigned char i;
      for( i=1; i<=20; i++)
      {
         if(FaultNCNO[i] ==NO)
         {
             if(FaultNow[i]==0)
             {   
               if(FaultAgo[i]==0)
               {
                   ReleaseTime[i]++;
                  
                   if(ReleaseTime[i]>=((FaultDelayTime[i]*75)+15)) //time per loop are 368 microSec. (1.89*10=200ms)
                   {   
                     ReleaseTime[i] = 0;
                     switch(i)
                     {
                        case 1:
                           Inputt.B1 = 0; 
                        break;
                        case 2:
                           Inputt.B2 = 0; 
                        break;
                        case 3:
                           Inputt.B3 = 0; 
                        break;
                        case 4:
                           Inputt.B4 = 0; 
                        break;
                        case 5:
                           Inputt.B5 = 0; 
                        break;
                        case 6:
                           Inputt.B6 = 0; 
                        break;
                        case 7:
                           Inputt.B7 = 0; 
                         break;
                         case 8:
                           Inputt.B8 = 0; 
                        break;
                        case 9:
                           Inputt.B9 = 0; 
                        break;
                        case 10:
                           Inputt.B10 = 0; 
                        break;
                        case 11:
                           Inputt.B11 = 0; 
                        break;
                        case 12:
                           Inputt.B12 = 0; 
                        break;
                        case 13:
                           Inputt.B13 = 0; 
                        break;
                        case 14:
                           Inputt.B14 = 0; 
                        break;
                        case 15:
                           Inputt.B15 = 0; 
                        break;
                        case 16:
                            Inputt.B16 = 0; 
                         break; 
                        case 17:
                           Inputt.B17 = 0; 
                         break;
                         case 18:
                           Inputt.B18 = 0; 
                        break;
                        case 19:
                           Inputt.B19 = 0; 
                        break;
                        case 20:
                           Inputt.B20 = 0; 
                        break;
                                   
                     }                 
                   }
               }
               else
               {
                  ReleaseTime[i] = 0;
               }
             }
             else //if(FaultNow[i]==1
             {
               if(FaultAgo[i]==1)
               {
                   ReleaseTime[i]++;
                  
                   if(ReleaseTime[i]>=((FaultDelayTime[i]*75)+15)) //time per loop are 368 microSec. (1.89*10=200ms)
                   {   
                     ReleaseTime[i] = 1;
                     switch(i)
                     {
                        case 1:
                           Inputt.B1 = 1; 
                        break;
                        case 2:
                           Inputt.B2 = 1; 
                        break;
                        case 3:
                           Inputt.B3 = 1; 
                        break;
                        case 4:
                           Inputt.B4 = 1; 
                        break;
                        case 5:
                           Inputt.B5 = 1; 
                        break;
                        case 6:
                           Inputt.B6 = 1; 
                        break;
                        case 7:
                           Inputt.B7 = 1; 
                         break;
                         case 8:
                           Inputt.B8 = 1; 
                        break;
                        case 9:
                           Inputt.B9 = 1; 
                        break;
                        case 10:
                           Inputt.B10 = 1; 
                        break;
                        case 11:
                           Inputt.B11 = 1; 
                        break;
                        case 12:
                           Inputt.B12 = 1; 
                        break;
                        case 13:
                           Inputt.B13 = 1; 
                        break;
                        case 14:
                           Inputt.B14 = 1; 
                        break;
                        case 15:
                           Inputt.B15 = 1; 
                        break;
                        case 16:
                            Inputt.B16 = 1;
                         break;
                         
                         case 17:
                           Inputt.B17 = 1; 
                         break;
                         case 18:
                           Inputt.B18 = 1; 
                        break;
                        case 19:
                           Inputt.B19 = 1; 
                        break;
                        case 20:
                           Inputt.B20 = 1; 
                        break;
                                   
                     }                 
                   }
               }
               else
               {
                  ReleaseTime[i] = 0;
               }
             
             /*
                  ReleaseTime[i] = 0;
                  switch(i)
                  {
                      case 1:
                         Input.B1 = 1; 
                      break;
                      case 2:
                         Input.B2 = 1; 
                      break;
                      case 3:
                         Input.B3 = 1; 
                      break;
                      case 4:
                         Input.B4 = 1; 
                      break;
                      case 5:
                         Input.B5 = 1; 
                      break;
                      case 6:
                         Input.B6 = 1; 
                      break;
                      case 7:
                         Input.B7 = 1; 
                      break;
                      case 8:
                         Input.B8 = 1; 
                      break;
                      case 9:
                         Input.B9 = 1; 
                      break;
                      case 10:
                         Input.B10 = 1; 
                      break;
                      case 11:
                         Input.B11 = 1; 
                      break;
                      case 12:
                         Input.B12 = 1; 
                      break;
                      case 13:
                         Input.B13 = 1; 
                      break;
                      case 14:
                         Input.B14 = 1; 
                      break;
                      case 15:
                         Input.B15 = 1; 
                      break;
                      case 16:
                         Input.B16 = 1; 
                      break;
                         
                  }
                  */
             }
          }
          else if(FaultNCNO[i] ==NC)
          {
             if(FaultNow[i]==1)
             {
               if(FaultAgo[i]==1)
               {
                   ReleaseTime[i]++;
                   if(ReleaseTime[i]>=((FaultDelayTime[i]*75)+15)) //time per loop are 368 microSec. (1.89*10=200ms)
                   {   
                     ReleaseTime[i] = 0;
                     switch(i)
                     {
                        case 1:
                           Inputt.B1 = 1; 
                        break;
                        case 2:
                           Inputt.B2 = 1; 
                        break;
                        case 3:
                           Inputt.B3 = 1; 
                        break;
                        case 4:
                           Inputt.B4 = 1; 
                        break;
                        case 5:
                           Inputt.B5 = 1; 
                        break;
                        case 6:
                           Inputt.B6 = 1; 
                        break;
                        case 7:
                           Inputt.B7 = 1; 
                         break;
                         case 8:
                           Inputt.B8 = 1; 
                        break;
                        case 9:
                           Inputt.B9 = 1; 
                        break;
                        case 10:
                           Inputt.B10 = 1; 
                        break;
                        case 11:
                           Inputt.B11 = 1; 
                        break;
                        case 12:
                           Inputt.B12 = 1; 
                        break;
                        case 13:
                           Inputt.B13 = 1; 
                        break;
                        case 14:
                           Inputt.B14 = 1; 
                        break;
                        case 15:
                           Inputt.B15 = 1; 
                        break;
                        case 16:
                            Inputt.B16 = 1; 
                         break;
                         
                         case 17:
                           Inputt.B17 = 1; 
                         break;
                         case 18:
                           Inputt.B18 = 1; 
                        break;
                        case 19:
                           Inputt.B19 = 1; 
                        break;
                        case 20:
                           Inputt.B20 = 1; 
                        break;
                                                                   
                     }                 
                  }
               }
               else
               {
                  ReleaseTime[i] = 0;
               }
             }
             else //if(FaultNow[i]==0
             {
               if(FaultAgo[i]==0)
               {
                   ReleaseTime[i]++;
                  
                   if(ReleaseTime[i]>=((FaultDelayTime[i]*75)+15)) //time per loop are 368 microSec. (1.89*10=200ms)
                   {   
                     ReleaseTime[i] = 0;
                     switch(i)
                     {
                        case 1:
                           Inputt.B1 = 0; 
                        break;
                        case 2:
                           Inputt.B2 = 0; 
                        break;
                        case 3:
                           Inputt.B3 = 0; 
                        break;
                        case 4:
                           Inputt.B4 = 0; 
                        break;
                        case 5:
                           Inputt.B5 = 0; 
                        break;
                        case 6:
                           Inputt.B6 = 0; 
                        break;
                        case 7:
                           Inputt.B7 = 0; 
                         break;
                         case 8:
                           Inputt.B8 = 0; 
                        break;
                        case 9:
                           Inputt.B9 = 0; 
                        break;
                        case 10:
                           Inputt.B10 = 0; 
                        break;
                        case 11:
                           Inputt.B11 = 0; 
                        break;
                        case 12:
                           Inputt.B12 = 0; 
                        break;
                        case 13:
                           Inputt.B13 = 0; 
                        break;
                        case 14:
                           Inputt.B14 = 0; 
                        break;
                        case 15:
                           Inputt.B15 = 0; 
                        break;
                        case 16:
                            Inputt.B16 = 0;
                         break;
                         
                         case 17:
                           Inputt.B17 = 0; 
                         break;
                         case 18:
                           Inputt.B18 = 0; 
                        break;
                        case 19:
                           Inputt.B19 = 0; 
                        break;
                        case 20:
                           Inputt.B20 = 0; 
                        break;
                                   
                     }                 
                   }
               }
               else
               {
                  ReleaseTime[i] = 0;
               }

             }
          }
      }
      
      
      StoreReleaseFault();
      
      ///////////////---Anal Fault---//////////////////
      
      ///////////////////////--Edit--///////////////////
           
      Input11_18 = 0x00;
      Input11_18 = Input11_18 | ~Inputt.B18;
      Input11_18 = (Input11_18 << 1) | ~Inputt.B17;
      Input11_18 = (Input11_18 << 1) | ~Inputt.B16;
      Input11_18 = (Input11_18 << 1) | ~Inputt.B15;
      Input11_18 = (Input11_18 << 1) | ~Inputt.B14;
      Input11_18 = (Input11_18 << 1) | ~Inputt.B13;
      Input11_18 = (Input11_18 << 1) | ~Inputt.B12;
      Input11_18 = (Input11_18 << 1) | ~Inputt.B11;
      
     
   
      Input19_20 = 0x00;
      Input19_20 = Input19_20 | ~Inputt.B20;
      Input19_20 = (Input19_20 << 1) | ~Inputt.B19;
      //}
   
      Input1_8 = 0x00;
      Input1_8 = Input1_8 | ~Inputt.B8;
      Input1_8 = (Input1_8 << 1) | ~Inputt.B7;
      Input1_8 = (Input1_8 << 1) | ~Inputt.B6;
      Input1_8 = (Input1_8 << 1) | ~Inputt.B5;
      Input1_8 = (Input1_8 << 1) | ~Inputt.B4;
      Input1_8 = (Input1_8 << 1) | ~Inputt.B3;
      Input1_8 = (Input1_8 << 1) | ~Inputt.B2;
      Input1_8 = (Input1_8 << 1) | ~Inputt.B1;
      
      Input9_16 = 0x00;
      Input9_16 = Input9_16 | ~Inputt.B16;
      Input9_16 = (Input9_16 << 1) | ~Inputt.B15;
      Input9_16 = (Input9_16 << 1) | ~Inputt.B14;
      Input9_16 = (Input9_16 << 1) | ~Inputt.B13;
      Input9_16 = (Input9_16 << 1) | ~Inputt.B12;
      Input9_16 = (Input9_16 << 1) | ~Inputt.B11;
      Input9_16 = (Input9_16 << 1) | ~Inputt.B10;
      Input9_16 = (Input9_16 << 1) | ~Inputt.B9;
      
      Input17_24 = 0x00;
      Input17_24 = Input17_24 | ~Inputt.B24;
      Input17_24 = (Input17_24 << 1) | ~Inputt.B23;
      Input17_24 = (Input17_24 << 1) | ~Inputt.B22;
      Input17_24 = (Input17_24 << 1) | ~Inputt.B21;
      Input17_24 = (Input17_24 << 1) | ~Inputt.B20;
      Input17_24 = (Input17_24 << 1) | ~Inputt.B19;
      Input17_24 = (Input17_24 << 1) | ~Inputt.B18;
      Input17_24 = (Input17_24 << 1) | ~Inputt.B17;
    
   

      Input9_10 = 0x00;
      Input9_10 = Input9_10 | ~Inputt.B10;
      Input9_10 = (Input9_10 << 1) | ~Inputt.B9;
      
      Input17_24 &= 0x0F;
      Input25_32 = 0x00;
      //+++++++++++++++++++++++++++++++++++++++++++

}
/*----------------------------------------------------------------------------*/


void Read_Config(void)
{
   

   EEpDat = read_eeprom(0x01);   // Inputt type 1-8
   InputType1_8 = EEpDat;

   InputType.B1 = EEpDat;
   InputType.B2 = EEpDat >> 1;
   InputType.B3 = EEpDat >> 2;
   InputType.B4 = EEpDat >> 3;
   InputType.B5 = EEpDat >> 4;
   InputType.B6 = EEpDat >> 5;
   InputType.B7 = EEpDat >> 6;
   InputType.B8 = EEpDat >> 7;

   EEpDat = read_eeprom(0x02);   // Inputt type 9-16
   InputType9_16 = EEpDat;

   InputType.B9 = EEpDat;
   InputType.B10 = EEpDat >> 1;
   InputType.B11 = EEpDat >> 2;
   InputType.B12 = EEpDat >> 3;
   InputType.B13 = EEpDat >> 4;
   InputType.B14 = EEpDat >> 5;
   InputType.B15 = EEpDat >> 6;
   InputType.B16 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x03);   // Inputt type 9-16
   InputType17_24 = EEpDat;

   InputType.B17 = EEpDat;
   InputType.B18 = EEpDat >> 1;
   InputType.B19 = EEpDat >> 2;
   InputType.B20 = EEpDat >> 3;
   InputType.B21 = EEpDat >> 4;
   InputType.B22 = EEpDat >> 5;
   InputType.B23 = EEpDat >> 6;
   InputType.B24 = EEpDat >> 7;
   
   FaultNCNO[1] = InputType.B1;
   FaultNCNO[2] = InputType.B2;
   FaultNCNO[3] = InputType.B3;
   FaultNCNO[4] = InputType.B4;
   FaultNCNO[5] = InputType.B5;
   FaultNCNO[6] = InputType.B6;
   FaultNCNO[7] = InputType.B7;
   FaultNCNO[8] = InputType.B8;
   FaultNCNO[9] = InputType.B9;
   FaultNCNO[10] = InputType.B10;
   FaultNCNO[11] = InputType.B11;
   FaultNCNO[12] = InputType.B12;
   FaultNCNO[13] = InputType.B13;
   FaultNCNO[14] = InputType.B14;
   FaultNCNO[15] = InputType.B15;
   FaultNCNO[16] = InputType.B16;
   FaultNCNO[17] = InputType.B17;
   FaultNCNO[18] = InputType.B18;
   FaultNCNO[19] = InputType.B19;
   FaultNCNO[20] = InputType.B20;

   ////////////////////////////////////////

   EEpDat = read_eeprom(0x04);   // Fault type 1-8
   FaultType1_8 = EEpDat;

   FaultType.B1 = EEpDat;
   FaultType.B2 = EEpDat >> 1;
   FaultType.B3 = EEpDat >> 2;
   FaultType.B4 = EEpDat >> 3;
   FaultType.B5 = EEpDat >> 4;
   FaultType.B6 = EEpDat >> 5;
   FaultType.B7 = EEpDat >> 6;
   FaultType.B8 = EEpDat >> 7;

   EEpDat = read_eeprom(0x05);   // Fault type 9-16
   FaultType9_16 = EEpDat;

   FaultType.B9 = EEpDat;
   FaultType.B10 = EEpDat >> 1;
   FaultType.B11 = EEpDat >> 2;
   FaultType.B12 = EEpDat >> 3;
   FaultType.B13 = EEpDat >> 4;
   FaultType.B14 = EEpDat >> 5;
   FaultType.B15 = EEpDat >> 6;
   FaultType.B16 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x06);   // Fault type 9-16
   FaultType17_24 = EEpDat;

   FaultType.B17 = EEpDat;
   FaultType.B18 = EEpDat >> 1;
   FaultType.B19 = EEpDat >> 2;
   FaultType.B20 = EEpDat >> 3;
   FaultType.B21 = EEpDat >> 4;
   FaultType.B22 = EEpDat >> 5;
   FaultType.B23 = EEpDat >> 6;
   FaultType.B24 = EEpDat >> 7;


   ////////////////////////////////////////////////

   EEpDat = read_eeprom(0x07);   // Output type 1-8
   OutputType1_8 = EEpDat;

   OutputType.B1 = EEpDat;
   OutputType.B2 = EEpDat >> 1;
   OutputType.B3 = EEpDat >> 2;
   OutputType.B4 = EEpDat >> 3;
   OutputType.B5 = EEpDat >> 4;
   OutputType.B6 = EEpDat >> 5;
   OutputType.B7 = EEpDat >> 6;
   OutputType.B8 = EEpDat >> 7;

   EEpDat = read_eeprom(0x08);   // Output type 9-16
   OutputType9_16 = EEpDat;

   OutputType.B9 = EEpDat;
   OutputType.B10 = EEpDat >> 1;
   OutputType.B11 = EEpDat >> 2;
   OutputType.B12 = EEpDat >> 3;
   OutputType.B13 = EEpDat >> 4;
   OutputType.B14 = EEpDat >> 5;
   OutputType.B15 = EEpDat >> 6;
   OutputType.B16 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x09);   // Output type 9-16
   OutputType17_24 = EEpDat;

   OutputType.B17 = EEpDat;
   OutputType.B18 = EEpDat >> 1;
   OutputType.B19 = EEpDat >> 2;
   OutputType.B20 = EEpDat >> 3;
   OutputType.B21 = EEpDat >> 4;
   OutputType.B22 = EEpDat >> 5;
   OutputType.B23 = EEpDat >> 6;
   OutputType.B24 = EEpDat >> 7;


   ////////////////////////////////////////

   EEpDat = read_eeprom(0x0A);   // Output Both 1-8
   OutputBoth1_8 = EEpDat;

   OutputBoth.B1 = EEpDat;
   OutputBoth.B2 = EEpDat >> 1;
   OutputBoth.B3 = EEpDat >> 2;
   OutputBoth.B4 = EEpDat >> 3;
   OutputBoth.B5 = EEpDat >> 4;
   OutputBoth.B6 = EEpDat >> 5;
   OutputBoth.B7 = EEpDat >> 6;
   OutputBoth.B8 = EEpDat >> 7;

   EEpDat = read_eeprom(0x0B);   // Output Both 9-16
   OutputBoth9_16 = EEpDat;

   OutputBoth.B9 = EEpDat;
   OutputBoth.B10 = EEpDat >> 1;
   OutputBoth.B11 = EEpDat >> 2;
   OutputBoth.B12 = EEpDat >> 3;
   OutputBoth.B13 = EEpDat >> 4;
   OutputBoth.B14 = EEpDat >> 5;
   OutputBoth.B15 = EEpDat >> 6;
   OutputBoth.B16 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x0C);   // Output Both 9-16
   OutputBoth17_24 = EEpDat;

   OutputBoth.B17 = EEpDat;
   OutputBoth.B18 = EEpDat >> 1;
   OutputBoth.B19 = EEpDat >> 2;
   OutputBoth.B20 = EEpDat >> 3;
   OutputBoth.B21 = EEpDat >> 4;
   OutputBoth.B22 = EEpDat >> 5;
   OutputBoth.B23 = EEpDat >> 6;
   OutputBoth.B24 = EEpDat >> 7;


   /////////////////////////////////////////////////

   EEpDat = read_eeprom(0x0D);   // AlarmIndicator 1-8
   Alarm_Indicator1_8 = EEpDat;

   AlarmIndicator.B1 = EEpDat;
   AlarmIndicator.B2 = EEpDat >> 1;
   AlarmIndicator.B3 = EEpDat >> 2;
   AlarmIndicator.B4 = EEpDat >> 3;
   AlarmIndicator.B5 = EEpDat >> 4;
   AlarmIndicator.B6 = EEpDat >> 5;
   AlarmIndicator.B7 = EEpDat >> 6;
   AlarmIndicator.B8 = EEpDat >> 7;

   EEpDat = read_eeprom(0x0E);   // AlarmIndicator 9-16
   Alarm_Indicator9_16 = EEpDat;

   AlarmIndicator.B9 = EEpDat;
   AlarmIndicator.B10 = EEpDat >> 1;
   AlarmIndicator.B11 = EEpDat >> 2;
   AlarmIndicator.B12 = EEpDat >> 3;
   AlarmIndicator.B13 = EEpDat >> 4;
   AlarmIndicator.B14 = EEpDat >> 5;
   AlarmIndicator.B15 = EEpDat >> 6;
   AlarmIndicator.B16 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x0F);   // AlarmIndicator 9-16
   Alarm_Indicator17_24 = EEpDat;

   AlarmIndicator.B17 = EEpDat;
   AlarmIndicator.B18 = EEpDat >> 1;
   AlarmIndicator.B19 = EEpDat >> 2;
   AlarmIndicator.B20 = EEpDat >> 3;
   AlarmIndicator.B21 = EEpDat >> 4;
   AlarmIndicator.B22 = EEpDat >> 5;
   AlarmIndicator.B23 = EEpDat >> 6;
   AlarmIndicator.B24 = EEpDat >> 7;

   //////////////////////////////////////////////////
   EEpDat = read_eeprom(0x10);
   //Red1_8 = EEpDat;
   RED_Colour.B1 = EEpDat;
   RED_Colour.B2 = EEpDat >> 1;
   RED_Colour.B3 = EEpDat >> 2;
   RED_Colour.B4 = EEpDat >> 3;
   RED_Colour.B5 = EEpDat >> 4;
   RED_Colour.B6 = EEpDat >> 5;
   RED_Colour.B7 = EEpDat >> 6;
   RED_Colour.B8 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x11);
   //Green9_10 = EEpDat;
   RED_Colour.B9 = EEpDat;
   RED_Colour.B10 = EEpDat >> 1;
   
   
   EEpDat = read_eeprom(0x12);
   //Red11_18 = EEpDat;
   RED_Colour.B11 = EEpDat;
   RED_Colour.B12 = EEpDat >> 1;
   RED_Colour.B13 = EEpDat >> 2;
   RED_Colour.B14 = EEpDat >> 3;
   RED_Colour.B15 = EEpDat >> 4;
   RED_Colour.B16 = EEpDat >> 5;
   RED_Colour.B17 = EEpDat >> 6;
   RED_Colour.B18 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x13);
   //Red19_20 = EEpDat;
   RED_Colour.B19 = EEpDat;
   RED_Colour.B20 = EEpDat >> 1;
   
   
   EEpDat = read_eeprom(0x14);
   //Green1_8 = EEpDat;
   GREEN_Colour.B1 = EEpDat;
   GREEN_Colour.B2 = EEpDat >> 1;
   GREEN_Colour.B3 = EEpDat >> 2;
   GREEN_Colour.B4 = EEpDat >> 3;
   GREEN_Colour.B5 = EEpDat >> 4;
   GREEN_Colour.B6 = EEpDat >> 5;
   GREEN_Colour.B7 = EEpDat >> 6;
   GREEN_Colour.B8 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x15);
   //Green9_10 = EEpDat;
   GREEN_Colour.B9 = EEpDat;
   GREEN_Colour.B10 = EEpDat >> 1;
   
   EEpDat = read_eeprom(0x16);
   //Green11_18 = EEpDat;
   GREEN_Colour.B11 = EEpDat;
   GREEN_Colour.B12 = EEpDat >> 1;
   GREEN_Colour.B13 = EEpDat >> 2;
   GREEN_Colour.B14 = EEpDat >> 3;
   GREEN_Colour.B15 = EEpDat >> 4;
   GREEN_Colour.B16 = EEpDat >> 5;
   GREEN_Colour.B17 = EEpDat >> 6;
   GREEN_Colour.B18 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x17);
   //Green19_20 = EEpDat;
   GREEN_Colour.B19 = EEpDat;
   GREEN_Colour.B20 = EEpDat >> 1;
   
  

   //////////////////////////////////////////////////

   AutoAck = read_eeprom(0x18);          // Auto Acknoeledge
   AutoAckTime = read_eeprom(0x19);      // Auto Acknoeledge Time
   FlashingRate = read_eeprom(0x1A); // Flashing rate
   NoOfPoint = read_eeprom(0x1B);        // Number of total point
   FaultDelayTime[0] = read_eeprom(0x1C);  // Master or slave sync(flashing)

   Address = read_eeprom(0x1D);          //Communication address
   
   FaultDelayTime[1] = read_eeprom(0x1E);
   FaultDelayTime[2] = read_eeprom(0x1F);
   FaultDelayTime[3] = read_eeprom(0x20);
   FaultDelayTime[4] = read_eeprom(0x21);
   FaultDelayTime[5] = read_eeprom(0x22);
   FaultDelayTime[6] = read_eeprom(0x23);
   FaultDelayTime[7] = read_eeprom(0x24);
   FaultDelayTime[8] = read_eeprom(0x25);
   FaultDelayTime[9] = read_eeprom(0x26);
   FaultDelayTime[10] = read_eeprom(0x27);
   FaultDelayTime[11] = read_eeprom(0x28);
   FaultDelayTime[12] = read_eeprom(0x29);
   FaultDelayTime[13] = read_eeprom(0x2A);
   FaultDelayTime[14] = read_eeprom(0x2B);
   FaultDelayTime[15] = read_eeprom(0x2C);
   FaultDelayTime[16] = read_eeprom(0x2D);
   FaultDelayTime[17] = read_eeprom(0x2E);
   FaultDelayTime[18] = read_eeprom(0x2F);
   FaultDelayTime[19] = read_eeprom(0x30);
   FaultDelayTime[20] = read_eeprom(0x31);
   
   unsigned int  a = 0; 
   unsigned char  phonenum;
   for(; ; a++)
   {
      restart_wdt();
      phonenum = read_eeprom(0x32 + a);
      if((phonenum == 0x0D) || (a > 15))
      {
         sms_phonenumber[a] =  '\0';
         write_eeprom(0x32+a,phonenum);
         break;
      }
      else
      {
         sms_phonenumber[a] = phonenum;
      }
   }
   
      
   int16  i =3,j=0,k=0 , buff;
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D)|| (j>30))
      {
         SMS_Massage1[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage1[j] = buff;  
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage2[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage2[j] = buff;         
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage3[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage3[j] = buff;         
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage4[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage4[j] = buff;         
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage5[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage5[j] = buff;         
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage6[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage6[j] = buff;         
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage7[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage7[j] = buff;         
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage8[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage8[j] = buff;        
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage9[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage9[j] = buff;         
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage10[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage10[j] = buff;        
      }
   }
      
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage11[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage11[j] = buff;        
      }
   }
      
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage12[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage12[j] = buff;         
      }
   }
      
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage13[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage13[j] = buff;       
      }
   }
      
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage14[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage14[j] = buff;        
      }
   }
      
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage15[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage15[j] = buff;         
      }
   }
      
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage16[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage16[j] = buff;         
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage17[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage17[j] = buff;         
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage18[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage18[j] = buff;         
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage19[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage19[j] = buff;        
      }
   }
   
   i++;
   j=0;
   
   for(; ; i++,j++)
   {
      restart_wdt();
      buff = read_eeprom(0x5D+i);
      
      if((buff == 0x0D) || (j>30))
      {
         SMS_Massage20[j] = '\0' ; // end string
         break;
      }
      else
      {
         SMS_Massage20[j] = buff;         
      }
   }

}
//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#int_RDA
void RDA_isr(void)
{

   //Data = getc();
   //putc(Data);
   SBUF = getc();
   checkCommand();
}
/*
#int_TIMER1
void  TIMER1_isr(void) 
{
   //ReadIn_flag =1;
}
*/
#int_TIMER2
void TIMER2_isr(void)      //10ms
{
   StatusTime++;

   if(T_timeout != 0)
   {
      T_timeout--;
      if(T_timeout == 0)
      {
         //sequence = stop_sq;         //timeout
         sequence = end_sq;         //timeout 
         output_bit(P485ctrl,0);
      }
   }
   ////////// Time Base 1 Second /////////////
   if(TimeBase1s != 0x00)
   {
      TimeBase1s--;
      if(TimeBase1s == 0x00)      // <====== code in time base 1 second
      {
         if(AutoAckDelayTime != 0x00)
         {
            AutoAckDelayTime--;
            if(AutoAckDelayTime == 0x00)
            {
               AutoAckFlag = 1 ;
            }
         }
         //////////////////////////////
         
         if(++StartReadCount >=2)
         {
            StartRead = 1;
            StartReadCount =2;
         }
         if(++send_SMS_count >=5){
            send_SMS_count =0;
            send_SMS_period = 1;
         }
         //////////////////////////////
         RefreshConfigData = 1;
         TimeBase1s = 100;
      }

   }
   ///////////// End Time base 1 Second ///////



   if(StatusTime == 40)    //500mS
   {
      StatusTime = 0;
      if(HearthbeatInd == 0)
      {
         HearthbeatInd = 1;
         output_bit(Pled,1);
      }
      else
      {
         HearthbeatInd = 0;
         output_bit(Pled,0);
      }
      /////////////////////////////////////////// time base 500 ms
      if(Test == 1)            //Test function
         {
             if(T_test > 0x00) T_test --;
             if(T_test == 0x00) Test_fault = 1;
         }
         else
         {
             //Test = 0;
             T_test = 0x00;
         }
   }
   ///////////////////////////////
   if(FlashingRateTime != 0)
   {
      FlashingRateTime--;
      if(FlashingRateTime == 0)
      {
         outmcp23 = 0xff;
         
         if(SyncStatus == 0)
         {
            if(FlashingFlag == 1)
            {
               FlashingFlag = 0;
               //output_bit(PSyncS,0);
            }
            else
            {

               FlashingFlag = 1;
               //output_bit(PSyncS,1);
            }
         }
         //FlashingFlag = ~FlashingFlag;
         FlashingRateTime = FlashingRate;      //reload value
      }

      if(Synctimer != 0x00)
      {
         Synctimer--;
         if(Synctimer == 0x00)
         {
            SyncStatus = 0;
         }
      }

   }
   /*
   ReadIn_Count=ReadIn_Count+1;
   if(ReadIn_Count==10)
   {
      ReadIn_Count=0;
      ReadIn_flag = 1;
   }
   */
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void Anal_Function(void)
{
   /////////////////////////////////////////////////////////////////////////////
   ////////////////////////////// MANUAL RESET /////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////


//if(NoOfPoint >= 10)
//{

//input1
   if(AlarmIndicator.B1 == 1)    // Alarm Function
   {
      if((((Inputt.B1 ^ InputType.B1) == 1) && (FaultType.B1 == 1))|| In.B1 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B1 == 0)
      {
         if(Ack.B1 == 0)
         {
            if(In.B1 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B1 = 1;                                 // setbit in1
            if(FlashingFlag == 0)                                                                                                                                                                                                                                                                
            {
               Output.B1 = 0;           //Flash output1
            }
            else
            {
               Output.B1 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B1 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B1 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending 
            if((SendSMS.B1 ==0) && (functointest_f ==0) && (Ack.B1 ==0))
            {
               SendSMS.B1 =1;
               strcat(SMS_MassageSUM, SMS_Massage1);
               strcat(SMS_MassageSUM, spacestring);
               /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
            
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              fprintf(CH2,SMS_Massage1);
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
        }
      }
      else if(FaultType.B1 == 1) Ack.B1 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B1 ^ InputType.B1) == 1)
      {
         Output.B1 = 0;
         // SMS Sending 
         if((SendSMS.B1 ==0) && (functointest_f ==0) && (Ack.B1 ==0))
         {
            SendSMS.B1 =1;
               strcat(SMS_MassageSUM, SMS_Massage1);
               strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
         
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           fprintf(CH2,SMS_Massage1);
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B1 = 1;
         SendSMS.B1 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input2
   if(AlarmIndicator.B2 == 1)    // Alarm Function
   {
      if((((Inputt.B2 ^ InputType.B2) == 1) && (FaultType.B2 == 1))|| In.B2 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B2 == 0)
      {
         if(Ack.B2 == 0)
         {
            if(In.B2 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B2 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B2 = 0;           //Flash output1
            }
            else
            {
               Output.B2 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B2 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B2 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B2 ==0)&& (functointest_f ==0) && (Ack.B2 ==0))
            {
               SendSMS.B2 =1;
               strcat(SMS_MassageSUM, SMS_Massage2);
               strcat(SMS_MassageSUM, spacestring);
               /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
                
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
               
              fprintf(CH2,SMS_Massage2);
              putc('\n',CH2);
             putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B2 == 1) Ack.B2 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B2 ^ InputType.B2) == 1)
      {
         Output.B2 = 0;
         // SMS Sending   
         if((SendSMS.B2 ==0)&& (functointest_f ==0) && (Ack.B2 ==0))
         {
            SendSMS.B2 =1;
            strcat(SMS_MassageSUM, SMS_Massage2);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
             
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
            
           fprintf(CH2,SMS_Massage2);
           putc('\n',CH2);
          putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B2 = 1;
         SendSMS.B2 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input3
   if(AlarmIndicator.B3 == 1)    // Alarm Function
   {
      if((((Inputt.B3 ^ InputType.B3) == 1) && (FaultType.B3 == 1))|| In.B3 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B3 == 0)
      {
         if(Ack.B3 == 0)
         {
            if(In.B3 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B3 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B3 = 0;           //Flash output1
            }
            else
            {
               Output.B3 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B3 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B3 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B3 ==0)&& (functointest_f ==0) && (Ack.B3 ==0))
            {
               SendSMS.B3 =1;
               
               strcat(SMS_MassageSUM, SMS_Massage3);
               strcat(SMS_MassageSUM, spacestring);
               /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              fprintf(CH2,SMS_Massage3);
              putc('\n',CH2);
             putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B3 == 1) Ack.B3 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B3 ^ InputType.B3) == 1)
      {
         Output.B3 = 0;
         // SMS Sending   
         if((SendSMS.B3 ==0)&& (functointest_f ==0) && (Ack.B3 ==0))
         {
            SendSMS.B3 =1;
            strcat(SMS_MassageSUM, SMS_Massage3);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           fprintf(CH2,SMS_Massage3);
           putc('\n',CH2);
          putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B3 = 1;
         SendSMS.B3 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input4
   if(AlarmIndicator.B4 == 1)    // Alarm Function
   {
      if((((Inputt.B4 ^ InputType.B4) == 1) && (FaultType.B4 == 1))|| In.B4 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B4 == 0)
      {
         if(Ack.B4 == 0)
         {
            if(In.B4 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B4 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B4 = 0;           //Flash output1
            }
            else
            {
               Output.B4 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B4 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B4 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B4 ==0)&& (functointest_f ==0) && (Ack.B4 ==0))
            {
               SendSMS.B4 =1;
               
               strcat(SMS_MassageSUM, SMS_Massage4);
               strcat(SMS_MassageSUM, spacestring);
               /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
            
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              fprintf(CH2,SMS_Massage4);
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }
      }
      else if(FaultType.B4 == 1) Ack.B4 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B4 ^ InputType.B4) == 1)
      {
         Output.B4 = 0;
         // SMS Sending   
         if((SendSMS.B4 ==0)&& (functointest_f ==0) && (Ack.B4 ==0))
         {
            SendSMS.B4 =1;
            strcat(SMS_MassageSUM, SMS_Massage4);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
         
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           fprintf(CH2,SMS_Massage4);
           putc('\n',CH2);
          putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B4 = 1;
         SendSMS.B4 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input5
   if(AlarmIndicator.B5 == 1)    // Alarm Function
   {
      if((((Inputt.B5 ^ InputType.B5) == 1) && (FaultType.B5 == 1))|| In.B5 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B5 == 0)
      {
         if(Ack.B5 == 0)
         {
            if(In.B5 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B5 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B5 = 0;           //Flash output1
            }
            else
            {
               Output.B5 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B5 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B5 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B5 ==0)&& (functointest_f ==0) && (Ack.B5 ==0))
            {
               SendSMS.B5 =1;
               strcat(SMS_MassageSUM, SMS_Massage5);
               strcat(SMS_MassageSUM, spacestring);
               /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              fprintf(CH2,SMS_Massage5);
              putc('\n',CH2);
             putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B5 == 1) Ack.B5 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B5 ^ InputType.B5) == 1)
      {
         Output.B5 = 0;
         // SMS Sending   
         if((SendSMS.B5 ==0)&& (functointest_f ==0) && (Ack.B5 ==0))
         {
            SendSMS.B5 =1;
            strcat(SMS_MassageSUM, SMS_Massage5);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           fprintf(CH2,SMS_Massage5);
           putc('\n',CH2);
          putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B5 = 1;
         SendSMS.B5 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input6
   if(AlarmIndicator.B6 == 1)    // Alarm Function
   {
      if((((Inputt.B6 ^ InputType.B6) == 1) && (FaultType.B6 == 1))|| In.B6 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B6 == 0)
      {
         if(Ack.B6 == 0)
         {
            if(In.B6 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B6 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B6 = 0;           //Flash output1
            }
            else
            {
               Output.B6 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B6 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B6 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B6 ==0)&& (functointest_f ==0) && (Ack.B6 ==0))
            {
               SendSMS.B6 =1;
               strcat(SMS_MassageSUM, SMS_Massage6);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              fprintf(CH2,SMS_Massage6);
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }
      }
      else if(FaultType.B6 == 1) Ack.B6 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B6 ^ InputType.B6) == 1)
      {
         Output.B6 = 0;
         // SMS Sending   
         if((SendSMS.B6 ==0)&& (functointest_f ==0) && (Ack.B6 ==0))
         {
            SendSMS.B6 =1;
            strcat(SMS_MassageSUM, SMS_Massage6);
            strcat(SMS_MassageSUM, spacestring);
         
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           fprintf(CH2,SMS_Massage6);
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B6 = 1;
         SendSMS.B6 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input7
   if(AlarmIndicator.B7 == 1)    // Alarm Function
   {
      if((((Inputt.B7 ^ InputType.B7) == 1) && (FaultType.B7 == 1))|| In.B7 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B7 == 0)
      {
         if(Ack.B7 == 0)
         {
            if(In.B7 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B7 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B7 = 0;           //Flash output1
            }
            else
            {
               Output.B7 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B7 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B7 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B7 ==0)&& (functointest_f ==0) && (Ack.B7 ==0))
            {
               SendSMS.B7 =1;
               strcat(SMS_MassageSUM, SMS_Massage7);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              fprintf(CH2,SMS_Massage7);  
              putc('\n',CH2);
             putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B7 == 1) Ack.B7 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B7 ^ InputType.B7) == 1)
      {
         Output.B7 = 0;
         // SMS Sending   
         if((SendSMS.B7 ==0)&& (functointest_f ==0) && (Ack.B7 ==0))
         {
            SendSMS.B7 =1;
            strcat(SMS_MassageSUM, SMS_Massage7);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           fprintf(CH2,SMS_Massage7);  
           putc('\n',CH2);
          putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B7 = 1;
         SendSMS.B7 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input8
   if(AlarmIndicator.B8 == 1)    // Alarm Function
   {
      if((((Inputt.B8 ^ InputType.B8) == 1) && (FaultType.B8 == 1))|| In.B8 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B8 == 0)
      {
         if(Ack.B8 == 0)
         {
            if(In.B8 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B8 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B8 = 0;           //Flash output1
            }
            else
            {
               Output.B8 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B8 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B8 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
                  // SMS Sending   
         if((SendSMS.B8 ==0)&& (functointest_f ==0) && (Ack.B8 ==0))
         {
            SendSMS.B8 =1;
            strcat(SMS_MassageSUM, SMS_Massage8);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage8);  
           putc('\n',CH2);
          putc(26,CH2);
              */
         }
       }

      }
      else if(FaultType.B8 == 1) Ack.B8 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B8 ^ InputType.B8) == 1)
      {
         Output.B8 = 0;
               // SMS Sending   
         if((SendSMS.B8 ==0)&& (functointest_f ==0) && (Ack.B8 ==0))
         {
            SendSMS.B8 =1;
            strcat(SMS_MassageSUM, SMS_Massage8);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage8);  
           putc('\n',CH2);
          putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B8 = 1;
         SendSMS.B8 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input9
   if(AlarmIndicator.B9 == 1)    // Alarm Function
   {
      if((((Inputt.B9 ^ InputType.B9) == 1) && (FaultType.B9 == 1))|| In.B9 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B9 == 0)
      {
         if(Ack.B9 == 0)
         {
            if(In.B9 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B9 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B9 = 0;           //Flash output1
            }
            else
            {
               Output.B9 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B9 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B9 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B9 ==0)&& (functointest_f ==0) && (Ack.B9 ==0))
            {
               SendSMS.B9 =1;
               strcat(SMS_MassageSUM, SMS_Massage9);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage9);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B9 == 1) Ack.B9 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B9 ^ InputType.B9) == 1)
      {
         Output.B9 = 0;
         // SMS Sending   
         if((SendSMS.B9 ==0)&& (functointest_f ==0) && (Ack.B9 ==0))
         {
            SendSMS.B9 =1;
            strcat(SMS_MassageSUM, SMS_Massage9);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage9);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B9 = 1;
         SendSMS.B9 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input10
   if(AlarmIndicator.B10 == 1)    // Alarm Function
   {
      if((((Inputt.B10 ^ InputType.B10) == 1) && (FaultType.B10 == 1))|| In.B10 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B10 == 0)
      {
         if(Ack.B10 == 0)
         {
            if(In.B10 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B10 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B10 = 0;           //Flash output1
            }
            else
            {
               Output.B10 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B10 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B10 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B10 ==0)&& (functointest_f ==0) && (Ack.B10 ==0))
            {
               SendSMS.B10 =1;
               strcat(SMS_MassageSUM, SMS_Massage10);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage10);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B10 == 1) Ack.B10 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B10 ^ InputType.B10) == 1)
      {
         Output.B10 = 0;
         // SMS Sending   
         if((SendSMS.B10 ==0)&& (functointest_f ==0) && (Ack.B10 ==0))
         {
            SendSMS.B10 =1;
            strcat(SMS_MassageSUM, SMS_Massage10);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage10);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B10 = 1;
         SendSMS.B10 =0;
         functointest_f =0;
      }
   }
//}
//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//if(NoOfPoint >= 20)
//{
///////////////////////////////////////////////////////////////////////////////
//input11
   if(AlarmIndicator.B11 == 1)    // Alarm Function
   {
      if((((Inputt.B11 ^ InputType.B11) == 1) && (FaultType.B11 == 1))|| In.B11 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B11 == 0)
      {
         if(Ack.B11 == 0)
         {
            if(In.B11 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B11 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B11 = 0;           //Flash output1
            }
            else
            {
               Output.B11 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B11 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B11 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B11 ==0)&& (functointest_f ==0) && (Ack.B11 ==0))
            {
               SendSMS.B11 =1;
               strcat(SMS_MassageSUM, SMS_Massage11);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage11);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B11 == 1) Ack.B11 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B11 ^ InputType.B11) == 1)
      {
         Output.B11 = 0;
         // SMS Sending   
         if((SendSMS.B11 ==0)&& (functointest_f ==0) && (Ack.B11 ==0))
         { 
            SendSMS.B11 =1;
            strcat(SMS_MassageSUM, SMS_Massage11);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
           
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage11);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B11 = 1;
         SendSMS.B11 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input12
   if(AlarmIndicator.B12 == 1)    // Alarm Function
   {
      if((((Inputt.B12 ^ InputType.B12) == 1) && (FaultType.B12 == 1))|| In.B12 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B12 == 0)
      {
         if(Ack.B12 == 0)
         {
            if(In.B12 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B12 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B12 = 0;           //Flash output1
            }
            else
            {
               Output.B12 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B12 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B12 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B12 ==0)&& (functointest_f ==0) && (Ack.B12 ==0))
            {
               SendSMS.B12 =1;
               strcat(SMS_MassageSUM, SMS_Massage12);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage12);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
        }

      }
      else if(FaultType.B12 == 1) Ack.B12 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B12 ^ InputType.B12) == 1)
      {
         Output.B12 = 0;
         // SMS Sending   
         if((SendSMS.B12 ==0)&& (functointest_f ==0) && (Ack.B12 ==0))
         {
            SendSMS.B12 =1;
            strcat(SMS_MassageSUM, SMS_Massage12);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage12);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B12 = 1;
         SendSMS.B12 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input13
   if(AlarmIndicator.B13 == 1)    // Alarm Function
   {
      if((((Inputt.B13 ^ InputType.B13) == 1) && (FaultType.B13 == 1))|| In.B13 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B13 == 0)
      {
         if(Ack.B13 == 0)
         {
            if(In.B13 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B13 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B13 = 0;           //Flash output1
            }
            else
            {
               Output.B13 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B13 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B13 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B13 ==0)&& (functointest_f ==0) && (Ack.B13 ==0))
            {
               SendSMS.B13 =1;
               strcat(SMS_MassageSUM, SMS_Massage13);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage13);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B13 == 1) Ack.B13 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B13 ^ InputType.B13) == 1)
      {
         Output.B13 = 0;
         // SMS Sending   
         if((SendSMS.B13 ==0)&& (functointest_f ==0) && (Ack.B13 ==0))
         {
            SendSMS.B13 =1;
            strcat(SMS_MassageSUM, SMS_Massage13);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage13);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B13 = 1;
         SendSMS.B13 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input14
   if(AlarmIndicator.B14 == 1)    // Alarm Function
   {
      if((((Inputt.B14 ^ InputType.B14) == 1) && (FaultType.B14 == 1))|| In.B14 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B14 == 0)
      {
         if(Ack.B14 == 0)
         {
            if(In.B14 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B14 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B14 = 0;           //Flash output1
            }
            else
            {
               Output.B14 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B14 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B14 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B14 ==0)&& (functointest_f ==0) && (Ack.B14 ==0))
            {
               SendSMS.B14 =1;
               strcat(SMS_MassageSUM, SMS_Massage14);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage14);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B14 == 1) Ack.B14 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B14 ^ InputType.B14) == 1)
      {
         Output.B14 = 0;
         // SMS Sending   
         if((SendSMS.B14 ==0)&& (functointest_f ==0) && (Ack.B14 ==0))
         {
            SendSMS.B14 =1;
            strcat(SMS_MassageSUM, SMS_Massage14);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage14);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B14 = 1;
         SendSMS.B14 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input15
   if(AlarmIndicator.B15 == 1)    // Alarm Function
   {
      if((((Inputt.B15 ^ InputType.B15) == 1) && (FaultType.B15 == 1))|| In.B15 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B15 == 0)
      {
         if(Ack.B15 == 0)
         {
            if(In.B15 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B15 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B15 = 0;           //Flash output1
            }
            else
            {
               Output.B15 = 1;
            }

            if (~AutoTestFlag)
            {
               if(OutputBoth.B15 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B15 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B15 ==0)&& (functointest_f ==0) && (Ack.B15 ==0))
            {
               SendSMS.B15 =1;
               strcat(SMS_MassageSUM, SMS_Massage15);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage15);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B15 == 1) Ack.B15 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B15 ^ InputType.B15) == 1)
      {
         Output.B15 = 0; 
         if((SendSMS.B15 ==0)&& (functointest_f ==0) && (Ack.B15 ==0))
         {
            SendSMS.B15 =1;
            strcat(SMS_MassageSUM, SMS_Massage15);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage15);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B15 = 1;
         SendSMS.B15 =0;
         functointest_f =0;
      }
   }
///////////////////////////////////////////////////////////////////////////////
//input16
   if(AlarmIndicator.B16 == 1)    // Alarm Function
   {
      if((((Inputt.B16 ^ InputType.B16) == 1) && (FaultType.B16 == 1))|| In.B16 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B16 == 0)
      {
         if(Ack.B16 == 0)
         {
            if(In.B16 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B16 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B16 = 0;           //Flash output1
            }
            else
            {
               Output.B16 = 1;
            }


            if (~AutoTestFlag)
            {
               if(OutputBoth.B16 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B16 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
 
            if((SendSMS.B16 ==0)&& (functointest_f ==0) && (Ack.B16 ==0))
            {
               SendSMS.B16 =1;
               strcat(SMS_MassageSUM, SMS_Massage16);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage16);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B16 == 1) Ack.B16 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B16 ^ InputType.B16) == 1)
      {
         Output.B16 = 0;  
         if((SendSMS.B16 ==0)&& (functointest_f ==0) && (Ack.B16 ==0))
         {
            SendSMS.B16 =1;
            strcat(SMS_MassageSUM, SMS_Massage16);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage16);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B16 = 1;
         SendSMS.B16 =0;
         functointest_f =0;
      }
   }
//}

///////////////////////////////////////////////////////////////////////////////
//input17
   
   if(AlarmIndicator.B17 == 1)    // Alarm Function
   {
      if((((Inputt.B17 ^ InputType.B17) == 1) && (FaultType.B17 == 1))|| In.B17 == 1)    // alarm1 occure and "Lock type"
      {
         if(Ack.B17 == 0)
         {
            if(In.B17 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B17 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B17 = 0;           //Flash output1
            }
            else
            {
               Output.B17 = 1;
            }


            if (~AutoTestFlag)
            {
               if(OutputBoth.B17 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B17 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B17 ==0)&& (functointest_f ==0) && (Ack.B17 ==0))
            {
               SendSMS.B17 =1;
               strcat(SMS_MassageSUM, SMS_Massage17);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage17);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B17 == 1) Ack.B17 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B17 ^ InputType.B17) == 1)
      {
         Output.B17 = 0;
         if((SendSMS.B17 ==0)&& (functointest_f ==0) && (Ack.B17 ==0))
         {
            SendSMS.B17 =1;
            strcat(SMS_MassageSUM, SMS_Massage17);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage17);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B17 = 1;
         SendSMS.B17 =0;
         functointest_f =0;
      }
   }
//}

///////////////////////////////////////////////////////////////////////////////

//input18
   if(AlarmIndicator.B18 == 1)    // Alarm Function
   {
      if((((Inputt.B18 ^ InputType.B18) == 1) && (FaultType.B18 == 1))|| In.B18 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B18 == 0)
      {
         if(Ack.B18 == 0)
         {
            if(In.B18 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B18 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B18 = 0;           //Flash output1
            }
            else
            {
               Output.B18 = 1;
            }


            if (~AutoTestFlag)
            {
               if(OutputBoth.B18 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B18 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B18 ==0)&& (functointest_f ==0) && (Ack.B18 ==0))
            {
               SendSMS.B18 =1;
               strcat(SMS_MassageSUM, SMS_Massage18);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage18);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B18 == 1) Ack.B18 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B18 ^ InputType.B18) == 1)
      {
         Output.B18 = 0;
         // SMS Sending   
         if((SendSMS.B18 ==0)&& (functointest_f ==0) && (Ack.B18 ==0))
         {
            SendSMS.B18 =1;
            strcat(SMS_MassageSUM, SMS_Massage18);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage18);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B18 = 1;
         SendSMS.B18 =0;
         functointest_f =0;
      }
   }
//}

///////////////////////////////////////////////////////////////////////////////
//input19
   if(AlarmIndicator.B19 == 1)    // Alarm Function
   {
      if((((Inputt.B19 ^ InputType.B19) == 1) && (FaultType.B19 == 1))|| In.B19 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B19 == 0)
      {
         if(Ack.B19 == 0)
         {
            if(In.B19 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B19 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B19 = 0;           //Flash output1
            }
            else
            {
               Output.B19 = 1;
            }


            if (~AutoTestFlag)
            {
               if(OutputBoth.B19 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B19 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B19 ==0)&& (functointest_f ==0) && (Ack.B19 ==0))
            {
               SendSMS.B19 =1;
               strcat(SMS_MassageSUM, SMS_Massage19);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage19);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B19 == 1) Ack.B19 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B19 ^ InputType.B19) == 1)
      {
         Output.B19 = 0;
         // SMS Sending   
         if((SendSMS.B19 ==0)&& (functointest_f ==0) && (Ack.B19 ==0))
         {
            SendSMS.B19 =1;
            strcat(SMS_MassageSUM, SMS_Massage19);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage19);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B19 = 1;
         SendSMS.B19 =0;
         functointest_f =0;
      }
   }
//}

///////////////////////////////////////////////////////////////////////////////
//input20
   if(AlarmIndicator.B20 == 1)    // Alarm Function
   {
      if((((Inputt.B20 ^ InputType.B20) == 1) && (FaultType.B20 == 1))|| In.B20 == 1)    // alarm1 occure and "Lock type"
      //if(Inputt.B18 == 0)
      {
         if(Ack.B20 == 0)
         {
            if(In.B20 == 0 && AutoAck == 0x0F)   //use auto acknowlegde
            {
               TimeBase1s = 100 ;            //reload again
               AutoAckDelayTime = AutoAckTime ;
            }

            In.B20 = 1;                                 // setbit in1
            if(FlashingFlag == 0)
            {
               Output.B20 = 0;           //Flash output1
            }
            else
            {
               Output.B20 = 1;
            }


            if (~AutoTestFlag)
            {
               if(OutputBoth.B20 == 0)                  //Both output
               {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
               }
               else
               {
                  if(OutputType.B20 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
               }
            }
            // SMS Sending   
            if((SendSMS.B20 ==0)&& (functointest_f ==0) && (Ack.B20 ==0))
            {
               SendSMS.B20 =1;
               strcat(SMS_MassageSUM, SMS_Massage20);
               strcat(SMS_MassageSUM, spacestring);
            /*
               fprintf(CH2,"AT+CMGF=1"); 
               putc('\n',CH2);
               delay_ms(10);
               
               fprintf(CH2,"AT+CMGS=\"");
               fprintf(CH2,sms_phonenumber);
               
               fprintf(CH2,"\"");
               putc('\n',CH2);
               delay_ms(50);
              
              printf(SMS_Massage20);  
              putc('\n',CH2);
              putc(26,CH2);
              */
            }
          }

      }
      else if(FaultType.B20 == 1) Ack.B20 = 0;
   }
   else  //Indicator Function
   {
      if((Inputt.B20 ^ InputType.B20) == 1)
      {
         Output.B20 = 0;
         // SMS Sending   
         if((SendSMS.B20 ==0)&& (functointest_f ==0) && (Ack.B20 ==0))
         {
            SendSMS.B20 =1;
            strcat(SMS_MassageSUM, SMS_Massage20);
            strcat(SMS_MassageSUM, spacestring);
            /*
            fprintf(CH2,"AT+CMGF=1"); 
            putc('\n',CH2);
            delay_ms(10);
            
            fprintf(CH2,"AT+CMGS=\"");
            fprintf(CH2,sms_phonenumber);
            
            fprintf(CH2,"\"");
            putc('\n',CH2);
            delay_ms(50);
           
           printf(SMS_Massage20);  
           putc('\n',CH2);
           putc(26,CH2);
              */
         }
      }
      else
      {
         Output.B20 = 1;
         SendSMS.B20 =0;
         functointest_f =0;
      }
   }
    
//}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// END MANUAL RESET //////////////////////////////////////////


   /////////////////////////////////////////////////////////////////////////////
   ////////////////////////////// AUTO RESET ///////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

//if(NoOfPoint >= 10)
//{
static unsigned char inputflag = 0;

//input1
   if((((Inputt.B1 ^ InputType.B1) == 1) && (FaultType.B1 == 0)) ||Test_fault==1)   // alarm1 occure and " Non Lock type"
   {
      if(In2.B1 == 0 && AutoAck == 0x0F && Ack.B1 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B1 = 1;
      if(Ack.B1 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B1 = 0;           //Flash output1
            }
            else
            {
               Output.B1 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B1 == 1){
                if(OutputBoth.B1 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B1 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
       inputflag =1;
       // SMS Sending   
      if((SendSMS.B1 ==0) && (functointest_f ==0) && (Ack.B1 ==0))
      {
         SendSMS.B1 =1;
         strcat(SMS_MassageSUM, SMS_Massage1);
         strcat(SMS_MassageSUM, spacestring);
          /*
         fprintf(CH2,"AT+CMGS=\"");
         fprintf(CH2,sms_phonenumber);
         
         fprintf(CH2,"\"");
         putc('\n',CH2);
         delay_ms(50);
        
        fprintf(CH2,SMS_Massage1);   
        putc('\n',CH2);
       putc(26,CH2);
       */
      }
   }
   
   else if(FaultType.B1 == 0)
   {
      In2.B1 = 0x00;
      Ack.B1 = 0;
      Output.B1 = 1;      //Off LED
      SendSMS.B1 =0;// Clear SMS

      if((OutputType.B1 == 1 || OutputBoth.B1 == 0) && inputflag ==0)     //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B1 == 0 || OutputBoth.B1 == 0) && inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input2
   if((((Inputt.B2 ^ InputType.B2) == 1) && (FaultType.B2 == 0)) ||Test_fault==1)   // alarm1 occure and " Non Lock type"
   {
      if(In2.B2 == 0 && AutoAck == 0x0F && Ack.B2 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B2 = 1;
      if(Ack.B2 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B2 = 0;           //Flash output1
            }
            else
            {
               Output.B2 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B2 == 1){
                if(OutputBoth.B2 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B2 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
      if((SendSMS.B2 ==0)&& (functointest_f ==0) && (Ack.B2 ==0))
      {
         SendSMS.B2 =1;
         strcat(SMS_MassageSUM, SMS_Massage2);
         strcat(SMS_MassageSUM, spacestring);
          /*
         fprintf(CH2,"AT+CMGS=\"");
         fprintf(CH2,sms_phonenumber);
         
         fprintf(CH2,"\"");
         putc('\n',CH2);
         delay_ms(50);
        
        fprintf(CH2,SMS_Massage2); 
        putc('\n',CH2);
       putc(26,CH2);
       */
      }
   }
   else if(FaultType.B2 == 0)
   {
      In2.B2 = 0x00;
      Ack.B2 = 0;
      Output.B2 = 1;      //Off LED
      SendSMS.B2 =0;// Clear SMS

      if((OutputType.B2 == 1 || OutputBoth.B2 == 0) && inputflag ==0)     //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B2 == 0 || OutputBoth.B2 == 0) && inputflag ==0) //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input3
   if((((Inputt.B3 ^ InputType.B3) == 1) && (FaultType.B3 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B3 == 0 && AutoAck == 0x0F && Ack.B3 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B3 = 1;
      if(Ack.B3 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B3 = 0;           //Flash output1
            }
            else
            {
               Output.B3 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B3 == 1){
                if(OutputBoth.B3 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B3 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
      if((SendSMS.B3 ==0)&& (functointest_f ==0) && (Ack.B3 ==0))
      {
         SendSMS.B3 =1;
         strcat(SMS_MassageSUM, SMS_Massage3);
         strcat(SMS_MassageSUM, spacestring);
          /*
         fprintf(CH2,"AT+CMGS=\"");
         fprintf(CH2,sms_phonenumber);
         
         fprintf(CH2,"\"");
         putc('\n',CH2);
         delay_ms(50);
        
        fprintf(CH2,SMS_Massage3); 
        putc('\n',CH2);
       putc(26,CH2);
       */
      }
   }
   else if(FaultType.B3 == 0)
   {
      In2.B3 = 0x00;
      Ack.B3 = 0;
      Output.B3 = 1;      //Off LED
      SendSMS.B3 =0;// Clear SMS

      if((OutputType.B3 == 1 || OutputBoth.B3 == 0) && inputflag ==0)     //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B3 == 0 || OutputBoth.B3 == 0) && inputflag ==0) //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input4
   if((((Inputt.B4 ^ InputType.B4) == 1) && (FaultType.B4 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B4 == 0 && AutoAck == 0x0F && Ack.B4 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B4 = 1;
      if(Ack.B4 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B4 = 0;           //Flash output1
            }
            else
            {
               Output.B4 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B4 == 1){
                if(OutputBoth.B4 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B4 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
      if((SendSMS.B4 ==0)&& (functointest_f ==0) && (Ack.B4 ==0))
      {
         SendSMS.B4 =1;
         strcat(SMS_MassageSUM, SMS_Massage4);
         strcat(SMS_MassageSUM, spacestring);
          /*
         fprintf(CH2,"AT+CMGS=\"");
         fprintf(CH2,sms_phonenumber);
         
         fprintf(CH2,"\"");
         putc('\n',CH2);
         delay_ms(50);
        
        fprintf(CH2,SMS_Massage4); 
        putc('\n',CH2);
       putc(26,CH2);
       */
      }
   }
   else if(FaultType.B4 == 0)
   {
      In2.B4 = 0x00;
      Ack.B4 = 0;
      Output.B4 = 1;      //Off LED
      SendSMS.B4 =0;// Clear SMS

      if((OutputType.B4 == 1 || OutputBoth.B4 == 0) && inputflag ==0)     //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B4 == 0 || OutputBoth.B4 == 0) && inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input5
   if((((Inputt.B5 ^ InputType.B5) == 1) && (FaultType.B5 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B5 == 0 && AutoAck == 0x0F && Ack.B5 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B5 = 1;
      if(Ack.B5 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B5 = 0;           //Flash output1
            }
            else
            {
               Output.B5 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B5 == 1){
                if(OutputBoth.B5 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B5 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
      if((SendSMS.B5 ==0)&& (functointest_f ==0) && (Ack.B5 ==0))
      {
         SendSMS.B5 =1;
         strcat(SMS_MassageSUM, SMS_Massage5);
         strcat(SMS_MassageSUM, spacestring);
          /*
         fprintf(CH2,"AT+CMGS=\"");
         fprintf(CH2,sms_phonenumber);
         
         fprintf(CH2,"\"");
         putc('\n',CH2);
         delay_ms(50);
        
        fprintf(CH2,SMS_Massage5); 
        putc('\n',CH2);
       putc(26,CH2);
       */
      }
   }
   else if(FaultType.B5 == 0)
   {
      In2.B5 = 0x00;
      Ack.B5 = 0;
      Output.B5 = 1;      //Off LED
      SendSMS.B5 =0;// Clear SMS

      if((OutputType.B5 == 1 || OutputBoth.B5 == 0) && inputflag ==0)     //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B5 == 0 || OutputBoth.B5 == 0) && inputflag ==0) //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input6
   if((((Inputt.B6 ^ InputType.B6) == 1) && (FaultType.B6 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B6 == 0 && AutoAck == 0x0F && Ack.B6 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B6 = 1;
      if(Ack.B6 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B6 = 0;           //Flash output1
            }
            else
            {
               Output.B6 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B6 == 1){
                if(OutputBoth.B6 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B6 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
      if((SendSMS.B6 ==0)&& (functointest_f ==0) && (Ack.B6 ==0))
      {
         SendSMS.B6 =1;
         strcat(SMS_MassageSUM, SMS_Massage6);
         strcat(SMS_MassageSUM, spacestring);
          /*
         fprintf(CH2,"AT+CMGS=\"");
         fprintf(CH2,sms_phonenumber);
         
         fprintf(CH2,"\"");
         putc('\n',CH2);
         delay_ms(50);
        
        fprintf(CH2,SMS_Massage6);  
        putc('\n',CH2);
       putc(26,CH2);
       */
      }
   }
   else if(FaultType.B6 == 0)
   {
      In2.B6 = 0x00;
      Ack.B6 = 0;
      Output.B6 = 1;      //Off LED
      SendSMS.B6 =0;// Clear SMS

      if((OutputType.B6 == 1 || OutputBoth.B6 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B6 == 0 || OutputBoth.B6 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input7
   if((((Inputt.B7 ^ InputType.B7) == 1) && (FaultType.B7 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B7 == 0 && AutoAck == 0x0F && Ack.B7 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B7 = 1;
      if(Ack.B7 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B7 = 0;           //Flash output1
            }
            else
            {
               Output.B7 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B7 == 1){
                if(OutputBoth.B7 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B7 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
      if((SendSMS.B7 ==0)&& (functointest_f ==0) && (Ack.B7 ==0))
      {
         SendSMS.B7 =1;
         strcat(SMS_MassageSUM, SMS_Massage7);
         strcat(SMS_MassageSUM, spacestring);
          /*
         fprintf(CH2,"AT+CMGS=\"");
         fprintf(CH2,sms_phonenumber);
         
         fprintf(CH2,"\"");
         putc('\n',CH2);
         delay_ms(50);
        
        fprintf(CH2,SMS_Massage7);  
        putc('\n',CH2);
       putc(26,CH2);
       */
      }
   }
   else if(FaultType.B7 == 0)
   {
      In2.B7 = 0x00;
      Ack.B7 = 0;
      Output.B7 = 1;      //Off LED
      SendSMS.B7 =0;// Clear SMS

      if((OutputType.B7 == 1 || OutputBoth.B7 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B7 == 0 || OutputBoth.B7 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input8
   if((((Inputt.B8 ^ InputType.B8) == 1) && (FaultType.B8 == 0)) ||Test_fault==1)   // alarm1 occure and " Non Lock type"
   {
      if(In2.B8 == 0 && AutoAck == 0x0F && Ack.B8 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B8 = 1;
      if(Ack.B8 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B8 = 0;           //Flash output1
            }
            else
            {
               Output.B8 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B8 == 1){
                if(OutputBoth.B8 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B8 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
      if((SendSMS.B8 ==0) && (functointest_f ==0) && (Ack.B8 ==0))
      {
         SendSMS.B8 =1;
         strcat(SMS_MassageSUM, SMS_Massage8);
         strcat(SMS_MassageSUM, spacestring);
          /*
         fprintf(CH2,"AT+CMGS=\"");
         fprintf(CH2,sms_phonenumber);
         
         fprintf(CH2,"\"");
         putc('\n',CH2);
         delay_ms(50);
        
        fprintf(CH2,SMS_Massage8);   
        putc('\n',CH2);
        putc(26,CH2);
       */
      }
   }
   else if(FaultType.B8 == 0)
   {
      In2.B8 = 0x00;
      Ack.B8 = 0;
      Output.B8 = 1;      //Off LED
      SendSMS.B8 =0;// Clear SMS

      if((OutputType.B8 == 1 || OutputBoth.B8 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B8 == 0 || OutputBoth.B8 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input9
   if((((Inputt.B9 ^ InputType.B9) == 1) && (FaultType.B9 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B9 == 0 && AutoAck == 0x0F && Ack.B9 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B9 = 1;
      if(Ack.B9 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B9 = 0;           //Flash output1
            }
            else
            {
               Output.B9 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B9 == 1){
                if(OutputBoth.B9 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B9 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
      if((SendSMS.B9 ==0) && (functointest_f ==0) && (Ack.B9 ==0))
      {
         SendSMS.B9 =1;
         strcat(SMS_MassageSUM, SMS_Massage9);
         strcat(SMS_MassageSUM, spacestring);
          /*
         fprintf(CH2,"AT+CMGS=\"");
         fprintf(CH2,sms_phonenumber);
         
         fprintf(CH2,"\"");
         putc('\n',CH2);
         delay_ms(50);
        
        fprintf(CH2,SMS_Massage9);   
        putc('\n',CH2);
        putc(26,CH2);
       */
      }
   }
   else if(FaultType.B9 == 0)
   {
      In2.B9 = 0x00;
      Ack.B9 = 0;
      Output.B9 = 1;      //Off LED
      SendSMS.B9 =0;// Clear SMS
      
      if((OutputType.B9 == 1 || OutputBoth.B9 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B9 == 0 || OutputBoth.B9 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input10
   if((((Inputt.B10 ^ InputType.B10) == 1) && (FaultType.B10 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B10 == 0 && AutoAck == 0x0F && Ack.B10 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B10 = 1;
      if(Ack.B10 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B10 = 0;           //Flash output1
            }
            else
            {
               Output.B10 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B10 == 1){
                if(OutputBoth.B10 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B10 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
      if((SendSMS.B10 ==0) && (functointest_f ==0) && (Ack.B10 ==0))
      {
         SendSMS.B10 =1;
          strcat(SMS_MassageSUM, SMS_Massage10);
          strcat(SMS_MassageSUM, spacestring);
          /*
         fprintf(CH2,"AT+CMGS=\"");
         fprintf(CH2,sms_phonenumber);
         
         fprintf(CH2,"\"");
         putc('\n',CH2);
         delay_ms(50);
        
        fprintf(CH2,SMS_Massage10);   
        putc('\n',CH2);
        putc(26,CH2);
       */
      }
   }
   else if(FaultType.B10 == 0)
   {
      In2.B10 = 0x00;
      Ack.B10 = 0;
      Output.B10 = 1;      //Off LED
      SendSMS.B10 =0;// Clear SMS

      if((OutputType.B10 == 1 || OutputBoth.B10 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B10 == 0 || OutputBoth.B10 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   
//}
////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//if(NoOfPoint >= 20)
//{
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input11
   if((((Inputt.B11 ^ InputType.B11) == 1) && (FaultType.B11 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B11 == 0 && AutoAck == 0x0F && Ack.B11 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B11 = 1;
      if(Ack.B11 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B11 = 0;           //Flash output1
            }
            else
            {
               Output.B11 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B11 == 1){
                if(OutputBoth.B11 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B11 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
     if((SendSMS.B11 ==0) && (functointest_f ==0) && (Ack.B11 ==0))
     {
       SendSMS.B11 =1;
          strcat(SMS_MassageSUM, SMS_Massage11);
          strcat(SMS_MassageSUM, spacestring);
          /*
       fprintf(CH2,"AT+CMGS=\"");
       fprintf(CH2,sms_phonenumber);
       
       fprintf(CH2,"\"");
       putc('\n',CH2);
       delay_ms(50);
      
      fprintf(CH2,SMS_Massage11);   
      putc('\n',CH2);
      putc(26,CH2);
       */
     }
   }
   else if(FaultType.B11 == 0)
   {
      In2.B11 = 0x00;
      Ack.B11 = 0;
      Output.B11 = 1;      //Off LED
      SendSMS.B11 =0;// Clear SMS

      if((OutputType.B11 == 1 || OutputBoth.B11 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B11 == 0 || OutputBoth.B11 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input12
   if((((Inputt.B12 ^ InputType.B12) == 1) && (FaultType.B12 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B12 == 0 && AutoAck == 0x0F && Ack.B12 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B12 = 1;
      if(Ack.B12 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B12 = 0;           //Flash output1
            }
            else
            {
               Output.B12 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B12 == 1){
                if(OutputBoth.B12 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B12 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
         // SMS Sending   
      if((SendSMS.B12 ==0) && (functointest_f ==0) && (Ack.B12 ==0))
      {
        SendSMS.B12 =1;
          strcat(SMS_MassageSUM, SMS_Massage12);
          strcat(SMS_MassageSUM, spacestring);
          /*
        fprintf(CH2,"AT+CMGS=\"");
        fprintf(CH2,sms_phonenumber);
        
        fprintf(CH2,"\"");
        putc('\n',CH2);
        delay_ms(50);
       
       fprintf(CH2,SMS_Massage12);   
       putc('\n',CH2);
       putc(26,CH2);
       */
      }

   }
   else if(FaultType.B12 == 0)
   {
      In2.B12 = 0x00;
      Ack.B12 = 0;
      Output.B12 = 1;      //Off LED
      SendSMS.B12 =0;// Clear SMS

      if((OutputType.B12 == 1 || OutputBoth.B12 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B12 == 0 || OutputBoth.B12 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input13
   if((((Inputt.B13 ^ InputType.B13) == 1) && (FaultType.B13 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B13 == 0 && AutoAck == 0x0F && Ack.B13 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B13 = 1;
      if(Ack.B13 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B13 = 0;           //Flash output1
            }
            else
            {
               Output.B13 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B13 == 1){
                if(OutputBoth.B13 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B13 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
        if((SendSMS.B13 ==0) && (functointest_f ==0) && (Ack.B13 ==0))
        {
          SendSMS.B13 =1;
          strcat(SMS_MassageSUM, SMS_Massage13);
          strcat(SMS_MassageSUM, spacestring);
          /*
          fprintf(CH2,"AT+CMGS=\"");
          fprintf(CH2,sms_phonenumber);
          
          fprintf(CH2,"\"");
          putc('\n',CH2);
          delay_ms(50);
         
         fprintf(CH2,SMS_Massage13);   
         putc('\n',CH2);
         putc(26,CH2);
       */
        }
   }
   else if(FaultType.B13 == 0)
   {
      In2.B13 = 0x00;
      Ack.B13 = 0;
      Output.B13 = 1;      //Off LED
      SendSMS.B13 =0;// Clear SMS

      if((OutputType.B13 == 1 || OutputBoth.B13 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B13 == 0 || OutputBoth.B13 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input14
   if((((Inputt.B14 ^ InputType.B14) == 1) && (FaultType.B14 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B14 == 0 && AutoAck == 0x0F && Ack.B14 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B14 = 1;
      if(Ack.B14 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B14 = 0;           //Flash output1
            }
            else
            {
               Output.B14 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B14 == 1){
                if(OutputBoth.B14 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B14 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
        if((SendSMS.B14 ==0) && (functointest_f ==0) && (Ack.B14 ==0))
        {
          SendSMS.B14 =1;
          strcat(SMS_MassageSUM, SMS_Massage14);
          strcat(SMS_MassageSUM, spacestring);
          /*
          fprintf(CH2,"AT+CMGS=\"");
          fprintf(CH2,sms_phonenumber);
          
          fprintf(CH2,"\"");
          putc('\n',CH2);
          delay_ms(50);
         
         fprintf(CH2,SMS_Massage14);   
         putc('\n',CH2);
         putc(26,CH2);
       */
        }
   }
   else if(FaultType.B14 == 0)
   {
      In2.B14 = 0x00;
      Ack.B14 = 0;
      Output.B14 = 1;      //Off LED
      SendSMS.B14 =0;// Clear SMS

      if((OutputType.B14 == 1 || OutputBoth.B14 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B14 == 0 || OutputBoth.B14 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input15
   if((((Inputt.B15 ^ InputType.B15) == 1) && (FaultType.B15 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B15 == 0 && AutoAck == 0x0F && Ack.B15 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B15 = 1;
      if(Ack.B15 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B15 = 0;           //Flash output1
            }
            else
            {
               Output.B15 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B15 == 1){
                if(OutputBoth.B15 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B15 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
      // SMS Sending   
        if((SendSMS.B15 ==0) && (functointest_f ==0) && (Ack.B15 ==0))
        {
          SendSMS.B15 =1;
          strcat(SMS_MassageSUM, SMS_Massage15);
          strcat(SMS_MassageSUM, spacestring);
          /*
          fprintf(CH2,"AT+CMGS=\"");
          fprintf(CH2,sms_phonenumber);
          
          fprintf(CH2,"\"");
          putc('\n',CH2);
          delay_ms(50);
         
         fprintf(CH2,SMS_Massage15);   
         putc('\n',CH2);
         putc(26,CH2);
       */
        }
   }
   else if(FaultType.B15 == 0)
   {
      In2.B15 = 0x00;
      Ack.B15 = 0;
      Output.B15 = 1;      //Off LED
      SendSMS.B15 =0;// Clear SMS

      if((OutputType.B15 == 1 || OutputBoth.B15 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B15 == 0 || OutputBoth.B15 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////
//input16
   if((((Inputt.B16 ^ InputType.B16) == 1) && (FaultType.B16 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B16 == 0 && AutoAck == 0x0F && Ack.B16 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B16 = 1;
      if(Ack.B16 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B16 = 0;         //Flash output1
            }
            else
            {
               Output.B16 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B16 == 1){
                if(OutputBoth.B16 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B16 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
       // SMS Sending   
     if((SendSMS.B16 ==0) && (functointest_f ==0) && (Ack.B16 ==0))
     {
       SendSMS.B16 =1;
       strcat(SMS_MassageSUM, SMS_Massage16);
       strcat(SMS_MassageSUM, spacestring);
       /*
       fprintf(CH2,"AT+CMGS=\"");
       fprintf(CH2,sms_phonenumber);
       
       fprintf(CH2,"\"");
       putc('\n',CH2);
       delay_ms(50);
      
      fprintf(CH2,SMS_Massage16);   
      putc('\n',CH2);
      putc(26,CH2);
       */
     }
   }
   else if(FaultType.B16 == 0)
   {
      In2.B16 = 0x00;
      Ack.B16 = 0;
      Output.B16 = 1;      //Off LED
      SendSMS.B16 =0;// Clear SMS

      if((OutputType.B16 == 1 || OutputBoth.B16 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B16 == 0 || OutputBoth.B16 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
//input17
   if((((Inputt.B17 ^ InputType.B17) == 1) && (FaultType.B17 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B17 == 0 && AutoAck == 0x0F && Ack.B17 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B17 = 1;
      if(Ack.B17 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B17 = 0;           //Flash output1
            }
            else
            {
               Output.B17 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B17 == 1){
                if(OutputBoth.B17 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B17 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
       // SMS Sending   
     if((SendSMS.B17 ==0) && (functointest_f ==0) && (Ack.B16 ==0))
     {
       SendSMS.B17 =1;
       strcat(SMS_MassageSUM, SMS_Massage17);
       strcat(SMS_MassageSUM, spacestring);
       /*
       fprintf(CH2,"AT+CMGS=\"");
       fprintf(CH2,sms_phonenumber);
       
       fprintf(CH2,"\"");
       putc('\n',CH2);
       delay_ms(50);
      
      fprintf(CH2,SMS_Massage17);   
      putc('\n',CH2);
      putc(26,CH2);
       */
     }
   }
   else if(FaultType.B17 == 0)
   {
      In2.B17 = 0x00;
      Ack.B17 = 0;
      Output.B17 = 1;      //Off LED
      SendSMS.B17 =0;// Clear SMS

      if((OutputType.B17 == 1 || OutputBoth.B17 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B17 == 0 || OutputBoth.B17 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
//input18
   if((((Inputt.B18 ^ InputType.B18) == 1) && (FaultType.B18 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B18 == 0 && AutoAck == 0x0F && Ack.B18 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B18 = 1;
      if(Ack.B18 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B18 = 0;           //Flash output1
            }
            else
            {
               Output.B18 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B18 == 1){
                if(OutputBoth.B18 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B18 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
       // SMS Sending   
     if((SendSMS.B18 ==0) && (functointest_f ==0) && (Ack.B18 ==0))
     {
       SendSMS.B18 =1;
       strcat(SMS_MassageSUM, SMS_Massage18);
       strcat(SMS_MassageSUM, spacestring);
       /*
       fprintf(CH2,"AT+CMGS=\"");
       fprintf(CH2,sms_phonenumber);
       
       fprintf(CH2,"\"");
       putc('\n',CH2);
       delay_ms(50);
      
      fprintf(CH2,SMS_Massage18);   
      putc('\n',CH2);
      putc(26,CH2);
       */
     }
   }
   else if(FaultType.B18 == 0)
   {
      In2.B18 = 0x00;
      Ack.B18 = 0;
      Output.B18 = 1;      //Off LED
      SendSMS.B18 =0;// Clear SMS

      if((OutputType.B18== 1 || OutputBoth.B18 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B18 == 0 || OutputBoth.B18 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
//input19
   if((((Inputt.B19 ^ InputType.B19) == 1) && (FaultType.B19 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B19 == 0 && AutoAck == 0x0F && Ack.B19 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B19 = 1;
      if(Ack.B19 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B19 = 0;           //Flash output1
            }
            else
            {
               Output.B19 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B19 == 1){
                if(OutputBoth.B19 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B19 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
      inputflag =1;
       // SMS Sending   
     if((SendSMS.B19 ==0) && (functointest_f ==0) && (Ack.B19 ==0))
     {
       SendSMS.B19 =1;
       strcat(SMS_MassageSUM, SMS_Massage19);
       strcat(SMS_MassageSUM, spacestring);
       /*
       fprintf(CH2,"AT+CMGS=\"");
       fprintf(CH2,sms_phonenumber);
       
       fprintf(CH2,"\"");
       putc('\n',CH2);
       delay_ms(50);
      
      fprintf(CH2,SMS_Massage19);   
      putc('\n',CH2);
      putc(26,CH2);
       */
     }
   }
   else if(FaultType.B19 == 0)
   {
      In2.B19 = 0x00;
      Ack.B19 = 0;
      Output.B19 = 1;      //Off LED
      SendSMS.B19 =0;// Clear SMS

      if((OutputType.B19== 1 || OutputBoth.B19 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B19 == 0 || OutputBoth.B19== 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
      
   //input20
   if((((Inputt.B20 ^ InputType.B20) == 1) && (FaultType.B20 == 0))||Test_fault==1)    // alarm1 occure and " Non Lock type"
   {
      if(In2.B20 == 0 && AutoAck == 0x0F && Ack.B20 == 0)   //use auto acknowlegde
      {
            TimeBase1s = 100 ;            //reload again
            AutoAckDelayTime = AutoAckTime ;
      }

      In2.B20 = 1;
      if(Ack.B20 == 0)
      {
         if(FlashingFlag == 0)
            {
               Output.B20 = 0;           //Flash output1
            }
            else
            {
               Output.B20 = 1;
            }
         if (~AutoTestFlag)
         {
            if(AlarmIndicator.B20 == 1){
                if(OutputBoth.B20 == 0)        //Both output
                {
                  output_bit(Pbuzzer,OnRelay);   //Buzzer
                  output_bit(Pbell,OnRelay);     //Bell
                }
                else
                {
                  if(OutputType.B20 == 1) output_bit(Pbuzzer,OnRelay);     //Buzzer
                  else output_bit(Pbell,OnRelay);                          //Bell
                }
            }
         }
       }
    inputflag =1;
     // SMS Sending   
     if((SendSMS.B20 ==0) && (functointest_f ==0) && (Ack.B20 ==0))
     {
       SendSMS.B20 =1;
       strcat(SMS_MassageSUM, SMS_Massage20);
       strcat(SMS_MassageSUM, spacestring);
       /*
       fprintf(CH2,"AT+CMGS=\"");
       fprintf(CH2,sms_phonenumber);
       
       fprintf(CH2,"\"");
       putc('\n',CH2);
       delay_ms(50);
      
      fprintf(CH2,SMS_Massage20);   
      putc('\n',CH2);
      putc(26,CH2);
       */
     }
   }
   else if(FaultType.B20 == 0)
   {
      In2.B20 = 0x00;
      Ack.B20 = 0;
      Output.B20 = 1;      //Off LED
      SendSMS.B20 =0;// Clear SMS

      if((OutputType.B20== 1 || OutputBoth.B20 == 0)&& inputflag ==0)      //If Buzzer or Both
      {
         if(CheckAutoReset(0x01) == 1);   //Check other Inputt "Ack" or not if not,do nothing
         else
         {
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }


      if((OutputType.B20 == 0 || OutputBoth.B20 == 0)&& inputflag ==0)  //If Bell or Both
      {
         if(CheckAutoReset(0x02) == 1);
         else
         {
            output_bit(Pbell,OffRelay);      //Clear Bell
            output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
         }
      }
      // jj modify
      if(In2.B1 ==0 && In2.B2 ==0 && In2.B3 ==0 && In2.B4 ==0 && In2.B5 ==0 && In2.B6 ==0 && In2.B7 ==0 && In2.B8 ==0 && In2.B9 ==0 && In2.B10 ==0 && In2.B11 ==0 && In2.B12 ==0 && In2.B13 ==0 && In2.B14 ==0 && In2.B15 ==0 && In2.B16 ==0 && In2.B17 ==0 && In2.B18 ==0 && In2.B19 ==0 && In2.B20 ==0 )
      {
         output_bit(Pbell,OffRelay);      //Clear Bell
         output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
      }
   }
   
   
 //}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

void Send_Ouput(void)
{

   if(T_test == 0x00)
   {
      //if(NoOfPoint >= 10)
      //{
      
     
           if(Output.B1)//++++++++++++B1+++++++++++++++++++++
           {
             bit_clear(Green1_8,0); bit_clear(Red1_8,0);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B1 )&&(GREEN_Colour.B1 ==0 ))//Red
             {
               bit_set(Red1_8,0); bit_clear(Green1_8,0);
             }
             else if((RED_Colour.B1==0 )&&(GREEN_Colour.B1))//Green
             {
               bit_clear(Red1_8,0); bit_set(Green1_8,0); 
             }
             else if((RED_Colour.B1)&&(GREEN_Colour.B1))//Ambian
             {
               bit_set(Red1_8,0); bit_set(Green1_8,0) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B2)//++++++++++++B2+++++++++++++++++++++
           {
             bit_clear(Green1_8,1); bit_clear(Red1_8,1);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B2 )&&(GREEN_Colour.B2 ==0 ))//Red
             {
               bit_set(Red1_8,1); bit_clear(Green1_8,1);
             }
             else if((RED_Colour.B2==0 )&&(GREEN_Colour.B2))//Green
             {
               bit_clear(Red1_8,1); bit_set(Green1_8,1); 
             }
             else if((RED_Colour.B2)&&(GREEN_Colour.B2))//Ambian
             {
               bit_set(Red1_8,1); bit_set(Green1_8,1) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B3)//++++++++++++B3+++++++++++++++++++++
           {
             bit_clear(Green1_8,2); bit_clear(Red1_8,2);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B3 )&&(GREEN_Colour.B3 ==0 ))//Red
             {
               bit_set(Red1_8,2); bit_clear(Green1_8,2);
             }
             else if((RED_Colour.B3==0 )&&(GREEN_Colour.B3))//Green
             {
               bit_clear(Red1_8,2); bit_set(Green1_8,2); 
             }
             else if((RED_Colour.B3)&&(GREEN_Colour.B3))//Ambian
             {
               bit_set(Red1_8,2); bit_set(Green1_8,2) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B4)//++++++++++++B4+++++++++++++++++++++
           {
             bit_clear(Green1_8,3); bit_clear(Red1_8,3);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B4 )&&(GREEN_Colour.B4 ==0 ))//Red
             {
               bit_set(Red1_8,3); bit_clear(Green1_8,3);
             }
             else if((RED_Colour.B4==0 )&&(GREEN_Colour.B4))//Green
             {
               bit_clear(Red1_8,3); bit_set(Green1_8,3); 
             }
             else if((RED_Colour.B4)&&(GREEN_Colour.B4))//Ambian
             {
               bit_set(Red1_8,3); bit_set(Green1_8,3) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B5)//++++++++++++B5+++++++++++++++++++++
           {
             bit_clear(Green1_8,4); bit_clear(Red1_8,4);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B5 )&&(GREEN_Colour.B5 ==0 ))//Red
             {
               bit_set(Red1_8,4); bit_clear(Green1_8,4);
             }
             else if((RED_Colour.B5==0 )&&(GREEN_Colour.B5))//Green
             {
               bit_clear(Red1_8,4); bit_set(Green1_8,4); 
             }
             else if((RED_Colour.B5)&&(GREEN_Colour.B5))//Ambian
             {
               bit_set(Red1_8,4); bit_set(Green1_8,4) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B6)//++++++++++++B6+++++++++++++++++++++
           {
             bit_clear(Green1_8,5); bit_clear(Red1_8,5);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B6 )&&(GREEN_Colour.B6 ==0 ))//Red
             {
               bit_set(Red1_8,5); bit_clear(Green1_8,5);
             }
             else if((RED_Colour.B6==0 )&&(GREEN_Colour.B6))//Green
             {
               bit_clear(Red1_8,5); bit_set(Green1_8,5); 
             }
             else if((RED_Colour.B6)&&(GREEN_Colour.B6))//Ambian
             {
               bit_set(Red1_8,5); bit_set(Green1_8,5) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B7)//++++++++++++B7+++++++++++++++++++++
           {
             bit_clear(Green1_8,6); bit_clear(Red1_8,6);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B7 )&&(GREEN_Colour.B7 ==0 ))//Red
             {
               bit_set(Red1_8,6); bit_clear(Green1_8,6);
             }
             else if((RED_Colour.B7==0 )&&(GREEN_Colour.B7))//Green
             {
               bit_clear(Red1_8,6); bit_set(Green1_8,6); 
             }
             else if((RED_Colour.B7)&&(GREEN_Colour.B7))//Ambian
             {
               bit_set(Red1_8,6); bit_set(Green1_8,6) ;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B8)//++++++++++++B8+++++++++++++++++++++
           {
             bit_clear(Green1_8,7); bit_clear(Red1_8,7);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B8 )&&(GREEN_Colour.B8 ==0 ))//Red
             {
               bit_set(Red1_8,7); bit_clear(Green1_8,7);
             }
             else if((RED_Colour.B8==0 )&&(GREEN_Colour.B8))//Green
             {
               bit_clear(Red1_8,7); bit_set(Green1_8,7); 
             }
             else if((RED_Colour.B8)&&(GREEN_Colour.B8))//Ambian
             {
               bit_set(Red1_8,7); bit_set(Green1_8,7) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           ////////////////////////////////////////////////////////
           if(Output.B9)//++++++++++++B9+++++++++++++++++++++
           {
             bit_clear(Green9_10,0); bit_clear(Red9_10,0);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B9 )&&(GREEN_Colour.B9 ==0 ))//Red
             {
               bit_set(Red9_10,0); bit_clear(Green9_10,0);
             }
             else if((RED_Colour.B9==0 )&&(GREEN_Colour.B9))//Green
             {
               bit_clear(Red9_10,0); bit_set(Green9_10,0); 
             }
             else if((RED_Colour.B9)&&(GREEN_Colour.B9))//Ambian
             {
               bit_set(Red9_10,0); bit_set(Green9_10,0) ; 
             }  
           }
           ////////////////////////////////////////////////////////
            if(Output.B10)//++++++++++++B10+++++++++++++++++++++
           {
             bit_clear(Green9_10,1); bit_clear(Red9_10,1);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B10 )&&(GREEN_Colour.B10 ==0 ))//Red
             {
               bit_set(Red9_10,1); bit_clear(Green9_10,1);
             }
             else if((RED_Colour.B10==0 )&&(GREEN_Colour.B10))//Green
             {
               bit_clear(Red9_10,1); bit_set(Green9_10,1); 
             }
             else if((RED_Colour.B10)&&(GREEN_Colour.B10))//Ambian
             {
               bit_set(Red9_10,1); bit_set(Green9_10,1) ; 
             }  
           }
           
       // }
        
       // if(NoOfPoint >= 20)
      //{   
           ////////////////////////////////////////////////////////
            if(Output.B11)//++++++++++++B11+++++++++++++++++++++
           {
             bit_clear(Green11_18,0); bit_clear(Red11_18,0);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B11 )&&(GREEN_Colour.B11 ==0 ))//Red
             {
               bit_set(Red11_18,0); bit_clear(Green11_18,0);
             }
             else if((RED_Colour.B11==0 )&&(GREEN_Colour.B11))//Green
             {
               bit_clear(Red11_18,0); bit_set(Green11_18,0); 
             }
             else if((RED_Colour.B11)&&(GREEN_Colour.B11))//Ambian
             {
               bit_set(Red11_18,0); bit_set(Green11_18,0) ; 
             }  
           }
           ////////////////////////////////////////////////////////
            if(Output.B11)//++++++++++++B11+++++++++++++++++++++
           {
             bit_clear(Green11_18,0); bit_clear(Red11_18,0);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B11 )&&(GREEN_Colour.B11 ==0 ))//Red
             {
               bit_set(Red11_18,0); bit_clear(Green11_18,0);
             }
             else if((RED_Colour.B11==0 )&&(GREEN_Colour.B11))//Green
             {
               bit_clear(Red11_18,0); bit_set(Green11_18,0); 
             }
             else if((RED_Colour.B11)&&(GREEN_Colour.B11))//Ambian
             {
               bit_set(Red11_18,0); bit_set(Green11_18,0) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B12)//++++++++++++B12+++++++++++++++++++++
           {
             bit_clear(Green11_18,1); bit_clear(Red11_18,1);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B12 )&&(GREEN_Colour.B12 ==0 ))//Red
             {
               bit_set(Red11_18,1); bit_clear(Green11_18,1);
             }
             else if((RED_Colour.B12==0 )&&(GREEN_Colour.B12))//Green
             {
               bit_clear(Red11_18,1); bit_set(Green11_18,1); 
             }
             else if((RED_Colour.B12)&&(GREEN_Colour.B12))//Ambian
             {
               bit_set(Red11_18,1); bit_set(Green11_18,1) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B13)//++++++++++++B13+++++++++++++++++++++
           {
             bit_clear(Green11_18,2); bit_clear(Red11_18,2);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B13 )&&(GREEN_Colour.B13 ==0 ))//Red
             {
               bit_set(Red11_18,2); bit_clear(Green11_18,2);
             }
             else if((RED_Colour.B13==0 )&&(GREEN_Colour.B13))//Green
             {
               bit_clear(Red11_18,2); bit_set(Green11_18,2); 
             }
             else if((RED_Colour.B13)&&(GREEN_Colour.B13))//Ambian
             {
               bit_set(Red11_18,2); bit_set(Green11_18,2) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B14)//++++++++++++B14+++++++++++++++++++++
           {
             bit_clear(Green11_18,3); bit_clear(Red11_18,3);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B14 )&&(GREEN_Colour.B14 ==0 ))//Red
             {
               bit_set(Red11_18,3); bit_clear(Green11_18,3);
             }
             else if((RED_Colour.B14==0 )&&(GREEN_Colour.B14))//Green
             {
               bit_clear(Red11_18,3); bit_set(Green11_18,3); 
             }
             else if((RED_Colour.B14)&&(GREEN_Colour.B14))//Ambian
             {
               bit_set(Red11_18,3); bit_set(Green11_18,3) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B15)//++++++++++++B15+++++++++++++++++++++
           {
             bit_clear(Green11_18,4); bit_clear(Red11_18,4);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B15 )&&(GREEN_Colour.B15 ==0 ))//Red
             {
               bit_set(Red11_18,4); bit_clear(Green11_18,4);
             }
             else if((RED_Colour.B15==0 )&&(GREEN_Colour.B15))//Green
             {
               bit_clear(Red11_18,4); bit_set(Green11_18,4); 
             }
             else if((RED_Colour.B15)&&(GREEN_Colour.B15))//Ambian
             {
               bit_set(Red11_18,4); bit_set(Green11_18,4) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B16)//++++++++++++B16+++++++++++++++++++++
           {
             bit_clear(Green11_18,5); bit_clear(Red11_18,5);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B16 )&&(GREEN_Colour.B16 ==0 ))//Red
             {
               bit_set(Red11_18,5); bit_clear(Green11_18,5);
             }
             else if((RED_Colour.B16==0 )&&(GREEN_Colour.B16))//Green
             {
               bit_clear(Red11_18,5); bit_set(Green11_18,5); 
             }
             else if((RED_Colour.B16)&&(GREEN_Colour.B16))//Ambian
             {
               bit_set(Red11_18,5); bit_set(Green11_18,5) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B17)//++++++++++++B17+++++++++++++++++++++
           {
             bit_clear(Green11_18,6); bit_clear(Red11_18,6);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B17 )&&(GREEN_Colour.B17 ==0 ))//Red
             {
               bit_set(Red11_18,6); bit_clear(Green11_18,6);
             }
             else if((RED_Colour.B17==0 )&&(GREEN_Colour.B17))//Green
             {
               bit_clear(Red11_18,6); bit_set(Green11_18,6); 
             }
             else if((RED_Colour.B17)&&(GREEN_Colour.B17))//Ambian
             {
               bit_set(Red11_18,6); bit_set(Green11_18,6) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B18)//++++++++++++B18+++++++++++++++++++++
           {
             bit_clear(Green11_18,7); bit_clear(Red11_18,7);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B18 )&&(GREEN_Colour.B18 ==0 ))//Red
             {
               bit_set(Red11_18,7); bit_clear(Green11_18,7);
             }
             else if((RED_Colour.B18==0 )&&(GREEN_Colour.B18))//Green
             {
               bit_clear(Red11_18,7); bit_set(Green11_18,7); 
             }
             else if((RED_Colour.B18)&&(GREEN_Colour.B18))//Ambian
             {
               bit_set(Red11_18,7); bit_set(Green11_18,7) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B19)//++++++++++++B19+++++++++++++++++++++
           {
             bit_clear(Green19_20,0); bit_clear(Red19_20,0);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B19 )&&(GREEN_Colour.B19 ==0 ))//Red
             {
               bit_set(Red19_20,0); bit_clear(Green19_20,0);
             }
             else if((RED_Colour.B19==0 )&&(GREEN_Colour.B19))//Green
             {
               bit_clear(Red19_20,0); bit_set(Green19_20,0); 
             }
             else if((RED_Colour.B19)&&(GREEN_Colour.B19))//Ambian
             {
               bit_set(Red19_20,0); bit_set(Green19_20,0) ; 
             }  
           }
           ////////////////////////////////////////////////////////
           if(Output.B20)//++++++++++++B20+++++++++++++++++++++
           {
             bit_clear(Green19_20,1); bit_clear(Red19_20,1);
           }
           else
           {
             //bit_set(OUTRed1_8,0); bit_clear(OUTGreen1_8,0);
             if((RED_Colour.B20 )&&(GREEN_Colour.B20 ==0 ))//Red
             {
               bit_set(Red19_20,1); bit_clear(Green19_20,1);
             }
             else if((RED_Colour.B20==0 )&&(GREEN_Colour.B20))//Green
             {
               bit_clear(Red19_20,1); bit_set(Green19_20,1); 
             }
             else if((RED_Colour.B20)&&(GREEN_Colour.B20))//Ambian
             {
               bit_set(Red19_20,1); bit_set(Green19_20,1) ; 
             }  
           }
           ////////////////////////////////////////////////////////
         
      //} 
          
        
           IO_OUTPUT_A(IO_DEVICE_2, Red1_8);
           IO_OUTPUT_B(IO_DEVICE_2, Red9_10);
                     
           IO_OUTPUT_A(IO_DEVICE_3, Green1_8);
           IO_OUTPUT_B(IO_DEVICE_3, Green9_10);
           
           IO_OUTPUT_A(IO_DEVICE_5, Red11_18);  
           IO_OUTPUT_B(IO_DEVICE_5, Red19_20);
                     
           IO_OUTPUT_A(IO_DEVICE_4, Green11_18);
           IO_OUTPUT_B(IO_DEVICE_4, Green19_20);
           
                                
           //IO_OUTPUT_A(IO_DEVICE_4, Red1_8); //jj test
           //IO_OUTPUT_B(IO_DEVICE_4, Red9_10); //jj test
           
           //IO_OUTPUT_A(IO_DEVICE_5, Green1_8);  //jj test
           //IO_OUTPUT_B(IO_DEVICE_5, Green9_10); //jj test
                                           
           
   }

}
//////////////////////////////////////////////////////////////////
void ForceAllAlarm(void)
{
/*
   if(NoOfPoint >= 10)
   {
      Inputt.B1 = ~InputType.B1;
      Inputt.B2 = ~InputType.B2;
      Inputt.B3 = ~InputType.B3;
      Inputt.B4 = ~InputType.B4;
      Inputt.B5 = ~InputType.B5;
      Inputt.B6 = ~InputType.B6;
      Inputt.B7 = ~InputType.B7;
      Inputt.B8 = ~InputType.B8;
      Inputt.B9 = ~InputType.B9;
      Inputt.B10 = ~InputType.B10;
   }
   if(NoOfPoint >= 20)
   {
      Inputt.B11 = ~InputType.B11;
      Inputt.B12 = ~InputType.B12;
      Inputt.B13 = ~InputType.B13;
      Inputt.B14 = ~InputType.B14;
      Inputt.B15 = ~InputType.B15;
      Inputt.B16 = ~InputType.B16;
      Inputt.B17 = ~InputType.B17;
      Inputt.B18 = ~InputType.B18;
      Inputt.B19 = ~InputType.B19;
      Inputt.B20 = ~InputType.B20;
   }
   */
 
   In.B1 = 1;
   In.B2 = 1;
   In.B3 = 1;
   In.B4 = 1;
   In.B5 = 1;
   In.B6 = 1;
   In.B7 = 1;
   In.B8 = 1;
   In.B9 = 1;
   In.B10 = 1;
   In.B11 = 1;
   In.B12 = 1;
   In.B13 = 1;
   In.B14 = 1;
   In.B15 = 1;
   In.B16 = 1;
   In.B17 = 1;
   In.B18 = 1;
   In.B19 = 1;
   In.B20 = 1;


   Ack.B1 = 0;
   Ack.B2 = 0;
   Ack.B3 = 0;
   Ack.B4 = 0;
   Ack.B5 = 0;
   Ack.B6 = 0;
   Ack.B7 = 0;
   Ack.B8 = 0;
   Ack.B9 = 0;
   Ack.B10 = 0;
   Ack.B11 = 0;
   Ack.B12 = 0;
   Ack.B13 = 0;
   Ack.B14 = 0;
   Ack.B15 = 0;
   Ack.B16 = 0;
   Ack.B17 = 0;
   Ack.B18 = 0;
   Ack.B19 = 0;
   Ack.B20 = 0;  
   
   // Debug    
}

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////////////
///////////// Check Test ////////////////
void check_test(void)
{
   int8 output_temp[8];
   
   output_temp[0] = Output1_8;
   output_temp[1] = output9_16;


       if((Input(Ptest) == 0 && Test == 0)|| AutoTestFlag == 1) delay_ms(50);
       if((Input(Ptest) == 0 && Test == 0)|| AutoTestFlag == 1)
       {
         send_SMS_count = 0;
         restart_wdt();
           if((Input(Ptest) == 0 && Test == 0)|| AutoTestFlag ==1)
           {
             restart_wdt();
             if(T_test == 0) T_test = 0x06;    //3 second for time base 500 ms
             Test = 1;
              //if(NoOfPoint >= 16)
             // {
             
              IO_OUTPUT_A(IO_DEVICE_2, 0xFF);
              IO_OUTPUT_B(IO_DEVICE_2, 0xFF);
                         
              IO_OUTPUT_A(IO_DEVICE_3, 0xFF);
              IO_OUTPUT_B(IO_DEVICE_3, 0xFF);
              
              IO_OUTPUT_A(IO_DEVICE_4, 0xFF);
              IO_OUTPUT_B(IO_DEVICE_4, 0xFF);
                        
              IO_OUTPUT_A(IO_DEVICE_5, 0xFF);
              IO_OUTPUT_B(IO_DEVICE_5, 0xFF);
                 //PCF8575_write(0xFF,0x00,Input1_8_Wr_addr);
                 //PCF8575_write(0xFF,0x00,Input9_16_Wr_addr);
                 //IO_OUTPUT_B(IO_DEVICE_0, 0x00);
                 //IO_OUTPUT_B(IO_DEVICE_1, 0x00);
             // }
             
           }
       }
       else if(Input(Ptest) == 1 && Test == 1)
       {
           T_test = 0x00;
           Test = 0;
           Test_fault = 0;
       }

       if(Test_fault == 1)
       {
          functointest_f = 1;
          ForceAllAlarm();

          //Anal_Function();
          // jj 26/7/60
          if(Input(Ptest) == 1)
          {
            Test_fault = 0;
          }
       }
       else if(Input(Ptest) == 1 && Test == 1)
       {
          if(NoOfPoint >= 16)
          {
             //PCF8575_write(0xFF,output_temp[0],Input1_8_Wr_addr);
             //PCF8575_write(0xFF,output_temp[1],Input9_16_Wr_addr);
             //IO_OUTPUT_B(IO_DEVICE_0, output_temp[0]);
             //IO_OUTPUT_B(IO_DEVICE_1, output_temp[1]); 
          }
          
          T_test = 0x00;
          Test = 0;
          Test_fault = 0;
       }
}


/////////////// Check acknowledge ////////////////////////////

void check_ack(void)
{

   if((Input(Pack) == 0 && Ack_F == 0) || AutoAckFlag == 1) delay_ms(50); //Acknowledge function
   if((Input(Pack) == 0 && Ack_F == 0) || AutoAckFlag == 1)
   {
            AutoAckFlag = 0;
            AutoAckDelayTime = 0x00;
      
               //if(In.B1 == 1 || ((Inputt.B1 ^ InputType.B1) == 1))
           if((In.B1 == 1 )||(In2.B1 == 1 ))
           {
             Ack.B1 = 1;
             In.B1 = 0;
             In2.B1 = 0;      //for auto reset
             Output.B1 = 0;
             SendSMS.B1 =0;
           }
           //if(In.B2 == 1 || ((Inputt.B2 ^ InputType.B2) == 1))
           if((In.B2 == 1)||(In2.B2 == 1))
           {
             Ack.B2 = 1;
             In.B2 = 0;
             In2.B2 = 0;      //for auto reset
             Output.B2 = 0;
             SendSMS.B2 =0;
           }
           //if(In.B3 == 1 || ((Inputt.B3 ^ InputType.B3) == 1))
           if((In.B3 == 1)||(In2.B3 == 1))
           {
             Ack.B3 = 1;
             In.B3 = 0;
             In2.B3 = 0;      //for auto reset
             Output.B3 = 0;
             SendSMS.B3 =0;
           }
           //if(In.B4 == 1 || ((Inputt.B4 ^ InputType.B4) == 1))
           if((In.B4 == 1)||(In2.B4 == 1))
           {
             Ack.B4 = 1;
             In.B4 = 0;
             In2.B4 = 0;      //for auto reset
             Output.B4 = 0;
             SendSMS.B4 =0;
           }
           //if(In.B5 == 1 || ((Inputt.B5 ^ InputType.B5) == 1))
           if((In.B5 == 1)||(In2.B5 == 1))
           {
             Ack.B5 = 1;
             In.B5 = 0;
             In2.B5 = 0;      //for auto reset
             Output.B5 = 0;
             SendSMS.B5 =0;
           }
           //if(In.B6 == 1 || ((Inputt.B6 ^ InputType.B6) == 1))
           if((In.B6 == 1 )||(In2.B6 == 1 ))
           {
             Ack.B6 = 1;
             In.B6 = 0;
             In2.B6 = 0;      //for auto reset
             Output.B6 = 0;
             SendSMS.B6 =0;
           }
           //if(In.B7 == 1 || ((Inputt.B7 ^ InputType.B7) == 1))
           if((In.B7 == 1 )||(In2.B7 == 1 ))
           {
             Ack.B7 = 1;
             In.B7 = 0;
             In2.B7 = 0;      //for auto reset
             Output.B7 = 0;
             SendSMS.B7 =0;
           }
           //if(In.B8 == 1 || ((Inputt.B8 ^ InputType.B8) == 1))
           if((In.B8 == 1 )||(In2.B8 == 1 ))
           {
             Ack.B8 = 1;
             In.B8 = 0;
             In2.B8 = 0;      //for auto reset
             Output.B8 = 0;
             SendSMS.B8 =0;
           }
          // if(In.B9 == 1 || ((Inputt.B9 ^ InputType.B9) == 1))
           if((In.B9 == 1 )||(In2.B9 == 1 ))
           {
             Ack.B9 = 1;
             In.B9 = 0;
             In2.B9 = 0;      //for auto reset
             Output.B9 = 0;
             SendSMS.B9 =0;
           }
           //if(In.B10 == 1 || ((Inputt.B10 ^ InputType.B10) == 1))
           if((In.B10 == 1)||(In2.B10 == 1))
           {
             Ack.B10 = 1;
             In.B10 = 0;
             In2.B10 = 0;      //for auto reset
             Output.B10 = 0;
             SendSMS.B10 =0;
           }
           //if(In.B11 == 1 || ((Inputt.B11 ^ InputType.B11) == 1))
           if((In.B11 == 1)||(In2.B11 == 1))
           {
             Ack.B11 = 1;
             In.B11 = 0;
             In2.B11 = 0;      //for auto reset
             Output.B11 = 0;
             SendSMS.B11 =0;
           }
           //if(In.B12 == 1 || ((Inputt.B12 ^ InputType.B12) == 1))
           if((In.B12 == 1)||(In2.B12 == 1))
           {
             Ack.B12 = 1;
             In.B12 = 0;
             In2.B12 = 0;      //for auto reset
             Output.B12 = 0;
             SendSMS.B12 =0;
           }
           //if(In.B13 == 1 || ((Inputt.B13 ^ InputType.B13) == 1))
           if((In.B13 == 1 )||(In2.B13 == 1 ))
           {
             Ack.B13 = 1;
             In.B13 = 0;
             In2.B13 = 0;      //for auto reset
             Output.B13 = 0;
             SendSMS.B13 =0;
           }
           //if(In.B14 == 1 || ((Inputt.B14 ^ InputType.B14) == 1))
           if((In.B14 == 1)||(In2.B14 == 1))
           {
             Ack.B14 = 1;
             In.B14 = 0;
             In2.B14 = 0;      //for auto reset
             Output.B14 = 0;
             SendSMS.B14 =0;
           }
           //if(In.B15 == 1 || ((Inputt.B15 ^ InputType.B15) == 1))
           if((In.B15 == 1)||(In2.B15 == 1))
           {
             Ack.B15 = 1;
             In.B15 = 0;
             In2.B15 = 0;      //for auto reset
             Output.B15 = 0;
             SendSMS.B15 =0;
           }
           //if(In.B16 == 1 || ((Inputt.B16 ^ InputType.B16) == 1))
           if((In.B16 == 1)||(In2.B16 == 1))
           {
             Ack.B16 = 1;
             In.B16 = 0;
             In2.B16 = 0;      //for auto reset
             Output.B16 = 0;
             SendSMS.B16 =0;
           }
        
           //if(In.B17 == 1 || ((Inputt.B17 ^ InputType.B17) == 1))
           if((In.B17 == 1)||(In2.B17 == 1))
           {
             Ack.B17 = 1;
             In.B17 = 0;
             In2.B17 = 0;      //for auto reset
             Output.B17 = 0;
             SendSMS.B17 =0;
           }
           //if(In.B18 == 1 || ((Inputt.B18 ^ InputType.B18) == 1))
           if((In.B18 == 1)||(In2.B18 == 1))
           {
             Ack.B18 = 1;
             In.B18 = 0;
             In2.B18 = 0;      //for auto reset
             Output.B18 = 0;
             SendSMS.B18 =0;
           }
           //if(In.B19 == 1 || ((Inputt.B19 ^ InputType.B19) == 1))
           if((In.B19 == 1)||(In2.B19 == 1))
           {
             Ack.B19 = 1;
             In.B19 = 0;
             In2.B19 = 0;      //for auto reset
             Output.B19 = 0;
             SendSMS.B19 =0;
           }
           if((In.B20 == 1)||(In2.B20 == 1))
           {
             Ack.B20 = 1;
             In.B20 = 0;
             In2.B20 = 0;      //for auto reset
             Output.B20 = 0;
             SendSMS.B20 =0;
           }
           ///////////////////////////////////////////////////////////
            if(functointest_f){
               functointest_f = 0;
            }
            Ack_F = 1;
            output_bit(Pbuzzer,OffRelay);   //Buzzer
            output_bit(Pbell,OffRelay);     //Bell
   }
   else if(Input(Pack) == 1 && Ack_F == 1)
   {
      Ack_F = 0;
   }

}


///////////////////// Check Reset ///////////////////////////////////
void check_reset(void)
{
    if((Input(Preset) == 0 && Reset_F == 0)|| AutoResetFlag ==1) delay_ms(50);
    if((Input(Preset) == 0 && Reset_F == 0)|| AutoResetFlag ==1)
    {
       AutoResetFlag = 0;
       
       if((Inputt.B1 ^ InputType.B1) == 0 && In.B1 == 0) Output.B1 = 1;
       if((Inputt.B2 ^ InputType.B2) == 0 && In.B2 == 0) Output.B2 = 1;
       if((Inputt.B3 ^ InputType.B3) == 0 && In.B3 == 0) Output.B3 = 1;
       if((Inputt.B4 ^ InputType.B4) == 0 && In.B4 == 0) Output.B4 = 1;
       if((Inputt.B5 ^ InputType.B5) == 0 && In.B5 == 0) Output.B5 = 1;
       if((Inputt.B6 ^ InputType.B6) == 0 && In.B6 == 0) Output.B6 = 1;
       if((Inputt.B7 ^ InputType.B7) == 0 && In.B7 == 0) Output.B7 = 1;
       if((Inputt.B8 ^ InputType.B8) == 0 && In.B8 == 0) Output.B8 = 1;
       if((Inputt.B9 ^ InputType.B9) == 0 && In.B9 == 0) Output.B9 = 1;
       if((Inputt.B10 ^ InputType.B10) == 0 && In.B10 == 0) Output.B10 = 1;
       if((Inputt.B11 ^ InputType.B11) == 0 && In.B11 == 0) Output.B11 = 1;
       if((Inputt.B12 ^ InputType.B12) == 0 && In.B12 == 0) Output.B12 = 1;
       if((Inputt.B13 ^ InputType.B13) == 0 && In.B13 == 0) Output.B13 = 1;
       if((Inputt.B14 ^ InputType.B14) == 0 && In.B14 == 0) Output.B14 = 1;
       if((Inputt.B15 ^ InputType.B15) == 0 && In.B15 == 0) Output.B15 = 1;
       if((Inputt.B16 ^ InputType.B16) == 0 && In.B16 == 0) Output.B16 = 1;
       if((Inputt.B17 ^ InputType.B17) == 0 && In.B17 == 0) Output.B17 = 1;
       if((Inputt.B18 ^ InputType.B18) == 0 && In.B18 == 0) Output.B18 = 1;
       if((Inputt.B19 ^ InputType.B19) == 0 && In.B19 == 0) Output.B19 = 1;
       if((Inputt.B20 ^ InputType.B20) == 0 && In.B20 == 0) Output.B20 = 1;
       

       Reset_F = 1;
    }
    else if(Input(Preset) == 1 && Reset_F == 1)
    {
       Reset_F = 0;
    }
}
//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////Check Auto Reset function ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
int1 CheckAutoReset(unsigned char DatType)
{
   int1 check;     /// "1" = true  //Other Alarm active or not "ACK"

   if(DatType == 0x01) // Buzzer
   {
      check = (In.B1 | In2.B1) & ~Ack.B1 & (OutputType.B1 | OutputBoth.B1);
      check = check | ((In.B2 | In2.B2) & ~Ack.B2 & (OutputType.B2 | OutputBoth.B2));
      check = check | ((In.B3 | In2.B3) & ~Ack.B3 & (OutputType.B3 | OutputBoth.B3));
      check = check | ((In.B4 | In2.B4) & ~Ack.B4 & (OutputType.B4 | OutputBoth.B4));
      check = check | ((In.B5 | In2.B5) & ~Ack.B5 & (OutputType.B5 | OutputBoth.B5));
      check = check | ((In.B6 | In2.B6) & ~Ack.B6 & (OutputType.B6 | OutputBoth.B6));
      check = check | ((In.B7 | In2.B7) & ~Ack.B7 & (OutputType.B7 | OutputBoth.B7));
      check = check | ((In.B8 | In2.B8) & ~Ack.B8 & (OutputType.B8 | OutputBoth.B8));
      check = check | ((In.B9 | In2.B9) & ~Ack.B9 & (OutputType.B9 | OutputBoth.B9));
      check = check | ((In.B10 | In2.B10) & ~Ack.B10 & (OutputType.B10 | OutputBoth.B10));
      check = check | ((In.B11 | In2.B11) & ~Ack.B11 & (OutputType.B11 | OutputBoth.B11));
      check = check | ((In.B12 | In2.B12) & ~Ack.B12 & (OutputType.B12 | OutputBoth.B12));
      check = check | ((In.B13 | In2.B13) & ~Ack.B13 & (OutputType.B13 | OutputBoth.B13));
      check = check | ((In.B14 | In2.B14) & ~Ack.B14 & (OutputType.B14 | OutputBoth.B14));
      check = check | ((In.B15 | In2.B15) & ~Ack.B15 & (OutputType.B15 | OutputBoth.B15));
      check = check | ((In.B16 | In2.B16) & ~Ack.B16 & (OutputType.B16 | OutputBoth.B16));
      check = check | ((In.B17 | In2.B17) & ~Ack.B17 & (OutputType.B17 | OutputBoth.B17));
      check = check | ((In.B18 | In2.B18) & ~Ack.B18 & (OutputType.B18 | OutputBoth.B18));
      check = check | ((In.B19 | In2.B19) & ~Ack.B19 & (OutputType.B19 | OutputBoth.B19));
      check = check | ((In.B20 | In2.B20) & ~Ack.B20 & (OutputType.B20 | OutputBoth.B20));
     
   }
   else if(DatType == 0x02)             //Bell
   {
      check = (In.B1 | In2.B1) & ~Ack.B1 & (~OutputType.B1 | OutputBoth.B1);
      check = check | ((In.B2 | In2.B2) & ~Ack.B2 & (~OutputType.B2 | OutputBoth.B2));
      check = check | ((In.B3 | In2.B3) & ~Ack.B3 & (~OutputType.B3 | OutputBoth.B3));
      check = check | ((In.B4 | In2.B4) & ~Ack.B4 & (~OutputType.B4 | OutputBoth.B4));
      check = check | ((In.B5 | In2.B5) & ~Ack.B5 & (~OutputType.B5 | OutputBoth.B5));
      check = check | ((In.B6 | In2.B6) & ~Ack.B6 & (~OutputType.B6 | OutputBoth.B6));
      check = check | ((In.B7 | In2.B7) & ~Ack.B7 & (~OutputType.B7 | OutputBoth.B7));
      check = check | ((In.B8 | In2.B8) & ~Ack.B8 & (~OutputType.B8 | OutputBoth.B8));
      check = check | ((In.B9 | In2.B9) & ~Ack.B9 & (~OutputType.B9 | OutputBoth.B9));
      check = check | ((In.B10 | In2.B10) & ~Ack.B10 & (~OutputType.B10 | OutputBoth.B10));
      check = check | ((In.B11 | In2.B11) & ~Ack.B11 & (~OutputType.B11 | OutputBoth.B11));
      check = check | ((In.B12 | In2.B12) & ~Ack.B12 & (~OutputType.B12 | OutputBoth.B12));
      check = check | ((In.B13 | In2.B13) & ~Ack.B13 & (~OutputType.B13 | OutputBoth.B13));
      check = check | ((In.B14 | In2.B14) & ~Ack.B14 & (~OutputType.B14 | OutputBoth.B14));
      check = check | ((In.B15 | In2.B15) & ~Ack.B15 & (~OutputType.B15 | OutputBoth.B15));
      check = check | ((In.B16 | In2.B16) & ~Ack.B16 & (~OutputType.B16 | OutputBoth.B16));
      check = check | ((In.B17 | In2.B17) & ~Ack.B17 & (~OutputType.B17 | OutputBoth.B17));
      check = check | ((In.B18 | In2.B18) & ~Ack.B18 & (~OutputType.B18 | OutputBoth.B18));
      check = check | ((In.B19 | In2.B19) & ~Ack.B19 & (~OutputType.B19 | OutputBoth.B19));
      check = check | ((In.B20 | In2.B20) & ~Ack.B20 & (~OutputType.B20 | OutputBoth.B20));

   }
   return(check);
}


///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


void main()
{  
   delay_ms(50);
   output_bit(Pled,0);   //power on watchdog signal

   
   setup_timer_0(RTCC_INTERNAL);
   setup_timer_2(T2_DIV_BY_16,196,16);         //10ms
   enable_interrupts(INT_TIMER2);
   enable_interrupts(INT_RDA);

   setup_spi(SPI_MASTER  | SPI_L_TO_H |SPI_SS_DISABLED | SPI_XMIT_L_TO_H| SPI_CLK_DIV_16 ); //master1 jj

  
   IO_INIT();   //initializes the MCP23S17 chip.//----------jj
  
   IO_SET_TRIS_A(IO_DEVICE_0, 0xFF); //addr.0 Set PortA As Inputt
   IO_SET_TRIS_B(IO_DEVICE_0, 0xFF); //addr.0 Set PortB As Inputt
   IO_SET_TRIS_A(IO_DEVICE_1, 0xFF); //addr.1 Set PortA As Inputt 
   IO_SET_TRIS_B(IO_DEVICE_1, 0xFF); //addr.1 Set PortB As Inputt
   //  jj
   IO_WRITE_REGISTER(IO_DEVICE_0, GPPUA, 0xFF); // Input Pullup
   IO_WRITE_REGISTER(IO_DEVICE_0, GPPUB, 0xFF); // Input Pullup
   IO_WRITE_REGISTER(IO_DEVICE_1, GPPUA, 0xFF); // Input Pullup
   IO_WRITE_REGISTER(IO_DEVICE_1, GPPUB, 0xFF); // Input Pullup
   //  jj
   IO_SET_TRIS_A(IO_DEVICE_2, 0x00); //addr.2 Set PortA As Output 
   IO_SET_TRIS_B(IO_DEVICE_2, 0x00); //addr.2 Set PortB As Output
   IO_SET_TRIS_A(IO_DEVICE_3, 0x00); //addr.3 Set PortA As Output 
   IO_SET_TRIS_B(IO_DEVICE_3, 0x00); //addr.3 Set PortB As Output
   IO_SET_TRIS_A(IO_DEVICE_4, 0x00); //addr.4 Set PortA As Output 
   IO_SET_TRIS_B(IO_DEVICE_4, 0x00); //addr.4 Set PortB As Output 
   IO_SET_TRIS_A(IO_DEVICE_5, 0x00); //addr.5 Set PortA As Output 
   IO_SET_TRIS_B(IO_DEVICE_5, 0x00); //addr.5 Set PortB As Output
   
   FlashingFlag = 1;
   //output_bit(P485ctrl,0);
   sequence = end_sq;
   Address = 1;

   Output.B1 = 1;
   Output.B2 = 1;
   Output.B3 = 1;
   Output.B4 = 1;
   Output.B5 = 1;
   Output.B6 = 1;
   Output.B7 = 1;
   Output.B8 = 1;
   Output.B9 = 1;
   Output.B10 = 1;
   Output.B11 = 1;
   Output.B12 = 1;
   Output.B13 = 1;
   Output.B14 = 1;
   Output.B15 = 1;
   Output.B16 = 1;
   Output.B17 = 1;
   Output.B18 = 1;
   Output.B19 = 1;
   Output.B20 = 1;
   
   ////////////////////
   In.B1 = 0;
   In.B2 = 0;
   In.B3 = 0;
   In.B4 = 0;
   In.B5 = 0;
   In.B6 = 0;
   In.B7 = 0;
   In.B8 = 0;
   In.B9 = 0;
   In.B10 = 0;
   In.B11 = 0;
   In.B12 = 0;
   In.B13 = 0;
   In.B14 = 0;
   In.B15 = 0;
   In.B16 = 0;
   In.B17 = 0;
   In.B18 = 0;
   In.B19 = 0;
   In.B20 = 0;
   
   ////////////////////
   In2.B1 = 0;
   In2.B2 = 0;
   In2.B3 = 0;
   In2.B4 = 0;
   In2.B5 = 0;
   In2.B6 = 0;
   In2.B7 = 0;
   In2.B8 = 0;
   In2.B9 = 0;
   In2.B10 = 0;
   In2.B11 = 0;
   In2.B12 = 0;
   In2.B13 = 0;
   In2.B14 = 0;
   In2.B15 = 0;
   In2.B16 = 0;
   In2.B17 = 0;
   In2.B18 = 0;
   In2.B19 = 0;
   In2.B20 = 0;
   
   ////////////////////////
   Ack.B1 = 0;
   Ack.B2 = 0;
   Ack.B3 = 0;
   Ack.B4 = 0;
   Ack.B5 = 0;
   Ack.B6 = 0;
   Ack.B7 = 0;
   Ack.B8 = 0;
   Ack.B9 = 0;
   Ack.B10 = 0;
   Ack.B11 = 0;
   Ack.B12 = 0;
   Ack.B13 = 0;
   Ack.B14 = 0;
   Ack.B15 = 0;
   Ack.B16 = 0;
   Ack.B17 = 0;
   Ack.B18 = 0;
   Ack.B19 = 0;
   Ack.B20 = 0;
 
 /*
   Inputt.B1 = 0;
   Inputt.B2 = 0;
   Inputt.B3 = 0;
   Inputt.B4 = 0;
   Inputt.B5 = 0;
   Inputt.B6 = 0;
   Inputt.B7 = 0;
   Inputt.B8 = 0;
   Inputt.B9 = 0;
   Inputt.B10 = 0;
   Inputt.B11 = 0;
   Inputt.B12 = 0;
   Inputt.B13 = 0;
   Inputt.B14 = 0;
   Inputt.B15 = 0;
   Inputt.B16 = 0;
   Inputt.B17 = 0;
   Inputt.B18 = 0;
   Inputt.B19 = 0;
   Inputt.B20 = 0;
  */
    /////// read setting //////////

   if(read_eeprom(0x00) == 0x0F)
   {
      Read_Config();
   }
   else
   {
      FlashingRateTime = 25;
      AutoAck = 0xF0; //not use auto ack
      Address = 0x01;

      AutoAckTime = 5;
      FlashingRate = 25;
      NoOfPoint = 20;
      char i;
      for(i =0; i<=NoOfPoint; i++)
      {
         FaultDelayTime[i] = 0;
      }


      InputType.B1 = 1;
      InputType.B2 = 1;
      InputType.B3 = 1;
      InputType.B4 = 1;
      InputType.B5 = 1;
      InputType.B6 = 1;
      InputType.B7 = 1;
      InputType.B8 = 1;
      InputType.B9 = 1;
      InputType.B10 = 1;
      InputType.B11 = 1;
      InputType.B12 = 1;
      InputType.B13 = 1;
      InputType.B14 = 1;
      InputType.B15 = 1;
      InputType.B16 = 1;
      InputType.B17 = 1;
      InputType.B18 = 1;
      InputType.B19 = 1;
      InputType.B20 = 1;
      
      FaultNCNO[1] = 1;
      FaultNCNO[2] = 1;
      FaultNCNO[3] = 1;
      FaultNCNO[4] = 1;
      FaultNCNO[5] = 1;
      FaultNCNO[6] = 1;
      FaultNCNO[7] = 1;
      FaultNCNO[8] = 1;
      FaultNCNO[9] = 1;
      FaultNCNO[10] = 1;
      FaultNCNO[11] = 1;
      FaultNCNO[12] = 1;
      FaultNCNO[13] = 1;
      FaultNCNO[14] = 1;
      FaultNCNO[15] = 1;
      FaultNCNO[16] = 1;
      FaultNCNO[17] = 1;
      FaultNCNO[18] = 1;
      FaultNCNO[19] = 1;
      FaultNCNO[20] = 1;
      /////////////////////////////
      FaultType.B1 = 1;
      FaultType.B2= 1;
      FaultType.B3 = 1;
      FaultType.B4 = 1;
      FaultType.B5 = 1;
      FaultType.B6 = 1;
      FaultType.B7 = 1;
      FaultType.B8 = 1;
      FaultType.B9 = 1;
      FaultType.B10 = 1;
      FaultType.B11 = 1;
      FaultType.B12 = 1;
      FaultType.B13 = 1;
      FaultType.B14 = 1;
      FaultType.B15 = 1;
      FaultType.B16 = 1;
      FaultType.B17 = 1;
      FaultType.B18 = 1;
      FaultType.B19 = 1;
      FaultType.B20 = 1;
      
      ////////////////////////////
      OutputType.B1 = 1;
      OutputType.B2 = 1;
      OutputType.B3 = 1;
      OutputType.B4 = 1;
      OutputType.B5 = 1;
      OutputType.B6 = 1;
      OutputType.B7 = 1;
      OutputType.B8 = 1;
      OutputType.B9 = 1;
      OutputType.B10 = 1;
      OutputType.B11 = 1;
      OutputType.B12 = 1;
      OutputType.B13 = 1;
      OutputType.B14 = 1;
      OutputType.B15 = 1;
      OutputType.B16 = 1;
      OutputType.B17 = 1;
      OutputType.B18 = 1;
      OutputType.B19 = 1;
      OutputType.B20 = 1;
 
      /////////////////////////////
      OutputBoth.B1 = 1;
      OutputBoth.B2 = 1;
      OutputBoth.B3 = 1;
      OutputBoth.B4 = 1;
      OutputBoth.B5 = 1;
      OutputBoth.B6 = 1;
      OutputBoth.B7 = 1;
      OutputBoth.B8 = 1;
      OutputBoth.B9 = 1;
      OutputBoth.B10 = 1;
      OutputBoth.B11 = 1;
      OutputBoth.B12 = 1;
      OutputBoth.B13 = 1;
      OutputBoth.B14 = 1;
      OutputBoth.B15 = 1;
      OutputBoth.B16 = 1;
      OutputBoth.B17 = 1;
      OutputBoth.B18 = 1;
      OutputBoth.B19 = 1;
      OutputBoth.B20 = 1;
      
      //////////////////////////////////////////////
      AlarmIndicator.B1 = 1;
      AlarmIndicator.B2 = 1;
      AlarmIndicator.B3 = 1;
      AlarmIndicator.B4 = 1;
      AlarmIndicator.B5 = 1;
      AlarmIndicator.B6 = 1;
      AlarmIndicator.B7 = 1;
      AlarmIndicator.B8 = 1;
      AlarmIndicator.B9 = 1;
      AlarmIndicator.B10 = 1;
      AlarmIndicator.B11 = 1;
      AlarmIndicator.B12 = 1;
      AlarmIndicator.B13 = 1;
      AlarmIndicator.B14 = 1;
      AlarmIndicator.B15 = 1;
      AlarmIndicator.B16 = 1;
      AlarmIndicator.B17 = 1;
      AlarmIndicator.B18 = 1;
      AlarmIndicator.B19 = 1;
      AlarmIndicator.B20 = 1;
      
      ///////////// JACK///////////////////
   EEpDat = read_eeprom(0x10);
   //Red1_8 = EEpDat;
   RED_Colour.B1 = EEpDat;
   RED_Colour.B2 = EEpDat >> 1;
   RED_Colour.B3 = EEpDat >> 2;
   RED_Colour.B4 = EEpDat >> 3;
   RED_Colour.B5 = EEpDat >> 4;
   RED_Colour.B6 = EEpDat >> 5;
   RED_Colour.B7 = EEpDat >> 6;
   RED_Colour.B8 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x11);
   //Green9_10 = EEpDat;
   RED_Colour.B9 = EEpDat;
   RED_Colour.B10 = EEpDat >> 1;
   
   
   EEpDat = read_eeprom(0x12);
   //Red11_18 = EEpDat;
   RED_Colour.B11 = EEpDat;
   RED_Colour.B12 = EEpDat >> 1;
   RED_Colour.B13 = EEpDat >> 2;
   RED_Colour.B14 = EEpDat >> 3;
   RED_Colour.B15 = EEpDat >> 4;
   RED_Colour.B16 = EEpDat >> 5;
   RED_Colour.B17 = EEpDat >> 6;
   RED_Colour.B18 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x13);
   //Red19_20 = EEpDat;
   RED_Colour.B19 = EEpDat;
   RED_Colour.B20 = EEpDat >> 1;
   
   
   EEpDat = read_eeprom(0x14);
   //Green1_8 = EEpDat;
   GREEN_Colour.B1 = EEpDat;
   GREEN_Colour.B2 = EEpDat >> 1;
   GREEN_Colour.B3 = EEpDat >> 2;
   GREEN_Colour.B4 = EEpDat >> 3;
   GREEN_Colour.B5 = EEpDat >> 4;
   GREEN_Colour.B6 = EEpDat >> 5;
   GREEN_Colour.B7 = EEpDat >> 6;
   GREEN_Colour.B8 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x15);
   //Green9_10 = EEpDat;
   GREEN_Colour.B9 = EEpDat;
   GREEN_Colour.B10 = EEpDat >> 1;
  
   EEpDat = read_eeprom(0x16);
   //Green11_18 = EEpDat;
   GREEN_Colour.B11 = EEpDat;
   GREEN_Colour.B12 = EEpDat >> 1;
   GREEN_Colour.B13 = EEpDat >> 2;
   GREEN_Colour.B14 = EEpDat >> 3;
   GREEN_Colour.B15 = EEpDat >> 4;
   GREEN_Colour.B16 = EEpDat >> 5;
   GREEN_Colour.B17 = EEpDat >> 6;
   GREEN_Colour.B18 = EEpDat >> 7;
   
   EEpDat = read_eeprom(0x17);
   //Green19_20 = EEpDat;
   GREEN_Colour.B19 = EEpDat;
   GREEN_Colour.B20 = EEpDat >> 1;
   
   ///////////// JACK/////////////////////////////////////
      
   }

   
   set_tris_a (0b00000000); //PORTA SET TO OUTPUT
   set_tris_b (0b00000000); //PORTA SET TO OUTPUT
   enable_interrupts(GLOBAL);
   setup_wdt(WDT_ON);
   Read_input(); restart_wdt();
   output_bit(Pbuzzer,OffRelay);   //Clear Buzzer
   output_bit(Pbell,OffRelay);     //Clear Bell
   output_bit(P485ctrl,0);
   output_bit(PIN_B4,1); //jj
   Send_Ouput();
   
   IO_OUTPUT_A(IO_DEVICE_2, 0xff);
   IO_OUTPUT_B(IO_DEVICE_2, 0xff);
            
   IO_OUTPUT_A(IO_DEVICE_3, 0xff);
   IO_OUTPUT_B(IO_DEVICE_3, 0xff);
  
   IO_OUTPUT_A(IO_DEVICE_5, 0xff);  
   IO_OUTPUT_B(IO_DEVICE_5, 0xff);
            
   IO_OUTPUT_A(IO_DEVICE_4, 0xff);
   IO_OUTPUT_B(IO_DEVICE_4, 0xff);
   
   
   
   // jj
   char i;
   for(i=1;i<=20;i++)
   {
      FaultAgo[i] = 0;
      FaultNow[i] = 0;
      ReleaseTime[i] = 0;
      
      if(FaultNCNO[i] ==NO)
      {
         switch(i)
         {
            case 1:
               Inputt.B1 = 1; 
            break;
            case 2:
               Inputt.B2 = 1; 
            break;
            case 3:
               Inputt.B3 = 1; 
            break;
            case 4:
               Inputt.B4 = 1; 
            break;
            case 5:
               Inputt.B5 = 1; 
            break;
            case 6:
               Inputt.B6 = 1; 
            break;
            case 7:
               Inputt.B7 = 1; 
             break;
             case 8:
               Inputt.B8 = 1; 
            break;
            case 9:
               Inputt.B9 = 1; 
            break;
            case 10:
               Inputt.B10 = 1; 
            break;
            case 11:
               Inputt.B11 = 1; 
            break;
            case 12:
               Inputt.B12 = 1; 
            break;
            case 13:
               Inputt.B13 = 1; 
            break;
            case 14:
               Inputt.B14 = 1; 
            break;
            case 15:
               Inputt.B15 = 1; 
            break;
            case 16:
                Inputt.B16 = 1;
             break;
             case 17:
               Inputt.B17 = 1; 
             break;
             case 18:
               Inputt.B18 = 1; 
            break;
            case 19:
               Inputt.B19 = 1; 
            break;
            case 20:
               Inputt.B20 = 1; 
            break;
                       
         }                 
      }
      else
      {
         switch(i)
         {
            case 1:
               Inputt.B1 = 0; 
            break;
            case 2:
               Inputt.B2 = 0; 
            break;
            case 3:
               Inputt.B3 = 0; 
            break;
            case 4:
               Inputt.B4 = 0; 
            break;
            case 5:
               Inputt.B5 = 0; 
            break;
            case 6:
               Inputt.B6 = 0; 
            break;
            case 7:
               Inputt.B7 = 0; 
             break;
             case 8:
               Inputt.B8 = 0; 
            break;
            case 9:
               Inputt.B9 = 0; 
            break;
            case 10:
               Inputt.B10 = 0; 
            break;
            case 11:
               Inputt.B11 = 0; 
            break;
            case 12:
               Inputt.B12 = 0; 
            break;
            case 13:
               Inputt.B13 = 0; 
            break;
            case 14:
               Inputt.B14 = 0; 
            break;
            case 15:
               Inputt.B15 = 0; 
            break;
            case 16:
                Inputt.B16 = 0;
             break;
             case 17:
               Inputt.B17 = 0; 
             break;
             case 18:
               Inputt.B18 = 0; 
            break;
            case 19:
               Inputt.B19 = 0; 
            break;
            case 20:
               Inputt.B20 = 0; 
            break;
                       
         }                 
      }
      
   }
   
   //GSM SIM900 Init
   delay_ms(1000);
   fprintf(CH2,"AT+CMGF=1"); 
   putc('\n',CH2);
   delay_ms(50);
   
   SendSMS.B1 =0;
   SendSMS.B2 =0;
   SendSMS.B3 =0;
   SendSMS.B4 =0;
   SendSMS.B5 =0;
   SendSMS.B6 =0;
   SendSMS.B7 =0;
   SendSMS.B8 =0;
   SendSMS.B9 =0;
   SendSMS.B10 =0;
   SendSMS.B11 =0;
   SendSMS.B12 =0;
   SendSMS.B13 =0;
   SendSMS.B14 =0;
   SendSMS.B15 =0;
   SendSMS.B16 =0;
   SendSMS.B17 =0;
   SendSMS.B18 =0;
   SendSMS.B19 =0;
   SendSMS.B20 =0;
   
   char m;
   for(m=0; m<10; m++)
   {
      sms_phonenumber[m] = read_eeprom(0x32 + m);
   }
   sms_phonenumber[m] = '\0' ; // end string
   delay_ms(500);
   
   char j;
   for(j=1;j<=3;j++)
   {

      IO_OUTPUT_A(IO_DEVICE_2, 0xff);
      IO_OUTPUT_B(IO_DEVICE_2, 0xff);
               
      IO_OUTPUT_A(IO_DEVICE_3, 0xff);
      IO_OUTPUT_B(IO_DEVICE_3, 0xff);
     
      IO_OUTPUT_A(IO_DEVICE_5, 0xff);  
      IO_OUTPUT_B(IO_DEVICE_5, 0xff);
               
      IO_OUTPUT_A(IO_DEVICE_4, 0xff);
      IO_OUTPUT_B(IO_DEVICE_4, 0xff);
      
      delay_ms(250);
      IO_OUTPUT_A(IO_DEVICE_2, 0);
      IO_OUTPUT_B(IO_DEVICE_2, 0);
               
      IO_OUTPUT_A(IO_DEVICE_3, 0);
      IO_OUTPUT_B(IO_DEVICE_3, 0);
     
      IO_OUTPUT_A(IO_DEVICE_5, 0);  
      IO_OUTPUT_B(IO_DEVICE_5, 0);
               
      IO_OUTPUT_A(IO_DEVICE_4, 0);
      IO_OUTPUT_B(IO_DEVICE_4, 0);
      delay_ms(250);
   }
   functointest_f = 0;
   
   while(TRUE)
   {
  
      IO_OUTPUT_A(IO_DEVICE_0, 0xFF); //jj 
      IO_OUTPUT_B(IO_DEVICE_0, 0xFF); //jj 
      IO_OUTPUT_A(IO_DEVICE_1, 0xFF); //jj 
      IO_OUTPUT_B(IO_DEVICE_1, 0xFF); //jj 
      restart_wdt();   
      if(recieve_completed == 1)
      {
         Modbus_Function();
         recieve_completed = 0 ;
      }
      check_ack();restart_wdt();
      check_reset();restart_wdt();
      check_test();restart_wdt();      
      
      if(StartRead)
      {
         Anal_Function(); restart_wdt();
         
         
         Send_Ouput(); restart_wdt();
        // Driver595(); restart_wdt();      
        // output_toggle(PIN_A0);
         Read_input(); restart_wdt(); //Must be first
         
         if(send_SMS_period){
            send_SMS_period =0;
            sendsum_sms();
         }
         
         if(RefreshConfigData)
         {
            RefreshConfigData =0;
            Read_Config();
            //IO_INIT();   //initializes the MCP23S17 chip.//----------jj
     
            IO_SET_TRIS_A(IO_DEVICE_0, 0xFF); //addr.0 Set PortA As Input
            IO_SET_TRIS_B(IO_DEVICE_0, 0xFF); //addr.0 Set PortB As Input
            IO_SET_TRIS_A(IO_DEVICE_1, 0xFF); //addr.1 Set PortA As Input 
            IO_SET_TRIS_B(IO_DEVICE_1, 0xFF); //addr.1 Set PortB As Input
            //  jj
            IO_WRITE_REGISTER(IO_DEVICE_0, GPPUA, 0xFF); // Input Pullup
            IO_WRITE_REGISTER(IO_DEVICE_0, GPPUB, 0xFF); // Input Pullup
            IO_WRITE_REGISTER(IO_DEVICE_1, GPPUA, 0xFF); // Input Pullup
            IO_WRITE_REGISTER(IO_DEVICE_1, GPPUB, 0xFF); // Input Pullup
            //  jj
            IO_SET_TRIS_A(IO_DEVICE_2, 0x00); //addr.2 Set PortA As Output 
            IO_SET_TRIS_B(IO_DEVICE_2, 0x00); //addr.2 Set PortB As Output
            IO_SET_TRIS_A(IO_DEVICE_3, 0x00); //addr.3 Set PortA As Output 
            IO_SET_TRIS_B(IO_DEVICE_3, 0x00); //addr.3 Set PortB As Output
            IO_SET_TRIS_A(IO_DEVICE_4, 0x00); //addr.4 Set PortA As Output 
            IO_SET_TRIS_B(IO_DEVICE_4, 0x00); //addr.4 Set PortB As Output 
            IO_SET_TRIS_A(IO_DEVICE_5, 0x00); //addr.5 Set PortA As Output 
            IO_SET_TRIS_B(IO_DEVICE_5, 0x00); //addr.5 Set PortB As Output
         }
         
      }     

   }
   
}
