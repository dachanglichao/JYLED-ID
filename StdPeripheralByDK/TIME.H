#ifndef __TIME_H
#define __TIME_H

void DK_TIMER_Init(u8 TIMERx);
void DK_TIM4_PWM_Init(unsigned char chan, unsigned int pul);
void DK_TIM3_INT_SER(void);
void DK_TIM4_INT_SER(void);
void DK_TIM_OPen_INT(u8 TIMERx);
void TIME3_RELOAD(u8 times);
void TIME4_RELOAD(u8 times);
void TIME_COUNT_RELOAD(u8 times);

#endif
