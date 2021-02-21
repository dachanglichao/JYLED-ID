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
* Function Name  : 延迟
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
void Delay_Count(__IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}

/*******************************************************************************
* Function Name  : 系统初始化
* Description    : 设置系统时钟、中断、端口配置、资源配置
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
  //串口设置
  DK_USART_Init(9600,2);
  // Configure the GPIO ports 
  GPIO_Configuration();
  //中断配置
  NVIC_Configuration();
  //串口配置

}

/*******************************************************************************
* Function Name  : 通用IO口配置
* Description    : 配置通用输入输出口
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

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.2
 	GPIO_SetBits(GPIOC,GPIO_Pin_1);	
}

//配置WAkeUp为外部中断
void WakeUpGpioSet(void)
{
	
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//PA0外部中断，中断线1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_ClearITPendingBit(EXTI_Line0); //清除中断线6挂起标志位
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn ;			//外部中断线1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void)
{
	OSIntEnter(); 
	EXTI_ClearITPendingBit(EXTI_Line0); //清除中断线6挂起标志位

	OSIntExit(); 
}
/*******************************************************************************
* Function Name  : 中断配置
* Description    : 配置各个资源的中断
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
* 名    称：void	Beep_init(void)
* 功    能：
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void	Beep_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	

}
/****************************************************************************
* 名    称：void	BeepOn(uchar Num)
* 功    能：
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void	BeepOn(unsigned char num)
{
	if(!BeepTimes)
	{
		BeepTimes=num;
		BeepDelayTimeCount=0;
	}
	
}



 





