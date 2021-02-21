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
*�������ܣ����ڳ�ʼ��
*����������
***********************************************************************************/
void DK_USART_Init(unsigned long Baud,unsigned char USARTx)
{								 
	USART_InitTypeDef    USART_InitStructure; 
	USART_InitStructure.USART_BaudRate =Baud;  //bps,��9600  19200
	#ifndef _USART_Use
	#define _USART_Use
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8λ����
		USART_InitStructure.USART_StopBits = USART_StopBits_1;	  //1λֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;	//	����żУ��
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	   //��Ӳ��������
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	 //���ͽ���ʹ��
	#endif
	if(USARTx==1)
	{											  
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
		DK_GPIO_Init(PA,GPIO_Pin_9,AF_PP);	//�����������  TX
		DK_GPIO_Init(PA,GPIO_Pin_10,IN_FLOATING);	//  RX
	  USART_Init(USART1, &USART_InitStructure);
		USART_Cmd(USART1, ENABLE);	  
	}
	if(USARTx==2)
	{
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
		DK_GPIO_Init(PA,GPIO_Pin_2,AF_PP);	//�����������  TX
		DK_GPIO_Init(PA,GPIO_Pin_3,IN_FLOATING);	//  RX
		USART_Init(USART2, &USART_InitStructure);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);//����ж�λ
		USART_Cmd(USART2, ENABLE);	  
	}
	if(USARTx==3)
	{
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
		GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);  //�˿ڹܽ���ӳ��
		DK_GPIO_Init(PC,GPIO_Pin_10,AF_PP);	//�����������  TX
		DK_GPIO_Init(PC,GPIO_Pin_11,IPU);	//  RX
		USART_Init(USART3, &USART_InitStructure);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);//����ж�λ
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
  	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;    		 		//�����������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 		//A�˿� 

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART3 RX
  	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;	//���ÿ�©����	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 		//A�˿� 

  	USART_InitStructure.USART_BaudRate = 9600;						//����9600bps
  	USART_InitStructure.USART_WordLength = USART_WordLength_9b;		//����λ8λ
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
  	USART_InitStructure.USART_Parity =USART_Parity_Even;//USART_Parity_Even;//USART_Parity_No;//USART_Parity_Odd;				//��У��λ
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ

  	USART_Init(USART1, &USART_InitStructure);	 
  	USART_Cmd(USART1, ENABLE);
		DK_NVIC_IRQ_Set(USART1_IRQn,0,0,DISABLE);

}
/*******************************************************************************
*�������ܣ����ж�
*����������
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
*�������ܣ��ر��ж�
*����������
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
*�������ܣ���ѯ������һ���ֽ�
*����������
***********************************************************************************/
void DK_USART_Send_Char(unsigned char USARTx,unsigned char data)
{
	switch(USARTx)
	{
	 	case 1:	USART_SendData(USART1,data); 
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);     //�ȴ��������
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
*�������ܣ�USARTx��ѯ��ʽ��������
*����������
*******************************************************************************/
unsigned char DK_USART_WAIT_RXBYTE(unsigned char USARTx)
{
    u8 i;
	switch(USARTx)
	{
		case 1:		 
	    	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);   //�ȴ���������  
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
*�������ܣ�USART���һ���ַ�������
*�������������bufferָ��һ�����飬����������һλ����Ϊ0
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
*�������ܣ�USART1��׼����������������ĳ������ֵ
*����������
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
			    case 'c':    //�ַ����
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
			    USART_SendData(USART1,*dat);
				break;
				
				case 's':    //�ַ������
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

//st_data--��Ҫ����������
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
			//OSSemPost(HaveReceiveData_Semp);  //�����ź��� ��������
    }
    return  0;
}

//����1�жϷ������
void USART1_IRQHandler(void)                	
{
	u8 st_data;
	OS_CPU_SR  cpu_sr;
  	
OSIntEnter(); 

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		st_data = USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		if (bitSerial_ReceiveEnd)
			return;		
		//RS485ͨѶ
		SerAPDU_Analyse(st_data); //�������ݷ���			 
  } 
	  OSIntExit(); 
}

u8 dataRecFromEWM[100];
u8 LenOfEWM = 0;;

u8 dataOfRecEWMSJ[20];

u8 isHaveEWM = 0;
//����2�жϷ������
void USART2_IRQHandler(void)                	
{
	u8 st_data;
	OS_CPU_SR  cpu_sr;
	u16 j = 0,i = 0;
	int addr = 0;
	int numdd = 0;

	//OSIntEnter(); 

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		st_data = USART_ReceiveData(USART2);//(USART1->DR);	//��ȡ���յ�������
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
