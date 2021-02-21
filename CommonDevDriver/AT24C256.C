#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "IIC.H"
#include  "USART.H"
#include <string.h>
#include <AT24C256.h>
#include "include.h"
/*****************************************************************
	AT24C256 ��Ӧ�Ű����ϵ�U23����ַ��ȫ��ѡ��أ������洢����������
****************************************************************/
unsigned char buffer[8];
//�����ͺ�
#define EE24C256 		    
//������ַ������ 	��Ϊ��ַ�߶��ӵ����Ե�ַ��a0
#define I2C_EEPROM_ADD    0xA0

#define EEPROM256_PageSize	64
#define EEPROM256_PageNum	128   
#define DwordOrByte_Add_256  1
#define EEPROM256_Total_Size	0x4000
		 		

/**************************************************************************************

**************************************************************************************/



//EEPROMоƬ���Ӳ����ʼ����ʼ��
void AT24C256_Init(void)
{
	Init_SWIIC();
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
unsigned char RdByte_24C256(uint ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_256)
	{
		IIC_Send_Byte(I2C_EEPROM_ADD);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8); //���͸ߵ�ַ
		//IIC_Wait_Ack();		 
	}else IIC_Send_Byte(I2C_EEPROM_ADD+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(I2C_EEPROM_ADD+1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void WrByte_24C256(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_256)
	{
		IIC_Send_Byte(I2C_EEPROM_ADD);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
		//IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(I2C_EEPROM_ADD+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	SW_EEPROM_Delay_nms(50);	 
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
unsigned char AT24CXX_Read(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
	unsigned int END_Page=0; 
	volatile unsigned char b[20];
  
	END_Page=(ReadAddr+NumToRead)/EEPROM256_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//����Χ 
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
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
unsigned char AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
	unsigned int END_Page=0; 

	END_Page=(WriteAddr+NumToWrite)/EEPROM256_PageSize;
	if(END_Page>EEPROM256_PageNum-1)
		return 1;//����Χ 
	while(NumToWrite)
	{
		WrByte_24C256(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
		NumToWrite--;
	}		
	return 0;
}


//��ʼ��ģ������
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
//Ӳ���Լ캯��
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

 //���������
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

void	Chg_BlkNameSub(uchar bbit,ulong iii)//���Ӻ�����
{//bbit=0-���Ӻ�������1-���ٺ�����
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
 