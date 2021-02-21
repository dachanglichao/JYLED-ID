#ifndef __AT24C256_H
#define __AT24C256_H	
#include "char.H"

#define EEPROM_PageSize	128
#define EEPROM_PageNum	512   
#define DwordOrByte_Add  1
#define EEPROM_Total_Size	EEPROM_PageSize*EEPROM_PageNum	
			   
//IIC���в�������
extern void Init_SWIIC(void);                //��ʼ��IIC��IO��				 
extern void IIC_Start(void);				//����IIC��ʼ�ź�
extern void IIC_Stop(void);	  			//����IICֹͣ�ź�
extern void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
extern unsigned char IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
extern unsigned char IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
extern void IIC_Ack(void);					//IIC����ACK�ź�
extern void IIC_NAck(void);				//IIC������ACK�ź�
extern void AT24C256_Init(void);
extern void WrByte_24c256(unsigned short WriteAddr,unsigned char DataToWrite);
extern unsigned char RdByte_24C256(uint ReadAddr);
//unsigned char Formate_EE(void);
extern void SW_EEPROM_Delay_nus(unsigned short nus);
extern void SW_EEPROM_Delay_nms(unsigned short nms);
extern void	Chg_BlkNameSub(uchar bbit,ulong iii);//���Ӻ�����
extern unsigned char		Check_24C256(uint	Addr);
extern void Clr_WhiteNum(void);
extern unsigned char		Check_BlackCardNum(ulong iii);
extern unsigned char AT24CXX_Write(unsigned short ,unsigned char *,unsigned short );
//void check_24C02(void);
#endif
	 


