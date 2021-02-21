#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "IIC.H"
#include  "USART.H"
#include <string.h>
#include <AT24C64.h>
/*****************************************************************
AT24C64 ��Ӧ�Ű����ϵ�U24����ַ��ȫ��ѡ��VCC�������洢ϵͳ��ʼ������
****************************************************************/
//�����ͺ�
#define EE24C64	    
//������ַ������ 	��Ϊ��ַ�߶���VCC���Ե�ַ��ae
#define C256_EEPROM_ADD    0xAE

#define EEPROM_PageSize	32
#define EEPROM_PageNum	128   
#define DwordOrByte_Add_64  1
#define EEPROM_Total_Size	0x2000
		 

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
static unsigned char AT24C64_ReadOneByte(unsigned short ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_64)
	{
		IIC_Send_Byte(C256_EEPROM_ADD);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8); //���͸ߵ�ַ
		//IIC_Wait_Ack();		 
	}else IIC_Send_Byte(C256_EEPROM_ADD+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(C256_EEPROM_ADD+1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24C64_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_64)
	{
		IIC_Send_Byte(C256_EEPROM_ADD);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
		//IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(C256_EEPROM_ADD+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	SW_EEPROM_Delay_nms(20);	 
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
unsigned char RdBytesFromAT24C64(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
	unsigned int END_Page=0; 
//	unsigned char b[50];
  
	END_Page=(ReadAddr+NumToRead)/EEPROM_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//����Χ 
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
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ 
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
unsigned char WrBytesToAT24C64(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
	unsigned int END_Page=0; 

	END_Page=(WriteAddr+NumToWrite)/EEPROM_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//����Χ 
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
**��    ��:  ��ʽ��EEPROM
**��ڲ���: 
**���ڲ���: �ɹ�����0
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



 