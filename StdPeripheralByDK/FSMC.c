#include "stm32f10x_fsmc.h"
#include "GPIO.H"
#include "stm32f10x_tim.h"
#include "TIME.H"
#include "FSMC.h"
/**************************************************************************************

**************************************************************************************/
/****************************************************************************
* 名    称：void DK_FSMC_LCD_Init(void)
* 功    能：基于FSMC接口的16位TFT接口初始化
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void DK_FSMC_LCD_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);	   //使能FSMC接口时钟
	DK_GPIO_Init(PD,GPIO_Pin_13,AF_PP);//背光控制管脚初始化
	DK_TIMER_Init(4);
	DK_TIM4_PWM_Init(2,100);
  
	DK_GPIO_Init(PE,GPIO_Pin_1,Out_PP);//TFT 复位脚初始化
    	
  /*  启用FSMC复用功能， 定义FSMC D0---D15及nWE, nOE对应的引脚  */
  /* 设置 PD.00(D2), PD.01(D3), PD.04(nOE), PD.05(nWE), PD.08(D13), PD.09(D14), PD.10(D15),
     PD.14(D0), PD.15(D1) 为复用上拉 */
	DK_GPIO_Init(PD,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15,AF_PP);
  
  /* 设置 PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) 为复用上拉 */
  DK_GPIO_Init(PE,GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15,AF_PP);

  /* FSMC  NE1 配置 PD7 */
  DK_GPIO_Init(PD,GPIO_Pin_7,AF_PP);
  
  /* FSMC RS配置 PD11-A16 */
	DK_GPIO_Init(PD,GPIO_Pin_11,AF_PP);

  p.FSMC_AddressSetupTime = 0x02;
  p.FSMC_AddressHoldTime = 0x00;
  p.FSMC_DataSetupTime = 0x05;
  p.FSMC_BusTurnAroundDuration = 0x00;
  p.FSMC_CLKDivision = 0x00;
  p.FSMC_DataLatency = 0x00;
  p.FSMC_AccessMode = FSMC_AccessMode_B;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* 使能FSMC BANK1_SRAM 模式 */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);    
}
/******************* (C) COPYRIGHT  *****END OF FILE****************/

