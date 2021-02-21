
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
extern void	Disp_Balance(unsigned char * );//显示余额
extern void	Disp_Hello(void);
extern void	DispConsumModeEnable(unsigned char ,unsigned char );//显示消费允许方式
extern void	DispDate(unsigned char * );//显示日期
extern void	DispTime(unsigned char *);//显示时间
extern void	Disp_Clr_Ram(void);//显示清空Pos;
extern void	Disp_Ver(unsigned char);//显示版本号
extern void	ChgIntToDispBuffer(uint,unsigned char * );
extern void	DISP_WriteError(unsigned char * );
extern void	DispMinBalance(void);//显示底金
extern void	DispOver(void);
extern unsigned char	FindMonth(unsigned char * );//查找日期
extern void	LED_Indata(unsigned char bbit,unsigned char aa);  //写入数据
extern unsigned char 	ReadByteFrom1629(void);//倒序接收
extern void 	LED_DispDatas_1629(unsigned char ledx,unsigned char * ptr,unsigned char num);
extern void 	LED_DispDatas(unsigned char bbit,unsigned char * ptr);
extern void 	LED_DispDatas_all(unsigned char * ptr);
extern void	ReakKeySub(void);
extern void 	LED_OneByte_Disp(unsigned char ledx,unsigned char prt);
extern void 	LED_NumFalsh_Disp(unsigned char ledx,unsigned char num,unsigned long delays);
extern void 	Led_Wait_Disp(void);
extern void	Disp_Clr_WhilteNum(void);//显示清空Pos;
extern void	DISP_HARDEErrorSub(uchar );
#endif
