
 #include "stm32f10x.h"
extern void PsamGpioInt(void) ;
extern u8 PsamReceive(u8,u8 *RevBuf,u16 RecLen,u16 Timeout_Ms);
extern void PsamSend(u8,u8 *SendBuf,u16 SendLen);
extern void PsamReset(void);
extern void ISO7816Test(void);
