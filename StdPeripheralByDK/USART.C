#include "stm32f10x_usart.h"
#include "GPIO.H"
#include "USART.H"
#include  "os_cpu.h"
#include  "app_cfg.h"
#include  "ucos_ii.h"
#include  "externvariable.h"
#include "string.h"

uchar	SerAPDU_Analyse(uchar );  
u8 Uart_Rec_Data[25],Uart_Rec_Len=0;
u8 Uart_Snd_Data[25],Uart_Snd_Len=0;
/**************************************************************************************

**************************************************************************************/

/*******************************************************************************
*函数功能：串口初始化
*函数描述：
***********************************************************************************/
void DK_USART_Init(unsigned long Baud,unsigned char USARTx)
{								 
	USART_InitTypeDef    USART_InitStructure; 
	USART_InitStructure.USART_BaudRate =Baud;  //bps,如9600  19200
	#ifndef _USART_Use
	#define _USART_Use
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8位数据
		USART_InitStructure.USART_StopBits = USART_StopBits_1;	  //1位停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;	//	无奇偶校验
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	   //无硬件控制流
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	 //发送接收使能
	#endif
	if(USARTx==1)
	{											  
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
		DK_GPIO_Init(PA,GPIO_Pin_9,AF_PP);	//复用推挽输出  TX
		DK_GPIO_Init(PA,GPIO_Pin_10,IN_FLOATING);	//  RX
	  USART_Init(USART1, &USART_InitStructure);
		USART_Cmd(USART1, ENABLE);	  
	}
	if(USARTx==2)
	{
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
		DK_GPIO_Init(PA,GPIO_Pin_2,AF_PP);	//复用推挽输出  TX
		DK_GPIO_Init(PA,GPIO_Pin_3,IN_FLOATING);	//  RX
		USART_Init(USART2, &USART_InitStructure);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);//清楚中断位
		USART_Cmd(USART2, ENABLE);	  
	}
	if(USARTx==3)
	{
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
		GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);  //端口管脚重映射
		DK_GPIO_Init(PC,GPIO_Pin_10,AF_PP);	//复用推挽输出  TX
		DK_GPIO_Init(PC,GPIO_Pin_11,IPU);	//  RX
		USART_Init(USART3, &USART_InitStructure);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);//清楚中断位
		USART_Cmd(USART3, ENABLE);					  
	}
 	
}

void PsamUSART_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;  

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE); 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 		//USART3 TX
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;    		 		//复用推挽输出
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 		//A端口 

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART3 RX
  	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;	//复用开漏输入	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 		//A端口 

  	USART_InitStructure.USART_BaudRate = 9600;						//速率9600bps
  	USART_InitStructure.USART_WordLength = USART_WordLength_9b;		//数据位8位
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
  	USART_InitStructure.USART_Parity =USART_Parity_Even;//USART_Parity_Even;//USART_Parity_No;//USART_Parity_Odd;				//无校验位
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式

  	USART_Init(USART1, &USART_InitStructure);	 
  	USART_Cmd(USART1, ENABLE);
		DK_NVIC_IRQ_Set(USART1_IRQn,0,0,DISABLE);

}
/*******************************************************************************
*函数功能：打开中断
*函数描述：
***********************************************************************************/
void DK_USART_OPen_INT(unsigned char USARTx)
{
	switch(USARTx)
	{
	 	case 1:	
			USART_ClearITPendingBit(USART1,USART_IT_RXNE);
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		break;
		
		case 2:	
			USART_ClearITPendingBit(USART2,USART_IT_RXNE);
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		break;

		case 3:	
			USART_ClearITPendingBit(USART3,USART_IT_RXNE);
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		break;
	} 
}
/*******************************************************************************
*函数功能：关闭中断
*函数描述：
***********************************************************************************/
void DK_USART_Close_INT(unsigned char USARTx)
{
	switch(USARTx)
	{
	 	case 1:	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
		break;
		
		case 2:	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
		break;

		case 3:	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
		break;
	} 
}
/*******************************************************************************
*函数功能：查询法发送一个字节
*函数描述：
***********************************************************************************/
void DK_USART_Send_Char(unsigned char USARTx,unsigned char data)
{
	switch(USARTx)
	{
	 	case 1:	USART_SendData(USART1,data); 
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);     //等待发送完毕
		break;
		
		case 2:	USART_SendData(USART2,data);  
				while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		break;

		case 3:	USART_SendData(USART3,data);  
				while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		break;
	} 
}
/*******************************************************************************
*函数功能：USARTx查询方式接收数据
*函数描述：
*******************************************************************************/
unsigned char DK_USART_WAIT_RXBYTE(unsigned char USARTx)
{
    u8 i;
	switch(USARTx)
	{
		case 1:		 
	    	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);   //等待接收数据  
			i=USART_ReceiveData(USART1);
		break;
		case 2:				   
	    	while(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)==RESET);
			i=USART_ReceiveData(USART2);
		break;
		case 3:				
	    	while(USART_GetFlagStatus(USART3,USART_FLAG_RXNE)==RESET);
			i=USART_ReceiveData(USART3);
		break;
	}
	return i;
} 

/*******************************************************************************
*函数功能：USART输出一个字符串数据
*函数描述：如果buffer指向一个数组，则数组的最后一位必须为0
*******************************************************************************/
void DK_USART_PUTS(unsigned char USARTx,unsigned char *buffer)
{	    
	while(*buffer)
	{	
		DK_USART_Send_Char(USARTx,*buffer);
		buffer++;
	}
} 
void DK_USART_PUTS_Const(unsigned char USARTx,const unsigned char *buffer)
{							 
    while(*buffer)
	{	
		DK_USART_Send_Char(USARTx,*buffer);
		buffer++;
	}
}
void DK_USART_PUTS_Arry(unsigned char USARTx,unsigned char *buffer,unsigned int len)
{			   
   while(len--)
	{
	    DK_USART_Send_Char(USARTx,*buffer);
		buffer++;
	}	   
} 

/*******************************************************************************
*函数功能：USART1标准输出函数，用于输出某变量的值
*函数描述：
*******************************************************************************/
void DK_USART1_Printf(unsigned char *p,unsigned char *dat)
{
    u8 shu[4];
    while((*p)!= 0)
	{
	    if((*p)!='%')
		{
		    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
		    USART_SendData(USART1,*p);
			p++;
		}
		else
		{
		    p++;
			switch(*p)
			{
			    case 'c':    //字符输出
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
			    USART_SendData(USART1,*dat);
				break;
				
				case 's':    //字符串输出
				while((*dat)!=0)
				{
				    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
				    USART_SendData(USART1,*dat);
					dat++;
				}
				break;
				
				case 'd':
				shu[0]=((*dat)/1000)|0x30;
				shu[1]=((*dat)%1000/100)|0x30;
				shu[2]=((*dat)%100/10)|0x30;
				shu[3]=((*dat)%10)|0x30;
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
				USART_SendData(USART1,shu[0]);
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
				USART_SendData(USART1,shu[1]);
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
				USART_SendData(USART1,shu[2]);
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
				USART_SendData(USART1,shu[3]);
				break;
			}
			p++;
		}
	}
}

//st_data--需要解析的数据
uchar	SerAPDU_Analyse(uchar st_data)
{
	uchar	j;
	
	
    if (!SerialReCase)
    {
        if (st_data==0x02)
        {
          SerialReCase=1;
          Uart_Rec_Len=0;	
					Uart_Rec_Data[Uart_Rec_Len++]=st_data;
        }
    }
    else if (SerialReCase==1)
    {
			if(st_data==0x03)	
				SerialReCase=2;
			Uart_Rec_Data[Uart_Rec_Len++]=st_data;
    } //
    else if (SerialReCase==2)
    {
			SerialReCase = 0;
			Uart_Rec_Data[Uart_Rec_Len++]=st_data;
			bitSerial_ReceiveEnd=1;
			j=Uart_Rec_Len;
			memcpy(Uart_Snd_Data,Uart_Rec_Data,10);
			//OSSemPost(HaveReceiveData_Semp);  //发送信号量 用任务处理
    }
    return  0;
}

//串口1中断服务程序
void USART1_IRQHandler(void)                	
{
	u8 st_data;
	OS_CPU_SR  cpu_sr;
  	
OSIntEnter(); 

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		st_data = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		if (bitSerial_ReceiveEnd)
			return;		
		//RS485通讯
		SerAPDU_Analyse(st_data); //串口数据分析			 
  } 
	  OSIntExit(); 
}

u8 dataRecFromEWM[100];
u8 LenOfEWM = 0;;

u8 dataOfRecEWMSJ[20];

u8 isHaveEWM = 0;
//串口2中断服务程序
void USART2_IRQHandler(void)                	
{
	u8 st_data;
	OS_CPU_SR  cpu_sr;
	u16 j = 0,i = 0;
	int addr = 0;
	int numdd = 0;

	//OSIntEnter(); 

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		st_data = USART_ReceiveData(USART2);//(USART1->DR);	//读取接收到的数据
		if(LenOfEWM >= 99) LenOfEWM = 0;	
		dataRecFromEWM[LenOfEWM++] = st_data;
		if(st_data == '\n')
		{
			for(i=0;i<99-13;i++)
			{
				if(memcmp(dataRecFromEWM+i,"security_code",13) == 0)
				{
					addr = i;
					break;
				}
			}
			if(i == 99-13) return;
			addr += 13;
			addr+= 3;
			if( addr != -1)
			{
				//if(LenOfEWM == 19)
				{
				//	memcpy(dataRecFromEWM,dataRecFromEWM+addr,16);
					
					for(j=addr;j<28+addr;j++)
					{
						if(dataRecFromEWM[j] >= '0' && dataRecFromEWM[j] <= '9')
						{
							dataRecFromEWM[j] -= '0';
						}else if(dataRecFromEWM[j] >= 'A' && dataRecFromEWM[j] <= 'F')
						{
							dataRecFromEWM[j] -= 'A';
							dataRecFromEWM[j] += 10;
						}
						else if(dataRecFromEWM[j] >= 'a' && dataRecFromEWM[j] <= 'f')
						{
							dataRecFromEWM[j] -= 'a';
							dataRecFromEWM[j] += 10;
						}
						else 
						{
							if(dataRecFromEWM[j] == '"')
							{
								numdd = j - addr;
								goto goon;
							}
							LenOfEWM = 0;
							return;
						}
					}
					goon:
					i = 0;
					for(j=0;j<numdd;j++)
					{
						dataRecFromEWM[j] = dataRecFromEWM[addr+i] << 4;
						i++;
						dataRecFromEWM[j] |= dataRecFromEWM[addr+i];
						i++;
					}
					
					memset(dataOfRecEWMSJ,0,20);
					memcpy(dataOfRecEWMSJ+(14-numdd/2),dataRecFromEWM,numdd/2);
					isHaveEWM = 1;
					
				}
			}
			LenOfEWM =0;
		}
	} 
	//OSIntExit(); 
}

void	SerialSendChar (uchar aa)
{
	DK_USART_Send_Char(1,aa);	
}
void	SerialSendArray (uchar *data,uchar len)
{
	while(len--)
	{
		DK_USART_Send_Char(1,*data++);	
	}
}

void	Init_Serial(void)
{
	SerialReceiveLen=0;
//	bitSerial_ReceiveEnd=0;
	//SerialReCase=0;
	SerialReceiveTimeCount=0;
} 
/******************* (C)*****END OF FILE****************/

