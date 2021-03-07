/*******************************************************************
端口0：UDP模式
端口1：TCP_CLIENT
端口2：TCP_SERVER
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
uchar TCPPacketNum = 0;//防止TCP半包发送
extern u8 svraddrip[4];
extern int svraddrport ;
unsigned short ReceivedataLen =0;
extern void	TCP_ReceiveSub(void);//接收数据处理
extern char *mystrstr(char *s1,char *s2)  ;

void InitW5500Net(void)
{
		W5500_NVIC_Configuration();	//W5500 接收引脚中断优先级设置
		SPI_Configuration();		//W5500 SPI初始化配置(STM32 SPI1)
		W5500_GPIO_Configuration();	//W5500 GPIO初始化配置			
		W5500_Hardware_Reset();		//硬件复位W5500
		W5500_Initialization();		//W5500初始货配置	
}
/*******************************************************************************
* 函数名  : W5500_Initialization
* 描述    : W5500初始货配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_Initialization(void)
{
	Load_Net_Parameters();
	W5500_Init();		//初始化W5500寄存器函数
	//Detect_Gateway(0);	//检查网关服务器 
	
}


/*******************************************************************************
* 函数名  : Load_Net_Parameters
* 描述    : 装载网络参数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
*******************************************************************************/
extern u8 snedData[15+6+10];
void Load_Net_Parameters(void)
{
	uchar	Buffer[24],i;
	uint16_t tempbuf[24];
	uchar	ParameterDatas[27];
	FLASH_Unlock();//解锁
	MainCode=(*(vu16*) 0x8061000);
	FLASH_Lock();//锁定	
	  

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
	
	Gateway_IP[0] = ParameterDatas[6];//加载网关参数
	Gateway_IP[1] = ParameterDatas[7];
	Gateway_IP[2] = ParameterDatas[8];
	Gateway_IP[3] = ParameterDatas[9];

	Sub_Mask[0]=ParameterDatas[10];//加载子网掩码
	Sub_Mask[1]=ParameterDatas[11];
	Sub_Mask[2]=ParameterDatas[12];
	Sub_Mask[3]=ParameterDatas[13];

	Phy_Addr[0]=0xF2;//加载物理地址
	Phy_Addr[1]=0x78;
	Phy_Addr[2]=0xd2;
	Phy_Addr[3]=0x34;
	Phy_Addr[4]=MainCode>>8;;
	Phy_Addr[5]=MainCode;
//	memcpy(Phy_Addr,snedData+1,6);
//	if(Phy_Addr[0] % 2 != 0) Phy_Addr[0] = 0x02;

	IP_Addr[0]=ParameterDatas[0];//加载本机IP地址
	IP_Addr[1]=ParameterDatas[1];
	IP_Addr[2]=ParameterDatas[2];
	IP_Addr[3]=ParameterDatas[3];

	S0_Port[0] = ParameterDatas[5];//加载端口0的端口号9000 
	S0_Port[1] = ParameterDatas[4];

	S1_Port[0] = ParameterDatas[5];//加载端口1的端口号9000 
	S1_Port[1] = ParameterDatas[4];
	
	S2_Port[0] = ParameterDatas[5];//加载端口2的端口号9000 
	S2_Port[1] = ParameterDatas[4];



	UDP_DIPR[0] = ParameterDatas[20];	//UDP(广播)模式,目的主机IP地址
	UDP_DIPR[1] = ParameterDatas[21];
	UDP_DIPR[2] = ParameterDatas[22];
	UDP_DIPR[3] = ParameterDatas[23];
	
	
	svraddrip[0] = ParameterDatas[20];	//TCP客户端模式,目的主机IP地址
	svraddrip[1] = ParameterDatas[21];
	svraddrip[2] = ParameterDatas[22];
	svraddrip[3] = ParameterDatas[23];
	
	memcpy(S1_DIP,svraddrip,4);
	
//	S1_DIP[0] = 192;	//TCP客户端模式,目的主机IP地址
//	S1_DIP[1] = 168;
//	S1_DIP[2] = 10;
//	S1_DIP[3] = 43;
	
	
//
	UDP_DPORT[0] = ParameterDatas[25];	//UDP(广播)模式,目的主机端口号
	UDP_DPORT[1] = ParameterDatas[24];
	
	svraddrport = UDP_DPORT[0]*256 +UDP_DPORT[1];

	S0_Mode=UDP_MODE;//加载端口0的工作模式,UDP模式
	S1_Mode=TCP_CLIENT;//加载端口1的工作模式,TCP客户端模式
	S2_Mode=TCP_SERVER;//加载端口1的工作模式,TCP服务器模式
}


/*******************************************************************************
* 函数名  : W5500_Socket_Set
* 描述    : W5500端口初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 分别设置4个端口,根据端口工作模式,将端口置于TCP服务器、TCP客户端或UDP模式.
*			从端口状态字节Socket_State可以判断端口的工作情况
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State==0)//端口0初始化配置
	{
			if(Socket_UDP(0)==TRUE)
				S0_State=S_INIT|S_CONN;
			else
				S0_State=0;
	}
}
//通讯数据解析
//st_data--需要解析的数据
unsigned char	TCP_Analyse(void)
{
//	unsigned int iii;
//	
//	if(SerialUnion.S_DatasStruct.POsAddrCode != (uchar)MainCode)//站点号错误
//	{
//		return 0xff;
//	}
//	TCP_ReceiveSub();
}


/*******************************************************************************
* 函数名  : Process_Socket_Data
* 描述    : W5500接收并发送接收到的数据
* 输入    : s:端口号
* 输出    : 无
* 返回值  : 无
* 说明    : 本过程先调用S_rx_process()从W5500的端口接收数据缓冲区读取数据,
*			然后将读取的数据从Rx_Buffer拷贝到Temp_Buffer缓冲区进行处理。
*			处理完毕，将数据从Temp_Buffer拷贝到Tx_Buffer缓冲区。调用S_tx_process()
*			发送数据。
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






 
