
#include "PCF8563.H"
#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "IIC.H"
#include  "USART.H"
#include <string.h>
#include "include.h"


#define PCF8563_Read_Add	0xA3
#define PCF8563_Write_Add	0xA2 
unsigned char		bitSysTimeDisable = 0;

/*****************************************************************/
/*
********************************************************************/ 
//PCF8563时钟停止指令   i2c_write_n_bytes(unsigned char SLA_W, uint16 ADDR,uint16 N, unsigned char *DAT)
static unsigned char PCF8563_stop(void)
{
 	unsigned char stopcode=0x20;
 	if(0==IIC_WriteBuf(0,&stopcode,1))
		return 0;
 	else return 1;
}
//PCF8563时钟启动指令
static unsigned char PCF8563_start(void)
{
 	unsigned char startcode=0x00;
	if(0==IIC_WriteBuf(0,&startcode,1))
		return 0;
 	else return 1;
}
////中断产生后清除TF标志位
unsigned char PCF8563_ClearINT(void)
{
 	unsigned char temp=0x12;
	if(0==IIC_WriteBuf(1,&temp,1))
		return 0;
 	else return 1;
}

/************************************************************************
PCF8563时钟设置
参数说明：（BCD码）
yy:  年（0x00到0x99）
mm:	 月（0x01到0x12）
dd:	 日（0x01到0x31）
hh:	 时（0x00到0x23）
mi:	 分（0x00到0x59）
ss:	 秒（0x00到0x59）
da:	 星期（0x01到0x07）
************************************************************************/
void Set_Sysdate(unsigned char * ptr)
{
	 unsigned char time[7];
	 time[6]=ptr[0];//年
	 time[5]=ptr[1];//月
	 //time[4]=ptr[2];//星期
	 time[3]=ptr[2];//日
	 time[2]=ptr[3];//时
	 time[1]=ptr[4];//分
	 time[0]=ptr[5];//秒
	 PCF8563_stop();
	 IIC_WriteBuf(2,time,7);

	 PCF8563_start();	    
}

/************************************************************************
PCF8563时钟读取
参数说明：（BCD码）
time:	  存放读出时间的数组名
time[6]:  年（0x00到0x99）
time[5]:  月（0x01到0x12）
time[3]:  日（0x01到0x31）
time[2]:  时（0x00到0x23）
time[1]:  分（0x00到0x59）
time[0]:  秒（0x00到0x59）
time[4]:  星期（0x01到0x07）
************************************************************************/
void Read_Sysdate(unsigned char *ptr)
{
	 unsigned char time[7];
	 IIC_ReadBuf(2,time,7);

		ptr[0]=time[6]; //年
    ptr[1]=time[5]&0x1f; //月
    ptr[2]=time[3]&0x3f; //日
    ptr[3]=time[2]&0x3f; //时
    ptr[4]=time[1]&0x7f; //分
		if (time[0]<0x80)
			bitSysTimeDisable=0;
		else
			bitSysTimeDisable=1;
    ptr[5]=time[0]&0x7f; //秒
		ptr[6]=time[4]&7;
	  
}

uchar		DiagTimeString(uchar bbit,uchar * ptr)
{
	uchar	 aa;
	uchar 	 bb;
	uchar	 Max;
	uchar	MaxDay[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	if (!bbit)
	{
		if (BCD_String_Diag(ptr,3))
			return	1;	
		aa=BCDToHex(ptr[1]);
		if (!aa || aa>12)
			return	1;
		Max=MaxDay[aa];
		if (aa==2)
		{
			bb=BCDToHex(ptr[0]);
			if (!(bb%4))
				Max=29;	
		}
		aa=BCDToHex(ptr[2]);
		if (!aa || aa>Max)
			return	1;				
	}
	else
	{
		if (BCD_String_Diag(ptr,3))
			return	1;	
		aa=BCDToHex(ptr[0]);
		if ( aa>23)
			return	1;
		aa=BCDToHex(ptr[1]);
		if ( aa>59)
			return	1;
		aa=BCDToHex(ptr[2]);
		if ( aa>59)
			return	1;
	}
	return  0;
}

unsigned char		CheckPF8563(void)
{
	unsigned char i;
	uchar		bbit;
	for (i=0;i<5;i++)
	{
		Read_Sysdate(SysTimeDatas.TimeString);
		bbit=DiagTimeString(0,SysTimeDatas.TimeString);
		bbit=DiagTimeString(0,SysTimeDatas.TimeString);
		if (!bbit)
			bbit=DiagTimeString(1,SysTimeDatas.TimeString+3);
		if (!bbit)
		{
			if (!bitSysTimeDisable)
			{
				memcpy(SysTimeDatasBak.TimeString,SysTimeDatas.TimeString,6);
				return	0;
			}
		}
	}
	return	1;
}



//在指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
static unsigned char IIC_ReadOneByte(unsigned short ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
  IIC_Start();  
	IIC_Send_Byte(PCF8563_Write_Add);   //发送器件地址PCF8563_Read_Add,写数据 	 
	IIC_Wait_Ack(); 
  IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(PCF8563_Read_Add);           //进入接收模式			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
static void IIC_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
  IIC_Start();  
	IIC_Send_Byte(PCF8563_Write_Add);   //发送器件地址PCF8563_Write_Add,写数据 
	IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
  IIC_Stop();//产生一个停止条件 
	SW_EEPROM_Delay_nms(10);	 
}
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
 unsigned char IIC_ReadBuf(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=IIC_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
	return 0;
}  
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
 unsigned char IIC_WriteBuf(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
	while(NumToWrite--)
	{
		IIC_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}		
	return 0;
}

