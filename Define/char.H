#ifndef __CHAR_H
#define __CHAR_H

#define		ulong	unsigned 	long
#define		uint	unsigned  	short
#define		uchar	unsigned  	char

struct	sDatasStruct//ͨѶ���ݸ�ʽ
{
	unsigned char	POsAddrCode;//վ���
	unsigned char	Comd;      //������
	unsigned char	Datas[147];//���ݰ�
};
union	sDatasUnion
{
	unsigned char	S_DatasBuffer[150];
	struct	sDatasStruct	S_DatasStruct;
};

//ʱ��ṹ
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
