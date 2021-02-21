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
*�������ܣ�										
*����������
***********************************************************************************/
void DK_TIMER_Init(u8 TIMERx)
{
   TIM_OCInitTypeDef        TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef  	TIM_BaseInitStructure;	 //ͨ�ö�ʱ��������ʼ��TIMx��ʱ�������λ
	NVIC_InitTypeDef NVIC_InitStructure;
	
	switch(TIMERx)
	{	   
		case 1:	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  //TIM1ʱ��ʹ��
		//TIM1���ڲ���PWM�ź�
		TIM_BaseInitStructure.TIM_Period = 0xff;  //PWM frequency : 281.250KHz
		TIM_BaseInitStructure.TIM_Prescaler = 0;  //TIM1CLK = 72 MHz
		TIM_BaseInitStructure.TIM_ClockDivision = 0;  //������ʱ�ӷָ�޷ָ�72M
		TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   // TIM���ϼ���ģ
		TIM_TimeBaseInit(TIM1,&TIM_BaseInitStructure);
		//TIM_ARRPreloadConfig(TIM1,ENABLE);
		//TIM1��1ͨ������PWM1ģʽ
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
		TIM_BaseInitStructure.TIM_Period = 100; //����ֵ0x0000��0xFFFF֮��  �ӳ�ֵ�ӵ����ֵ�Ͳ����ж�Դ 35ms����һ���ж�
		TIM_BaseInitStructure.TIM_Prescaler = 719;  //Ԥ��Ƶֵ 0x0000��0xFFFF֮��,��ʱ�ӷ�Ƶ  
	  TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //������ʱ�ӷָ�޷ָ�72M
		TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   // TIM���ϼ���ģ
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //TIM2ʱ��ʹ��
		TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure);	
		TIM_SetCounter(TIM2, 0);
		
			//�ж����ȼ�NVIC����
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
		TIM_Cmd(TIM2, ENABLE);	//TIM2�ܿ��أ�����
		break;
		case 3:								 //��ʱ4���ж�ι��
		TIM_BaseInitStructure.TIM_Period = 10; //����ֵ0x0000��0xFFFF֮��  �ӳ�ֵ�ӵ����ֵ�Ͳ����ж�Դ��1ms����һ���ж�
		TIM_BaseInitStructure.TIM_Prescaler = 7199;  //Ԥ��Ƶֵ 0x0000��0xFFFF֮��,��ʱ�ӷ�Ƶ    
		TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //������ʱ�ӷָ�޷ָ�72M
		TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   // TIM���ϼ���ģ
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //TIM3ʱ��ʹ��
		TIM_TimeBaseInit(TIM3, &TIM_BaseInitStructure);	
		
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
		NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 	
		TIM_SetCounter(TIM3, 0);				
		TIM_Cmd(TIM3, ENABLE);	//TIM3�ܿ��أ�����	
		TIM_ITConfig( TIM3,TIM_IT_Update|TIM_IT_Trigger,ENABLE);		
		break;

		case 4:	//��ΪAPB1�ķ�ƵֵΪ2�����Զ�ʱ��2~7������ʱ�Ӷ���2��Ƶ�ˣ���Ӧ���Ͽɿ�ʱ����
		TIM_BaseInitStructure.TIM_Period = 500; //����ֵ0x0000��0xFFFF֮��  �ӳ�ֵ�ӵ����ֵ�Ͳ����ж�Դ20ms
		TIM_BaseInitStructure.TIM_Prescaler = 297;  //Ԥ��Ƶֵ 0x0000��0xFFFF֮��,��ʱ�ӷ�Ƶ    
		TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //������ʱ�ӷָ�޷ָ�
		TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   // TIM���ϼ���ģ
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  //TIM4ʱ��ʹ��
		TIM_TimeBaseInit(TIM4, &TIM_BaseInitStructure);						
		TIM_Cmd(TIM4, ENABLE);	//TIM4�ܿ��أ�����
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
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM������ȵ���ģʽ2
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
		TIM_OCInitStructure.TIM_Pulse = pul;                   //���ô�װ�벶��ȽϼĴ���������ֵ����ʼ��ռ�ձ�
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
		TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIMx��CCR2�ϵ�Ԥװ�ؼĴ���	
		TIM_ARRPreloadConfig(TIM4, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ��� 
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
		TIM_ClearFlag(TIM2, TIM_FLAG_Update); 	//���жϣ�����һ�����жϺ����������ж�				
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //ʹ���ж�Դ	
		break;
		
		case 3:
		TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	//���жϣ�����һ�����жϺ����������ж�				
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //ʹ���ж�Դ	
		break;
		
		case 4:
		TIM_ClearFlag(TIM4, TIM_FLAG_Update); 	//���жϣ�����һ�����жϺ����������ж�				
		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //ʹ���ж�Դ	
		break;
	}
}

//��ʱ��3�򿪵ĺ���
void TIM3_OPen_INT(void)
{
	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	//���жϣ�����һ�����жϺ����������ж�				
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //ʹ���ж�Դ	
}
//��ʱ��3�Ĺرպ���
void TIM3_Close_INT(void)
{
  	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	//���жϣ�����һ�����жϺ����������ж�				
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); //ʹ���ж�Դ	
}
/*******************************************************************************
*��ʱ��3����
***********************************************************************************/

//u8 TIME3_Taketime=0;
//u8 TIME3_Retry=0;
//u8 Time3_countnum=0;
/*******************************************************************************
*�������ܣ�										
*����������	��ʱ��3�жϷ�����
***********************************************************************************/
void  DK_TIM3_INT_SER(void)
{
	
}

/*******************************************************************************
*��ʱ��4����
***********************************************************************************/

u8 TIME4_Taketime=0;
u8 TIME4_Retry=0;
u8 Time4_countnum=0;
/*******************************************************************************
*�������ܣ�										
*����������	��ʱ��4�жϷ�����
***********************************************************************************/
void  DK_TIM4_INT_SER(void)
{
	Time4_countnum++;										    
	if(Time4_countnum>=TIME4_Taketime)
	{
	    Time4_countnum=0;
		TIME4_Retry=1;
		TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE); //�ر��ж�Դ
	}
}
void TIME4_RELOAD(u8 times)//��ʱ��4����
{
    TIME4_Retry=0;
	Time4_countnum=0;

	TIME4_Taketime=times;
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //ʹ���ж�Դ	
	TIM_SetCounter(TIM4, 0);	
}



void TIM3_IRQHandler(void)
{
	OSIntEnter(); 
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		T3_TimeCount++;
		Led_Open_time++;
		TimerCount++;
		//��������ʱ
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

		//////////////////����///////////////
		if(Forbidden)
		ReadKey_Pause=1;

		if((ReadKey_Pause || !(T3_TimeCount%10)) && !Forbidden)  
		{   
			ReakKeySub();
			temp=ChgKeyStringToUlong(keybuff,4);
		global_key = temp;
			if(temp)//�м�����
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
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
	OSIntExit(); 
}
/******************* (C) COPYRIGHT 2012*****END OF FILE****************/


