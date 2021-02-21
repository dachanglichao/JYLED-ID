#ifndef __CHAR_H
#define __CHAR_H

#define		ulong	unsigned 	long
#define		uint	unsigned  	short
#define		uchar	unsigned  	char

struct	sDatasStruct//通讯数据格式
{
	unsigned char	POsAddrCode;//站点号
	unsigned char	Comd;      //命令字
	unsigned char	Datas[147];//数据包
};
union	sDatasUnion
{
	unsigned char	S_DatasBuffer[150];
	struct	sDatasStruct	S_DatasStruct;
};

//时间结构
struct	sTimeStruct
{
	unsigned char	YearChar;
	unsigned char	MonthChar;
	unsigned char	DayChar;
	unsigned char	HourChar;
	unsigned char	MinuteChar;
	unsigned char	SecondChar;
};
union	uTimeUnion
{
	unsigned char	TimeString[6];
	struct	sTimeStruct	S_Time;
}; 
#endif

