/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "hw_config.h"
#include <stdio.h>
#include <string.h>
#include <misc.h>
#include "USART.H"
#include  "stm32f10x_exti.h"
#include "os_cpu.h"
#include "ucos_ii.h"
#include "externvariable.h"
/* Private typedef -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*******************************************************************************
* Function Name  : �ӳ�
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
void Delay_Count(__IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}

/*******************************************************************************
* Function Name  : ϵͳ��ʼ��
* Description    : ����ϵͳʱ�ӡ��жϡ��˿����á���Դ����
* Input          : None.
* Return         : None.
*******************************************************************************/
void System_Setup(void)
{
  // Setup STM32 clock, PLL and Flash configuration) 
  SystemInit();

  // Enable GPIOs and ADC1 clocks 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | 
												 RCC_APB2Periph_GPIOG| RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , ENABLE);

  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
  //��������
  DK_USART_Init(9600,2);
  // Configure the GPIO ports 
  GPIO_Configuration();
  //�ж�����
  NVIC_Configuration();
  //��������

}

/*******************************************************************************
* Function Name  : ͨ��IO������
* Description    : ����ͨ�����������
* Input          : None.
* Return         : None.
*******************************************************************************/
void GPIO_Configuration(void)
{
		
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | 
												 RCC_APB2Periph_GPIOG| RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , ENABLE);
	//LED

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.2
 	GPIO_SetBits(GPIOC,GPIO_Pin_1);	
}

//����WAkeUpΪ�ⲿ�ж�
void WakeUpGpioSet(void)
{
	
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//PA0�ⲿ�жϣ��ж���1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_ClearITPendingBit(EXTI_Line0); //����ж���6�����־λ
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn ;			//�ⲿ�ж���1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void)
{
	OSIntEnter(); 
	EXTI_ClearITPendingBit(EXTI_Line0); //����ж���6�����־λ

	OSIntExit(); 
}
/*******************************************************************************
* Function Name  : �ж�����
* Description    : ���ø�����Դ���ж�
* Input          : None.
* Return         : None.
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);


  /* Enable USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
}

  /****************************************************************************
* ��    �ƣ�void	Beep_init(void)
* ��    �ܣ�
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void	Beep_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	

}
/****************************************************************************
* ��    �ƣ�void	BeepOn(uchar Num)
* ��    �ܣ�
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void	BeepOn(unsigned char num)
{
	if(!BeepTimes)
	{
		BeepTimes=num;
		BeepDelayTimeCount=0;
	}
	
}



 





