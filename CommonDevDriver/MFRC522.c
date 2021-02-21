#include  "cpu.h"
#include  "os_cpu.h"
#include  "app_cfg.h"
#include  "ucos_ii.h"
#include  "include.h"

//�ܽ�����

char TypeA_PiccRATS(void);
u8 MInfo_collPos;
//test
u8  irq_regdata;
u16 wait_count;
u8  error_regdata;
u8  last_bitsdata;
//AD lc COPY 51POS
#define		_RC531BufSize		66 //ģ���������������(=ʵ�ʵ����ݳ���+2) //h531
static MfCmdInfo MInfo ;
uchar  	MRcvBuffer[80];//h531
uchar  	MSndBuffer[80];//h531
unsigned char  T_CL_Block_number; //T=CLЭ��Ŀ�� //h531

const unsigned char FSCI_Code_Tab[16]= {16,24,32,40,48,64,96,128,255,255,255,255,255,255,255,255};//CPU��֡�����
unsigned char Maximum_Frame_Size; //CPU��֡��������� //h531
extern	char 	ISO7816_TRANSCEIVE(unsigned char, void *, unsigned char *, void *);//7816����ͨѶ

unsigned char check_RFCPU_ResetInfo(unsigned char * RcvLen, void * Rcvdata);
//�����Ƿ��п�

extern u8 isHaveEWM;
uchar	RequestCard(void)
{
	if(isHaveEWM) return 0;
	return check_RFCPU_ResetInfo(CPU_RcLen,CPU_CommBuf);//����λ��Ϣ
}

//
void Delay1_us(u16 count)
{
  u16 i;
  
  for(i=0;i<count;i++)
  {
		u8 us=12;
		
		while(us--);
  }
}

//
void SPI1_Send(u8 val)  
{ 
	RC522_WriteByte(val);
}
//
u8 SPI1_Receive(void)  
{ 
	u8 temp; 
	temp = RC522_ReadByte();
  return temp;  
}
//������������MFRC522��ĳһ�Ĵ���дһ���ֽ�����
//���������addr--�Ĵ�����ַ��val--Ҫд���ֵ
void Write_MFRC522(u8 addr, u8 val) 
{
	//��ַ��ʽ��0XXXXXX0  
	RC522_XCS_L();  
	SPI1_Send((addr<<1)&0x7E);  
	SPI1_Send(val);    
	RC522_XCS_H() ;

}
//������������MFRC522��ĳһ�Ĵ�����һ���ֽ�����
//���������addr--�Ĵ�����ַ
//�� �� ֵ�����ض�ȡ����һ���ֽ����� 
u8 Read_MFRC522(u8 addr) 
{  
	u8 val;
	//��ַ��ʽ��1XXXXXX0   
	RC522_XCS_L();     
	SPI1_Send(((addr<<1)&0x7E)|0x80);   
	val=SPI1_Receive();    
	RC522_XCS_H() ;
	//   
	return val;  

}
//������������ֻ���ܶ�дλ��Ч
//������������RC522�Ĵ���λ
//���������reg--�Ĵ�����ַ;mask--��λֵ
void SetBitMask(u8 reg, u8 mask)   
{     
	u8 tmp=0;  
	tmp=Read_MFRC522(reg);     
	Write_MFRC522(reg,tmp|mask);  // set bit mask 
}
//������������RC522�Ĵ���λ
//���������reg--�Ĵ�����ַ;mask--��λֵ
void ClearBitMask(u8 reg, u8 mask)   
{     
	u8 tmp=0;
	//     
	tmp=Read_MFRC522(reg);     
	Write_MFRC522(reg,tmp&(~mask));  //clear bit mask 
}
//������������������,ÿ��������ر����߷���֮��Ӧ������1ms�ļ��
void AntennaOn(void) 
{  
	u8 temp;
	//   
	temp=Read_MFRC522(TxControlReg);  
	if ((temp&0x03)==0)  
	{   
		SetBitMask(TxControlReg,0x03);  
	}
}
//�����������ر�����,ÿ��������ر����߷���֮��Ӧ������1ms�ļ��
void AntennaOff(void) 
{  
	ClearBitMask(TxControlReg,0x03);
}
void ResetInfo(void)
{
	MInfo.cmd            = 0;
	MInfo.status         = MI_OK;
	MInfo.irqSource      = 0;
	MInfo.nBytesSent     = 0;
	MInfo.nBytesToSend   = 0;
	MInfo.nBytesReceived = 0;
	MInfo.nBitsReceived  = 0;
	MInfo.collPos        = 0;
}
//������������λMFRC522
void MFRC522_Reset(void) 
{ 	
	//�⸴λ���Բ���
	RC522_XRESET_H();
	Delay1_us(10);
	RC522_XRESET_L();
	Delay1_us(10);
	RC522_XRESET_H();
	Delay1_us(10); 

	Write_MFRC522(0x01,0x0F); //soft reset
  while(Read_MFRC522(0x27) != 0x88); //wait chip start ok	
	//�ڸ�λ   
	Write_MFRC522(CommandReg, PCD_RESETPHASE); 
	Write_MFRC522(TModeReg,0x8D);

}
//
void MFRC522_Initializtion(void) 
{	
	 
		MFRC522_Reset();         
	//Timer: TPrescaler*TreloadVal/6.78MHz = 0xD3E*0x32/6.78=25ms     
	Write_MFRC522(TModeReg,0x8D);				//TAuto=1Ϊ�Զ�����ģʽ����ͨ��Э��Ӱ�򡣵�4λΪԤ��Ƶֵ�ĸ�4λ
	//aa = Read_MFRC522(TModeReg);
	//Write_MFRC522(TModeReg,0x1D);				//TAutoRestart=1Ϊ�Զ����ؼ�ʱ��0x0D3E��0.5ms�Ķ�ʱ��ֵ//test    
	Write_MFRC522(TPrescalerReg,0x3E); 	//Ԥ��Ƶֵ�ĵ�8λ     
	Write_MFRC522(TReloadRegL,0x52);		//�������ĵ�8λ                
	Write_MFRC522(TReloadRegH,0x00);		//�������ĸ�8λ       
	Write_MFRC522(TxAutoReg,0x40); 			//100%ASK     
	Write_MFRC522(ModeReg,0x3D); 				//CRC��ʼֵ0x6363
	Write_MFRC522(CommandReg,0x00);			//����MFRC522  
	Write_MFRC522(RFCfgReg, 0x4F);    //RxGain = 48dB���ڿ���Ӧ����      
	AntennaOn();          							//������        							//������ 
}
//����������RC522��ISO14443��ͨѶ
//���������command--MF522������
//					sendData--ͨ��RC522���͵���Ƭ������
//					sendLen--���͵����ݳ���
//					BackData--���յ��Ŀ�Ƭ��������
//					BackLen--�������ݵ�λ����
//�� �� ֵ���ɹ�����MI_O
u8 MFRC522_ToCard(u8 command, u8 *sendData, u8 sendLen, u8 *backData, u16 *backLen) 
{
	u8  status=MI_ERR;
	u8  irqEn=0x00;
	u8  waitIRq=0x00;
	u8  lastBits;
	u8  n;
	u16 i;
	//������Ԥ���жϲ���
	switch (command)     
	{         
		case PCD_AUTHENT:  		//��֤����   
			irqEn 	= 0x12;			//    
			waitIRq = 0x10;			//    
			break;
		case PCD_TRANSCEIVE: 	//����FIFO������      
			irqEn 	= 0x77;			//    
			waitIRq = 0x30;			//    
			break;      
		default:    
			break;     
	}
	//
	Write_MFRC522(ComIEnReg, irqEn|0x80);		//�����ж�����     
	ClearBitMask(ComIrqReg, 0x80);  				//��������ж�����λ               	
	SetBitMask(FIFOLevelReg, 0x80);  				//FlushBuffer=1, FIFO��ʼ��
	Write_MFRC522(CommandReg, PCD_IDLE); 		//ʹMFRC522����   
	//��FIFO��д������     
	for (i=0; i<sendLen; i++)
		Write_MFRC522(FIFODataReg, sendData[i]);
	//ִ������
	//delay_ms(1);
	Write_MFRC522(CommandReg, command);
//	delay_ms(1);
	//���߷�������     
	if (command == PCD_TRANSCEIVE)					//����ǿ�Ƭͨ�����MFRC522��ʼ�����߷�������      
		SetBitMask(BitFramingReg, 0x80);  		//StartSend=1,transmission of data starts    
	//�ȴ������������     
	i = 100; //i����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms     
	do      
	{  
			OSTimeDlyHMSM(0,0,0,1); //���������1S		
		n = Read_MFRC522(ComIrqReg);
		//__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		//irq_rgdata=n;	//test         
		i--;
	
		//wait_count=i;		//test		     
	}while ((i!=0) && !(n&0x01) && !(n&waitIRq));	//��������˳�n=0x64
	//ֹͣ����
	if(n>0x64)
	{
	ClearBitMask(BitFramingReg, 0x80);   		//StartSend=0	
	}
	ClearBitMask(BitFramingReg, 0x80);   		//StartSend=0
	//�����25ms�ڶ�����
	if (i != 0)     
	{            
		if(!(Read_MFRC522(ErrorReg) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr         
		{   
			status = MI_OK;         
			if (n & irqEn & 0x01)			//                  
				status = MI_NOTAGERR;		//
			//
			if (command == PCD_TRANSCEIVE)             
			{                 
				n = Read_MFRC522(FIFOLevelReg);		//n=0x02                
				lastBits = Read_MFRC522(ControlReg) & 0x07;	//lastBits=0               
				if (lastBits!=0)                         
					*backLen = (n-1)*8 + lastBits; 
				else
					*backLen = n*8;									//backLen=0x10=16
				//
				if (n == 0)                         
				 	n = 1;                        
				if (n > MAX_LEN)         
				 	n = MAX_LEN;
				//
				for (i=0; i<n; i++)                 
					backData[i] = Read_MFRC522(FIFODataReg); 
			}
			//
			status = MI_OK;		
		}
		else
			status = MI_ERR;
	}	
	//
	Write_MFRC522(ControlReg,0x80);				//timer stops     
	Write_MFRC522(CommandReg, PCD_IDLE);	//
	//
	//
	return status;
}
//����������Ѱ������ȡ�����ͺ�
//���������reqMode--Ѱ����ʽ
//					TagType--���ؿ�Ƭ����
//					0x4400 = Mifare_UltraLight
//					0x0400 = Mifare_One(S50)
//					0x0200 = Mifare_One(S70)
//					0x0800 = Mifare_Pro(X)
//					0x4403 = Mifare_DESFire
//�� �� ֵ���ɹ�����MI_OK	
u8 Mf500PiccRequest(u8 reqMode, u8 *TagType)
{  
	u8  status;    
	u16 backBits;   //���յ�������λ��
	//   
	Write_MFRC522(BitFramingReg, 0x07);  //TxLastBists = BitFramingReg[2..0]   
	TagType[0] = reqMode;  
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits); 
	// 
	if ((status != MI_OK) || (backBits != 0x10))  
	{       
		status = MI_ERR;
	}
	//  
	return status; 
}
//��������������ͻ��⪡��ȡѡ�п�Ƭ�Ŀ����к�
//���������serNum--����4�ֽڿ����к�,��5�ֽ�ΪУ���ֽ�
//�� �� ֵ���ɹ�����MI_OK
u8 MFRC522_Anticoll(u8 *serNum) 
{     
	u8  status;     
	u8  i;     
	u8  serNumCheck=0;     
	u16 unLen;
	u8  CardUID[8];
	//           
	ClearBitMask(Status2Reg, 0x08);  			//TempSensclear     
	ClearBitMask(CollReg,0x80);   				//ValuesAfterColl  
	Write_MFRC522(BitFramingReg, 0x00);  	//TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL1;     
	serNum[1] = 0x20;     
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, CardUID, &unLen);
	//      
	if (status == MI_OK)
	{   
		//У�鿨���к�   
		for(i=0;i<4;i++)   
			serNumCheck^=CardUID[i];
		//
		if(serNumCheck!=CardUID[i])        
			status=MI_ERR;
		memcpy(serNum,CardUID,4);
	}
	SetBitMask(CollReg,0x80);  //ValuesAfterColl=1
	//      
	return status;
}
//������������MF522����CRC
//���������pIndata--Ҫ����CRC�����ݪ�len--���ݳ��Ȫ�pOutData--�����CRC���
void CalulateCRC(u8 *pIndata, u8 len, u8 *pOutData) 
{     
	u16 i;
	u8  n;
	//      
	ClearBitMask(DivIrqReg, 0x04);   			//CRCIrq = 0     
	SetBitMask(FIFOLevelReg, 0x80);   		//��FIFOָ��     
	Write_MFRC522(CommandReg, PCD_IDLE);   
	//��FIFO��д������      
	for (i=0; i<len; i++)
		Write_MFRC522(FIFODataReg, *(pIndata+i));
	//��ʼRCR����
	Write_MFRC522(CommandReg, PCD_CALCCRC);
	//�ȴ�CRC�������     
	i = 1000;     
	do      
	{         
		n = Read_MFRC522(DivIrqReg);         
		i--;   
	}while ((i!=0) && !(n&0x04));   //CRCIrq = 1
	//��ȡCRC������     
	pOutData[0] = Read_MFRC522(CRCResultRegL);     
	pOutData[1] = Read_MFRC522(CRCResultRegH);
	Write_MFRC522(CommandReg, PCD_IDLE);
}
//����������ѡ������ȡ���洢������
//���������serNum--���뿨���к�
//�� �� ֵ���ɹ����ؿ�����
u8 MFRC522_SelectTag(u8 *serNum) 
{     
	u8  i;     
	u8  status;     
	volatile u8  size;     
	u16 recvBits;     
	u8  buffer[9];
	//     
	buffer[0] = PICC_ANTICOLL1;	//��ײ��1     
	buffer[1] = 0x70;
	buffer[6] = 0x00;						     
	for (i=0; i<4; i++)					
	{
		buffer[i+2] = *(serNum+i);	//buffer[2]-buffer[5]Ϊ�����к�
		buffer[6]  ^=	*(serNum+i);	//��У����
	}
	//
	CalulateCRC(buffer, 7, &buffer[7]);	//buffer[7]-buffer[8]ΪRCRУ����
	ClearBitMask(Status2Reg,0x08);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
	//
	if ((status == MI_OK) && (recvBits == 0x18))    
		size = buffer[0];     
	else    
		size = 0;
	//	     
	return status; 
}
//������������֤��Ƭ����
//���������authMode--������֤ģʽ
//					0x60 = ��֤A��Կ
//					0x61 = ��֤B��Կ
//					BlockAddr--���ַ
//					Sectorkey--��������
//					serNum--��Ƭ���кŪ�4�ֽ�
//�� �� ֵ���ɹ�����MI_OK
u8 MFRC522_Auth(u8 authMode, u8 BlockAddr, u8 *Sectorkey, u8 *serNum) 
{     
	u8  status;     
	u16 recvBits;     
	u8  i;  
	u8  buff[12];    
	//��֤ģʽ+���ַ+��������+�����к�     
	buff[0] = authMode;		//��֤ģʽ     
	buff[1] = BlockAddr;	//���ַ     
	for (i=0; i<6; i++)
		buff[i+2] = *(Sectorkey+i);	//��������
	//
	for (i=0; i<4; i++)
		buff[i+8] = *(serNum+i);		//�����к�
	//
	status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
	//      
	if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
		status = MI_ERR;
	//
	return status;
}
//������������������
//���������blockAddr--���ַ;recvData--�����Ŀ�����
//�� �� ֵ���ɹ�����MI_OK
u8 MFRC522_Read(u8 blockAddr, u8 *recvData) 
{     
	u8  status;     
	u16 unLen;
	//      
	recvData[0] = PICC_READ;     
	recvData[1] = blockAddr;     
	CalulateCRC(recvData,2, &recvData[2]);     
	status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
	//
	if ((status != MI_OK) || (unLen != 0x90))
		status = MI_ERR;
	//
	return status;
}
//����������д������
//���������blockAddr--���ַ;writeData--���д16�ֽ�����
//�� �� ֵ���ɹ�����MI_OK
u8 MFRC522_Write(u8 blockAddr, u8 *writeData) 
{     
	u8  status;     
	u16 recvBits;     
	u8  i;  
	u8  buff[18];
	//           
	buff[0] = PICC_WRITE;     
	buff[1] = blockAddr;     
	CalulateCRC(buff, 2, &buff[2]);     
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
	//
	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
		status = MI_ERR;
	//
	if (status == MI_OK)     
	{         
		for (i=0; i<16; i++)  //��FIFOд16Byte����                     
			buff[i] = *(writeData+i);
		//                     
		CalulateCRC(buff, 16, &buff[16]);         
		status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);           
		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))               
			status = MI_ERR;         
	}          
	return status;
}
//�������������Ƭ��������״̬
void MFRC522_Halt(void) 
{    
	u16 unLen;     
	u8  buff[4];
	//       
	buff[0] = PICC_HALT;     
	buff[1] = 0;     
	CalulateCRC(buff, 2, &buff[2]);       
	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}
//


///////////////////////////////////////////////////////////////////////
//                      C O D E   K E Y S
///////////////////////////////////////////////////////////////////////
u8 Mf500HostCodeKey(  unsigned char * uncoded, unsigned char *	coded)   
{
//	uchar   status = MI_OK;
	u8   cnt = 0;
	u8   ln  = 0;     // low nibble
	u8   hn  = 0;     // high nibble
   	for (cnt = 0; cnt < 6; cnt++)
   	{
		ln = uncoded[cnt] & 0x0F;
		hn = uncoded[cnt] >> 4;
		coded[cnt * 2 + 1]     =  (~ln << 4) | ln;
		coded[cnt * 2 ] =  (~hn << 4) | hn;
  	}
   	return MI_OK;
}

/*------------------------------------------------
���RF CPU����λ��Ϣ
type_A Ѱ��
------------------------------------------------*/
unsigned char check_RFCPU_ResetInfo(unsigned char * RcvLen, void * Rcvdata)
{
	unsigned char i;
	unsigned char status;
	

	MFRC522_Initializtion();
	
	OSTimeDlyHMSM(0,0,0,10); //���������1S
	//DeSelect();
	
	for (i=0;i<3;i++)
	{	TypeA_Sort = 0xff;
		
		status=Mf500PiccRequest(0x52,CardType);

		if (!status)
		{
			if ((CardType[0]==0x04 || CardType[0]==0x02)&& !CardType[1])//04-s50;02-s70
				TypeA_Sort=0;//M1��
			else if (CardType[0]==0x08 && !CardType[1])//CPU
				TypeA_Sort=0;//CPU��
			else
				return CARD_NOCARD;

			status=MFRC522_Anticoll(CardSerialNum);	//��ײ����	
			if (!status)
			{
				status=MFRC522_SelectTag(CardSerialNum);	//ѡ��
				//status =0;
				if (!status)
					break;
			}	
		}
		if(status)
		{
//			MFRC522_Initializtion();
//			OSTimeDlyHMSM(0,0,0,10); //���������1S
		}
	}

	if (status)//�޿�
		return CARD_NOCARD; 
	//�п�
	if (!TypeA_Sort)//04-s50;02-s70
		return	CARD_OK;
	else //CPU
	{
		status=TypeA_PiccRATS();//�˴�������
		if (!status)
		{		    
			memcpy(Rcvdata,MRcvBuffer,MInfo.nBytesReceived);
			*RcvLen=MInfo.nBytesReceived;
			i=MRcvBuffer[1]&0x0f;
			do//����TYPE_A CPU��֡���������
			{
				Maximum_Frame_Size=FSCI_Code_Tab[i];
				i--;
			}while (Maximum_Frame_Size>_RC531BufSize);
		}

		if (status)//�޿�
			return CARD_NOCARD;
	    else
			return CARD_OK;
	}
}


/*----------------------------------------------------------
TYPE A��B ����MSndBuffer����������(��ڣ����ͳ���)(����0 is ok)
----------------------------------------------------------*/
char TypeAB_PiccTRANSCEIVE(unsigned char SendLen)
{
	u8 status = MI_OK;
	ResetInfo();
	CalulateCRC(MSndBuffer, SendLen, &MSndBuffer[SendLen]);  
	MInfo.nBytesToSend  = SendLen+2;	
	status = MFRC522_ToCard(PCD_TRANSCEIVE, MSndBuffer, MInfo.nBytesToSend, MRcvBuffer, &MInfo.nBitsReceived); 

	//return status;
}
/*----------------------------------------------------------
����TYPE A RATS(��ڣ���)(����0 is ok)
----------------------------------------------------------*/
char TypeA_PiccRATS(void)
{
	unsigned char  status;

	MSndBuffer[0] = 0xe0; //select_code;
	status=9;
//	srMode=2;
	do//����TYPE_A CPU��֡���������
	{
		status--;
		MSndBuffer[1]=FSCI_Code_Tab[status];
	}while (MSndBuffer[1]>=_RC531BufSize);
	MSndBuffer[1]=status<<4; 
	status = TypeAB_PiccTRANSCEIVE(2);
	T_CL_Block_number=0;
	return status;
}
/*----------------------------------------------------------
���Ͳ�����7816�����ݴ�(��ڣ�����ָ�룬���ͳ���)(����0 is ok)
----------------------------------------------------------*/
char ISO7816_TRANSCEIVE(unsigned char SendLen, void * SendBuf, unsigned char * RcvLen, void * Rcvdata)
{
	char  status;
	char  i;
	unsigned char TotalSendLen; //�ۺϷ��ͳ���
	unsigned char OnceSendLen; //���η��ͳ���
	if (SendLen>=178)//������󻺴���
 	{
		return 0x50; //Ҫ���͵����ݹ���
	}
	TotalSendLen=0;
	do
	{
		i=0;
		//-------------------------------------------------
		//zjx_add_20111130

		if(!SendLen)//Deselect
		{
			MSndBuffer[0]=0xc2;
		}
		else//I-block
		{
			MSndBuffer[0]=0x02;	
		}
		MSndBuffer[0]|=T_CL_Block_number;
		//-------------------------------------------------
		if ((SendLen-TotalSendLen)>(Maximum_Frame_Size-5))
		{
			MSndBuffer[0]|=0x10;
			OnceSendLen=Maximum_Frame_Size-5;
		}
		else
		{
			OnceSendLen=SendLen-TotalSendLen;
		}
		
		MSndBuffer[0] |=0x08;  //����CID
		MSndBuffer[++i] = 0x00;  //CID  ...Ϊ����TF COS����

		TotalSendLen+=OnceSendLen;
		memcpy(&MSndBuffer[++i],SendBuf,OnceSendLen);
		(unsigned char*)SendBuf+=OnceSendLen;
		status = TypeAB_PiccTRANSCEIVE(OnceSendLen+i);
		MInfo.nBytesReceived =MInfo.nBitsReceived>>3;
		T_CL_Block_number ^= 0x01;
		if ((MSndBuffer[0]&0x10)==0x00) break;//�ֶη��ͽ���
		if (!status)
		{
			if ((MRcvBuffer[0]&0xf0)!=0xa0)//���ֶ�ȷ��(R-block)
				return 0x51;//�ֶη���δ�õ�ȷ��
		}
		else
			return status;//���ʹ���
	}while(1);

	*RcvLen=0;
	
	do
	{
		if (!status)
		{
			//====================================================================
			//zjx_change_201111130

			if((MRcvBuffer[0]&0xf0)==0xf0) //WTX
			{
				if(!i)	
					return	0x53;
				i--;   
				memcpy(MSndBuffer,MRcvBuffer,3);
				MRcvBuffer[0]=0;
				status = TypeAB_PiccTRANSCEIVE(3);//����ͨѶʱ����չ
				MInfo.nBytesReceived =MInfo.nBitsReceived>>3;			
			}
			else if((MRcvBuffer[0]&0xf0)==0xc0)//Deselect
			{
				* RcvLen=0;
				return	0;
			}
			else if(MRcvBuffer[0] && (MRcvBuffer[0]&0xc0)==0)//��Ϣ���� 
			{
				if ((*RcvLen+MInfo.nBytesReceived-1)>_RC531BufSize)
				{
					return (0x52);//�ֶν��յ����ݹ���
				}
				if(MInfo.nBytesReceived>=2)
				{
					memcpy(*RcvLen+(unsigned char*)Rcvdata,&MRcvBuffer[2],MInfo.nBytesReceived-2);
					*RcvLen+=MInfo.nBytesReceived-2;
				}

				if ((MRcvBuffer[0]&0xf0)==0) //���һ֡��Ϣ
					break;
				else if ((MRcvBuffer[0]&0xf0)==0x10)//�к�����Ϣ
				{
					MSndBuffer[0]=((MRcvBuffer[0]&0x0f)|0xa0)^0x01;//���ͷֶν���ȷ��
					MSndBuffer[0] |=0x08;  //����CID
					MSndBuffer[1] = 0x00;  //CID  ...Ϊ����TF COS����
					status = TypeAB_PiccTRANSCEIVE(2);
					MInfo.nBytesReceived =MInfo.nBitsReceived>>3;
				}
			}
			else 
				return 0xff;
			//===================================================================
		}
		else
			return status;
	}while(1);

	return status;
}

//522�Լ�ĺ����ɹ�����0��ʧ�ܷ���1
char CV522Pcd_ResetState(void)
{
	char aa;
	  
	MFRC522_Reset();         
	//Timer: TPrescaler*TreloadVal/6.78MHz = 0xD3E*0x32/6.78=25ms     
	Write_MFRC522(TModeReg,0x8D);				//TAuto=1Ϊ�Զ�����ģʽ����ͨ��Э��Ӱ�򡣵�4λΪԤ��Ƶֵ�ĸ�4λ
	aa = Read_MFRC522(TModeReg);
	if(aa == 0x8d)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}