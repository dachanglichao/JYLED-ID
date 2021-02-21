#include "74HC595D.H"
void HC595SendData(unsigned char SendVal)//
{  
  unsigned char i;
  for(i=0;i<8;i++) 
   {
			if((SendVal<<i)&0x80) SI_595=1;
			else SI_595=0;
			SCK_595=0;//SCK_595
			//Delay_Count(10);
			SCK_595=1; 
   }
		RCK_595=0;//SCK_595
		//Delay_Count(10);
		RCK_595=1; 
} 




