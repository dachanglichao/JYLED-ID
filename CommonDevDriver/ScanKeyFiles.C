#include "MyDefine.h"
#include "externVariable.h"
#include "char.h"
#include "ScanKeyFiles.h"
#include "include.h"


extern uchar bitHaveKey;

unsigned long	ScanKeySub(ulong	ll)//尽量只用在扫描一个键时
{
	unsigned long	lll_Data=0;
	if (bitHaveKey)
	{
		if (KeyValue & ll)
		{
			switch(KeyValue)
			{
///////////////////////////键盘按键定义//////////////////
				case		KEY_SIMPLE		:   
				case		KEY_NUM			:
				case		KEY_RATION		:
				case		KEY_MONEY		:
				case		KEY_MENU		:				
				case		KEY_ESC			:				
				case		KEY_MUL			:
				case		KEY_ENTER		:
				case		KEY_ADD			:
				case		KEY_POINT		:
				case		KEY_0			:
				case		KEY_1			:
				case		KEY_2			:
				case		KEY_3			:
				case		KEY_4			:
				case		KEY_5			:
				case		KEY_6			:
				case		KEY_7			:
				case		KEY_8			:
				case		KEY_9			:
				case		KEY_0_9			:
				case		KEY_ANY			:
				case		KEY_NO			:
				case 		KEY_ESC | KEY_MENU:

					BeepOn(1);
					lll_Data=KeyValue;
					break;
				default:
					break;
			}		
	 	}
		bitHaveKey=0;
		KeyValue=0;
	}
	return	lll_Data;
}
unsigned long	ScanKeySub3(unsigned long	ll)
{
	unsigned long		lll_Data=0;
	if (bitHaveKey)
	{
		if (KeyValue & ll)
		{
			BeepOn(1);
			lll_Data=KeyValue;
	 	}
		bitHaveKey=0;
		KeyValue=0;
	}
	return	lll_Data;
}
//zjx_change8_080602
unsigned char	ChgKeyValueToChar(unsigned long ll)//将键值转换为0-9
{
	if (ll==KEY_0)
		return	0;
	else if (ll==KEY_1)
		return	1;
	else if (ll==KEY_2)
		return	2;
	else if (ll==KEY_3)
		return	3;
	else if (ll==KEY_4)
		return	4;
	else if (ll==KEY_5)
		return	5;
	else if (ll&KEY_6)
		return	6;
	else if (ll==KEY_7)
		return	7;
	else if (ll==KEY_8)
		return	8;
	else if (ll==KEY_9)
		return	9;

}
