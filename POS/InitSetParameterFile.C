#include "MyDefine.h"
#include "externVariable.h"
#include "char.h"
#include "stm32f10x_flash.h"
#include "include.h"
#include "InitSetParameterFile.h"

 #define MainCodeAddress    0x08061000
 uchar		bitSetDateEnable=0;//时间设置使能 
 uchar		bitPinEnable=0;//超限额密码消费使能
 uchar 	ConsumStaCodeSymbl[4]={0xc1,0xc2,0xc3,0xc4};

extern void 	LED_DispDatas_all(unsigned char * ptr);
	
 
void	SetnonetLimit(void);//设置脱机情况下是否增加限额判断
void	SetCommModeSub(void);
void	SetSysDateTime(ulong);//设置日期时间
uchar	SelectDate(uchar * ptr);//选择消费日期
void	ChgUlongToLongDispBuffer(ulong iii,uchar * ptr);
//void	ChgInputToLedBuffer(uchar LedX,uchar * ptr,uchar Num)//将输入的字符转换为显示字符
 extern u8 scidcode[6];
// u8 usesectornum = 0;
 extern u8 snedData[15+6+10];
 
 uint16_t FLASH_ReadHalfWord(uint32_t address)
 {
   return *(__IO uint16_t*)address; 
 }
 
//往参数扇区写数据
static void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{    
	uint16_t dataIndex;
 FLASH_Unlock();         //?????

 FLASH_ErasePage(MainCodeAddress);//??????
 
 for(dataIndex=0;dataIndex<countToWrite;dataIndex++)
 {
	 FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
 }
 
 FLASH_Lock();//?????
}
//写参数到mcu flash
static uint16_t tempbuf[256];
void writeSysParameterToFlash(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{
	//uint16_t tempbuf[256];
	uint16_t dataIndex;
	uint32_t addDev =0;
	//备份数据
   for(dataIndex=0;dataIndex<256;dataIndex++)
   {
     tempbuf[dataIndex]=FLASH_ReadHalfWord(MainCodeAddress+dataIndex*2);
   }
	 addDev = (startAddress - MainCodeAddress)/2;
	 memcpy(tempbuf +addDev,writeData,countToWrite);
	 FLASH_WriteMoreData(MainCodeAddress,tempbuf,256);
	 
	 for(dataIndex=0;dataIndex<256;dataIndex++)
   {
     tempbuf[dataIndex]=FLASH_ReadHalfWord(MainCodeAddress+dataIndex*2);
   }
}
 
void	ReadSysParameter(uchar bbit)
{
	uchar	Buffer[40];
	uchar	i;
	uint16_t buf[256];
	
	//经过download后，擦除站点
	FLASH_Unlock();//解锁
	MainCode=(*(vu16*) 0x8061000);
	FLASH_Lock();//锁定
	
	RdBytesFromAT24C64(usesectorNum_Addr,Buffer,3);//应用的扇区
	if (!BytesCheckSum(Buffer,3) && Buffer[0]==0xa0)
		usesectornum=Buffer[1];
	
	RdBytesFromAT24C64(useUsrID,Buffer,8);
	if (!BytesCheckSum(Buffer,8)&& Buffer[0]==0xa0)
		memcpy(scidcode,Buffer+1,6);//公共山区卡密码
	
	RdBytesFromAT24C64(LimitedMoney,Buffer,6);
	if (!BytesCheckSum(Buffer,6)&& Buffer[0]==0xa0)
	{
		memcpy(LimtConsumeBuf,Buffer+1,4);//读取脱机限额
		LimtConsumeMoney = *(uint32_t*)(&LimtConsumeBuf);
	}
	
	RdBytesFromAT24C64(ConsumStatus_Addr,Buffer,4);//
	if (!(memcmp(Buffer,ConsumStaCodeSymbl,4)))
		CanNoNetConsume=1;//为1时不允许脱机消费
	else
		CanNoNetConsume=0;//允许脱机消费
	
	RdBytesFromAT24C64(NoNetLimit_Addr,Buffer,4);//CanNoNetLimit
	if (!(memcmp(Buffer,ConsumStaCodeSymbl,4)))
			CanNoNetLimit=1;//为1时脱机时判断限额
	else
			CanNoNetLimit=0;//允许脱机不判断限额
	
	RdBytesFromAT24C64(ConsumModeEnable_Addr,Buffer,3);//消费方式允许
	if (!BytesCheckSum(Buffer,3) && Buffer[0]==0xa0)
		ConsumModeEnable=Buffer[1];
	if (!bbit)
	{//第一次初试化，对消费方式进行初试化
		ConsumMode=CONSUM_MONEY;//默认的消费方式
		if (!(ConsumModeEnable&8))
		{//不允许金额消费
				for (i=1;i<4;i++)
				{
					if (ConsumModeEnable & (1<<i))
					{	
						ConsumMode=i;
						break;	
					}	
			}
		}		
	}
}

void	Setstationcode(void)//手动设置消费站点号
{
	ulong	ReadKeyValue;
	uchar	Count=0;
	uchar	Buffer[5];
	uchar	st_data;
	uchar	LedX;
	uchar	i;
	ulong	iii;
	uint MainCodeBak;
	
	LedX=5;
	Count=5;	
 	ChgIntToStrings(stationcode,Buffer);
	DispBuffer[0]=0x39;//C
	DispBuffer[1]=0x5e;//d
	DispBuffer[2]=0x5e;//d
	DispBuffer[3]=0x50;//r
	for (i=0;i<5;i++)
		DispBuffer[LedX+i]=Disp0_9String[Buffer[i]];
	LED_DispDatas_all(DispBuffer);
	while (1)
	{
		LED_NumFalsh_Disp(LedX+5-Count,1,0x10000);
		ReadKeyValue=ScanKeySub(KEY_0_9 |KEY_ESC | KEY_ENTER);
		if (ReadKeyValue==KEY_ESC)
			return;
		else if (ReadKeyValue==KEY_ENTER)
			break;
		else if (ReadKeyValue & KEY_0_9)
		{
			if (Count)
			{
				st_data=ChgKeyValueToChar(ReadKeyValue);
				Buffer[5-Count]=st_data;							
				DispBuffer[LedX+5-Count]=Disp0_9String[st_data];
				Forbidden=1;
				LED_OneByte_Disp(LedX+5-Count,DispBuffer[LedX+5-Count]);
				Forbidden=0;
				Count--;

				if (!Count)					
					Count=5;
			}
		}
		Clr_WatchDog();
	}
	iii=ChgStringsToInt(Buffer);
	st_data=1;
	if (iii<65536 && iii)
		st_data=0;
	if (!st_data)
	{
		stationcode=(uint)ChgStringsToInt(Buffer);
		bitSetDateEnable=1;
		Buffer[0]=0xa0;
		Buffer[1]=* (uchar *)&stationcode;
		Buffer[2]=* (1+ (uchar *)&stationcode);
		Buffer[3]=CalCheckSum(Buffer,3);
		WrBytesToAT24C64(stationcode_Addr,Buffer,4);
		
	}
	bitUpdateParameter=1;
	ReadSysParameter(1);	
}

void	SetMainCodeSub(void)//手动设置站点号
{
	ulong	ReadKeyValue;
	uchar	Count=0;
	uchar	Buffer[5];
	uchar	st_data;
	uchar	LedX;
	uchar	i;
	ulong	iii;
	uint MainCodeBak;
	
	LedX=5;
	Count=5;	
 	ChgIntToStrings(MainCode,Buffer);
	DispBuffer[0]=0x77;//A
	DispBuffer[1]=0x5e;//d
	DispBuffer[2]=0x5e;//d
	DispBuffer[3]=0x50;//r
	for (i=0;i<5;i++)
		DispBuffer[LedX+i]=Disp0_9String[Buffer[i]];
	LED_DispDatas_all(DispBuffer);
	while (1)
	{
		LED_NumFalsh_Disp(LedX+5-Count,1,0x10000);
		ReadKeyValue=ScanKeySub(KEY_0_9 |KEY_ESC | KEY_ENTER);
		if (ReadKeyValue==KEY_ESC)
			return;
		else if (ReadKeyValue==KEY_ENTER)
			break;
		else if (ReadKeyValue & KEY_0_9)
		{
			if (Count)
			{
				st_data=ChgKeyValueToChar(ReadKeyValue);
				Buffer[5-Count]=st_data;							
				DispBuffer[LedX+5-Count]=Disp0_9String[st_data];
				Forbidden=1;
				LED_OneByte_Disp(LedX+5-Count,DispBuffer[LedX+5-Count]);
				Forbidden=0;
				Count--;

				if (!Count)					
					Count=5;
			}
		}
		Clr_WatchDog();
	}
	iii=ChgStringsToInt(Buffer);
	st_data=1;
	if (iii<65536 && iii)
		st_data=0;
	if (!st_data)
	{
		MainCode=(uint)ChgStringsToInt(Buffer);
		bitSetDateEnable=1;
		Buffer[0]=0xa0;
		Buffer[1]=* (uchar *)&MainCode;
		Buffer[2]=* (1+ (uchar *)&MainCode);
		Buffer[3]=CalCheckSum(Buffer,3);
		WrBytesToAT24C64(MainCode_Addr,Buffer,4);
		{
			FLASH_Unlock();//解锁
			MainCodeBak=(*(vu16*) 0x8061000);
			FLASH_Lock();//锁定
			if((MainCodeBak!=MainCode)||(!MainCode))
			{
				FLASH_Unlock();//解锁
				FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
				FLASH_ErasePage(MainCodeAddress);//擦除整页
				FLASH_ProgramHalfWord(MainCodeAddress,MainCode);
				FLASH_Lock();//锁定
			}			
		}

		FLASH_Unlock();//解锁
		MainCode=(*(vu16*) 0x8061000);
		FLASH_Lock();//锁定
	}
	bitUpdateParameter=1;
	ReadSysParameter(1);	
}


void	ChgInputToLedBuffer(uchar LedX,uchar * ptr,uchar Num)//将输入的字符转换为显示字符
{
	uchar	st_data,i;
	uchar	j=0;
	for (i=0;i<Num;i++)
	{
		st_data=ptr[i];
	 	if (st_data==0xff)
			j++;
	}
	if(!bitHaveDispBalance)
		memset(DispBuffer+LedX,0,Num);
	else
		memset(DispBuffer+5,0,Num);
	if (j<Num)
	{
		for (i=0;i<(Num-j);i++)
		{
			st_data=ptr[i];
			DispBuffer[LedX+j+i]=Disp0_9String[st_data&0x0f];
			if (st_data&0x80)
				DispBuffer[LedX+j+i]=DispBuffer[LedX+j+i]|0x80;
		}
	}
	LED_DispDatas_all(DispBuffer);
}

void	ChgUlongToLongDispBuffer(ulong lll,uchar * ptr)
{
	ulong	jjj=10000;
	uchar	i,j;
	uchar	st_data;
	uchar	bbit=0;
	j=2;
	if (lll>999999)
	{
		lll=lll/100;
		j=4;
	}
	else if (lll>99999)
	{
		lll=lll/10;
		j=3;
	}
	for (i=0;i<5;i++)
	{
		st_data=lll/jjj;
		if (!st_data && !bbit && i<j)
		{
			ptr[i]=0;
		}
		else
		{
			ptr[i]=Disp0_9String[st_data];
			bbit=1;
		}
		lll=lll%jjj;
		jjj=jjj/10;
	}
	if (j<5 )
		ptr[j]|=0x80;
}


void	DispConsumStatus(uchar bbit)	
{
	if(!bbit)
		memcpy(DispBuffer,"\x76\x77\x3e\x79\x00\x00\x37\x79\x78\x00",10);//have net
	else
		memcpy(DispBuffer,"\x00\x37\x5c\x00\x00\x00\x37\x79\x78\x00",10);//no net
	LED_DispDatas_all(DispBuffer);
}

void	DispNonetLimit(uchar bbit)	
{
	if(!bbit)
		memcpy(DispBuffer,"\x76\x77\x3e\x79\x00\x00\x38\x06\x78\x00",10);//have LIT
	else
		memcpy(DispBuffer,"\x00\x37\x5c\x00\x00\x00\x38\x06\x78\x00",10);//no LIT
	LED_DispDatas_all(DispBuffer);
}
void	SetConsumModeEnableSub(void)//设置消费方式允许位
{
	uchar	Buffer[4];
	ulong	Key_Long;
	uchar	i;
	uchar	st_data=0;
	uchar	Num=0;
	DispConsumModeEnable(0,ConsumModeEnable);
	while (1)
	{
		Key_Long=ScanKeySub(KEY_ESC|KEY_ENTER|KEY_SIMPLE | KEY_NUM | KEY_RATION| KEY_MONEY|KEY_ADD);
		if (Key_Long)
		{
			if (Key_Long==KEY_ESC)
				return;
			if (Key_Long==KEY_ENTER)
			{
				if (!Num)
					Num=1;
				else
					break;
				DispConsumModeEnable(Num,ConsumModeEnable);	 
			}
			else
			{
				if (Key_Long==KEY_SIMPLE)
					i=CONSUM_SIMPLE;
			  	else if (Key_Long==KEY_NUM)
					i=CONSUM_NUM;
			  	else if (Key_Long==KEY_RATION)
					i=CONSUM_RATION;
				else  if (Key_Long==KEY_MONEY)
					i=CONSUM_MONEY;
				else if (Key_Long==KEY_ADD)
				{
					if (!Num)
						i=4;
					else
						i=5;
				}
				st_data=1<<i;
				if (ConsumModeEnable & st_data)
					ConsumModeEnable&=~st_data;
				else
					ConsumModeEnable|=st_data;
				DispConsumModeEnable(Num,ConsumModeEnable);
			}
		}
		Clr_WatchDog();		
	}
	Buffer[0]=0xa0;
	Buffer[1]=ConsumModeEnable;
	Buffer[2]=CalCheckSum(Buffer,2);
	WrBytesToAT24C64(ConsumModeEnable_Addr,Buffer,3);

	bitUpdateParameter=1;
	ReadSysParameter(0);	

}
void	SetSysPrince(uchar bitMode,ulong	 InputKeyValue)//设置简易\菜号及单价
{
	uchar	bbit=0;
	uint	Addr;
	uchar	st_data;
	uchar	RdBuffer[8];
	ulong	iii;
	DispBuffer[10]=0x10;
	Forbidden=1;
	LED_DispDatas(1,DispBuffer);//显示菜单
	Forbidden=0;
	if (!bitMode)
	{//菜单价格
		if (InputCase==0x30)
		{
			bbit=1;
			InputCount=0;
			InputNum=0;
		 	bitHaveInputDatas=0;
			InputCase=0x31;
		}
	}
	else
	{//简易消费价格
		if (InputCase==0x40)
		{
			bbit=1;
			InputCount=0;
			InputNum=0;
			bitHaveInputDatas=0;
			InputCase=0x41;
		}
	}
	if (InputKeyValue==KEY_ENTER)
	{
			InputCase|=2;
			if (bitHaveInputDatas)
			{
				bitHaveInputDatas=0;
				if (!bitMode)
					Addr=MenuPrince_Addr;
				else
					Addr=SimplePrince_Addr;
				iii=ChgInputToUlong(InputBuffer,5);
										 //不出现自动模式下出现0元消费
				ChgUlongToBCDString(iii,RdBuffer+1,3);
				RdBuffer[0]=0xa0;
				RdBuffer[4]=CalCheckSum(RdBuffer,4);
				if(Consum_Status)
					LimtMoney = 20000;	
//				if(iii > LimtMoney )
//				{
//					while(1)
//					{
//						BeepOn(3);
//						DISP_ErrorSub(LimtMoney_ERROR);	
//						if(ScanKeySub(KEY_ANY) == KEY_ESC)
//							break;	
//						Clr_WatchDog();
//					}	
//				}
//				if(!ChgInputToUlong(InputBuffer,5))
//				{
//					while(1)
//					{
//						BeepOn(3);
//						DISP_ErrorSub(CARD_ERROR0);
//						if(ScanKeySub(KEY_ANY) == KEY_ESC)
//							break;	
//						Clr_WatchDog();
//						
//					}
//				}
//				else if(iii < LimtMoney)
				  WrBytesToAT24C64(Addr+InputNum*5,RdBuffer,5);
				BeepOn(1);
			}
			InputNum++;
			if (InputNum>=InputMaxNum)
				InputNum=0;	
			bbit=1;
		
	}
	if (bbit)
	{
		if (!bitMode)
		{//菜号方式
			Addr=MenuPrince_Addr;
			InputMaxNum=0;
			for (st_data=0;st_data<51;st_data++)
			{
				RdBytesFromAT24C64(Addr,RdBuffer,5);
				if (RdBuffer[0]==0xa0 && !BytesCheckSum(RdBuffer,5))
					InputMaxNum++;
				else
					break;	
				Addr+=5;	
			}
			Addr=MenuPrince_Addr;
			DispBuffer[0]=0x54;//N
			DispBuffer[1]=0xdc;//O.
			DispBuffer[2]=Disp0_9String[InputNum/10];
			DispBuffer[3]=Disp0_9String[InputNum%10];
		}
		else
		{
			Addr=SimplePrince_Addr;
			DispBuffer[0]=0x39;//[
			DispBuffer[3]=0x0f;//]
			DispBuffer[1]=Disp0_9String[(InputNum+1)/10];
			DispBuffer[2]=Disp0_9String[(InputNum+1)%10];
			InputMaxNum=12;
		}
		if (InputNum>InputMaxNum )
			InputNum=0;
		RdBytesFromAT24C64(Addr+InputNum*5,RdBuffer,5);
		if (RdBuffer[0]==0xa0 && !BytesCheckSum(RdBuffer,5))
			memcpy(InputBuffer,RdBuffer+1,3);
	  	else
			memset(InputBuffer,0,3);
		iii=ChgBCDStringToUlong(InputBuffer,3);		
		DispBuffer[4]=0;
		ChgUlongToLongDispBuffer(iii,DispBuffer+5);	
		bbit=0;
		InputCount=0;
		bitInPutPoint=0;
		memset(InputBuffer,0xff,6);	
		bitHaveInputDatas=0;
		LED_DispDatas_all(DispBuffer);
	}
	if (InputNum<=InputMaxNum)
	{
		if (InputKeyValue & KEY_0_9 )
		{
			InputCase|=2;
			st_data=5;
			if (InputCount<st_data)
			{
				st_data=0;
				if (bitInPutPoint && InputCount>=3 )
				{//允许输入小数点2位
					if ( InputBuffer[InputCount-3]&0x80 )
						st_data=1;
				}
			
				if (!st_data)
				{
					bitHaveInputDatas=1;
					st_data=ChgKeyValueToChar(InputKeyValue);	
					InputBuffer[InputCount++]=st_data;
	 				if (InputCount==2 && !bitInPutPoint)
					{//最前面的输入为0，去掉前面的0
						if (!InputBuffer[0])
						{
							memset(InputBuffer,0xff,6);
							InputBuffer[0]=st_data;
							InputCount=1;
						}
					}
					if (InputCount>=4 && !bitInPutPoint)
					{//3字节整数循环
						memset(RdBuffer,0xff,5);
						memcpy(RdBuffer,InputBuffer+1,3);
						memcpy(InputBuffer,RdBuffer,4);
						InputCount=3;
						FormatBuffer(3,InputBuffer,&InputCount);
					}	
				}
				DispBuffer[4]=0;
				ChgInputToLedBuffer(5,InputBuffer,5);
			}
			else
				BeepOn(3);
		}
		else if (InputKeyValue==KEY_POINT)
		{
			InputCase|=2;
			if (!bitInPutPoint)
			{
				bitInPutPoint=1;
				if (InputCount)
					InputBuffer[InputCount-1]|=0x80;
				else
				{
					InputBuffer[0]=0x80;	
					InputCount=1;
				}
				ChgInputToLedBuffer(5,InputBuffer,5);
			}
		}

	}	
}

void	SetConsumStatusCode(void)//设置脱机消费
{
	uchar		Buffer[4];
	ulong	Key_Long;
	uchar	bbit;
	
	RdBytesFromAT24C64(ConsumStatus_Addr,Buffer,4);
	if (!(memcmp(Buffer,ConsumStaCodeSymbl,4)))
			bbit=1;//为1时不允许脱机消费
	else
			bbit=0;//允许脱机消费
	
	DispConsumStatus(!bbit);
	while (1)
	{
		LED_NumFalsh_Disp(0,4,0xa000);
		Key_Long=ScanKeySub(KEY_ESC|KEY_ENTER|KEY_ADD);
		if (Key_Long==KEY_ESC)
			return;
		if (Key_Long==KEY_ENTER)
			break;
		if (Key_Long==KEY_ADD)
		{
			if(bbit)
			{
				bbit =0;
			}
			else
				bbit =1;
			DispConsumStatus(!bbit);	
		}
		Clr_WatchDog();
	}
	if (!bbit)
	{
		memset(Buffer,0xff,4);
		CanNoNetConsume = 0;
	}
	else
	{
		memcpy(Buffer,ConsumStaCodeSymbl,4);
		CanNoNetConsume = 1;
	}
	WrBytesToAT24C64(ConsumStatus_Addr,Buffer,4);
	BeepOn(2);
	bitUpdateParameter=1;
	ReadSysParameter(0);
}

void	SetnonetLimit(void)//设置脱机情况下是否增加限额判断
{
	uchar		Buffer[4];
	ulong	Key_Long;
	uchar	bbit;
	
	RdBytesFromAT24C64(NoNetLimit_Addr,Buffer,4);
	if (!(memcmp(Buffer,ConsumStaCodeSymbl,4)))
			bbit=1;//为1时不允许脱机消费
	else
			bbit=0;//允许脱机消费
	
	DispNonetLimit(!bbit);
	while (1)
	{
		LED_NumFalsh_Disp(0,4,0xa000);
		Key_Long=ScanKeySub(KEY_ESC|KEY_ENTER|KEY_ADD);
		if (Key_Long==KEY_ESC)
			return;
		if (Key_Long==KEY_ENTER)
			break;
		if (Key_Long==KEY_ADD)
		{
			if(bbit)
			{
				bbit =0;
			}
			else
				bbit =1;
			DispNonetLimit(!bbit);	
		}
		Clr_WatchDog();
	}
	if (!bbit)
	{
		memset(Buffer,0xff,4);
		CanNoNetLimit = 0;
	}
	else
	{
		memcpy(Buffer,ConsumStaCodeSymbl,4);
		CanNoNetLimit = 1;
	}
	WrBytesToAT24C64(NoNetLimit_Addr,Buffer,4);
	BeepOn(2);
	bitUpdateParameter=1;
	ReadSysParameter(0);
}

void	DispIPStatus(uchar bbit)	
{
	if(!bbit)
		memcpy(DispBuffer,"\x5E\x76\x58\x73\x00\x00\x00\x00\x00\x00",10);//dhcp
	else
		memcpy(DispBuffer,"\x06\x73\x00\x00\x00\x00\x00\x00\x00\x00",10);//static
	LED_DispDatas_all(DispBuffer);
}

extern u8 isneeddhcp;
void	SetIPModeCode(void)//设置脱机消费
{
	uchar		Buffer[4];
	ulong	Key_Long;
	uchar	bbit;
	
	RdBytesFromAT24C64(staticsum,Buffer,4);
	if (!BytesCheckSum(Buffer,3) && Buffer[0]==0xa0)
	{
		bbit = Buffer[1];
	}else
	{
		bbit = 0;
	}
	
	DispIPStatus(!bbit);
	while (1)
	{
		LED_NumFalsh_Disp(0,4,0xa000);
		Key_Long=ScanKeySub(KEY_ESC|KEY_ENTER|KEY_ADD);
		if (Key_Long==KEY_ESC)
			return;
		if (Key_Long==KEY_ENTER)
			break;
		if (Key_Long==KEY_ADD)
		{
			if(bbit)
			{
				bbit =0;
			}
			else
				bbit =1;
			DispIPStatus(!bbit);	
		}
		Clr_WatchDog();
	}
	if (!bbit)
	{
		memset(Buffer,0xff,4);
		//CanNoNetConsume = 0;
	}
	else
	{
		memcpy(Buffer,ConsumStaCodeSymbl,4);
		//CanNoNetConsume = 1;
	}
	Buffer[0]=0xa0;
	isneeddhcp = Buffer[1]=bbit;
	Buffer[2]=CalCheckSum(Buffer,2);
	WrBytesToAT24C64(staticsum,Buffer,3);//
	BeepOn(2);
	bitUpdateParameter=1;
	ReadSysParameter(0);
}
