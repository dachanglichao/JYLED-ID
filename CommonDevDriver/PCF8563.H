#ifndef __PCF8563_H_
#define __PCF8563_H_	

extern unsigned char		bitSysTimeDisable;
unsigned char PCF8563_set(unsigned char year,unsigned char month,unsigned char day,unsigned char hour,unsigned char minute,unsigned char second,unsigned char date);
unsigned char PCF8563_read(unsigned char *time);
unsigned char PCF8563_CloseAlarm(void);
unsigned char PCF8563_SetAlarm(unsigned char time,unsigned char set);
void PCF8563_Init(void);
void PCF8563_INT_SERVER_FUN(void);
unsigned char PCF8563_ClearINT(void);
extern void Set_Sysdate(unsigned char * ptr);
extern void Read_Sysdate(unsigned char *ptr);
extern void Init_SWIIC(void);
extern  unsigned char IIC_ReadBuf(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead);
extern  unsigned char IIC_WriteBuf(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite);

#endif

