
/**************************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "gpio.H"
#include "watchdog.h"
#include "externVariable.h"

//看门狗管脚配置
#define 	WATCH_DOG_GPIO     		PC
#define 	WATCH_DOG_BIT      		GPIO_Pin_0
#define 	WATCH_DOG_Type_Out     	Out_PP

#define 	WATCH_DOG_OUT()  DK_GPIO_Init(WATCH_DOG_GPIO,WATCH_DOG_BIT,WATCH_DOG_Type_Out);

//IO操作函数	 
#define WATCH_DOG    PCout(0) //
/****************************************************************************
* 名    称：void	BeepOn(uchar Num)
* 功    能：
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void	WatchDog_Gpio_init(void)
{
	WATCH_DOG_OUT();
}
//喂狗操作
volatile char isOnKMD = 1;


void Clr_WatchDog(void)
{
	if(isOnKMD)
	{
	WATCH_DOG = 0;
	WATCH_DOG = 1;
	}
}


