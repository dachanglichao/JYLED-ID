
//#include  "stm32f10x_gpio.h"
//#include	"char.h"
//#include "gpio.H"
//#include "usart.H"
//#include "stm32f10x_flash.h"
//#include "externvariable.h"
//#include  "hw_config.h"
//#include "include.h"
//#include "MyDefine.h"
//#include "stdio.h"

//extern uint		MenuSort;//菜号
//extern uchar	ConsumCase;
//uchar QueryConsumeResult(void);
//uchar   POSVersion =1;//程序版本
//extern void	Clr_PosSub(uchar bbit,uchar Sort);

///*********************************************************
////接收命令解析

//*********************************************************/
//void	TCP_ReceiveSub(void)//接收数据处理
//{
//	uchar status,i;
//	ulong iii,Addr,ii;
//	uchar Buffer[32];
//	
//	
//	switch(SerialUnion.S_DatasStruct.Comd)
//	{
//		case HeartBeatCmd://请求心跳包	
//			HeartBeatNum = 0;//心跳计数器清零
//			Consum_Status = 0;   //在网状态
//			DispBuffer[10]=DispModeChar[ConsumMode];
//			Set_Sysdate(SerialUnion.S_DatasStruct.Datas);
//			Read_Sysdate(SysTimeDatas.TimeString);
//		break;
//		
//		case QueryDayConsumeCmd://请求日消费额
//			HaveSendDataFlag =0;
//			NeedDispFlag =1;
//			memcpy(LookMony,SerialUnion.S_DatasStruct.Datas,8);
//		break;
//		
//		case AppointDayConsumeCmd://指定某日的消费金额查询
//			NeedDispFlag =1;
//		HaveSendDataFlag =0;
//			memcpy(LookMony,SerialUnion.S_DatasStruct.Datas,8);
//		break;
//		
//		case QueryConsumeRecordCmd://请求最近三笔的消费记录
//			  NeedDispFlag =1;
//		HaveSendDataFlag =0;
//		memcpy(ThreeRecord,SerialUnion.S_DatasStruct.Datas,24);
//		break;
//				
//		case	Clr_PosData://清空
//				memcpy(Tx_Buffer+1,"\xC9\xaa\xaa\xaa\xaa\xaa\xaa",7);
//		HaveSendDataFlag =0;
//				if(!status)
//				{
//					Clr_PosSub(0,1);  //ad bt lc
//					Clr_PosSub(0,2);	//ad by lc					
//				}
//				else
//				{
//					memcpy(Tx_Buffer+1,"\xC9\x01\x02\x03\x04\x05\x06",7);
//					if(!status)
//						Erase_Entire_Chip();	
//				}
//				
//				Tx_Buffer[0] = (uchar)MainCode;	
//				Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//				Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//		break;
//				
//		case	Limt_Consume_COMD://下载消费限额	
//			HaveSendDataFlag =0;
//				memcpy(LimtConsumeBuf+1,SerialUnion.S_DatasStruct.Datas,4);
//				LimtConsumeBuf[0] = 0xa0;
//				WrBytesToAT24C64(LimtConsume_Addr,LimtConsumeBuf,5);
//	
//				RdBytesFromAT24C64(LimtConsume_Addr,Buffer,5);
//				memcpy(SerialUnion.S_DatasStruct.Datas,Buffer+1,4);	
//				// GetU32_HiLo(Buffer+1);		
//				NoNetLimetMoney =ChgBCDStringToUlong(Buffer+1,4);
//				Tx_Buffer[0] = (uchar)MainCode;
//				memcpy(Tx_Buffer+1,"\x55\xA0\xA1\xA2\xA3\xA4\xA5",7);
//				Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//				Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//		
//			break;
//		
//		case ADD_BLKNUM_COMD://增加白名单
//			if (SerialUnion.S_DatasStruct.Datas[0] )
//			{
//				status=SerialUnion.S_DatasStruct.Datas[0];
//				for(i=0;i<status;i++)
//				{
//					iii=ChgBCDStringToUlong((SerialUnion.S_DatasStruct.Datas+i*4+1),4);
//					if (iii<MAXCARDPRINTERNUM)
//						Chg_BlkNameSub(0,iii);
//				}
//				Tx_Buffer[0] = (uchar)MainCode;
//				memcpy(Tx_Buffer+1,"\x2C\xA0\xA1\xA2\xA3\xA4\xA5",7);
//				Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//				Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//			}	
//			break;
//		//	break;
//		
//		case DEL_BLKNUM_COMD://删除白名单
//			if (SerialUnion.S_DatasStruct.Datas[0] )
//			{	  
//				status=SerialUnion.S_DatasStruct.Datas[0];
//				for(i=0;i<status;i++)
//				{	  
//					iii=ChgBCDStringToUlong((SerialUnion.S_DatasStruct.Datas+i*4+1),4);
//					if (iii<MAXCARDPRINTERNUM)
//						Chg_BlkNameSub(1,iii);
//				}
//				Tx_Buffer[0] = (uchar)MainCode;
//				memcpy(Tx_Buffer+1,"\x4C\xA0\xA1\xA2\xA3\xA4\xA5",7);
//				Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//				Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//			}	
//			break;
//		
//		case CLR_BLKNUM_COMD://清空白名单
//			if(!memcmp(SerialUnion.S_DatasStruct.Datas,"\xA0\xA1\xA2\xA3\xA4\xA5",6))
//			{
//			//	Disp_Clr_WhilteNum();				
//			}	
//			Tx_Buffer[0] = (uchar)MainCode;
//			memcpy(Tx_Buffer+1,"\x44\xA0\xA1\xA2\xA3\xA4\xA5",7);
//			Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//			Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//			Clr_WhiteNum() ;
//			break;
//	case	Rd_Recode_Comd://脱机记录移指针
//			
//					Addr =(ulong)(offline_UpIndex)*RECORD_SIZE;
//					status = 0;
//					Flash_Write_Bytes(Addr,&status,1);//清除记录标记
//          offline_UpIndex++;
//		break;
//		case	Request_CardErr1://错误状态
//		case	Request_CardErr2://错误状态
//		case	Request_CardErr3://错误状态
//		case	Request_CardErr4://错误状态
//		case	Request_CardErr5://错误状态
//		case	Request_CardErr6://密码效验
//		case	PinCode_Error:	//密码错误
//		case Request_CardCon://请求消费
//			HaveQueryConDat = 1;
//			HaveSendDataFlag =0;
//		break;
//		
//		default:
//			ConsumCase = 0;
//		break;
//	}
//}
////根据返回数据查询消费结果
//uchar QueryConsumeResult(void)
//{
//		if(SerialUnion.S_DatasStruct.Comd==0x22)//消费成功
//		{
//			memcpy(Consum_Datas+1,SerialUnion.S_DatasStruct.Datas,6);//消费数据
//			return SYS_OK	;
//		}
//		else
//		{
//			Consum_Datas[0] =   SerialUnion.S_DatasStruct.Comd;
//			memcpy(Consum_Datas+1,SerialUnion.S_DatasStruct.Datas,6);//消费数据
//			return (SerialUnion.S_DatasStruct.Comd-0x12);//返回错误码
//		}
////	return SYS_OK	;
//}
///***************************************************
////心跳包发送
//***************************************************/
//void HeartBeat(void)
//{
//	Tx_Buffer[0] = (uchar)MainCode;
//	memcpy(Tx_Buffer+1,"\xc8\x00\x00\x00\x00\x00\x00\x00",8);
//	Tx_Buffer[9] = AddQuFan(Tx_Buffer+1,8);
//	Write_SOCK_Data_Buffer(0,Tx_Buffer,10);
//}
///***************************************************
////查询当日消费总额跟笔数
//***************************************************/
//void QueryDayConsume(void)
//{
//  Tx_Buffer[0] = (uchar)MainCode;
//	memcpy(Tx_Buffer+1,"\xF1\x00\x00\x00\x00\x00\x00",7);
//	Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//	Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//}

///***************************************************
////请求消费
//***************************************************/

//extern u8 isHaveEWM;
//extern u8 dataOfRecEWMSJ[20];

//void RequestConsume(void)
//{
//	int j = 0;
//	int i = 0;


//	
//	if(isHaveEWM)
//	{
//		
//		Tx_Buffer[i++] = (uchar)MainCode;
//		Tx_Buffer[i++] = 0x01;
//		memcpy(Tx_Buffer+i,dataOfRecEWMSJ,14);
//		i += 14;
//		Tx_Buffer[i++] = 0;
//		Tx_Buffer[i++] = 0;
//		Tx_Buffer[i++] = 0;
//		Tx_Buffer[i++]=(*(2+((uchar *)(&CurrentConsumMoney))))&0x0f;//屏蔽高位
//		Tx_Buffer[i++]=*(1+((uchar *)(&CurrentConsumMoney)));
//		Tx_Buffer[i++]=*(0+((uchar *)(&CurrentConsumMoney)));			

//		Tx_Buffer[i++] = 0;
//		Tx_Buffer[i++] = 0;
//		Tx_Buffer[i++] = 0;
//		Tx_Buffer[i++] = 0;
//		
//		j = i;
//		Tx_Buffer[i++] = AddQuFan(Tx_Buffer+1,j-1);		
//		Write_SOCK_Data_Buffer(0,Tx_Buffer,i);
//	}else
//	{
//	
//			if(ConsumMode==CONSUM_RATION)   
//		{				
//			if(Purse_num==1)
//				Tx_Buffer[1] =0x63;
//			else if(Purse_num==3)
//				Tx_Buffer[1] =0x60;
//			else//2
//				Tx_Buffer[1] =0x62;             
//		}
//		else
//		{
//			if(Purse_num==1)
//				Tx_Buffer[1] =0;
//			else if(Purse_num==3)
//				Tx_Buffer[1] =0x5f;
//			else//2
//				Tx_Buffer[1] =0x61;
//		}
//		
//		Tx_Buffer[2] = CardPrinterNum[0];
//		Tx_Buffer[3] = CardPrinterNum[1];
//		Tx_Buffer[4] = CardPrinterNum[2];
//	//	
//	//	Tx_Buffer[2] = 0x00;
//	//	Tx_Buffer[3] = 0x01;
//	//	Tx_Buffer[4] = 0x07;
//		
//		Tx_Buffer[5]=(*(2+((uchar *)(&CurrentConsumMoney))))&0x0f;//屏蔽高位
//		Tx_Buffer[6]=*(1+((uchar *)(&CurrentConsumMoney)));
//		Tx_Buffer[7]=*(0+((uchar *)(&CurrentConsumMoney)));
//		

//		Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//		Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//	}
//	
//}
///***************************************************
////查询最近三笔的消费记录
//***************************************************/
//void QueryConsumeRecord(void)
//{
//	Tx_Buffer[0] = (uchar)MainCode;
//	memcpy(Tx_Buffer+1,"\xAF\x00\x00\x00\x00\x00\x00",7);
//	Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//	Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//}

///***************************************************
////查询某日的消费金额跟笔数
//***************************************************/
//void QueryAppointdAYConsumeMoney()
//{
//	Tx_Buffer[0] = (uchar)MainCode;
//	Tx_Buffer[1] = AppointDayConsumeCmd;
//	memcpy(Tx_Buffer+2,ComsumeDate,3);
//	memset(Tx_Buffer+5,0x00,4);
//	Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//	Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//	NeedDispFlag =0;
//}
///************************************************************
////数据发送
//************************************************************/
//void TCP_SendSub(uchar Cmd)
//{
//	switch(Cmd)
//	{
//		case HeartBeatCmd://心跳发送
//			HeartBeat();
//		break;
//		
//		case QueryDayConsumeCmd://请求日消费额
//			QueryDayConsume();
//		HaveSendDataFlag=2;
//		break;
//		
//		case QueryConsumeRecordCmd://请求最近三笔的消费记录
//			NeedDispFlag  =0;
//			QueryConsumeRecord();
//		HaveSendDataFlag=2;
//		break;
//		
//		case RequestConsumeCmd://请求消费
//			RequestConsume();
//		HaveSendDataFlag=2;
//		break;
//		
//		case AppointDayConsumeCmd://指定某日的消费总额查询
//			QueryAppointdAYConsumeMoney();
//		HaveSendDataFlag=2;
//		break;
//		
//		default:
//		break;
//	}
//}


