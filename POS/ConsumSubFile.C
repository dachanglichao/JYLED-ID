#include "MyDefine.h"
#include "include.h"
#include "char.h"


uchar		InputMenuSort=0;
uchar ShuRuMoney_Err = 0;//金额输入不完全
uchar		bitHaveLedError=0;
uchar		PinCount=0;
uchar		InPutPinCode[3];

extern void	BeepOn(unsigned char num);
extern uchar QueryConsumeResult(void);


void	SetConsumMode(ulong);
uchar	InputSumConsumMoney(uchar,uchar,ulong);//消费模式
uchar	KeyBoardSetSysParamenter(uchar,ulong);//通过键盘设置系统参数
void	Init_Printer(void);
void	ChgMoneyToPrintBuffer(uchar ,ulong ,uchar * );
void	PrintConsumDatas(uchar bbit);
void  LookConsumMoney(void);
uchar	CheckPinSub(ulong ReadKeyValue );//PIN认证
extern u8 isHaveEWM;
uchar	ConsumCase=0;
uint	MenuSort=0;//菜号
uchar printconsumeMoney[4];
uint16_t   disp_delaycnt=0;//余额显示变量
u8 disp_delaycntflag=0;//定义显示开关变量
#define  DISP_DELAYTIME  30//定义余额显示时间30*20ms
int tcp_send_and_rec_packet(int cmd,u8 * data,int sendLength,int entType,u8 * dataRec,int * length,int timeout);
#include "cJSON.h"
void	DISP_ErrorSub(uchar Num);
u8 refushstatusnetcnt = 0;
extern u8 isReg;
u8 istuoji = 1;
extern ulong global_key;
extern ulong  BatchNum;



//写脱机限额
uchar WriteNoNetLimitMoney(uchar *buf)
{
	uint 	sw1sw2=0;
	uchar	FileAID[10];
	uchar tempdata[] = {0x01,0x10,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	int getreturnS;
	uchar keydata[100];
	uchar keydatabd[10];
		
	getreturnS = MFRC522_Auth(0x60,usesectornum*4,keydatabd,CardSerialNum);		
	getreturnS = MFRC522_Read(usesectornum*4+2,keydata);
	
	if(memcmp(keydata,buf,4) == 0)
	{
		return 0;
	}
		
	memcpy(keydata,buf,4);	
	getreturnS = MFRC522_Write(usesectornum*4+2,keydata);
		
	return getreturnS;

}
void	ConsumSub(void)//消费主程序
{
	uchar		status,i;
	ulong		ReadKeyValue;
	ulong		iii,jjj;
	uchar Buffer[10];
	uchar BufferBak[10];
	uchar CardPrinterNumBak[4];
	u8 recDataTemp[500],datagetd[50];
	int recDataLength;
	cJSON * object;
	cJSON *arrayItem;
	float databalance;
	static uchar bitled = 0;
	

	ReadKeyValue=ScanKeySub(KEY_ANY);//等待按键
	if (global_key==(KEY_ESC | KEY_MENU))
	{ 
		if(isReg == 0)
		{
			istuoji = 1;
		}
	}
	
	if(isReg == 1)
	{
		istuoji = 0;
	}
	
	refushstatusnetcnt++;
	if(refushstatusnetcnt >= 4)
	{
		DispBuffer[10]=DispModeChar[ConsumMode];
		if (BatModeFlag) //
						DispBuffer[10]|=0x01;		
					else
						DispBuffer[10]&=0xfe;
		if(isReg == 0 && istuoji == 1)
		{
			if(bitled)
			{
				bitled = 0;
				DispBuffer[10] &= ~0x10;
			}else
			{
				bitled = 1;
				DispBuffer[10] |= 0x10;
			}
			
		}else
		{
			//DispBuffer[10] &= ~0x10;
		}
		LED_DispDatas_all(DispBuffer);
		refushstatusnetcnt =0;
	}
	
	CheckPF8563();
	switch(ConsumCase)
	{
		case	0://读卡消费变量初始化
			InputCase=0;					//键盘状态控制字
			bitHaveReadBalance=0;			//读到卡标识
			SumConsumMoney=0;	 			//总消费
			CurrentConsumMoney=0;			//消费额
			MenuSort=0;
			Num_MulValue=1;
			memset(CardSerialNumBak,0,4);	//消费参数初试化
			bitHaveLedError=0;				//单次错误显示标识
			ConsumCase=1;
			status=CARD_NOCARD;
			NeedDispFlag = 0;
		  disp_delaycntflag=0;//显示关闭
		  disp_delaycnt=0;//余额显示时间变量初始化
			bitHaveDispBalance = 0;//已经显示金额
		break;	
		
		case 1:
			if (!bitHaveReadBalance )
			{
				status=RequestCard();
				if (!status)
        {
//					for(i=0;i<5;i++)
//					{
//						status=ReadCardCommonDatas_CPU();//ad by lc
//						
//						if(!status)
//						{
//							break;
//						}
//						else
//							  RequestCard();
//					}
					 if(!status&&ConsumMode!=CONSUM_RATION)
					 {
							//CurrentConsumMoney=0;//清消费额
							InputCase=0;
							MenuSort=0;
							Num_MulValue=1;
							BeepOn(1);
							bitHaveLedError=0;
							SumPrintMoney=0;
							bitHaveReadBalance=1;
							if(!bitHaveDispBalance)
							{
								ConsumCase=2;//进入余额查看流程
								return;
							}
					 }
					 else
					 {
					   ConsumCase=0;
						 return;
					 }
         }
         else
				 {
            status=CARD_NOCARD;		 
				 }
			}
			if(bitHaveDispBalance)//已经显示余额
			{
				status=RequestCard();
				if (status)
				{
					ConsumCase=0;//
					return;
				}
			}
			if(status==CARD_NOCARD)	
			{
	        if (ReadKeyValue==KEY_ADD && SumPrintMoney && !InputCase)
	           PrintConsumDatas(1);			
			}
			status=KeyBoardSetSysParamenter(bitHaveReadBalance|bitHaveLedError,ReadKeyValue);//输入消费金额
      if (!status && (CurrentConsumMoney || ((InputCase&0xf0)==0x70 && ConsumMode==CONSUM_NUM) ) )//有消费额输入
      {
        SumPrintMoney=0;
        MenuSort=InputMenuSort;
        InputCase=0;
				DispSumConsumMoney(ConsumMode,InputMenuSort,CurrentConsumMoney);//显示消费金额
        ConsumCase=2;//进入消费流程
      }
		break;
		case 2:
			if (!bitHaveReadBalance)//有输入金额，未读卡
      {
				status=RequestCard();
        if (!status)
        {
//						for(i=0;i<5;i++)
//						{
//							status=ReadCardCommonDatas_CPU();//ad by lc
//							
//							if(!status)
//							{
//								HaveQueryConDat = 0;
//								break;
//							}
//							else
//								RequestCard();
//						}
					 if(!status)
					 {
						memcpy(CardSerialNumBak,CardSerialNum,4);
						InputCase=0;                    
						BeepOn(1);
						bitHaveLedError=0;
						SumPrintMoney=0;
						bitHaveReadBalance=1;
					 }
					 else
					 {
						 ConsumCase=0;
						 return;
					 }
        }
        else
				{
          Led_Wait_Disp();
					if (ReadKeyValue==KEY_MONEY)
					{
						if(ConsumMode==CONSUM_RATION)
						{
							if (ConsumModeEnable & (1<<CONSUM_MONEY))
								ConsumMode=CONSUM_MONEY;
							else
								BeepOn(3);	
						}
						ConsumCase=0;
					}
					if (ReadKeyValue==KEY_ESC)
					{
						ConsumCase=0;
					}
					if(status==CARD_NOCARD)	
					{
			       if (ReadKeyValue==KEY_ADD && SumPrintMoney && ((InputCase&0xf0)==0x70))
			          PrintConsumDatas(1);
						 //ConsumCase=0;
					}
					
				}
      }
			//已经读到卡
			if (bitHaveReadBalance)//读到卡
			{
				if(isReg) //联机消费
				{
					//memcpy(NoNetRecord,CardPrinterNum,3);
					//CurrentConsumMoney
					if(CurrentConsumMoney == 0)
						status = tcp_send_and_rec_packet(GetBalanceCmd,CardPrinterNum,6,1,recDataTemp,&recDataLength,3);
					else
					{
						BatchNum++;
						memcpy(recDataTemp,CardPrinterNum,6);
						ChgUlongToBCDString(CurrentConsumMoney,recDataTemp+6,4);
						status = tcp_send_and_rec_packet(ConsumeCmd,recDataTemp,10,1,recDataTemp,&recDataLength,3);
					}
					if(status != 0)
					{
						ConsumCase=4;
					}else
					{
						object = cJSON_Parse(recDataTemp); 
						if(object != 0)
						{
							arrayItem = cJSON_GetObjectItem(object,"message");  
							if(!strcmp(arrayItem->valuestring,"CODE1000") || (!strcmp(arrayItem->valuestring,"0")) )//消费成功
							{
								arrayItem = cJSON_GetObjectItem(object,"resultData");  //balance1
								if(arrayItem != 0)
								{
									memset(datagetd,0,50);
									memcpy(datagetd,cJSON_GetObjectItem(arrayItem,"cashBalance")->valuestring,strlen(cJSON_GetObjectItem(arrayItem,"cashBalance")->valuestring));
									//databalance = atof(datagetd); 
									Write_SOCK_Data_Buffer(5,datagetd,strlen(cJSON_GetObjectItem(arrayItem,"cashBalance")->valuestring));	
									sscanf(datagetd,"%f",&databalance);		
									databalance += 0.005;
									iii = databalance ;
									recDataTemp[0] = (iii >> 16) & 0xFF;
									recDataTemp[1] = (iii >> 8) & 0xFF;
									recDataTemp[2] = (iii >> 0) & 0xFF;
									Disp_Balance(recDataTemp);//显示余额
									ConsumCase=4;
									cJSON_Delete(object);
									
									ChgUlongToBCDString(0,Buffer,4);
									for(i=0;i<5;i++)
									{
//										status = WriteNoNetLimitMoney(Buffer); //写入脱机限额
//										if(!status)
//											break;
									}			
									
									bitHaveDispBalance = 1;
									//ConsumCase = 1;
									ConsumCase = 4;
									
									if(ConsumMode==CONSUM_RATION)
									{
										ConsumCase = 4;
									}
									
									return;
								}
							}
							else
							{
								arrayItem = cJSON_GetObjectItem(object,"message"); 
								status = atoi(arrayItem->valuestring);
								DISP_ErrorSub(status);
							}			
						}
						cJSON_Delete(object);				
					}
					ConsumCase=4;
					return;
					
				}
				else if(istuoji)//脱机消费
				{
						if(CanNoNetConsume)        //脱机允许消费
						{
							iii=ChgBCDStringToUlong(CardPrinterNum,3);
							//if (!Check_BlackCardNum(iii)) //白名单检查
							if(0)
							{
								BeepOn(3);
								DISP_ErrorSub(CARD_CANEL_ERROR); //脱机不允许消费
								ConsumCase=4;//挂失卡
								return;
							}
						else
						{
							jjj=ChgBCDStringToUlong(LimtConsumeData_CPU,4)+CurrentConsumMoney;
							iii=LimtConsumeMoney;				
							if(jjj>iii)  //超过脱机限额
							{
								BeepOn(3);
								DISP_ErrorSub(CARD_PURSENUM_ERROR); //超过脱机限额
								ConsumCase=4;
							}
							else
							{
								if(CurrentConsumMoney)
								{
									ChgUlongToBCDString(jjj,Buffer,4);
									for(i=0;i<5;i++)
									{
										status = WriteNoNetLimitMoney(Buffer); //写入脱机限额
										if(!status)
											break;
									}									
									if(!status)
									{
											if(ConsumMode==CONSUM_RATION)   
											{				
												if(Purse_num==1)
													NoNetRecord[0] =0x63;
												else if(Purse_num==3)
													NoNetRecord[0] =0x60;
												else//2
													NoNetRecord[0] =0x62;
											}
											else
											{
												if(Purse_num==1)
													NoNetRecord[0] =0;
												else if(Purse_num==3)
													NoNetRecord[0] =0x5f;
												else//2
													NoNetRecord[0] =0x61;
											}
											NoNetRecord[0] +=100;																																								
											memcpy(NoNetRecord+1,CardPrinterNum,6);
													
											ChgUlongToBCDString(CurrentConsumMoney,NoNetRecord+7,4);

											Read_Sysdate(NoNetRecord+11);
											
											NoNetRecord[11] = BCDToHex(NoNetRecord[11]);
											NoNetRecord[12] = BCDToHex(NoNetRecord[12]);
											NoNetRecord[13] = BCDToHex(NoNetRecord[13]);
											NoNetRecord[14] = BCDToHex(NoNetRecord[14]);
											NoNetRecord[15] = BCDToHex(NoNetRecord[15]);
											NoNetRecord[16] = BCDToHex(NoNetRecord[16]);
											
											NoNetRecord[17] = 0;

											Save_OffRecode();//存储脱机记录
											Disp_Hello();//显示
											ConsumCase=4;											
									}
									else
									{
										 ConsumCase=0;
									}
								}
								else
								{
									Disp_Hello();
									ConsumCase=4;
								}							
							}
						}
					}
					else //脱机不允许消费
					{
						if(isHaveEWM) isHaveEWM = 0;
						   ConsumCase = 0;
					}
				}else if(istuoji == 0)
				{
					DISP_ErrorSub(CARD_TUOJINULL_ERROR); //超过脱机限额
					ConsumCase=4;
				}
			}
			
		break;
			
		case 3://查询处理结果
			
		break;			
		case 4:
			status=RequestCard();
			if (status)//无卡
			{
				if(!bitHaveLedError)
				{
				//	OSTimeDlyHMSM(0,0,2,0); //余额显示时间
					ConsumCase=0;
					DispSumConsumMoney(ConsumMode,0xff,0);
				//	DISP_ErrorSub(QueryConsumeResult());//显示错误码
				}
				else
				{
					if (ReadKeyValue==KEY_ESC)//等待清除错误||disp_delaycnt>disp_delaytime
					{
						bitHaveLedError=0;
						ConsumCase=0;
						DispSumConsumMoney(ConsumMode,0xff,0);

					}		
				}
			}
//			else
//			{
//				OSTimeDlyHMSM(0,0,1,0); //余额显示时间
//				bitHaveLedError=0;
//						ConsumCase=0;
//						DispSumConsumMoney(ConsumMode,0xff,0);
//			}
			break;
		case 5:  //密码验证
			 status=CheckPinSub(ReadKeyValue);//PIN认证
			if (status<2)
			{	
				if(!status)//正确的结果
				{

					ConsumCase=3;
				}
				else  //取消
				{
					ConsumCase=4;
				}	                  
			}	
		break;
		default:
		break;
	}
}

uchar	InputSumConsumMoney(uchar bbit,uchar Mode,ulong  InKeyValue)//消费模式
{
	uchar	Buffer[5];
	uchar	st_data;
	uchar	aaa[8];
	if (Mode==CONSUM_SIMPLE)//简易消费模式
	{
		if (!(InKeyValue & (KEY_0_9 | KEY_POINT | KEY_ADD) ))
			return	0x10;//无键
		else
		{
			CurrentConsumMoney=0;
			if (InKeyValue==KEY_POINT)
				InputMenuSort=11;
			else if (InKeyValue==KEY_ADD)
				InputMenuSort=12;
			else 
			{
				InputMenuSort=ChgKeyValueToChar(InKeyValue);
				if (!InputMenuSort)
					InputMenuSort=10;
		   	}
			RdBytesFromAT24C64(SimplePrince_Addr+(InputMenuSort-1)*4,Buffer,4);
			if ( BytesCheckSum(Buffer,4) || BCD_String_Diag(Buffer,3))
				memset(Buffer,0,4);
			CurrentConsumMoney=ChgBCDStringToUlong(Buffer,3);
			if (!CurrentConsumMoney)
				BeepOn(3);
			DispSumConsumMoney(ConsumMode,0xff,CurrentConsumMoney);
			return	0;
		}
	}
	else if (Mode==CONSUM_RATION)//自动消费模式
	{
		InputMenuSort=0;	 
		RdBytesFromAT24C64(MenuPrince_Addr,Buffer,5);
		if (BytesCheckSum(Buffer,5) || BCD_String_Diag(Buffer+1,3))
			memset(Buffer,0,3);
		CurrentConsumMoney=ChgBCDStringToUlong(Buffer+1,3);
		if (!CurrentConsumMoney)
		{}
		else
		{
			CurrentConsumMoney*=Card_Rebate;
			CurrentConsumMoney=(CurrentConsumMoney+50)/100;//四舍五入处理
			st_data=0;
			DispSumConsumMoney(ConsumMode,0xff,CurrentConsumMoney);
		}
		return	st_data; 
	}
	else if (Mode==CONSUM_NUM)//菜号方式
	{
		if (InputCase==0x70 || InputCase==0x71)
		{
			if (InKeyValue&KEY_0_9)
			{
				st_data=ChgKeyValueToChar(InKeyValue);
				if (InputCase==0x70)
					InputNum=st_data;
				else
					InputNum=(InputNum%10)*10+st_data;
				if (InputNum>70)
					InputNum=0;
				InputCase=0x71;
				InputMenuSort=InputNum;//菜号
				RdBytesFromAT24C64(MenuPrince_Addr+InputNum*4,Buffer,4);
				if (BytesCheckSum(Buffer,4) || BCD_String_Diag(Buffer,3))
					memset(Buffer,0,4);
				SingleMoney=ChgBCDStringToUlong(Buffer,3);
				DispSumConsumMoney(ConsumMode,InputNum,SingleMoney);
				if (!SingleMoney)
					BeepOn(3);
				else
					bitInPutPoint=1;
			}
			else if (InKeyValue==KEY_ENTER)
			{
				bitAddStatus=0;
				CurrentConsumMoney+=SingleMoney;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x73;
			   	}
				else
				{
					DispSumConsumMoney(ConsumMode,InputMenuSort,CurrentConsumMoney);
					if (!CurrentConsumMoney)
						BeepOn(3);
					InputCase=0x70;
			
					return	0;
				}
			}
			else if (InKeyValue==KEY_ADD &&SingleMoney)
			{
				bitAddStatus=1;
				CurrentConsumMoney+=SingleMoney;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x73;
			   	}
				else
				{
					if (bitInPutPoint)
						InputMaxNum++;
					bitInPutPoint=0;
					InputCase=0x70;
					DispSumConsumMoney(ConsumMode,InputMaxNum,CurrentConsumMoney);
					SingleMoney=0;
					InputNum=0;
				}
			}
			else if (InKeyValue==KEY_MUL)
			{
				InputCount=0;
				InputNum=0;
				InputCase=0x72;
				memset(InputBuffer,0xff,6);
				MulData=1;
			}
		}
		else if (InputCase==0x72)
		{
			if (InKeyValue & KEY_0_9)
			{
				st_data=ChgKeyValueToChar(InKeyValue);
				if (!InputCount && st_data)
				{
					InputBuffer[0]=st_data;
				}
				else
					InputBuffer[InputCount]=st_data;
				if (InputCount==3)
				{
					InputBuffer[0]=InputBuffer[1];
					InputBuffer[1]=InputBuffer[2];
					InputBuffer[2]=st_data;
					InputBuffer[3]=0xff;
					InputCount=2;
				}
				InputCount++;

				ChgInputToLedBuffer(5,InputBuffer,5);
				MulData=ChgInputToUlong(InputBuffer,6)/100;
			}
			else if (InKeyValue==KEY_ADD && SingleMoney)
			{
				bitAddStatus=1;
				if (MulData)
				{
					SingleMoney=SingleMoney*MulData;
					CurrentConsumMoney+=SingleMoney;
				}
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x73;
			   	}
				else
				{	
					if (bitInPutPoint)
						InputMaxNum++;
					bitInPutPoint=0;
					DispSumConsumMoney(ConsumMode,InputMaxNum,CurrentConsumMoney);
					SingleMoney=0;
					InputNum=0;
					InputCase=0x70;
				}
			}
			else if (InKeyValue==KEY_ENTER)
			{
				bitAddStatus=0;
				if (MulData)
				{
					SingleMoney=SingleMoney*MulData;
					CurrentConsumMoney+=SingleMoney;
				}
				InputCase=0x70;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x73;
			   	}
				else
				{		
					DispSumConsumMoney(ConsumMode,InputMenuSort,CurrentConsumMoney);
					return	0;
				}								
			}
		}
		else
		{
			DispOver();
			if (InKeyValue==KEY_ESC)
			{
				bitAddStatus=0;
				InputCase=0;
			}
		}
 	}
	else
	{//金额消费
		if (InputCase==0x70)
		{
			if (!InputCount)
			{
				memset(InputBuffer,0xff,6);
				bitInPutPoint=0;
				SingleMoney=0;
			}
			if ( InKeyValue&KEY_0_9) 
			{
				ShuRuMoney_Err =1;
				st_data=0;
				if (InputCount>=3 &&  bitInPutPoint)
				{
					if (InputBuffer[InputCount-3]&0x80)
						st_data=1;
				}
				if (!st_data)
				{
					st_data=ChgKeyValueToChar(InKeyValue);
					InputBuffer[InputCount++]=st_data;
					if ( InputCount==2 && !InputBuffer[0])
					{
						memset(InputBuffer,0xff,6);
						InputBuffer[0]=st_data;
						InputCount=1;
					}
					if (InputCount>=5 && !bitInPutPoint)
					{
						memset(Buffer,0xff,6);
						memcpy(Buffer,InputBuffer+1,4);
						memcpy(InputBuffer,Buffer,6);
						InputCount=4;
						FormatBuffer(3,InputBuffer,&InputCount);
					}
			 	}
			    DispBuffer[0]=0x40;
			    DispBuffer[1]=0x40;
			    DispBuffer[2]=0x40;
			    DispBuffer[3]=0x40;
			    ChgInputToLedBuffer(4,InputBuffer,6);
			   	DispBuffer[10]=DispModeChar[ConsumMode];
					if (BatModeFlag) //
						DispBuffer[10]|=0x01;		
					else
						DispBuffer[10]&=0xfe;					
				SingleMoney=ChgInputToUlong(InputBuffer,6);
				if (InputCount>=6)
					InputCount=5;
			}
			else if (InKeyValue==KEY_POINT && !bitInPutPoint )
			{
				bitInPutPoint=1;
				ShuRuMoney_Err =1;
				if (InputCount)
					InputBuffer[InputCount-1]|=0x80;
				else
				{
					InputBuffer[0]=0x80;
					InputCount=1;
				}					
				ChgInputToLedBuffer(4,InputBuffer,6);

					DispBuffer[10]=DispModeChar[ConsumMode];
					if (BatModeFlag) //
						DispBuffer[10]|=0x01;		
					else
						DispBuffer[10]&=0xfe;
				SingleMoney=ChgInputToUlong(InputBuffer,6);						
			}
			else if (InKeyValue==KEY_ENTER)
			{
				ShuRuMoney_Err = 0;
				InputCount=0;
				bitAddStatus=0;
				CurrentConsumMoney+=SingleMoney;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x72;
				}
				else
				{
					DispBuffer[0]=0x40;
					DispBuffer[1]=0x40;
					DispBuffer[2]=0x40;
					ChgUlongToLongDispBuffer(CurrentConsumMoney,aaa);

			   		DispBuffer[3]=0x40;
					memcpy(DispBuffer+4,aaa+2,6);					
					return	0;
				}
			}
			else if (InKeyValue==KEY_ADD && SingleMoney)
			{
				bitAddStatus=1;
				CurrentConsumMoney+=SingleMoney;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x72;
				}
				else
				{
				   DispBuffer[0]=0x40;
				   DispBuffer[1]=0x40;
				   DispBuffer[2]=0x40;
					ChgUlongToLongDispBuffer(CurrentConsumMoney,aaa);

			   		DispBuffer[3]=0x40;
					memcpy(DispBuffer+4,aaa+2,6);					
	
					InputCount=0;	
					InputCase=0x70;
				}
			}
			else if (InKeyValue==KEY_MUL)
			{
				InputCount=0;
				InputCase=0x71;
				memset(InputBuffer,0xff,6);
				MulData=1;
			}
		}
		else if (InputCase==0x71)
		{//输入乘数
			if (InKeyValue & KEY_0_9)
			{
				st_data=ChgKeyValueToChar(InKeyValue);
				if (!InputCount && st_data)
					InputBuffer[0]=st_data;
				else
					InputBuffer[InputCount]=st_data;
				if (InputCount==3)
				{
					InputBuffer[0]=InputBuffer[1];
					InputBuffer[1]=InputBuffer[2];
					InputBuffer[2]=st_data;
					InputBuffer[3]=0xff;
					FormatBuffer(3,InputBuffer,&InputCount);
				}
				InputCount++;
				ChgInputToLedBuffer(5,InputBuffer,5);

					DispBuffer[10]=DispModeChar[ConsumMode];
					if (BatModeFlag) //
						DispBuffer[10]|=0x01;		
					else
						DispBuffer[10]&=0xfe;	
				MulData=ChgInputToUlong(InputBuffer,6)/100;
			}
			else if (InKeyValue==KEY_ADD && SingleMoney )
			{
				bitAddStatus=1;
				if (MulData)
				{
					SingleMoney=SingleMoney*MulData;
					CurrentConsumMoney+=SingleMoney;
				}
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x72;
			   	}
				else
				{
				   DispBuffer[0]=0x40;
				   DispBuffer[1]=0x40;
				   DispBuffer[2]=0x40;
					ChgUlongToLongDispBuffer(CurrentConsumMoney,aaa);

			   		DispBuffer[3]=0x40;
					memcpy(DispBuffer+4,aaa+2,6);					
						DispBuffer[10]=DispModeChar[ConsumMode];
					if (BatModeFlag) //
						DispBuffer[10]|=0x01;		
					else
						DispBuffer[10]&=0xfe;
					InputCount=0;
					InputCase=0x70;
				}
			}
			else if (InKeyValue==KEY_ENTER)
			{
				bitAddStatus=0;
				if (MulData)
				{
					SingleMoney=SingleMoney*MulData;
					CurrentConsumMoney+=SingleMoney;
				}
				InputCase=0x70;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x72;
		   		}
				else
				{	
				   DispBuffer[0]=0x40;
				   DispBuffer[1]=0x40;
				   DispBuffer[2]=0x40;
					ChgUlongToLongDispBuffer(CurrentConsumMoney,aaa);

			   		DispBuffer[3]=0x40;
					memcpy(DispBuffer+4,aaa+2,6);					
	
						DispBuffer[10]=DispModeChar[ConsumMode];
					if (BatModeFlag) //
						DispBuffer[10]|=0x01;		
					else
						DispBuffer[10]&=0xfe;
					return	0;
				}					
			}
		}
		else
		{
			DispOver();
			if (InKeyValue==KEY_ESC)
			{
				bitAddStatus=0;
				InputCase=0;
			}
		}
	}
	return	0xff;
}


uchar	SelectDate(uchar * ptr)//选择消费日期
{
	uchar	st_data;
	uchar	bbit=1;
	uchar	Count=0;
	uchar 	Fx[4]={5,6,8,9};
	ulong	Long_Value;
	DispBuffer[0]=0x6d;//S
	DispBuffer[1]=0x08;//_
	DispBuffer[2]=0x5e;//d
	DispBuffer[3]=0x08;//_
	DispBuffer[7]=0x40;//-
	ptr[0]=SysTimeDatas.S_Time.MonthChar;
	ptr[1]=SysTimeDatas.S_Time.DayChar;
	while (1)
	{
		if (bbit)
		{
			bbit=0;
			st_data=ptr[0]>>4;
			DispBuffer[Fx[0]]=Disp0_9String[st_data];
			st_data=ptr[0]&0x0f;
			DispBuffer[Fx[1]]=Disp0_9String[st_data];
			st_data=ptr[1]>>4;
			DispBuffer[Fx[2]]=Disp0_9String[st_data];
			st_data=ptr[1]&0x0f;
			DispBuffer[Fx[3]]=Disp0_9String[st_data];
			LED_DispDatas_all(DispBuffer);
		}
		LED_NumFalsh_Disp(Fx[Count],1,0x10000);
		Long_Value=ScanKeySub(KEY_0_9 | KEY_ENTER|KEY_ESC|KEY_MUL);
		if (Long_Value)
		{
			if (Long_Value==KEY_ESC)
				return	1;
			else if (Long_Value==KEY_ENTER||Long_Value==KEY_MUL )
				return	0;
		 	else
			{
				st_data=ChgKeyValueToChar(Long_Value);
				if (!Count)
				{
					ptr[0]&=0x0f;
					st_data<<=4;
					ptr[0]+=st_data;
			  	}
				else if (Count==1)
				{
					ptr[0]&=0xf0;
					ptr[0]+=st_data;
			  	}
				else if (Count==2)
				{
					st_data<<=4;
					ptr[1]&=0x0f;
					ptr[1]+=st_data;
			   	}
				else if (Count==3)
				{
					ptr[1]&=0xf0;
					ptr[1]+=st_data;
				}
				bbit=1;
				Count++;
				Count=Count%4;
			}
		}

		Clr_WatchDog();
	}
}

uchar	KeyBoardSetSysParamenter(uchar  bbit, ulong	InKeyValue)
{
	uchar	status,page;
	u8 recDataTemp[100],sendDataTemp[100],datagetd[50];
	int recDataLength;
	cJSON * root,*rootfor;
	float databalance;
	ulong iii,ii,Addr;
	ulong	SumMoney=0;
	uchar	LedX;
	uchar	Buffer[32];
	uchar	PPage;
	//if(InKeyValue == 0) return 1;
	
	if (InKeyValue==KEY_ESC)
	{//取消键，返回主菜单
		InputCase=0;
		bitHaveReadBalance=0;
		CurrentConsumMoney=0;
		LookOk=0;
		fristLook=0;
		bitAddStatus=0;
		NeedDispFlag = 0;
		DispSumConsumMoney(ConsumMode,0xff,CurrentConsumMoney);	
		
   }

	if (!bbit)
	{
		if (InKeyValue==KEY_MENU)
			InputCase=0x30;//设置菜号
		else if (InKeyValue & (KEY_SIMPLE|KEY_NUM|KEY_RATION|KEY_MONEY))
		{//设置消费方式
			LookOk=0;
			fristLook=0;
			SetConsumMode(InKeyValue);
			InputCase=0;	
			if (ConsumMode!=CONSUM_RATION)
				CurrentConsumMoney=0;	
		}		
		else if (ConsumMode==CONSUM_SIMPLE && InKeyValue==KEY_ADD)//查询参数
		{
			bitDispFlash=0;
			bitLookSysDatas=1;
			InputCase=0x50;	
			memset(LookMony,0,8);			
			
			//L值：全部未采集消费额之和
			DispBuffer[0]=0x6d;//S
			DispBuffer[1]=0x39;//C
			DispBuffer[2]=0x77;//A
			DispBuffer[3]=0x37;//n	
			memset(DispBuffer+4,0,6);
			LED_DispDatas_all(DispBuffer);
			SumMoney=0;
			LedX=0;
			for (ii=0;ii<MAXRECORD_NUM;ii++)
			{
				//iii=(ulong)ii*RECORD_SIZE;
				Addr=(ulong)ii*RECORD_SIZE;
				//PPage=*(1+(uchar *)&iii);
				//memcpy((uchar *)&Addr,(2+(uchar *)&iii),2);
				Flash_Rd_Bytes(Addr,Buffer,22);
				if (Buffer[0]==0xa0 && (Buffer[19]==CalCheckSum(Buffer+1,18)))
				//if (Buffer[0]==0xa0)
					SumMoney+=ChgBCDStringToUlong(Buffer+8,4);
				Clr_WatchDog();
				if (!(ii%200))
				{
						LedX++;
					LedX=LedX%10;
					for (PPage=0;PPage<10;PPage++)
					{
						if (PPage<LedX)
							DispBuffer[PPage]|=0x80;
						else
							DispBuffer[PPage]&=0x7f;	
					}
					LED_DispDatas_all(DispBuffer);			
				}
			}
			DispBuffer[0]=0x38;//L
			DispBuffer[1]=0x40;
			ChgUlongToLongDispBuffer(SumMoney,Buffer);
			DispBuffer[1]=8;
			memcpy(DispBuffer+5,Buffer,5);		
			
			LED_DispDatas_all(DispBuffer);			
		}
		else if (InputCase==0x31 && InKeyValue==KEY_MUL)//查询参数
		{
			bitDispFlash=0;
			bitLookSysDatas=1;
			InputCase=0x50;	
			memset(LookMony,0,8);			
		

			ComsumeDate[0] = SysTimeDatas.S_Time.YearChar;						
			page =SelectDate(ComsumeDate+1);//选择消费日期	
			if(page)
			{
				InputCase = 0;
				return;
			}	
			
			sprintf(sendDataTemp,"%d-%02d-%02d%d-%02d-%02d",BCDToHex(SysTimeDatas.S_Time.YearChar)+2000,BCDToHex(ComsumeDate[1]),BCDToHex(ComsumeDate[2])
			,BCDToHex(SysTimeDatas.S_Time.YearChar)+2000,BCDToHex(ComsumeDate[1]),BCDToHex(ComsumeDate[2]));
			
			status = tcp_send_and_rec_packet(0x2103,sendDataTemp,20,1,recDataTemp,&recDataLength,3);
			
			root = cJSON_Parse(recDataTemp+3); 
			if(root != 0)
			{
				status = cJSON_GetObjectItem(root,"returnType")->valueint;  
				if(status == 0)
				{
					rootfor = cJSON_GetObjectItem(root,"data");  //balance1
					if(rootfor != 0)
					{
						memcpy(datagetd,cJSON_GetObjectItem(rootfor,"amountsum")->valuestring,strlen(cJSON_GetObjectItem(rootfor,"amountsum")->valuestring));
						//databalance = atof(datagetd); 
						sscanf(datagetd,"%f",&databalance);									
						iii = databalance * 100;
						memcpy(DispBuffer,"\x6e\x79\x08\x08\x08",5);
						ChgUlongToLongDispBuffer(iii,DispBuffer+5);	
						LED_DispDatas_all(DispBuffer);
						//return;
					}
				}
			}
			cJSON_Delete(root);
		
		}		
		else if (InputCase==0x31 &&  InKeyValue==KEY_MUL )//菜单+份数，设置简易价格
		{//简易单价
			InputCase=0x40;	
		}
		else if	(ConsumMode==CONSUM_NUM &&  InKeyValue==KEY_ADD)
			InputCase=0x80;	
		if ((InputCase&0xf0)==0x50)//查询参数控制
		{
			if (bitLookSysDatas && InKeyValue==KEY_ENTER )
			{
				bitDispFlash=0;	
			InputCase++;				
				if(InputCase==0x52)
				{
					bitDispFlash=0;
					bitLookSysDatas=1;
						
				InputCase =0x50;
					
				}				
	      							
			}
		}	
		if (ConsumMode==CONSUM_NUM && (InputCase&0xf0)!=0x70 &&  InKeyValue == KEY_MUL)//倒查流水，菜号+份数
		{
			InputCase=0x60;
			SerialUnion.S_DatasStruct.Comd = QueryConsumeRecordCmd;//	最近三笔消费查询
//			OSSemPost(NeedSendData_Semp);  //发送信号量 数据发送	
		}

	}
	status=0x20;
	if (!InputCase)//取消费额部分
	{
		bitAddStatus=0;
		if (!bitHaveLedError )
		{
	       if (ConsumMode==CONSUM_SIMPLE)
			{
				CurrentConsumMoney=0;
				if (( InKeyValue&(KEY_0_9|KEY_POINT|KEY_ADD)))			
					InputCase=0x70;
			}
			
			if (ConsumMode==CONSUM_RATION)
				 	InputCase=0x70;
			else 
			{
				InputCount=0;
				InputMaxNum=0;
				CurrentConsumMoney=0;
				if (ConsumMode==CONSUM_NUM)	
				{
					if ((InKeyValue & KEY_0_9) && (InKeyValue!=KEY_0))
						InputCase=0x70;
				}
				else
				{
					if (InKeyValue &(KEY_0_9|KEY_POINT) )
						InputCase=0x70;					
				}
			}
		}
		if (!bbit)
			DispSumConsumMoney(ConsumMode,0xff,CurrentConsumMoney);
	}
	if (!bitHaveLedError)
	{

		if ((InputCase&0xf0)==0x30)
		{
			if(InKeyValue == KEY_ADD)
			{
				
				InputCase=0;
				bitHaveReadBalance=0;
				CurrentConsumMoney=0;
				LookOk=0;
				fristLook=0;
				bitAddStatus=0;
				NeedDispFlag = 0;
				ConsumMode = CONSUM_MONEY;
				if(isReg == 0) istuoji = 1;
				DispSumConsumMoney(ConsumMode,0xff,CurrentConsumMoney);	
			}
			
			SetSysPrince(0,InKeyValue);	//菜号	
		}
		if ((InputCase&0xf0)==0x40)
		{
			//SetSysPrince(1,InKeyValue);// 菜单+份数
		}
			//	上传所有的记录
		if ((InputCase&0xf0)==0x80)
		{}	
	
		if ((InputCase&0xf0)==0x50)
			LookSysPatameter();//查询日消费额和次数  简易+份数
		if ((InputCase&0xf0)==0x60) 
			LookRecordDatas(InKeyValue);//流水查询 菜号+份数
		if ((InputCase&0xf0)==0x70)
		{
			if((Purse_num>1) && (ConsumMode<2))//限制消费
				status=0x20;
			else
				status=InputSumConsumMoney(0,ConsumMode,InKeyValue);
		}
	}
	return	status;
}


void	SetConsumMode(ulong	lll)
{
	uchar	i;
	if (lll==KEY_SIMPLE)
		i=CONSUM_SIMPLE;
	else if (lll==KEY_NUM)
		i=CONSUM_NUM;
	else if (lll==KEY_RATION)
		i=CONSUM_RATION;
	else
		i=CONSUM_MONEY;
	if (ConsumModeEnable & (1<<i))
		ConsumMode=i;
	else
		BeepOn(3);
}

void	LookSysPatameter( )//察看系统参数
{
	uchar	PPage;
	ulong	iii;
	uint	ii;
	uchar 	DatasBuffer[32];
	
  if(NeedDispFlag)
	{
		//-----------------------------------------------------------------------
		if (InputCase==0x50||InputCase==0x52) //消费总额
		{
			iii= ChgBCDStringToUlong(LookMony,4);
			memcpy(DatasBuffer,"\x77\x08\x08\x08\x08",5);
			ChgUlongToLongDispBuffer(iii,DatasBuffer+5);
			LED_DispDatas_all(DatasBuffer);
		}
		else if (InputCase==0x51||InputCase==0x53)  //消费笔数
		{				
			iii= ChgBCDStringToUlong(LookMony+4,3);
			memcpy(DispBuffer,"\x0e\x08\x08\x08\x08",5);//J____
			DispBuffer[5]=Disp0_9String[iii/10000];
			iii=iii%10000;
			DispBuffer[6]=Disp0_9String[iii/1000];
			iii=iii%1000;
			DispBuffer[7]=Disp0_9String[iii/100];
			iii=iii%100;
			DispBuffer[8]=Disp0_9String[iii/10];
			DispBuffer[9]=Disp0_9String[iii%10];
			LED_DispDatas_all(DispBuffer); 
		}
	}
	if (InputCase>=0x53)
	    InputCase=0x50;
	//-----------------------------------------------------------------------	
}
void	LookRecordDatas(ulong InputKeyValue)//察看最近的消费记录
{	
	ulong iii =0;
	ulong aaa = 0;
	if(!NeedDispFlag )
		return;
	
		if (InputCase==0x60)
	{
		
		InputBuffer[0]=0;
		InputBuffer[1]=0;
		InputCase=0x61;	 
	
	}
	if (InputKeyValue==KEY_ENTER)
	{
		
		InputBuffer[1]++;
		InputBuffer[1]=InputBuffer[1]%3;
	
	}
	  
	switch(InputBuffer[1])
	{
		case	0://第一笔消费
			aaa = ChgBCDStringToUlong(ThreeRecord,4);
			DispBuffer[0]=0x77;
			DispBuffer[1]=Disp0_9String[(aaa/1000)%10];
			DispBuffer[2]=Disp0_9String[(aaa/100)%10];
			DispBuffer[3]=Disp0_9String[(aaa/10)%10];
			DispBuffer[4]=Disp0_9String[aaa%10];
			iii=((ulong)(ThreeRecord[5]&0x0f))*0x10000+((ulong)ThreeRecord[6])*0x100+((ulong)ThreeRecord[7]);
			ChgUlongToLongDispBuffer(iii,DispBuffer+5);	
			break;
		case	1://第二笔消费
			aaa = ChgBCDStringToUlong(ThreeRecord+8,4);
			DispBuffer[0]=0x7c;
			DispBuffer[1]=Disp0_9String[(aaa/1000)%10];
			DispBuffer[2]=Disp0_9String[(aaa/100)%10];
			DispBuffer[3]=Disp0_9String[(aaa/10)%10];
			DispBuffer[4]=Disp0_9String[aaa%10];
			iii=((ulong)(ThreeRecord[13]&0x0f))*0x10000+((ulong)ThreeRecord[14])*0x100+((ulong)ThreeRecord[15]);
			ChgUlongToLongDispBuffer(iii,DispBuffer+5);			
			break;
		case	2://第三笔消费
			aaa = ChgBCDStringToUlong(ThreeRecord+16,4);
			DispBuffer[0]=0x39;
			DispBuffer[1]=Disp0_9String[(aaa/1000)%10];
			DispBuffer[2]=Disp0_9String[(aaa/100)%10];
			DispBuffer[3]=Disp0_9String[(aaa/10)%10];
			DispBuffer[4]=Disp0_9String[aaa%10];
			iii=((ulong)(ThreeRecord[21]&0x0f))*0x10000+((ulong)ThreeRecord[22])*0x100+((ulong)ThreeRecord[23]);
			ChgUlongToLongDispBuffer(iii,DispBuffer+5);		
			break;
		default:
			break;
	}
	LED_DispDatas_all(DispBuffer);

}
//发送查询数据
void    LookConsumMoney(void)
{

}

void	PrintConsumDatas(uchar bbit)
{
	 uchar	PrintTopDatas[]={
		//"\x0c\x00\x00"
		 "           消费清单     ""\x0d\x00"
	  	"站点号:-------------------""\x01\x00"
	  	"卡编号:------------------""\x02\x00"
	   	"消费额:----------------""\x03\x00"
		"新余额:----------------""\x04\x00"
		"消费日期:20""\x05""年""\x06""月""\x07""日 ""\x08"":""\x09"":""\x0a\x0"
		"\x0"
		"     谢谢惠顾欢迎下次光临""\x0b\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff"};

	uchar	Buffer[16];
	uchar	aa;
	uchar * ptr;
	ulong	iii;
	uchar	i;
	uint	ii;
	Init_Printer();
	ptr=PrintTopDatas;
	Usart2ToPrint();
	Read_Sysdate(SysTimeDatas.TimeString);
	memcpy(Pos_SysDate,SysTimeDatas.TimeString,6);
		
	while (1)
	{
		aa= * ptr++;
		if (aa==0xff)
			break;
		else if (aa==0x0b)
		{
			if (bbit)
			{
				DK_USART_PUTS_Arry(2,"(复打)",6);
				delay_ms(1);
			}			
		}
		else  if (!aa)
		{
			Buffer[0]=0x0d;
			Buffer[1]=0x0a;
			DK_USART_PUTS_Arry(2,Buffer,2);
			delay_ms(1);
		}
		else if (aa>=0x80 && aa<0xff)
		{
			Buffer[0]=aa;
			Buffer[1]= * ptr++;
			DK_USART_PUTS_Arry(2,Buffer,2);		
		}
		else if (aa<0x80 && aa>=0x20 )			
		DK_USART_PUTS_Arry(2,&aa,1);
		else
		{
			memset(Buffer,0x20,16);
			switch(aa)
			{
				case	1://站点
					Buffer[0]=0x30+MainCode/10000;
					ii=MainCode%10000;
					Buffer[1]=0x30+MainCode/1000;
					ii=ii%1000;
					Buffer[2]=0x30+MainCode/100;
					ii=ii%100;
					Buffer[3]=0x30+ii/10;
					ii=ii%10;
					Buffer[4]=0x30+ii;
					DK_USART_PUTS_Arry(2,Buffer,5);				
					break;
				case	2://卡号
					for (i=0;i<3;i++)
					{
						Buffer[i*2]=0x30+CardPrinterNum[i]/0x10;
						Buffer[i*2+1]=0x30+CardPrinterNum[i]%0x10;
					}
					DK_USART_PUTS_Arry(2,Buffer,6);							
					break;
				case	3://消费额				 
					ChgMoneyToPrintBuffer('-',SumPrintMoney,Buffer);
					DK_USART_PUTS_Arry(2,Buffer,9);	
					break;
				case	4://新余额
          iii=((ulong)printconsumeMoney[1])*0x10000+((ulong)printconsumeMoney[2])*0x100+((ulong)printconsumeMoney[3]);;
					if (MoneyMulValue==10)
						iii*=10;
					else if (MoneyMulValue==100)
						iii*=100;
					ChgMoneyToPrintBuffer('-',iii,Buffer);							
					DK_USART_PUTS_Arry(2,Buffer,9);	
					break;
				case	5://年
					aa=BCDToHex(Pos_SysDate[0]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					DK_USART_PUTS_Arry(2,Buffer,2);
					break;
				case	6://月
					aa=BCDToHex(Pos_SysDate[1]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					DK_USART_PUTS_Arry(2,Buffer,2);
					break;
				case	7://日
					aa=BCDToHex(Pos_SysDate[2]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					DK_USART_PUTS_Arry(2,Buffer,2);
					break;
				case	8://时
					aa=BCDToHex(Pos_SysDate[3]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					DK_USART_PUTS_Arry(2,Buffer,2);

					break;
				case	9://分
					aa=BCDToHex(Pos_SysDate[4]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					DK_USART_PUTS_Arry(2,Buffer,2);
					break;
				case	0x0a://秒
					aa=BCDToHex(Pos_SysDate[5]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					DK_USART_PUTS_Arry(2,Buffer,2);
					break;
				default:
					break;	
			}
		}		
		Clr_WatchDog();
	} 
	Usart2ToErMa(); 	
}  

void	Init_Printer(void)
{
	uchar	Buffer[2];
	Buffer[0]=0x1b;
	Buffer[1]=0x40;
	DK_USART_Send_Char(2,Buffer[0]);
	DK_USART_Send_Char(2,Buffer[1]);
	delay_ms(10);
}

void	ChgMoneyToPrintBuffer(uchar aa,ulong lll,uchar * ptr)
{
	uchar	i;
	ulong	iii=10000000;
	uchar	bbit=0;	
	uchar	bb;
	for (i=0;i<8;i++)
	{
		if (i==6)
			* ptr++=0x2e;
		bb=(uchar) (lll/iii);
		if (!bb)
		{
			if (!bbit && i<5)
				*ptr++=aa;
			else
				* ptr++=0x30+bb;	
		}
		else
		{
			bbit=1;
			* ptr ++=0x30+bb;
		}
		lll=lll%iii;	
		iii=iii/10;
	}
}


//zjx_change_201312
uchar	CheckPinSub(ulong ReadKeyValue )//PIN认证
{
	uchar	i;	
	uchar	st_data;
	DispBuffer[0]=0x73;//P
	DispBuffer[1]=0x06;//I
	DispBuffer[2]=0x37;//N
	memset(DispBuffer+3,0,7);
	if (ReadKeyValue & KEY_0_9 && PinCount<6)
	{//输入PIN
		st_data=ChgKeyValueToChar(ReadKeyValue);
		if (!PinCount)
			InputBuffer[0]=st_data<<4;
		else if (PinCount==1)
			InputBuffer[0]+=st_data;
		else if (PinCount==2)
			InputBuffer[1]=st_data<<4;
		else if (PinCount==3)
			InputBuffer[1]+=st_data;
		else if (PinCount==4)
			InputBuffer[2]=st_data<<4;
		else
			InputBuffer[2]+=st_data;
		PinCount++;
	}
	else if (ReadKeyValue==KEY_ENTER && PinCount==6)
	{
		memcpy(InPutPinCode,InputBuffer,3);
		
		Tx_Buffer[0] = (uchar)MainCode;
		Tx_Buffer[1]=0x18;
		memcpy(Tx_Buffer+2,Consum_Datas+1,6);//交易流水号
		memcpy(Tx_Buffer+8,InPutPinCode,3);//输入密码	
		Tx_Buffer[11] = AddQuFan(Tx_Buffer+1,10);						
	  Write_SOCK_Data_Buffer(0,Tx_Buffer,12);						
		OutTimeFlag = 2;//超时计次
		PinCount = 0;
		return 0 ;
	}
	else if (ReadKeyValue==KEY_ESC)
		return	0x01;
	for (i=0;i<PinCount;i++)
		DispBuffer[3+i]=0x40;

	LED_DispDatas_all(DispBuffer);
	return	0xff;	
}














