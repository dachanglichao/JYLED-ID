
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "stm32f10x.h"
#include "usart.h"


void Delay_Count(__IO uint32_t nCount);
void System_Setup(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);


extern void	Beep_init(void);
extern void	BeepOn(unsigned char num);



#endif 

