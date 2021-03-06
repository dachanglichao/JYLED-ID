
#include "iic.h"
#include "stm32f10x_gpio.h"
#include "GPIO.H"


/**************************************************************************************/
//驱动中的延时函数，入口参数为0到65535
 void SW_EEPROM_Delay_nus(unsigned short nus)
{
	for(;nus>0;nus--)
	{__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();}
}
 void SW_EEPROM_Delay_nms(unsigned short nms)
{
	for(;nms>0;nms--)
	SW_EEPROM_Delay_nus(1000);
}

//初始化模拟总线
void Init_IIC(void)
{
	SDA_OUT();				   
	CLK_OUT();
}
//产生IIC起始信号
 void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	SW_EEPROM_Delay_nus(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	SW_EEPROM_Delay_nus(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
 void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	SW_EEPROM_Delay_nus(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	SW_EEPROM_Delay_nus(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
 unsigned char IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;//SW_EEPROM_Delay_nus(1);	   
	IIC_SCL=1;//SW_EEPROM_Delay_nus(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
 void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=1;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=1;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
 void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		SW_EEPROM_Delay_nus(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		SW_EEPROM_Delay_nus(2); 
		IIC_SCL=0;	
		SW_EEPROM_Delay_nus(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
 unsigned char IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        SW_EEPROM_Delay_nus(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		SW_EEPROM_Delay_nus(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
} 	  
