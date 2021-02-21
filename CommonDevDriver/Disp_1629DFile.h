
#ifndef __DISP_1629FILES_H
#define __DISP_1629FILES_H
#include "char.h"

extern void 	Disp_Gpio(void);
extern void	Disp_AllOn(void);
extern void	Disp_AllOff(void);
extern void	DISP_ErrorSub(unsigned char);
extern void	DispMainCode(void);
extern void	SetNetDatas(void);
extern void	DispSumConsumMoney(unsigned char,unsigned char,unsigned long);
extern void	Disp_Balance(unsigned char * );//��ʾ���
extern void	Disp_Hello(void);
extern void	DispConsumModeEnable(unsigned char ,unsigned char );//��ʾ��������ʽ
extern void	DispDate(unsigned char * );//��ʾ����
extern void	DispTime(unsigned char *);//��ʾʱ��
extern void	Disp_Clr_Ram(void);//��ʾ���Pos;
extern void	Disp_Ver(unsigned char);//��ʾ�汾��
extern void	ChgIntToDispBuffer(uint,unsigned char * );
extern void	DISP_WriteError(unsigned char * );
extern void	DispMinBalance(void);//��ʾ�׽�
extern void	DispOver(void);
extern unsigned char	FindMonth(unsigned char * );//��������
extern void	LED_Indata(unsigned char bbit,unsigned char aa);  //д������
extern unsigned char 	ReadByteFrom1629(void);//�������
extern void 	LED_DispDatas_1629(unsigned char ledx,unsigned char * ptr,unsigned char num);
extern void 	LED_DispDatas(unsigned char bbit,unsigned char * ptr);
extern void 	LED_DispDatas_all(unsigned char * ptr);
extern void	ReakKeySub(void);
extern void 	LED_OneByte_Disp(unsigned char ledx,unsigned char prt);
extern void 	LED_NumFalsh_Disp(unsigned char ledx,unsigned char num,unsigned long delays);
extern void 	Led_Wait_Disp(void);
extern void	Disp_Clr_WhilteNum(void);//��ʾ���Pos;
extern void	DISP_HARDEErrorSub(uchar );
#endif
