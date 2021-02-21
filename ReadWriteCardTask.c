#include  "cpu.h"
#include  "os_cpu.h"
#include  "app_cfg.h"
#include  "ucos_ii.h"
#include  "include.h"
#include "w5500.h"
#include "descode.h"

void	ConsumSub(void);//����������
extern 	unsigned char		BatModeFlag;
void ChangeLedBright(uchar ptr);

void Handle_Card(void* p_arg)
{
	uchar BatModeFlagBag;
	GPIO_InitTypeDef  GPIO_InitStructure;
		//��ص������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	//ģ������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.2
	
	while(1)
	{
		BatModeFlag =PCin(2);
		
		if(BatModeFlag !=BatModeFlagBag)	 //ÿ��ģʽת��������
		{
			if(BatModeFlag)
			{
				ChangeLedBright(0x8b); //�޸���������ȵ�
				DispBuffer[10]|=0x01;
			}
			else
			{
				ChangeLedBright(0x8a); //�޸���������ȸ�
				 DispBuffer[10]&=0xfe;
			}
			BeepOn(2);
		}
		BatModeFlagBag =  BatModeFlag;
		
		ConsumSub();
	}
	
}