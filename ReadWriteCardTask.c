#include  "cpu.h"
#include  "os_cpu.h"
#include  "app_cfg.h"
#include  "ucos_ii.h"
#include  "include.h"
#include "w5500.h"
#include "descode.h"

void	ConsumSub(void);//消费主程序
extern 	unsigned char		BatModeFlag;
void ChangeLedBright(uchar ptr);

void Handle_Card(void* p_arg)
{
	uchar BatModeFlagBag;
	GPIO_InitTypeDef  GPIO_InitStructure;
		//电池电量检测
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	//模拟输入
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.2
	
	while(1)
	{
		BatModeFlag =PCin(2);
		
		if(BatModeFlag !=BatModeFlagBag)	 //每次模式转换响两声
		{
			if(BatModeFlag)
			{
				ChangeLedBright(0x8b); //修改数码管亮度低
				DispBuffer[10]|=0x01;
			}
			else
			{
				ChangeLedBright(0x8a); //修改数码管亮度高
				 DispBuffer[10]&=0xfe;
			}
			BeepOn(2);
		}
		BatModeFlagBag =  BatModeFlag;
		
		ConsumSub();
	}
	
}