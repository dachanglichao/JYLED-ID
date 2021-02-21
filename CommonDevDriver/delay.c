/**************************************************************************************

**************************************************************************************/

#include "stm32f10x.h"
#include "delay.h"

void  Delay(unsigned int d)
{
	unsigned int x;
	for(x=0;x<1000;x++);
}

void delay_ms(unsigned long i)
{	 		  	  
	unsigned int j;
	for(;i>0;i--)
		for(j=0;j<1000;j++);  	    
} 

