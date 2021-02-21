#include  "cpu.h"
#include  "os_cpu.h"
#include  "app_cfg.h"
#include  "ucos_ii.h"
#include  "include.h"

void PsamPowerDown(u8 Sel);
u8 PsamReceive(u8,u8 *RevBuf,u16 RecLen,u16 Timeout_Ms);

void PsamGpioInt(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   //PSAM
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
		GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;	         		 		//
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    		 		//推挽输出
  	GPIO_Init(GPIOE, &GPIO_InitStructure);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14;	         		 		
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    		 	
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : Delay_us
* Description    : 延时函数
* Input 1        : Enable
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_us(u16 us)
{
	u16 x,y;
	for(x=0;x<5;x++)
		for(y=0;y<us;y++);
}
/*******************************************************************************
* Function Name  : TimerStart
* Description    : 定时开始
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TimerStart(void)
{
	TimerCount=0;
}
/*******************************************************************************
* Function Name  : IsEndTimer
* Description    : 定时函数
* Input 1        : MS
* Output         : None
* Return         : None
*******************************************************************************/
u8 IsEndTimer(u16 Ms)
{
	if(TimerCount<Ms)
		return 0;
	else
		return 1;
}

/*******************************************************************************
* Function Name  : PsamReset
* Description    : PSAM卡复位
* Input 1        : 选择要复位的PSAM卡 1 or 2
* Output         : None
* Return         : None
*******************************************************************************/
void PsamReset(void)
{

	GPIO_ResetBits(GPIOE, GPIO_Pin_2);
	Delay_us(20);
	GPIO_SetBits(GPIOE, GPIO_Pin_2);
}

/*******************************************************************************
* Function Name  : PsamSend
* Description    : PSAM卡发送数据
* Input 3        : Sel：1 or 2;
* Output         : None
* Return         : None
*******************************************************************************/
void PsamSend(u8 Sel,u8 *SendBuf,u16 SendLen)
{
	u16 i;
	//PsamControl(Sel,0);
	Delay_us(1);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
	for(i=0;i<SendLen;i++)
	{
		USART_SendData(USART1,SendBuf[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
	}
	
	//USART_PUTS_Arry(1,SendBuf,SendLen);
}
/*******************************************************************************
* Function Name  : PsamReceive
* Description    : PSAM接收函数
* Input 3        : Enable
* Output         : None
* Return         : None
*******************************************************************************/
u8 PsamReceive(u8 Sel,u8 *RevBuf,u16 RecLen,u16 Timeout_Ms)
{
//	u16  RevCount=0;
	u16  timeout;
	PsamRcvBufCount = 0;
//	PsamControl(Sel,1);
	if(Timeout_Ms<10)
		timeout=1;
	else
		timeout=Timeout_Ms/10;
	TimerStart();
	
	while(1)
	{
		if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
		{
			RevBuf[PsamRcvBufCount++]=USART_ReceiveData(USART1);  
			if(PsamRcvBufCount==RecLen)
			{
				return PsamRcvBufCount;
			}				
		}
		if(IsEndTimer(timeout))
		{
			return PsamRcvBufCount;
		} 
			
	}
}


/*****************************************************************************
* FUNCTION
*  WriteLog
* DESCRIPTION
*  write the log in file
*	NOTE:  
* PARAMETERS
*	Priority_level		IN		log level  for examle: fatal, error, debug...
*	Fmt				IN		string 

* RETURNS
*  vod
*****************************************************************************/
void WriteLog(u8 *ptr,u16 Len)
{
	u16 i;
	u16 SendLen;
	if(Len==0)
		SendLen=strlen(ptr);
	else
		SendLen=Len;
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
	for(i=0;i<SendLen;i++)
	{
		USART_SendData(USART1,ptr[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);	
	}
	return;
}

void msleep(u16 ms)
{
	TimerStart();
	while(1)
	{
		if(IsEndTimer(ms/10))
		{
			return ;	
		}
	}	
}


