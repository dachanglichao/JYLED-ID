
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

//extern uint		MenuSort;//�˺�
//extern uchar	ConsumCase;
//uchar QueryConsumeResult(void);
//uchar   POSVersion =1;//����汾
//extern void	Clr_PosSub(uchar bbit,uchar Sort);

///*********************************************************
////�����������

//*********************************************************/
//void	TCP_ReceiveSub(void)//�������ݴ���
//{
//	uchar status,i;
//	ulong iii,Addr,ii;
//	uchar Buffer[32];
//	
//	
//	switch(SerialUnion.S_DatasStruct.Comd)
//	{
//		case HeartBeatCmd://����������	
//			HeartBeatNum = 0;//��������������
//			Consum_Status = 0;   //����״̬
//			DispBuffer[10]=DispModeChar[ConsumMode];
//			Set_Sysdate(SerialUnion.S_DatasStruct.Datas);
//			Read_Sysdate(SysTimeDatas.TimeString);
//		break;
//		
//		case QueryDayConsumeCmd://���������Ѷ�
//			HaveSendDataFlag =0;
//			NeedDispFlag =1;
//			memcpy(LookMony,SerialUnion.S_DatasStruct.Datas,8);
//		break;
//		
//		case AppointDayConsumeCmd://ָ��ĳ�յ����ѽ���ѯ
//			NeedDispFlag =1;
//		HaveSendDataFlag =0;
//			memcpy(LookMony,SerialUnion.S_DatasStruct.Datas,8);
//		break;
//		
//		case QueryConsumeRecordCmd://����������ʵ����Ѽ�¼
//			  NeedDispFlag =1;
//		HaveSendDataFlag =0;
//		memcpy(ThreeRecord,SerialUnion.S_DatasStruct.Datas,24);
//		break;
//				
//		case	Clr_PosData://���
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
//		case	Limt_Consume_COMD://���������޶�	
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
//		case ADD_BLKNUM_COMD://���Ӱ�����
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
//		case DEL_BLKNUM_COMD://ɾ��������
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
//		case CLR_BLKNUM_COMD://��հ�����
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
//	case	Rd_Recode_Comd://�ѻ���¼��ָ��
//			
//					Addr =(ulong)(offline_UpIndex)*RECORD_SIZE;
//					status = 0;
//					Flash_Write_Bytes(Addr,&status,1);//�����¼���
//          offline_UpIndex++;
//		break;
//		case	Request_CardErr1://����״̬
//		case	Request_CardErr2://����״̬
//		case	Request_CardErr3://����״̬
//		case	Request_CardErr4://����״̬
//		case	Request_CardErr5://����״̬
//		case	Request_CardErr6://����Ч��
//		case	PinCode_Error:	//�������
//		case Request_CardCon://��������
//			HaveQueryConDat = 1;
//			HaveSendDataFlag =0;
//		break;
//		
//		default:
//			ConsumCase = 0;
//		break;
//	}
//}
////���ݷ������ݲ�ѯ���ѽ��
//uchar QueryConsumeResult(void)
//{
//		if(SerialUnion.S_DatasStruct.Comd==0x22)//���ѳɹ�
//		{
//			memcpy(Consum_Datas+1,SerialUnion.S_DatasStruct.Datas,6);//��������
//			return SYS_OK	;
//		}
//		else
//		{
//			Consum_Datas[0] =   SerialUnion.S_DatasStruct.Comd;
//			memcpy(Consum_Datas+1,SerialUnion.S_DatasStruct.Datas,6);//��������
//			return (SerialUnion.S_DatasStruct.Comd-0x12);//���ش�����
//		}
////	return SYS_OK	;
//}
///***************************************************
////����������
//***************************************************/
//void HeartBeat(void)
//{
//	Tx_Buffer[0] = (uchar)MainCode;
//	memcpy(Tx_Buffer+1,"\xc8\x00\x00\x00\x00\x00\x00\x00",8);
//	Tx_Buffer[9] = AddQuFan(Tx_Buffer+1,8);
//	Write_SOCK_Data_Buffer(0,Tx_Buffer,10);
//}
///***************************************************
////��ѯ���������ܶ������
//***************************************************/
//void QueryDayConsume(void)
//{
//  Tx_Buffer[0] = (uchar)MainCode;
//	memcpy(Tx_Buffer+1,"\xF1\x00\x00\x00\x00\x00\x00",7);
//	Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//	Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//}

///***************************************************
////��������
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
//		Tx_Buffer[i++]=(*(2+((uchar *)(&CurrentConsumMoney))))&0x0f;//���θ�λ
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
//		Tx_Buffer[5]=(*(2+((uchar *)(&CurrentConsumMoney))))&0x0f;//���θ�λ
//		Tx_Buffer[6]=*(1+((uchar *)(&CurrentConsumMoney)));
//		Tx_Buffer[7]=*(0+((uchar *)(&CurrentConsumMoney)));
//		

//		Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//		Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//	}
//	
//}
///***************************************************
////��ѯ������ʵ����Ѽ�¼
//***************************************************/
//void QueryConsumeRecord(void)
//{
//	Tx_Buffer[0] = (uchar)MainCode;
//	memcpy(Tx_Buffer+1,"\xAF\x00\x00\x00\x00\x00\x00",7);
//	Tx_Buffer[8] = AddQuFan(Tx_Buffer+1,7);
//	Write_SOCK_Data_Buffer(0,Tx_Buffer,9);
//}

///***************************************************
////��ѯĳ�յ����ѽ�������
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
////���ݷ���
//************************************************************/
//void TCP_SendSub(uchar Cmd)
//{
//	switch(Cmd)
//	{
//		case HeartBeatCmd://��������
//			HeartBeat();
//		break;
//		
//		case QueryDayConsumeCmd://���������Ѷ�
//			QueryDayConsume();
//		HaveSendDataFlag=2;
//		break;
//		
//		case QueryConsumeRecordCmd://����������ʵ����Ѽ�¼
//			NeedDispFlag  =0;
//			QueryConsumeRecord();
//		HaveSendDataFlag=2;
//		break;
//		
//		case RequestConsumeCmd://��������
//			RequestConsume();
//		HaveSendDataFlag=2;
//		break;
//		
//		case AppointDayConsumeCmd://ָ��ĳ�յ������ܶ��ѯ
//			QueryAppointdAYConsumeMoney();
//		HaveSendDataFlag=2;
//		break;
//		
//		default:
//		break;
//	}
//}

