#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "IIC.H"
#include  "USART.H"
#include <string.h>
#include <AT24C256.h>
#include "include.h"
/*****************************************************************
	AT24C256 对应着板子上的U23，地址线全部选择地，用来存储黑名单参数
****************************************************************/
unsigned char buffer[8];
//器件型号
#define EE24C256 		    
//器件地址及参数 	因为地址线都接地所以地址是a0
#define I2C_EEPROM_ADD    0xA0

#define EEPROM256_PageSize	64
#define EEPROM256_PageNum	128   
#define DwordOrByte_Add_256  1
#define EEPROM256_Total_Size	0x4000
		 		

/**************************************************************************************

**************************************************************************************/



//EEPROM芯片相关硬件初始化初始化
void AT24C256_Init(void)
{
	Init_SWIIC();
}

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
unsigned char RdByte_24C256(uint ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_256)
	{
		IIC_Send_Byte(I2C_EEPROM_ADD);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8); //发送高地址
		//IIC_Wait_Ack();		 
	}else IIC_Send_Byte(I2C_EEPROM_ADD+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(I2C_EEPROM_ADD+1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void WrByte_24C256(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_256)
	{
		IIC_Send_Byte(I2C_EEPROM_ADD);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
		//IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(I2C_EEPROM_ADD+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	SW_EEPROM_Delay_nms(50);	 
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
unsigned char AT24CXX_Read(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
	unsigned int END_Page=0; 
	volatile unsigned char b[20];
  
	END_Page=(ReadAddr+NumToRead)/EEPROM256_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//超范围 
	while(NumToRead)
	{
		*pBuffer=RdByte_24C256(ReadAddr);	
		b[NumToRead] = *pBuffer;
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
unsigned char AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
	unsigned int END_Page=0; 

	END_Page=(WriteAddr+NumToWrite)/EEPROM256_PageSize;
	if(END_Page>EEPROM256_PageNum-1)
		return 1;//超范围 
	while(NumToWrite)
	{
		WrByte_24C256(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
		NumToWrite--;
	}		
	return 0;
}


//初始化模拟总线
void Init_SWIIC(void)
{
	SDA_OUT();				   
	CLK_OUT();
}
			 				     
	   
unsigned char		Check_BlackCardNum(ulong iii)
{
	uint		Addr;
	uchar		i,j;
	uchar		st_data;
	Addr=iii/8;
	i=iii%8;
	st_data=RdByte_24C256(Addr);
	j=1<<i;
	if (st_data & j)
		return	0;
	else
		return	1;
}
//硬件自检函数
unsigned char		Check_24C256(uint	Addr)
{
	uchar	st_data;
	st_data=RdByte_24C256(Addr);	
	WrByte_24C256(Addr,0xaa);
	if (RdByte_24C256(Addr)!=0xaa)
		return	1;
	else
	{
		WrByte_24C256(Addr,0x55);
		if (RdByte_24C256(Addr)!=0x55)
			return	1;
		else
		{
			WrByte_24C256(Addr,st_data);
			return	0;
		}
	}	
}
void Clr_24C02(void)
{	ulong ii;
	for (ii=0;ii<512*64;ii++)
	{
			WrByte_24C256(ii,0xff);
	}
}

 //清楚白名单
void Clr_WhiteNum(void)
{
	uint  ii;
//	uchar Addr1 = 0;
	uchar Buffer[64];
	
	memset(Buffer,0xff,64);		
	for (ii=0;ii<10;ii++)
	{
		AT24CXX_Write(ii*64,Buffer,64);
		Clr_WatchDog();
	}
	
}

void	Chg_BlkNameSub(uchar bbit,ulong iii)//增加黑名单
{//bbit=0-增加黑名单，1-减少黑名单
	ulong	Addr;
	uchar	 i,j;
	uchar	 st_data;
    //zjx_change1_mark
	Addr= (iii / 8);
    //-------------------
	i=iii%8;
	st_data=RdByte_24C256(Addr);
	j=1<<i;
	if (!bbit)
	{
		j=~j;
		st_data&=j;
	}
	else
		st_data|=j;
	WrByte_24C256(Addr,st_data);
}
 