#include "485.H"
#include "GPIO.H"
#include "USART.H"
#include "stdio.h"

//串口的4052选择管脚配置
#define 	COM_SEL0_GPIO     		PE
#define 	COM_SEL0_BIT      		GPIO_Pin_15
#define 	COM_SEL0_Type_Out     	Out_PP

#define 	COM_SEL1_GPIO     		PE
#define 	COM_SEL1_BIT      		GPIO_Pin_14
#define 	COM_SEL1_Type_Out     	Out_PP

#define 	COM1_SEL0_GPIO     		PE
#define 	COM1_SEL0_BIT      		GPIO_Pin_13
#define 	COM1_SEL0_Type_Out     	Out_PP

#define 	COM1_SEL1_GPIO     		PE
#define 	COM1_SEL1_BIT      		GPIO_Pin_12
#define 	COM1_SEL1_Type_Out     	Out_PP
 
//IO方向设置
#define COM_SEL0_OUT()  DK_GPIO_Init(COM_SEL0_GPIO,COM_SEL0_BIT,COM_SEL0_Type_Out);
#define COM_SEL1_OUT()  DK_GPIO_Init(COM_SEL1_GPIO,COM_SEL1_BIT,COM_SEL1_Type_Out);
#define COM1_SEL0_OUT() DK_GPIO_Init(COM1_SEL0_GPIO,COM1_SEL0_BIT,COM1_SEL0_Type_Out);
#define COM1_SEL1_OUT() DK_GPIO_Init(COM1_SEL1_GPIO,COM1_SEL1_BIT,COM1_SEL1_Type_Out);
//IO操作函数	 
#define COM_SEL0    PEout(15) //
#define COM_SEL1    PEout(14) //
#define COM1_SEL0    PEout(13) //
#define COM1_SEL1    PEout(12) //

/***********************************************************************
*函数描述：
*函数功能：
*************************************************************************/
void u485_GPIO_INT(void)
{
	u485_OUT  ;
	u485_RE;
}
//串口的4052选择管脚初始化
void USART_COMSEL_GPIO_INIT(void)
{
	COM_SEL0_OUT();
	COM_SEL1_OUT();
	COM1_SEL0_OUT();
	COM1_SEL1_OUT();
}
//485模式4052选择
void COMSEL_485Mode(void)
{
	COM_SEL0 = 0;
	COM_SEL1 = 0;	
}
//232模式4052选择
void COMSEL_232Mode(void)
{
	COM_SEL0 = 1;
	COM_SEL1 = 0;	
}
//串口3空闲
void COMSEL_NoMode(void)
{
	COM_SEL0 = 1;
	COM_SEL1 = 1;	
}
//232模式4052选择
void SelWifiMode(void)
{
	COM_SEL0 = 0;
	COM_SEL1 = 1;	
}
 //串口2工作在打印机模式
 void Usart2ToPrint(void)
 {
 	COM1_SEL0 = 0;
	COM1_SEL1 = 0;
 }
  //串口2工作在MP3模式
 void Usart2ToMP3(void)
 {
 	COM1_SEL0 = 1;
	COM1_SEL1 = 0;
 }
   //串口2工作在二维码
 void Usart2ToErMa(void)
 {
 	COM1_SEL0 = 0;
	COM1_SEL1 = 1;
 }
 
 //TTS文字转语音
 void mp3play(uint8_t *data)
 {
		DK_USART_PUTS_Arry(3,data,strlen(data));
 }
 //设置音量大小1--4
 void setVolume(uint8_t num)
 {
	 uint8_t data1[]={"<V>1"};
	 uint8_t data2[]={"<V>2"};
	 uint8_t data3[]={"<V>3"};
	 uint8_t data4[]={"<V>4"};
	 
	 switch (num)
	 {
		 case 1:
			 mp3play(data1);
		 break;
		 case 2:
			 mp3play(data2);
		 break;
		 case 3:
			 mp3play(data3);
		 break;
		 case 4:
			 mp3play(data4);
		 break;
	 }
 }
 
  //设置播放速度1--3
 void setPlaySpeed(uint8_t num)
 {
	 uint8_t data1[]={"<S>1"};
	 uint8_t data2[]={"<S>2"};
	 uint8_t data3[]={"<S>3"};

	 
	 switch (num)
	 {
		 case 1:
			 mp3play(data1);
		 break;
		 case 2:
			 mp3play(data2);
		 break;
		 case 3:
			 mp3play(data3);
		 break;
	 }
 }
 extern void	ChgMoneyToPrintBuffer(uint8_t ,uint32_t ,uint8_t * );
////播放消费额
//void playConsumeMoney(uint32_t money)
//{
//	uint8_t Buffer[20];
//	uint8_t moneyBuf[20];
//	uint8_t xiaofei[] = {"消费"};
//	uint8_t yuan[] = {"元"};
//	
//	memset(	moneyBuf,0,20);
//	memset(	Buffer,0,20);
//	ChgMoneyToPrintBuffer('-',money,moneyBuf);
//	
//	memcpy(Buffer,xiaofei,strlen(xiaofei));
//	memcpy(Buffer+strlen(xiaofei),moneyBuf,strlen(moneyBuf));
//	memcpy(Buffer+strlen(xiaofei)+strlen(moneyBuf),yuan,2);
//	
//	DK_USART_PUTS_Arry(3,Buffer,sizeof(Buffer)); 
//}


//播放消费额
void playConsumeMoney(float money)
{
	char Buffer[20];
	char moneyBuf[20];
	char xiaofei[] = {"消费"};
	char yuan[] = {"元"};
	
	memset(	moneyBuf,0,20);
	memset(	Buffer,0,20);
	//ChgMoneyToPrintBuffer('-',money,moneyBuf);
FToStr1(money,moneyBuf,2);
  //sprintf(moneyBuf,"%.2f",money);
	
	memcpy(Buffer,xiaofei,strlen(xiaofei));
	memcpy(Buffer+strlen(xiaofei),moneyBuf,strlen(moneyBuf));
	memcpy(Buffer+strlen(xiaofei)+strlen(moneyBuf),yuan,2);
	
	DK_USART_PUTS_Arry(3,Buffer,sizeof(Buffer)); 
}