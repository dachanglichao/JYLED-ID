/**************************************************************************************

**************************************************************************************/

#include "stm32f10x.h"
#include "GPIO.H"//要使用GPIO中间件的话记得包含头文件
#include "FSMC.h"//要使用中间件的话记得包含头文件
#include "USART.H"
#include "TIME.H"
#include "NVIC.H"
#include  "os_cpu.h"
#include  "os_cfg.h"
#include  "hw_config.h"
#include  "stm32f10x_exti.h"
#include "Disp_1629DFile.h"
#include "watchdog.h"
#include "iic.h"
#include "include.h"

uchar		LoadProgamReDatas[6] ;
uchar		DownLoadPragamSymbl[6];
void	SysIC_Test(void);

/****************************************************************************
* 名    称：void BSP_Init(void)
* 功    能：Sunny号初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
u8 isneedupdate = 0;
void	Disp_Device_Num(uchar Num);
void	SetIPModeCode(void);
extern u8 isneeddhcp;
void BSP_Init(void)
{   

	ulong	ReadKeyValue,downflag;
 	uchar	i,Downloadflag;
	uchar Buffer[1];
	
  SystemInit();
	delay_ms(3000);
	DK_NVIC_Init(4);//选择NVIC第4组设置
	DK_USART_Close_INT(1);
	DK_USART_Close_INT(2);
	DK_USART_Close_INT(3);
	memset(LoadProgamReDatas,0,6);
	memset(DownLoadPragamSymbl,0,6);
	
	WatchDog_Gpio_init();//看门狗管脚初始化
	Init_IIC();//IIC端口初始化
	SPI_Flash_Init();  //SPI_FLASH初始化 	
	RC522_GPIO_INIT();
	MFRC522_Initializtion();  //RC522初始化
	GPIO_Configuration();
	Disp_Gpio();     //1629的管脚初始化
	DK_TIMER_Init(3);
	//TIM3_OPen_INT();
	delay_ms(3000);
	ReadSysParameter(0);//初始化系统参数
	Load_Net_Parameters();//装载网络参数
	//Beep_init();
	SysIC_Test();
	
	u485_GPIO_INT();
	DK_USART_Init(9600,2);     //二维码
	DK_USART_OPen_INT(2);
	DK_NVIC_IRQ_Set(USART2_IRQn,0,0,ENABLE);//要实现外设中断，必须先设置外设的优先级
	Usart2ToErMa(); 
	
	PsamGpioInt();
	PsamUSART_Configuration();	 	//串口1初始化为9600用于PSAM卡 
//	MainCode = 1;
	Disp_AllOn();
	delay_ms(3000);
	Clr_WatchDog();
	Disp_AllOff();
	delay_ms(3000);
	Clr_WatchDog();
	DispMainCode();//显示站点号
	ReadKeyValue=ScanKeySub3(KEY_ANY);
	if (ReadKeyValue==(KEY_1 | KEY_2 | KEY_3))
	{ 
		isneedupdate = 1;
	}else if (ReadKeyValue==(KEY_1 | KEY_5 | KEY_9))
	{ 
		SetMainCodeSub();//手动设置站点好
		SetConsumStatusCode();
	}
	else if (ReadKeyValue==(KEY_4 | KEY_8 | KEY_ADD))
	{		
		SetIPModeCode();
		SetNetDatas();//查看并设置网络参数			
	}
	else if (ReadKeyValue==(KEY_3 | KEY_5 | KEY_7))
	{		
			SetConsumModeEnableSub();//设置消费方式允许位及AL值查看			
	}
	else if (ReadKeyValue==(KEY_ENTER))
	{
		//memcpy(DispBuffer,PosVerString,5);
		//LED_DispDatas_all(DispBuffer);
		Disp_Ver(1);
		while(1)
		{
			ReadKeyValue=ScanKeySub(KEY_ADD|KEY_ESC | KEY_ENTER);
			if (ReadKeyValue==KEY_ESC)
				return;
		}			
	}
	else if (ReadKeyValue==(KEY_ADD))
	{
		//memcpy(DispBuffer,PosVerString,5);
		//LED_DispDatas_all(DispBuffer);
		Disp_Device_Num(1);
		while(1)
		{
			ReadKeyValue=ScanKeySub(KEY_ADD|KEY_ESC | KEY_ENTER);
			if (ReadKeyValue==KEY_ESC)
				return;
		}			
	}//Erase_Entire_Chip();
	Disp_Ver(1);
	BeepOn(3);
	InitOffLineRecord();//初始化脱机记录指针
	FindPosConsumCountSub();//查找最后一条记录的流水号
	//Erase_Entire_Chip();
	ReadSysParameter(0);//初始化系统参数
	Load_Net_Parameters();//装载网络参数
}
unsigned char		CheckPF8563(void);

//	硬件自检
void	SysIC_Test(void)
{
	u8 bitStatus8563,bitPF8563Error;
	
 	if (CV522Pcd_ResetState())//初试化RC531
	{
		DISP_HARDEErrorSub(SYS_RC500_ERROR);
	 	BeepOn(3);
		while (1);
	}
	if(Check_SPI_FLASH())
	{
		DISP_HARDEErrorSub(SYS_FLASH_ERROR);
		BeepOn(3);
		while (1)
		{
			if(!Check_SPI_FLASH())
			break;
		}
	}
	 if (Check_24C256(0))
	{
		DISP_HARDEErrorSub(SYS_24C256_ERROR);;
		BeepOn(3);
		while (1);
	}
	if (Check_24C64(0))
	{
		DISP_HARDEErrorSub(SYS_24C64_ERROR); 
		BeepOn(3);
		while (1);
	}
	
	bitStatus8563=0;
	bitPF8563Error=CheckPF8563();
	if (bitStatus8563)
	{
		DISP_ErrorSub(PASM_ERROR);
 		BeepOn(3);
		while (1);		 			
	}
}

void	Clr_PosSub(uchar bbit,uchar Sort)
{
	uchar	Buffer[64];
	uchar	XBuffer[25];
	uint	i;
	ulong	ii;

	if (Sort&1)
	{
		Forbidden=1;
		RdBytesFromAT24C64(TcpCardDatas_Addr,XBuffer,24);
		memset(Buffer,0xff,32);
		for (i=0;i<10;i++)
		{
			WrBytesToAT24C64(i*32,Buffer,32);
		}

		WrBytesToAT24C64(TcpCardDatas_Addr,XBuffer,24);//网络参数
		Buffer[0]=0xa0;
		Buffer[1]=* (uchar *)&MainCode;
		Buffer[2]=* (1+ (uchar *)&MainCode);
		Buffer[3]=CalCheckSum(Buffer,3);
		WrBytesToAT24C64(MainCode_Addr,Buffer,4);//重写站点号	

		Forbidden=0;
	}
	if (Sort&2)
	{	
		memset(Buffer,0xff,64);		
		for (ii=0;ii<10;ii++)
		{
			AT24CXX_Write(ii*64,Buffer,64);
			Clr_WatchDog();
		}
	}
	if (Sort&4)
		Erase_Entire_Chip();
}

/****************************************************************************
* 名    称：void  OS_CPU_SysTickInit(void)
* 功    能：ucos 系统节拍时钟初始化  初始设置为10ms一次节拍
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void  OS_CPU_SysTickInit(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;
    RCC_GetClocksFreq(&rcc_clocks);		                        //获得系统时钟的值	 
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;	//算出时钟节拍的值	
		SysTick_Config(cnts);										//设置时钟节拍	     
}
/******************* ***/
