#include "stm32f10x_tim.h"
#include "TIME.H"
#include "os_cpu.h"
#include "ucos_ii.h"
#include "hw_config.h"
#include "externvariable.h"
#include "misc.h"
#include "NVIC.H"
#include "char.h"
#include "Disp_1629DFile.h"
#include "CalucationFile.h"

ulong global_key = 0;
uchar		T3_TimeCount=0;
u32 Led_Open_time = 0;
uchar		DelayReadCardTimeCount=0;
unsigned char   	ReadKey_Pause; 
uchar		bitHaveKey=0;
uchar		bitHaveReleaseKey=1;
u32 temp = 0;
/**************************************************************************************

**************************************************************************************/

/*******************************************************************************
*函数功能：										
*函数描述：
***********************************************************************************/
void DK_TIMER_Init(u8 TIMERx)
{
   TIM_OCInitTypeDef        TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef  	TIM_BaseInitStructure;	 //通用定时计数器初始化TIMx的时间基数单位
	NVIC_InitTypeDef NVIC_InitStructure;
	
	switch(TIMERx)
	{	   
		case 1:	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  //TIM1时钟使能
		//TIM1用于产生PWM信号
		TIM_BaseInitStructure.TIM_Period = 0xff;  //PWM frequency : 281.250KHz
		TIM_BaseInitStructure.TIM_Prescaler = 0;  //TIM1CLK = 72 MHz
		TIM_BaseInitStructure.TIM_ClockDivision = 0;  //设置了时钟分割，无分割72M
		TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   // TIM向上计数模
		TIM_TimeBaseInit(TIM1,&TIM_BaseInitStructure);
		//TIM_ARRPreloadConfig(TIM1,ENABLE);
		//TIM1的1通道用于PWM1模式
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  	TIM_OCInitStructure.TIM_Pulse = 0x7F;  /* Duty cycle: 50%*/  
  	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
  	//TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
		TIM_Cmd(TIM1, ENABLE);
		TIM_CtrlPWMOutputs(TIM1,ENABLE);
		break;	  
		case 2:	
		TIM_BaseInitStructure.TIM_Period = 100; //周期值0x0000和0xFFFF之间  从初值加到这个值就产生中断源 35ms产生一次中断
		TIM_BaseInitStructure.TIM_Prescaler = 719;  //预分频值 0x0000和0xFFFF之间,对时钟分频  
	  TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置了时钟分割，无分割72M
		TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   // TIM向上计数模
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //TIM2时钟使能
		TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure);	
		TIM_SetCounter(TIM2, 0);
		
			//中断优先级NVIC设置
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
		TIM_Cmd(TIM2, ENABLE);	//TIM2总开关：开启
		break;
		case 3:								 //定时4秒中断喂狗
		TIM_BaseInitStructure.TIM_Period = 10; //周期值0x0000和0xFFFF之间  从初值加到这个值就产生中断源，1ms产生一次中断
		TIM_BaseInitStructure.TIM_Prescaler = 7199;  //预分频值 0x0000和0xFFFF之间,对时钟分频    
		TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置了时钟分割，无分割72M
		TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   // TIM向上计数模
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //TIM3时钟使能
		TIM_TimeBaseInit(TIM3, &TIM_BaseInitStructure);	
		
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 	
		TIM_SetCounter(TIM3, 0);				
		TIM_Cmd(TIM3, ENABLE);	//TIM3总开关：开启	
		TIM_ITConfig( TIM3,TIM_IT_Update|TIM_IT_Trigger,ENABLE);		
		break;

		case 4:	//因为APB1的分频值为2，所以定时器2~7的输入时钟都被2倍频了，相应资料可看时间树
		TIM_BaseInitStructure.TIM_Period = 500; //周期值0x0000和0xFFFF之间  从初值加到这个值就产生中断源20ms
		TIM_BaseInitStructure.TIM_Prescaler = 297;  //预分频值 0x0000和0xFFFF之间,对时钟分频    
		TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置了时钟分割，无分割
		TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   // TIM向上计数模
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  //TIM4时钟使能
		TIM_TimeBaseInit(TIM4, &TIM_BaseInitStructure);						
		TIM_Cmd(TIM4, ENABLE);	//TIM4总开关：开启
		break;
	}	
}

void DK_TIM4_PWM_Init(unsigned char chan, unsigned int pul)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	switch(chan)
	{
		case 2:
		GPIO_PinRemapConfig(GPIO_Remap_TIM4 , ENABLE);
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
		TIM_OCInitStructure.TIM_Pulse = pul;                   //设置待装入捕获比较寄存器的脉冲值，初始的占空比
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
		TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIMx在CCR2上的预装载寄存器	
		TIM_ARRPreloadConfig(TIM4, ENABLE); //使能TIMx在ARR上的预装载寄存器 
		break;
	}
}

void DK_TIM_OPen_INT(u8 TIMERx)
{
	switch(TIMERx)
	{
		case 1:
			
		break;
		case 2:
		TIM_ClearFlag(TIM2, TIM_FLAG_Update); 	//清中断，以免一启用中断后立即产生中断				
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //使能中断源	
		break;
		
		case 3:
		TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	//清中断，以免一启用中断后立即产生中断				
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //使能中断源	
		break;
		
		case 4:
		TIM_ClearFlag(TIM4, TIM_FLAG_Update); 	//清中断，以免一启用中断后立即产生中断				
		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //使能中断源	
		break;
	}
}

//定时器3打开的函数
void TIM3_OPen_INT(void)
{
	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	//清中断，以免一启用中断后立即产生中断				
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //使能中断源	
}
//定时器3的关闭函数
void TIM3_Close_INT(void)
{
  	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	//清中断，以免一启用中断后立即产生中断				
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); //使能中断源	
}
/*******************************************************************************
*定时器3变量
***********************************************************************************/

//u8 TIME3_Taketime=0;
//u8 TIME3_Retry=0;
//u8 Time3_countnum=0;
/*******************************************************************************
*函数功能：										
*函数描述：	定时器3中断服务函数
***********************************************************************************/
void  DK_TIM3_INT_SER(void)
{
	
}

/*******************************************************************************
*定时器4变量
***********************************************************************************/

u8 TIME4_Taketime=0;
u8 TIME4_Retry=0;
u8 Time4_countnum=0;
/*******************************************************************************
*函数功能：										
*函数描述：	定时器4中断服务函数
***********************************************************************************/
void  DK_TIM4_INT_SER(void)
{
	Time4_countnum++;										    
	if(Time4_countnum>=TIME4_Taketime)
	{
	    Time4_countnum=0;
		TIME4_Retry=1;
		TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE); //关闭中断源
	}
}
void TIME4_RELOAD(u8 times)//定时器4重载
{
    TIME4_Retry=0;
	Time4_countnum=0;

	TIME4_Taketime=times;
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //使能中断源	
	TIM_SetCounter(TIM4, 0);	
}



void TIM3_IRQHandler(void)
{
	OSIntEnter(); 
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		T3_TimeCount++;
		Led_Open_time++;
		TimerCount++;
		//蜂鸣器计时
		if (DelayReadCardTimeCount)
			DelayReadCardTimeCount--;
			BeepDelayTimeCount++;
		
		if(ResetW5500Num)
			ResetW5500Num--;
		if(BeepTimes)
		{
			
			if(BeepDelayTimeCount>110)
			{
				PDout(7)=0;
			}
			else
			{
				PDout(7)=1;
			}
			if(BeepDelayTimeCount>=220)
			{
				BeepDelayTimeCount=0;
				BeepTimes--;
			}
			
		}

		//////////////////键盘///////////////
		if(Forbidden)
		ReadKey_Pause=1;

		if((ReadKey_Pause || !(T3_TimeCount%10)) && !Forbidden)  
		{   
			ReakKeySub();
			temp=ChgKeyStringToUlong(keybuff,4);
		global_key = temp;
			if(temp)//有键按下
			{	
				if (bitHaveReleaseKey && !bitHaveKey )
				{
					bitHaveReleaseKey=0;
					bitHaveKey=1;
					memset(keybuff,0,4);
					KeyValue=temp;					
				}
			}
			else
				bitHaveReleaseKey=1;   
        		ReadKey_Pause=0;
		}	
		
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
	OSIntExit(); 
}
/******************* (C) COPYRIGHT 2012*****END OF FILE****************/



