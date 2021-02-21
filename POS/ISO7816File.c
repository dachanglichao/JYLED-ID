//#include  "cpu.h"
//#include  "os_cpu.h"
//#include  "app_cfg.h"
//#include  "ucos_ii.h"
//#include  "include.h"


// extern	char 	ISO7816_TRANSCEIVE(unsigned char, void *, unsigned char *, void *);//7816数据通讯
//uchar     ComChallenge[5]={0x00,0x84,0x00,0x00,0x04};//取随机数
////目标卡选择
//#define CPUtarget  0
//#define PSAMtarget 1

////函数声明
//uint 	IC7816_relay_station(uchar,uchar, void *, uchar *, void *);//IC7816中转发送接收
//uchar 	CPU_Get_Challenge(uchar *, uchar *);//终端从卡上读取随机数
//uint 	CPU_Select_File_AID(uchar, uchar * ,uchar *,uchar *);//AID选择文件

////uint 	CPU_Select_File_DF(bit, uchar *,uchar *);//DF选择文件

//uint 	CPU_Read_Binary(uchar,uchar,uchar,uchar *);




////----------------------------------------------------------
////通用函数：IC7816中转发送接收
////入口：Sort：0--CPU卡  1--PSAM卡	
////	SendLen：发送长度
////	SendBuf：发送指针
////	 RcvLen：接收长度指针
////	 RcvBuf：接收数据指针	
////出口：结果代码
////----------------------------------------------------------
//uint IC7816_relay_station(uchar Sort, uchar SendLen, void * SendBuf, uchar * RcvLen, void * RcvBuf)
//{
//	char  status;
//	uint   ret7816;
//	
//	if(!Sort)
//		status=ISO7816_TRANSCEIVE(SendLen, SendBuf, RcvLen, RcvBuf);
//	else
//		status=ISO7816_PSAM(SendLen, SendBuf, RcvLen, RcvBuf);
//	
//	if(!status)
//	{
//		if(!Sort)
//		*RcvLen-=4;
//		else
//		*RcvLen-=2;
//		ret7816=GetU16_HiLo(&((uchar*)RcvBuf)[*RcvLen]);
//		return ret7816;
//	}
//	else
//		return (0x5000+status);
//	
//}

////----------------------------------------------------------
////终端从卡上读取随机数
////命令：00 84 00 00 
////用此命令模拟卡是否存在
////------------------------------------------------------------

//uchar CPU_Get_Challenge(uchar * RcvLen, uchar * RcvBuf)
//{
//    uint 	 sw1sw2=0;
//	sw1sw2=IC7816_relay_station(0,5,ComChallenge,RcvLen,RcvBuf);
//	if(sw1sw2!=0x9000)
//		return	CARD_NOCARD;
//	else
//		return	CARD_OK;
//}

////----------------------------------------------------------
////通过应用标识符来选择文件
////----------------------------------------------------------
//uint CPU_Select_File_AID(uchar Sort, uchar * FileAID,uchar * RcvLen, uchar * RcvBuf)
//{
//	uchar SendBuf[7];

//	SendBuf[0]=0x00;//CLA
//	SendBuf[1]=0xA4;//INS
//	SendBuf[2]=0x00;//P1
//	SendBuf[3]=0x00;//P2
//	SendBuf[4]=0x02;//Lc
//	memcpy(SendBuf+5,FileAID,2);
//	
//	return IC7816_relay_station(Sort,7, SendBuf, RcvLen, RcvBuf);
//}

////----------------------------------------------------------
////通过应用标识符来选择文件
////----------------------------------------------------------
//uint CPU_Select_DKFile_AID(uchar Sort, uchar * FileAID,uchar * RcvLen, uchar * RcvBuf)
//{
//	uchar SendBuf[14];

//	SendBuf[0]=0x00;//CLA
//	SendBuf[1]=0xA4;//INS
//	SendBuf[2]=0x04;//P1
//	SendBuf[3]=0x00;//P2
//	SendBuf[4]=0x09;//Lc
//	memcpy(SendBuf+5,FileAID,9);
//	
//	return IC7816_relay_station(Sort,14, SendBuf, RcvLen, RcvBuf);
//}


////zz+e
////---------------------------------------------------------------------------------
////通过短文件标识，0为读出所有文件内容
////FileSFI:短文件标识
////---------------------------------------------------------------------------------
//uint CPU_Read_Binary(uchar Sort,uchar FileSFI,uchar len,uchar * RcvBuf)
//{
//	uchar  SendBuf[5];
//	uchar  RcvLen;
//	uint    ret;
//	
//	SendBuf[0]=0x00;//CLA
//	SendBuf[1]=0xB0;//INS
//	SendBuf[2]=(FileSFI&0x1f)|0x80 ;//P1
//	SendBuf[3]=0x00;//P2
//	SendBuf[4]=len;
//	ret=IC7816_relay_station(Sort,5,SendBuf, &RcvLen, RcvBuf);
//	if((ret&0xff00)==0x6c00)
//	{
//		SendBuf[4]=(uchar)ret;
//		return	IC7816_relay_station(Sort,5,SendBuf, &RcvLen, RcvBuf);
//	}
//	else
//		return ret;	
//}

////-----------------------------------------------------------------
////下面的4个基本函数是PSAM专有的,DES初始化、DES计算在读写器上有应用
////包括：DES初始化、DES计算、MAC1、MAC2
////pSAM初始化金融目录
////------------------------------------------------------------------
//uchar PSAM_Init_Dir(void)
//{

//	uint 	sw1sw2=0;  
//	uchar	FileAID[20];

//	FileAID[0]=0x3f;
//	FileAID[1]=0x00;
//	sw1sw2=CPU_Select_File_AID(1,FileAID,CPU_RcLen,CPU_CommBuf); //选择主文件 
//	if(sw1sw2!=0x9000)
//		return	PSAM_FILEPARA_ERR;
//	delay_ms(100);
//	memcpy(FileAID,"\xa0\x00\x00\x00\x03\x86\x98\x07\x01",9);
//	sw1sw2=CPU_Select_DKFile_AID(1,FileAID,CPU_RcLen,CPU_CommBuf);	

//	if(sw1sw2!=0x9000)
//		return	PSAM_FILEPARA_ERR;
//	
//	return	CARD_OK;
//}

///*******************************************************************************
////终端从卡上读取随机数
////命令：00 84 00 00 
////用此命令模拟卡是否存在
//*******************************************************************************/
//u16 Psam_GetChallenge(u8 sel,u8 *pBuffer,u16 Len)
//{
//	u8 	Sendbuf[32];
//	u8  Revbuf[32];
//	u16 RevLen;
//	u16 sw1sw2;
//	memset(Revbuf,0,32) ;
//	Sendbuf[0] = 0x00;
//	Sendbuf[1] = 0x84;
//	Sendbuf[2] = 0x00;
//	Sendbuf[3] = 0x00;
//	Sendbuf[4] = Len;
//	PsamSend(1,Sendbuf,5);
//	RevLen=PsamReceive(1,Revbuf,Len+3,8000); //50Ms
//	sw1sw2=GetU8ToU16(Revbuf+RevLen-2);
//	if(sw1sw2==0x9000)
//	{
//		memcpy(pBuffer,Revbuf+1,Len);
//	}
//	return sw1sw2;
//}
////----------------------------------------------------------------
////MAC1计算
////SndLen：指明SndInfo的数据长度=14H+8*n(n=1,2,3),这里用N=1.
////SndInfo：发送参数指针，结构如下(以字节顺序)
////用户卡随机数(4)+用户卡交易序号(2)+交易金额(4)+交易类型(1,05/06)+
////  交易日期(4)+交易时间(3)+消费密钥版本号(1)+消费密钥算法标识(1)+
////  用户卡应用序列号(8,要最右边的)+成员银行标识(8)+试点城市标识(8)
////RcvInfo：接收参数指针,终端交易序号(4)+MAC1(4)
////-----------------------------------------------------------------


////------------------------------------------------------------------
////下列函数均为"卡操作应用"相关的
////------------------------------------------------------------------
//extern u8 isHaveEWM;
//void encrypt_data(unsigned char * bytedatain, int inLength, unsigned char * outHEXdata, int * outLength,char * key);
//extern u8 MFRC522_Auth(u8 authMode, u8 BlockAddr, u8 *Sectorkey, u8 *serNum) ;
//extern u8 MFRC522_Read(u8 blockAddr, u8 *recvData) ;
//extern u8 scidcode[6];
//extern u8 usesectornum;

//uchar ReadCardCommonDatas_CPU(void)
//{
//	uint 	 sw1sw2=0;
//	uchar	   FileAID[10];
//	uchar tempdata[] = {0x01,0x10,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//	int getreturnS;
//	uchar keydata[100];
//	uchar keydataback[20];
//	uchar keydatabd[10],CardPrinterNumback[10];
//	uchar samecardnumcnt = 0;
//		
//		int i = 0;
//		
//		memcpy(tempdata+1,scidcode,6);
//		HexGroupToHexString(CardSerialNum,keydatabd,4);
//		
//		encrypt_data((unsigned char *)tempdata,6,(unsigned char *)keydata,&getreturnS, (char *)keydatabd);
//		HexStringToHexGroup(keydata+1,keydatabd,6);
//		for(i=0;i<5;i++)
//		{
//			getreturnS = MFRC522_Auth(0x60,usesectornum*4,keydatabd,CardSerialNum);
//			if(getreturnS == 0) break;
//		}
//		if(getreturnS != 0) return getreturnS;
//		
//		
//		for(i=0;i<5;i++)
//		{
//			getreturnS = MFRC522_Read(usesectornum*4,keydata);
//			if(getreturnS ==  0)
//			{
//				getreturnS=	BytesCheckSum(keydata,16);
//				if (!getreturnS)
//					break;
//				else
//				{
//					if(memcmp(CardPrinterNumback,keydata,6) != 0)
//					{
//						memcpy(CardPrinterNumback,keydata,6);
//						samecardnumcnt = 1;
//					}else
//					{
//						samecardnumcnt++;
//					}
//				}
//			}
//		}
//		
//		if(getreturnS != 0)
//		{
//			if(samecardnumcnt == 5)
//			{
//				memcpy(CardPrinterNum,CardPrinterNumback,6);
//				
//			}else
//			{
//				return 1;
//			}
//		}else
//		{
//			memcpy(CardPrinterNum,keydata,6);
//		}
//	
//	
//		
//		for(i=0;i<5;i++)
//		{
//			getreturnS = MFRC522_Read(usesectornum*4+2,keydata);
//			if(getreturnS == 0) break;
//		}
//		if(getreturnS != 0) return getreturnS;
//	
//		
//	memcpy(LimtConsumeData_CPU,keydata,4);	
//	if(BCD_String_Diag(LimtConsumeData_CPU,4) != 0)
//	{
//		memset(LimtConsumeData_CPU,0,4);
//	}
//		
//	if(BCD_String_Diag(CardPrinterNum,6) != 0) return 0xFF;
//		
//	return 0;
//	
//	if(isHaveEWM) return 0;
//	
//	//默认主文
//	//选择一卡通文件
//	FileAID[0]=0x3f;
//	FileAID[1]=0x00;//zjx_change_20130606
//	sw1sw2=CPU_Select_File_AID(0,FileAID,CPU_RcLen,CPU_CommBuf);
//	if(sw1sw2!=0x9000)
//	{
//		if((sw1sw2&0xf000)==0x9000||(sw1sw2&0xf000)==0x6000)
//			return	CPU_SELFILE_FAIL;
//		else
//			return	CARD_NOCARD;
//	}
//	memcpy(FileAID,"\xa0\x00\x00\x00\x03\x86\x98\x10\x01",9);
//	sw1sw2=CPU_Select_DKFile_AID(0,FileAID,CPU_RcLen,CPU_CommBuf);	
//	if(sw1sw2!=0x9000)
//	{
//		if((sw1sw2&0xf000)==0x9000||(sw1sw2&0xf000)==0x6000)
//			return	CPU_SELFILE_FAIL;
//		else
//			return	CARD_NOCARD;
//	}
//	
//	sw1sw2=CPU_Read_Binary(0,0x15,41,CPU_CommBuf);//发行信息区，即公共信息区
//	/**************************************************************/	
//	if(sw1sw2==0x9000)//
//	{
//		memcpy(CardPrinterNum,CPU_CommBuf+1,3);
//		memcpy(LimtConsumeData_CPU,CPU_CommBuf+5,4);
//		return	CARD_OK;
//	}

//  	return	CARD_NOCARD	;
//}
////写脱机限额
//uchar WriteNoNetLimitMoney(uchar *buf)
//{

//	
//	uint 	 sw1sw2=0;
//	uchar	   FileAID[10];
//	uchar tempdata[] = {0x01,0x10,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//	int getreturnS;
//	uchar keydata[100];
//	uchar keydatabd[10];
//		
//	memcpy(tempdata+1,scidcode,6);
//	HexGroupToHexString(CardSerialNum,keydatabd,4);
//	
//	encrypt_data((unsigned char *)tempdata,6,(unsigned char *)keydata,&getreturnS, (char *)keydatabd);
//	HexStringToHexGroup(keydata+1,keydatabd,6);
//	getreturnS = MFRC522_Auth(0x60,usesectornum*4,keydatabd,CardSerialNum);
//		
//	getreturnS = MFRC522_Read(usesectornum*4+2,keydata);
//	
//	if(memcmp(keydata,buf,4) == 0)
//	{
//		return 0;
//	}
//		
//	memcpy(keydata,buf,4);	
//	getreturnS = MFRC522_Write(usesectornum*4+2,keydata);
//		
//	return getreturnS;

//}
////----------------------------------------------------------
////PSAM发送并接收7816的数据串(入口：发送指针，发送长度)(返回0 is ok)
////命令頭:CLA+INS+P1+P2+P3
////过程字节;
////zjx_change_20120712
////----------------------------------------------------------
//char ISO7816_PSAM(unsigned char SendLen, unsigned char * SendBuf, unsigned char * RcvLen, unsigned char * Rcvdata)
//{
//	uchar PsamSndBuf[5];
//	uchar ulen=0;

//	if (SendLen < 5)
//		return PSAM_COM_ERROR; //长度错	

//	//=============================================
//	//计算ulen个数
//	if(SendLen==5)
//	{
//		if(!SendBuf[4])//情形1及情形2最大数量情况
//			ulen=2;
//		else//情形2,有期望返回
//			ulen=SendBuf[4]+3;	
//	}
//	else
//	{
//		if((SendBuf[4]+5)==SendLen || (SendBuf[4]+6)==SendLen )	
//			ulen=1;	
//	    else
//	    	return PSAM_COM_ERROR; //长度错	

//	}
//	//=============================================	

//	PsamSend(1,SendBuf,5);//发送命令头

//	PsamReceive(1,PsamRcvBuf,ulen,2000);//接收过程字节INC/INC补码/60/6x、9x

//	if (PsamRcvBufCount==0)	
//		return PSAM_COM_ERROR; //无应答
//	else if(PsamRcvBufCount==1)
//	{
//		if (ulen==1 && PsamRcvBuf[0]==SendBuf[1]) //过程字节等于INS
//		{
//			//发送以后的数据
//			//均按get
//			if((SendBuf[4]+5)==SendLen)
//				ulen=2;
//			else
//			    ulen=SendBuf[4]+3;
//			delay_ms(2);
//			PsamSend(1,SendBuf+5,SendLen-5);

//			//接收数据
//			PsamReceive(1,PsamRcvBuf,ulen,2000);
//			__nop();
//		}
//		else //非过程字节
//		{
//			//接收数据
//			Rcvdata[0]=PsamRcvBuf[0];
//			PsamReceive(1,PsamRcvBuf,1,2000);			
//			if(PsamRcvBufCount==1)//错误状态
//			{
//				Rcvdata[1]=PsamRcvBuf[0];
//				*RcvLen=2;				
//				return 0;
//			}
//			else
//				return PSAM_COM_ERROR; //应答错误
//		}
//	}

//	if( (PsamRcvBuf[0]==0x61) && (PsamRcvBufCount==2) )
//	{
//		//取响应数据
//		PsamSndBuf[0]=0x00;
//		PsamSndBuf[1]=0xc0;
//		PsamSndBuf[2]=0x00;
//		PsamSndBuf[3]=0x00;
//		PsamSndBuf[4]=PsamRcvBuf[1];
//		//发送以后的数据
//		delay_ms(2);
//		PsamSend(1,PsamSndBuf,5);
//		//接收数据
//		PsamReceive(1,PsamRcvBuf,PsamSndBuf[4]+3,2000);
//	}
//    //检查接收到的数据
//	if(PsamRcvBufCount<2)
//		return PSAM_COM_ERROR; //应答错误
//	else if(PsamRcvBufCount==2) 
//	{
//		memcpy(Rcvdata,PsamRcvBuf,PsamRcvBufCount);
//		*RcvLen=PsamRcvBufCount;
//		return 0;
//	}
//	else//转移接收到的数据
//	{
//		memcpy(Rcvdata,&PsamRcvBuf[1],PsamRcvBufCount-1); //第一个字节为INS
//		*RcvLen=PsamRcvBufCount-1;
//		return 0;
//	}
//}

//u16 GetPsamResetATR(u8 Sel ,u8 *pBufer)
//{ 
//	u8 Revbuf[32];
//	u16 RevLen=0;
//	PsamReset();
//	RevLen=PsamReceive(1,Revbuf,30,2000);//50Ms
//	memcpy(pBufer,Revbuf,RevLen);
//	return RevLen;
//}
////PSAM卡的测试函数
//void ISO7816Test(void)
//{
//	u8 ATR[32];
//	u8 buf[8];


//	char pBufer[32];
//	u16 RevLen;

//	RevLen=GetPsamResetATR(1,ATR);
//	if(RevLen)
//	{
//		RevLen=Psam_GetChallenge(1,buf,8);
//		if(RevLen==0x9000)
//		{
//			delay_ms(10);
//		}
//		else
//		{
//				DISP_HARDEErrorSub(PASM_ERROR);
//				while(1);
//		}
//	}
//	else
//	{
//			DISP_HARDEErrorSub(PASM_ERROR);
//			while(1);
//	}		
//}



//u16 GetU8ToU16(u8 * Buf)
//{
//	u16 sw1sw2;
//	sw1sw2 = (u16)Buf[1] + ((u16)Buf[0]<<8);
//	return sw1sw2;
//} 






