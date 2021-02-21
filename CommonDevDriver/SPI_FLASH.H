#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H	

#define Dummy_Byte 0

extern unsigned char fac_id,dev_id;



void SPI_Flash_Init(void);	        //SPI��ʼ��
u8 SPI_Flash_ReadByte(void);		//flash����������������һ���ֽ�
u8 SPI_Flash_SendByte(u8 byte);		//	FLASH������������������һ���ֽ�


extern void FlashWaitBusy(void);			//Flashæ���
extern void FlashReadID(void);		        //��ȡflashID�ĸ��ֽ�

//void Erase_One_Sector (unsigned long Addr);//FLASH�����

void SPI_Flash_Text(void); //FLASH�Ĳ��Ժ���
extern u8	Check_SPI_FLASH(void);
extern void Flash_Rd_Bytes(unsigned long addr, unsigned char *readbuff, unsigned short BlockSize);
extern void Flash_Write_Bytes(uint32_t addr, u8 *readbuff, unsigned short BlockSize);
extern void Erase_One_Sector(unsigned long a1);
extern void	Erase_Entire_Chip(void);
#endif
