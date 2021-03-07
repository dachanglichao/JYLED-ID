/*******************************************************************
�˿�0��UDPģʽ
�˿�1��TCP_CLIENT
�˿�2��TCP_SERVER
******************************************************************/

#include "MyDefine.h"
#include "char.h"
#include "CalucationFile.h"
#include "SetW5500Net.h"
#include "include.h"
#include "externVariable.h"
#include "NVIC.H"
#include "stm32f10x_flash.h"
#include "W5500.h"
#include <string.h> 
#include <stdio.h>  
#include <stdbool.h>
#include  <stdlib.h>
#include "stm32f10x_flash.h"

uint 	UdpDatasCount;
uchar	ReAddrCount=0;
uchar	bitHaveReComd=0;
uint	ReComputerAddr;
uchar TCPPacketNum = 0;//��ֹTCP�������
extern u8 svraddrip[4];
extern int svraddrport ;
unsigned short ReceivedataLen =0;
extern void	TCP_ReceiveSub(void);//�������ݴ���
extern char *mystrstr(char *s1,char *s2)  ;

void InitW5500Net(void)
{
		W5500_NVIC_Configuration();	//W5500 ���������ж����ȼ�����
		SPI_Configuration();		//W5500 SPI��ʼ������(STM32 SPI1)
		W5500_GPIO_Configuration();	//W5500 GPIO��ʼ������			
		W5500_Hardware_Reset();		//Ӳ����λW5500
		W5500_Initialization();		//W5500��ʼ������	
}
/*******************************************************************************
* ������  : W5500_Initialization
* ����    : W5500��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_Initialization(void)
{
	Load_Net_Parameters();
	W5500_Init();		//��ʼ��W5500�Ĵ�������
	//Detect_Gateway(0);	//������ط����� 
	
}


/*******************************************************************************
* ������  : Load_Net_Parameters
* ����    : װ���������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢�����ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/
extern u8 snedData[15+6+10];
void Load_Net_Parameters(void)
{
	uchar	Buffer[24],i;
	uint16_t tempbuf[24];
	uchar	ParameterDatas[27];
	FLASH_Unlock();//����
	MainCode=(*(vu16*) 0x8061000);
	FLASH_Lock();//����	
	  

	RdBytesFromAT24C64(TcpCardDatas_Addr,Buffer,24);
	if (Buffer[0]!=0xa0 || BytesCheckSum(Buffer,24))
	{
//		memcpy(ParameterDatas,DefaultNetDatas,14);
//		memcpy(ParameterDatas+14,DefaultNetDatas,4);
//		memcpy(ParameterDatas+18,DefaultNetDatas+14,8);
		for(i=0;i<11;i++)
		{
			tempbuf[i] = FLASH_ReadHalfWord(TcpCardDatasFlashAdd +i*2);
			Buffer[i*2+1] = tempbuf[i]>>8 ;
			Buffer[i*2+2] = tempbuf[i] ;
			memcpy(ParameterDatas,Buffer+1,14);
			memcpy(ParameterDatas+14,Buffer+1,4);
			memcpy(ParameterDatas+18,Buffer+15,8);
			
			//memset(Bufferb,0,1);
		}
	}
	else
	{
		memcpy(ParameterDatas,Buffer+1,14);
		memcpy(ParameterDatas+14,Buffer+1,4);
		memcpy(ParameterDatas+18,Buffer+15,8);
	}
	
	Gateway_IP[0] = ParameterDatas[6];//�������ز���
	Gateway_IP[1] = ParameterDatas[7];
	Gateway_IP[2] = ParameterDatas[8];
	Gateway_IP[3] = ParameterDatas[9];

	Sub_Mask[0]=ParameterDatas[10];//������������
	Sub_Mask[1]=ParameterDatas[11];
	Sub_Mask[2]=ParameterDatas[12];
	Sub_Mask[3]=ParameterDatas[13];

	Phy_Addr[0]=0xF2;//���������ַ
	Phy_Addr[1]=0x78;
	Phy_Addr[2]=0xd2;
	Phy_Addr[3]=0x34;
	Phy_Addr[4]=MainCode>>8;;
	Phy_Addr[5]=MainCode;
//	memcpy(Phy_Addr,snedData+1,6);
//	if(Phy_Addr[0] % 2 != 0) Phy_Addr[0] = 0x02;

	IP_Addr[0]=ParameterDatas[0];//���ر���IP��ַ
	IP_Addr[1]=ParameterDatas[1];
	IP_Addr[2]=ParameterDatas[2];
	IP_Addr[3]=ParameterDatas[3];

	S0_Port[0] = ParameterDatas[5];//���ض˿�0�Ķ˿ں�9000 
	S0_Port[1] = ParameterDatas[4];

	S1_Port[0] = ParameterDatas[5];//���ض˿�1�Ķ˿ں�9000 
	S1_Port[1] = ParameterDatas[4];
	
	S2_Port[0] = ParameterDatas[5];//���ض˿�2�Ķ˿ں�9000 
	S2_Port[1] = ParameterDatas[4];



	UDP_DIPR[0] = ParameterDatas[20];	//UDP(�㲥)ģʽ,Ŀ������IP��ַ
	UDP_DIPR[1] = ParameterDatas[21];
	UDP_DIPR[2] = ParameterDatas[22];
	UDP_DIPR[3] = ParameterDatas[23];
	
	
	svraddrip[0] = ParameterDatas[20];	//TCP�ͻ���ģʽ,Ŀ������IP��ַ
	svraddrip[1] = ParameterDatas[21];
	svraddrip[2] = ParameterDatas[22];
	svraddrip[3] = ParameterDatas[23];
	
	memcpy(S1_DIP,svraddrip,4);
	
//	S1_DIP[0] = 192;	//TCP�ͻ���ģʽ,Ŀ������IP��ַ
//	S1_DIP[1] = 168;
//	S1_DIP[2] = 10;
//	S1_DIP[3] = 43;
	
	
//
	UDP_DPORT[0] = ParameterDatas[25];	//UDP(�㲥)ģʽ,Ŀ�������˿ں�
	UDP_DPORT[1] = ParameterDatas[24];
	
	svraddrport = UDP_DPORT[0]*256 +UDP_DPORT[1];

	S0_Mode=UDP_MODE;//���ض˿�0�Ĺ���ģʽ,UDPģʽ
	S1_Mode=TCP_CLIENT;//���ض˿�1�Ĺ���ģʽ,TCP�ͻ���ģʽ
	S2_Mode=TCP_SERVER;//���ض˿�1�Ĺ���ģʽ,TCP������ģʽ
}


/*******************************************************************************
* ������  : W5500_Socket_Set
* ����    : W5500�˿ڳ�ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : �ֱ�����4���˿�,���ݶ˿ڹ���ģʽ,���˿�����TCP��������TCP�ͻ��˻�UDPģʽ.
*			�Ӷ˿�״̬�ֽ�Socket_State�����ж϶˿ڵĹ������
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State==0)//�˿�0��ʼ������
	{
			if(Socket_UDP(0)==TRUE)
				S0_State=S_INIT|S_CONN;
			else
				S0_State=0;
	}
}
//ͨѶ���ݽ���
//st_data--��Ҫ����������
unsigned char	TCP_Analyse(void)
{
//	unsigned int iii;
//	
//	if(SerialUnion.S_DatasStruct.POsAddrCode != (uchar)MainCode)//վ��Ŵ���
//	{
//		return 0xff;
//	}
//	TCP_ReceiveSub();
}


/*******************************************************************************
* ������  : Process_Socket_Data
* ����    : W5500���ղ����ͽ��յ�������
* ����    : s:�˿ں�
* ���    : ��
* ����ֵ  : ��
* ˵��    : �������ȵ���S_rx_process()��W5500�Ķ˿ڽ������ݻ�������ȡ����,
*			Ȼ�󽫶�ȡ�����ݴ�Rx_Buffer������Temp_Buffer���������д���
*			������ϣ������ݴ�Temp_Buffer������Tx_Buffer������������S_tx_process()
*			�������ݡ�
*******************************************************************************/
void Process_Socket_Data(SOCKET s)
{
	unsigned short size;
	size=Read_SOCK_Data_Buffer(s, Rx_Buffer);
//	UDP_DIPR[0] = Rx_Buffer[0];
//	UDP_DIPR[1] = Rx_Buffer[1];
//	UDP_DIPR[2] = Rx_Buffer[2];
//	UDP_DIPR[3] = Rx_Buffer[3];

	UDP1_DPORT[0] = Rx_Buffer[4];
	UDP1_DPORT[1] = Rx_Buffer[5];
	if(size<150&&size>8)
	{
		memcpy(SerialUnion.S_DatasBuffer, Rx_Buffer+8, size-8);	
		TCP_Analyse();
	}		
	bitSerial_ReceiveEnd=1;
	SerialReceiveLen = size -8;
	
}






 
