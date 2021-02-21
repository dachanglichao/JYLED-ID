 #include "stm32f10x.h"
extern char ISO7816_PSAM(unsigned char SendLen, unsigned char * SendBuf, unsigned char * RcvLen, unsigned char * Rcvdata);
extern   u16 GetU8ToU16(u8 * Buf);
extern unsigned char ReadCardCommonDatas_CPU(void);
extern unsigned char WriteNoNetLimitMoney(unsigned char *buf);
extern unsigned char 	PSAM_Init_Dir(void);//pSAM初始化金融目录 

