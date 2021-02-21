#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "IIC.H"
#include  "USART.H"
#include <string.h>
#include <AT24C64.h>
/*****************************************************************
AT24C64 对应着板子上的U24，地址线全部选择VCC，用来存储系统初始化参数
****************************************************************/
//器件型号
#define EE24C64	    
//器件地址及参数 	因为地址线都接VCC所以地址是ae
#define C256_EEPROM_ADD    0xAE

#define EEPROM_PageSize	32
#define EEPROM_PageNum	128   
#define DwordOrByte_Add_64  1
#define EEPROM_Total_Size	0x2000
		 

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
static unsigned char AT24C64_ReadOneByte(unsigned short ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_64)
	{
		IIC_Send_Byte(C256_EEPROM_ADD);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8); //发送高地址
		//IIC_Wait_Ack();		 
	}else IIC_Send_Byte(C256_EEPROM_ADD+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(C256_EEPROM_ADD+1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24C64_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_64)
	{
		IIC_Send_Byte(C256_EEPROM_ADD);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
		//IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(C256_EEPROM_ADD+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	SW_EEPROM_Delay_nms(20);	 
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
unsigned char RdBytesFromAT24C64(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
	unsigned int END_Page=0; 
//	unsigned char b[50];
  
	END_Page=(ReadAddr+NumToRead)/EEPROM_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//超范围 
	while(NumToRead)
	{
		*pBuffer=AT24C64_ReadOneByte(ReadAddr);	
	//	b[NumToRead] = *pBuffer;
		ReadAddr++;
		pBuffer++;
		NumToRead--;
	}
	return 0;
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
unsigned char WrBytesToAT24C64(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
	unsigned int END_Page=0; 

	END_Page=(WriteAddr+NumToWrite)/EEPROM_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//超范围 
	while(NumToWrite)
	{
		AT24C64_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
		NumToWrite--;
	}		
	return 0;
}
/************************************************************************************
**功    能:  格式化EEPROM
**入口参数: 
**出口参数: 成功返回0
************************************************************************************/


unsigned char Check_24C64(unsigned short Addr)
{
	char	st_data;
	st_data=AT24C64_ReadOneByte(Addr);	
	AT24C64_WriteOneByte(Addr,0xaa);
	if (AT24C64_ReadOneByte(Addr)!=0xaa)
		return	1;
	else
	{
		AT24C64_WriteOneByte(Addr,0x55);
		if (AT24C64_ReadOneByte(Addr)!=0x55)
			return	1;
		else
		{
			AT24C64_WriteOneByte(Addr,st_data);
			return	0;
		}
	}
}



 