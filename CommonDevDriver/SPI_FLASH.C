#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "DK_SPI.H"
#include "SPI_FLASH.H"
#include  "USART.H"
#include <string.h>
#include <AT24C256.h>
#include <delay.h>


unsigned char SST25_buffer[40];
void Erase_One_Sector(unsigned long a1);
extern void USART_PUTS_Arry(unsigned char USARTx,unsigned char *buffer,unsigned int len);
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))
uint8_t TxBuffer1[] = "SPI SST25VF016B Example: This is SPI DEMO, �ն��ϳ�����һ���֣�˵��SST25VF016B�Ķ�д����";
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
/**************************************************************************************

**************************************************************************************/


void Busy_Test(void);   //æ���
void Write_EN(void);  //дʹ��
void Write_DIS(void);  //д��ֹ
void Write_CMD(void); //д״̬
unsigned char Read_CMD(void); //��״̬
void Page_Clr(unsigned long a1);  //ҳ����

/* Select SPI FLASH: ChipSelect pin low  */
#define Select_Flash()      PBout(12)=0                //GPIO_ResetBits(GPIOA, GPIO_Pin_4)
/* Deselect SPI FLASH: ChipSelect pin high */
#define NotSelect_Flash()    PBout(12)=1               //GPIO_SetBits(GPIOA, GPIO_Pin_4)




 static void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
/**************************************************************************************
* ��    ��:    void Write_EN(void)
* ��    ��:    дʹ��
* ��    ��:    
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void Write_EN(void){
	Select_Flash();        //SPI CS ʹ��
	SPI_Flash_SendByte(0x06); //дʹ��ָ��
	NotSelect_Flash();  //SPI CS ����
}


/**************************************************************************************
* ��    ��:    Write_DIS(void)
* ��    ��:    д��ֹ
* ��    ��:    
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void Write_DIS(void){

	Select_Flash();  //SPI CS ʹ��
	SPI_Flash_SendByte(0x04); //д��ָֹ��
	NotSelect_Flash();  //SPI CS ����
	Busy_Test();   //æ���
	
}	


/**************************************************************************************
* ��    ��:    Busy_Test(void)
* ��    ��:    æ���
* ��    ��:    
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void Busy_Test(void){
	unsigned char a=1;
	while((a&0x01)==1) 
	{
		a=Read_CMD();
	}	

}

/**************************************************************************************
* ��    ��:    unsigned char Read_CMD(void)
* ��    ��:    ��״̬�Ĵ���
* ��    ��:    
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
unsigned char Read_CMD(void){
	unsigned char busy;
	Select_Flash();
	SPI_Flash_SendByte(0x05);
	busy = SPI_Flash_ReadByte();
	NotSelect_Flash();
	return(busy);
	
}

/**************************************************************************************
* ��    ��:    SST25_R_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize)
* ��    ��:    ҳ��
* ��    ��:    unsigned long addr--ҳ   unsigned char *readbuff--����   unsigned int BlockSize--����
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void Flash_Rd_Bytes(unsigned long addr, unsigned char *readbuff, unsigned short BlockSize){
	
	unsigned int i=0; 	
	Select_Flash();
	SPI_Flash_SendByte(0x03);
	SPI_Flash_SendByte((addr&0xffffff)>>16);
	SPI_Flash_SendByte((addr&0xffff)>>8);
	SPI_Flash_SendByte(addr&0xff);
	//SPI_Flash_SendByte(0);
	while(i<BlockSize){	
		readbuff[i]=SPI_Flash_ReadByte();		
		i++;
	}
	NotSelect_Flash();	 	
}

/**************************************************************************************
* ��    ��:    SST25_W_BLOCK(uint32_t addr, u8 *readbuff, uint16_t BlockSize)
* ��    ��:    ҳд
* ��    ��:    uint32_t addr--ҳ   u8 *readbuff--����   uint16_t BlockSize--����    
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void Flash_Write_Bytes_onepage(uint32_t addr, u8 *readbuff, unsigned short BlockSize){
	unsigned int i=0,a2;
//	Erase_One_Sector(addr);   								  //ɾ��ҳ		  
//	Write_CMD();
  	Write_EN();	
	Select_Flash();    
	SPI_Flash_SendByte(0x02);
	SPI_Flash_SendByte((addr&0xffffff)>>16);
	SPI_Flash_SendByte((addr&0xffff)>>8);
	SPI_Flash_SendByte(addr&0xff);
 // 	SPI_Flash_SendByte(readbuff[0]);
	//SPI_Flash_SendByte(readbuff[1]);
//	NotSelect_Flash();
//	i=2;
	i = 0;
	while(i<BlockSize){
		//a2=120;
		//while(a2>0) a2--;
		//Select_Flash();
		//SPI_Flash_SendByte(0xad);
		SPI_Flash_SendByte(readbuff[i++]);
		//SPI_Flash_SendByte(readbuff[i++]);
		//NotSelect_Flash();
	}
	
	a2=100;
	while(a2>0) a2--;
	NotSelect_Flash();
	Write_DIS();	
	//Select_Flash();	
	Busy_Test();
}


void Flash_Write_Bytes(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite)   
{  
	u16 pageremain;   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//���Ҫд����ֽ���С����ڵ�ҳʣ����ֽ�����������256���ֽ�
	while(1)
	{   
		Flash_Write_Bytes_onepage(WriteAddr,pBuffer,pageremain);//Ҫд����ֽ���С����ڵ�ҳʣ����ֽ���ֱ��д
		if(NumByteToWrite==pageremain)break;//д�������

		else //NumByteToWrite>pageremain�����Ҫд������ݴ��ڵ�ҳʣ����ֽ�����
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;

			NumByteToWrite-=pageremain;  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite;  //����256���ֽ���
		}
	}   
} 

/**************************************************************************************
* ��    ��:    Page_Clr(unsigned long a1)
* ��    ��:    ҳ����
* ��    ��:    unsigned long a1--ҳ   
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void Erase_One_Sector(unsigned long a1){
	//Write_CMD();
	Write_EN();     
	Select_Flash();	  
	SPI_Flash_SendByte(0x20);
	SPI_Flash_SendByte((a1&0xffffff)>>16);          //addh
	SPI_Flash_SendByte((a1&0xffff)>>8);          //addl 
	SPI_Flash_SendByte(a1&0xff);                 //wtt
	NotSelect_Flash();
	Busy_Test();
	
}

/**************************************************************************************
* ��    ��:    Page_Clr(unsigned long a1)
* ��    ��:    ��������оƬ
* ��    ��:    unsigned long a1--ҳ   
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void	Erase_Entire_Chip(void)  //��������оƬ
{
	//Write_CMD();
	Write_EN();     
	Select_Flash();	  
	SPI_Flash_SendByte(0xc7);   
	delay_ms(200);              //wtt
	NotSelect_Flash();
	Busy_Test();
	delay_ms(200);
	
}

/**************************************************************************************
* ��    ��:    FlashReadID(void)
* ��    ��:    �������뼰�ͺŵĺ���
* ��    ��:    
* �� �� ֵ:    
* ��    �ߣ�   D&K
**************************************************************************************/ 
void FlashReadID(void)
{
//	char fac_id,dev_id;
	Select_Flash();	
  	SPI_Flash_SendByte(0x90);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	//fac_id = 0;
  //	fac_id= SPI_Flash_ReadByte();		          //BFH: ������SST
//	dev_id = 0;
//	dev_id= SPI_Flash_ReadByte();	              //41H: �����ͺ�SST25VF016B    
  	NotSelect_Flash();	
}

/**************************************************************************************
* ��    ��:    SPI_Flash_Init(void)
* ��    ��:    ��ʼ��SPI FLASH ����
* ��    ��:    
* �� �� ֵ:    
* ��    �ߣ�   D&K
**************************************************************************************/ 
void SPI_Flash_Init(void)
{
  DK_SPI_Init(2);
}

/**************************************************************************************
* ��    ��:    SPI_Flash_SendByte(u8 byte)
* ��    ��:    ��SPI FLASH ��ȡһ���ֽ�  ����ʹ����Start_Read_Sequenceǰ
* ��    ��:    
* �� �� ֵ:    
* ��    �ߣ�   D&K
**************************************************************************************/ 
u8 SPI_Flash_ReadByte(void)
{
  return (SPI_Flash_SendByte(Dummy_Byte));
}

/**************************************************************************************
* ��    ��:    SPI_Flash_SendByte(u8 byte)
* ��    ��:    ͨ���ڲ�SPI����һ���ֽڲ���SPI���߷����ֽ�
* ��    ��:    �ֽ�
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
u8 SPI_Flash_SendByte(u8 byte)
{
//   while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); //SPI1 �ȴ���������
//   SPI_I2S_SendData(SPI1, byte); //SPI1 ��������
// 	
//   while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//SPI1 �ȴ���������
//   return SPI_I2S_ReceiveData(SPI1); //��SPI1 �������� ���ز��� 	
	
	   return DK_SPI1_S_R_Byte(2,byte);
}

//FLASHӲ���Լ�
u8	Check_SPI_FLASH(void)
{	
	char Fac_id,Dev_id ;
	Select_Flash();	
  	SPI_Flash_SendByte(0x90);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	delay_ms(100);
  	Fac_id= SPI_Flash_ReadByte();		          //BFH: ������SST
	Dev_id= SPI_Flash_ReadByte();	              //8dH: �����ͺ�SST25VF04B    
  	NotSelect_Flash();	
	if(Fac_id == 0xef)
	{
		return 0;
	}
	else
	return 1;	
}