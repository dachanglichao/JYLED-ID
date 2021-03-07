#ifndef __485_H__
#define __485_H__
#include "stdio.h"
#include "stdint.h"

#define u485_OUT  DK_GPIO_Init(PE,GPIO_Pin_11,Out_PP)
#define u485_RE   PEout(11)=0;
#define u485_TE   PEout(11)=1;
void u485_GPIO_INT(void);
void Usart2ToMP3(void);
void Usart2ToErMa(void);
void SelWifiMode(void);
void mp3play(unsigned char *data);
 //设置音量大小1--4
void setVolume(unsigned char num);
   //设置播放速度1--3
void setPlaySpeed(unsigned char num);
//播放消费额
void playConsumeMoney(float money);
#endif

