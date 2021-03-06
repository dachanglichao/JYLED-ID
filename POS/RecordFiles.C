#include "MyDefine.h"
#include "include.h"
#include "char.h"



ulong		Sys_SumConsumMoney=0;//消费额累计
uint		RecordSum=0;//记录总数
uint		NoCollectRecordSum=0;//没有采集的记录总数
uint		NoCollectRecordIndex=0;//没有采集的记录指针
uint		ReCollectRecordIndex=0;//已经采集的记录指针
uint		SaveRecordIndex=0;//存储记录指针
uint		ReCollectAddrBak;
uint		ConsumCount=0;//消费流水号
uchar		SerialSendNoCollectNum=0;//上次传送的未采记录个数
uchar		SerialSendReCollectNum=0;//上次传送的复采记录个数

uint		ReCollectCount=0;
uchar		bitReCollect=0;
uint		PosConsumCount=0;//POS的消费流水号
extern OS_EVENT *SemFlashSpi;  //互斥信号量


void	InitSaveRecordSub(uchar);//初始化指针
uchar	RecordBlockDiag(uchar);//00 -无记录 ;11-满全部未采; 22-全部为复采记录
void	SaveRecordSub(void);//存储消费记录
uchar		RecordDatasToBuffer(uchar *);//存储记录的内容写入缓存
void 	InitUdp_SerNum(void);
void	FindPosConsumCountSub(void);//查找最后一条记录的流水号

void	SaveRecordSub(void)//存储消费记录
{
	uchar       Curr_BlockNum;   
	uint		Addr;
	uchar		i;
	uchar		Buffer_39SF020[32];
	uchar		Buffer[32];
	uchar		status;
	u8 err;
	uchar	AllFF[32]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
						   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	
	OSMutexPend(SemFlashSpi,0,&err);  //申请互斥资源使用
	if (RecordDatasToBuffer(Buffer_39SF020))	
	{
		OSMutexPost(SemFlashSpi);  //互斥资源释放
		return;
	}

	Sys_SumConsumMoney+=SumConsumMoney;
	SumPrintMoney+=SumConsumMoney;
	while(1)
	{
		Addr=(ulong)SaveRecordIndex	*RECORD_SIZE;
	 	Flash_Rd_Bytes(Addr,Buffer,22);
		status=memcmp(Buffer,AllFF,22);
		if (!status)
		{
			status=1;
			for (i=0;i<5;i++)
			{
				Flash_Write_Bytes(Addr,Buffer_39SF020,22);//写入
				Flash_Rd_Bytes(Addr,Buffer,22);
				if (!memcmp(Buffer,Buffer_39SF020,22))
				{
					PosConsumCount++;
					status=0;
					break;
				}
			}
			if (!status)//成功
			{
				SaveRecordIndex++;                              //记录指针指向下一单元                
			  	SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;	//控制最后一条跳转回初始位置
	          	if (!(SaveRecordIndex&0xff))//每个扇区开始
			    {       
			  	   Curr_BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
				   
		           Addr=(ulong)Curr_BlockNum*0x1000;
		           Erase_One_Sector(Addr);  //擦扇区	
			   }
				break;
			}
			else   //写错误,当前单元写0数据写入下个单元
			{
			  memset(Buffer,0,22);
			  Flash_Write_Bytes(Addr,Buffer,22);
			  SaveRecordIndex++;
			  SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;
			  if (!(SaveRecordIndex&0xff))//每个扇区开始
			    {       
			  	   Curr_BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
		           Erase_One_Sector(Curr_BlockNum);  //擦扇区	
//						OSMutexPost(SemFlashSpi);  //互斥资源释放
			    }
			}		
		}
		else
		{
			SaveRecordIndex++;
			SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;  
		    if (!(SaveRecordIndex&0xff))//如果记录满，回到起始地址，擦除第1块
			{       
			  	Curr_BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
		        Erase_One_Sector(Curr_BlockNum);  //擦扇区	
			}
		}
		Clr_WatchDog();	
	}
	InitSaveRecordSub(1);	
	OSMutexPost(SemFlashSpi);  //互斥资源释放
}
uchar	RecordDatasToBuffer(uchar * ptr)//存储记录的内容写入缓存
{
//	if (!SumConsumMoney )
//		return	 1;	
//	ptr[0] = 0x03;
//	memcpy(ptr+1,send_buf+1,6);
//	memcpy(ptr+7,Consum_Datas+1,3);
//	memcpy(ptr+10,send_buf+10,6); 
//	memcpy(ptr+16,send_buf+16,5);
//	ptr[21]=CalCheckSum(ptr,21);
	return	0;
}


void	InitSaveRecordSub(uchar bbit )//初始化指针
{
 	uchar  			i;
	uint			Addr,jj;
	uchar			st_data;
	uchar			status;
	uchar			EndBlock;
	uchar			StartBlock;
	uchar 	bitFindReIndex=0; //复采记录指针
	uchar		bitFindNoIndex=0;//未采指针
	uchar		bitFindSymbl=0; 
	RecordSum=0;//记录总数
	NoCollectRecordSum=0;//没有采集的记录总数
	NoCollectRecordIndex=0;//没有采集的记录指针
	if (!bbit)
		ReCollectRecordIndex=0;//已经采集的记录指针
	SaveRecordIndex=0;//存储记录指针
	for (i=0;i<RECORD_BLOCK_NUM;i++)
	{//找出记录的起始块
		status=RecordBlockDiag(i);
		if ( (status&0xf0)&& !(status&0x0f))
		{//该块数据有记录但未满。下一块为起始记录的位置
			bitFindSymbl=1;
			EndBlock=i;
			break;
		}
		else if ((status&0xf0) && (status&0x0f))
		{//如果该块已满，诊断下一块的记录
			status=RecordBlockDiag((i+1)%RECORD_BLOCK_NUM);
			if (!status)
			{//如果下一个记录为空,则下一块为起始记录起始块
				bitFindSymbl=1;
				EndBlock=i;
				break;
			}
		}
	}
	if (!bitFindSymbl)//不存在记录，返回
	{//没有找到有记录的块
		status=RecordBlockDiag(0);	
		if (!status)//如果第一块为空，则没有存储记录
			return;//无记录
		else//如果第一块为满, 
		{
			for (i=0;i<RECORD_BLOCK_NUM;i++)
			{//如果某块含有未采记录，下块为采集过的记录，则该快为起始块
				status=RecordBlockDiag(i);	
				if (status==0x11 || status==0x21)
				{
					status=RecordBlockDiag((i+1)%RECORD_BLOCK_NUM);	
					if (status==0x22)
					{
						bitFindSymbl=1;
						EndBlock=i;
						break;
					}
				}
			}
			if (!bitFindSymbl)
				EndBlock=RECORD_BLOCK_NUM-1;//如果全部为未采记录或采集过的记录，第1快为开始
		}		
	}
	//记录存储指针
	st_data=RecordBlockDiag(EndBlock);
	if ((st_data&0xf0) && (st_data&0x0f) )
	{//如果最后一块的记录已满，下块的起始地址为存储指针
		if (EndBlock==RECORD_BLOCK_NUM-1)
			SaveRecordIndex=0;//如果记录出口为最后一块的最后一条记录，则记录存储指针为0
		else
			SaveRecordIndex=(EndBlock+1)*RECORDNUM_BLOCK;//否则为下一块的第一条		
	}
	else
	{//当前块没有满，找出下次存储记录的索引号
		SaveRecordIndex=EndBlock*RECORDNUM_BLOCK;	
		Addr= (ulong)EndBlock* 0x1000;
		for (jj=0;jj<RECORDNUM_BLOCK;jj++)
		{
			Flash_Rd_Bytes(Addr,&st_data,1);//块的第一条记录
			Addr+=RECORD_SIZE;
			if (st_data==0xff)
			{//找到存储记录的出口
				SaveRecordIndex+=jj;
				break;
			}
		}	
	}
	NoCollectRecordIndex=SaveRecordIndex;
///////////////////////运行到此，找到记录的出口，即下次存储记录的指针//////////////////////�
	StartBlock=(EndBlock+1)%RECORD_BLOCK_NUM;//下块为记录的入口
	for (i=0;i<RECORD_BLOCK_NUM;i++)
	{//从记录的开始位置检索
		status=RecordBlockDiag(StartBlock);	
	
		switch (status)
		{
			case	0x10:// 开始未采,后为空
				if (!bitFindReIndex)
				{//如果未找到复采入口，置该位置为复采入口
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				if (!bitFindNoIndex)
				{//如果未找到采集入口，置该位置为采集入口
					bitFindNoIndex=1;
					NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				Addr=(ulong)StartBlock*0x1000;
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//块的第一条记录
					Addr+=RECORD_SIZE;
					if (st_data!=0xff)
					{
						RecordSum++;//记录总数
						NoCollectRecordSum++;//未采记录总数	
					}
					else
						break;
				}
				break;
			case	0x11://开始和最后都是未采集
				if (!bitFindReIndex)
				{//如果未找到复采入口，置该位置为复采入口
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				if (!bitFindNoIndex)
				{//如果未找到采集入口，置该位置为采集入口
					bitFindNoIndex=1;
					NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				RecordSum+=RECORDNUM_BLOCK;
				NoCollectRecordSum+=RECORDNUM_BLOCK;
				break;
			case	0x20: //开始已采集，最后无记录 
				if (!bitFindReIndex)
				{//如果未找到复采入口，置该位置为复采入口
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				Addr=(ulong)StartBlock*0x1000;			
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//块的第一条记录
					Addr+=RECORD_SIZE;
					if (st_data!=0xff)
					{
						RecordSum++;//记录总数
						if (st_data==0xa0)
						{
							NoCollectRecordSum++;
							if (!bitFindNoIndex)
							{//如果未找到采集入口，置该位置为采集入口
								bitFindNoIndex=1;
								NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK+jj;	
							}	
						}
				   	}
					else
						break;
				}
				break;
			case	0x21://开始已采集，最后未采集
				if (!bitFindReIndex)
				{//如果未找到复采入口，置该位置为复采入口
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}			
				RecordSum+=RECORDNUM_BLOCK;
				Addr=(ulong)StartBlock*0x1000;
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)	
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//块的第一条记录
					Addr+=RECORD_SIZE;
					if (st_data==0xa0)
					{
						NoCollectRecordSum++;
						if (!bitFindNoIndex)
						{//如果未找到采集入口，置该位置为采集入口
							bitFindNoIndex=1;
							NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK+jj;	
						}
					}					
				}
				break;
			case	0x22://第一条和最后一条都采集过
				if (!bitFindReIndex)
				{//如果未找到复采入口，置该位置为复采入口
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				RecordSum+=RECORDNUM_BLOCK;
				break;
			default:
				break;											
		}
		StartBlock=(StartBlock+1)%RECORD_BLOCK_NUM;
	}
//以上程序找到了开门记录的记录总数，复采记录初试指针，和最后一条消费记录
}	
uchar	RecordBlockDiag(uchar BlockNum)
//该块记录存储情况 对于39SF020--为0-63块
//00 -无记录 ;11-满全部未采; 2-复采记录
{
	uint		Addr;
	uchar		st_data;
	uchar		status=0;


	Addr=(ulong)BlockNum* 0x1000;
	Flash_Rd_Bytes(Addr,&st_data,1);
	if (st_data==0xa0)
		status=0x10;
	else if (st_data==0xff)
		status=0;
	else
		status=0x20;
	Addr=Addr+0x1000-RECORD_SIZE;//最后一条
	Flash_Rd_Bytes(Addr,&st_data,1);
	if (st_data==0xa0)
		status+=1;
	else if (st_data==0xff)
		status&=0xf0;
	else
		status+=2;
	return	status;	
}

void	Save_OffRecode(void)//存储脱机记录
{ 
	uint		Addr;
	uchar		i;
	uchar		Buffer_39SF020[24];
	uchar		Buffer[32];
	uchar		status;

	uchar	AllFF[32]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
						   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	Buffer_39SF020[0]=0xa0;
	memcpy(Buffer_39SF020+1,NoNetRecord,19);
	Buffer_39SF020[20]=CalCheckSum(Buffer_39SF020+1,19);

	while(1)
	{
		Addr=(ulong)offline_ReIndex*RECORD_SIZE;	
		Flash_Rd_Bytes(Addr,Buffer,32);
		status=memcmp(Buffer,AllFF,32);
		if (!status)
		{
			status=1;
			for (i=0;i<5;i++)
			{
				Flash_Write_Bytes(Addr,Buffer_39SF020,21);//写入
				Flash_Rd_Bytes(Addr,Buffer,21);
				if (!memcmp(Buffer,Buffer_39SF020,21))
				{
					PosConsumCount++;
					status=0;
					break;
				}
			}
			if (!status)//成功
			{
				offline_ReIndex++; //记录指针指向下一单元                
				offline_ReIndex = offline_ReIndex%Off_MAXRECORD;
				flag_off_full=RecordFullDiag();
				offline_UpIndex = 0;
				break;
			}
		}
		if(status)  //写错误,当前单元写0数据写入下个单元
		{
			memset(Buffer,0,21);
			Flash_Write_Bytes(Addr,Buffer,21);

			offline_ReIndex++; //记录指针指向下一单元                
			offline_ReIndex = offline_ReIndex%Off_MAXRECORD;
			flag_off_full=RecordFullDiag();
			offline_UpIndex = 0;
			if(flag_off_full)
				break;	
		}	
		Clr_WatchDog();	
	}	
}

uchar	RecordBlockDiag1(uchar BlockNum)
//该块记录存储情况 对于39SF020--为0-63块
//00 -无记录 ;11-满全部未采; 2-复采记录
{
	uint		Addr;
	uchar		st_data;
	uchar		status=0;

	Addr=(ulong)BlockNum* 0x1000;
	Flash_Rd_Bytes(Addr,&st_data,1);
	if (st_data==0xa0)
		status=0x10;
	else if (st_data==0xff)
		status=0;
	else
		status=0x20;
	Addr=Addr+0x1000-32;//最后一条
	Flash_Rd_Bytes(Addr,&st_data,1);
	if (st_data==0xa0)
		status+=1;
	else if (st_data==0xff)
		status&=0xf0;
	else
		status+=2;
	return	status;	
}

void InitOffLineRecord(void)//初始化脱机记录指针
{//这是一个简单的顺序结构
	uchar  			i;
	uint			Addr,jj;
	uchar			st_data;
	uchar			status;
	uchar			EndBlock;
	uchar			bitFindSymbl=0; 

	offline_ReIndex=0;//存储记录指针
	for (i=0;i<Off_RECORD_BLOCK_NUM;i++)
	{//找出记录的起始块
		status=RecordBlockDiag1(i);
		if ( (status&0xf0)&& !(status&0x0f))
		{//该块数据有记录但未满。下一块为起始记录的位置
			bitFindSymbl=1;
			EndBlock=i;
			break;
		}
		else if ((status&0xf0) && (status&0x0f))
		{//如果该块已满，诊断下一块的记录
			status=RecordBlockDiag1((i+1)%Off_RECORD_BLOCK_NUM);
			if (!status)
			{//如果下一个记录为空,则下一块为起始记录起始块
				bitFindSymbl=1;
				EndBlock=i;
				break;
			}
		}
	}
	if (!bitFindSymbl)//不存在记录，返回
	{//没有找到有记录的块
		status=RecordBlockDiag1(0);	
		if (!status)//如果第一块为空，则没有存储记录
			return;//无记录
		else//如果第一块为满, 
		{
			for (i=0;i<Off_RECORD_BLOCK_NUM;i++)
			{//如果某块含有未采记录，下块为采集过的记录，则该快为起始块
				status=RecordBlockDiag1(i);	
				if (status==0x11 || status==0x21)
				{
					status=RecordBlockDiag1((i+1)%Off_RECORD_BLOCK_NUM);	
					if (status==0x22)
					{
						bitFindSymbl=1;
						EndBlock=i;
						break;
					}
				}
			}
			if (!bitFindSymbl)
				EndBlock=15;//如果全部为未采记录或采集过的记录，第1快为开始
		}		
	}
	//记录存储指针
	st_data=RecordBlockDiag1(EndBlock);
	if ((st_data&0xf0) && (st_data&0x0f) )
	{//如果最后一块的记录已满，下块的起始地址为存储指针
		if (EndBlock==15)
			offline_ReIndex=0;//如果记录出口为最后一块的最后一条记录，则记录存储指针为0
		else
			offline_ReIndex=(EndBlock+1)*128;//否则为下一块的第一条		
	}
	else
	{//当前块没有满，找出下次存储记录的索引号
		offline_ReIndex=EndBlock*RECORDNUM_BLOCK;	
		
		Addr= (ulong)EndBlock* 0x1000;
		for (jj=0;jj<128;jj++)
		{
			Flash_Rd_Bytes(Addr,&st_data,1);//块的第一条记录
			Addr+=32;
			if (st_data==0xff)
			{//找到存储记录的出口
				offline_ReIndex+=jj;
				break;
			}
		}	
	}
}

uchar	RecordFullDiag(void)//记录存储是否已满诊断	
{
	uchar		st_data;
	uint		Addr;
	uchar		BlockNum;
	uint	    ii,jj;
	uchar		status;
	uchar		OffLineBlockNum;


	offline_ReIndex=offline_ReIndex%Off_MAXRECORD;

	st_data=* (1+(uchar *)&offline_ReIndex);//每一扇区的开始
 	if (st_data)
		return	0;
	BlockNum=offline_ReIndex/128;
	BlockNum=(BlockNum+1)%Off_RECORD_BLOCK_NUM;
	jj=128;
	Addr=(ulong)BlockNum*0x1000;

	status=0;
	for (ii=0;ii<jj;ii++)
	{
		Flash_Rd_Bytes(Addr,&st_data,1);
		if (st_data==0xa0)
		{
			status=1;
			return	1;
		}
		else
		{
			if (st_data!=0xff)
				status=2;
		}
		Addr+=32;
	}
	if (status)	
	{	
		Addr=(ulong)BlockNum*0x1000;
		Erase_One_Sector(Addr);
		OffLineBlockNum = offline_UpIndex/128;
		if(BlockNum==OffLineBlockNum)
			InitOffLineRecord();
	 }
	return	0;

}

#include "cJSON.h"
int tcp_send_and_rec_packet(int cmd,u8 * data,int sendLength,int entType,u8 * dataRec,int * length,int timeout);;
void	Active_UpOffRe(void)
{
	uint   ii;
	uchar  status;
	uint   Addr;
	uchar  DatasBuffer[32];
	uchar recdata[512];
	int reclen;
	u8 err;
	cJSON *object,*arrayItem;

	//return;
	OSMutexPend(SemFlashSpi,0,&err);  //申请互斥资源使用
	status=0;
	//offline_UpIndex=0;//主动上传脱机记录指针	

	for(ii=offline_UpIndex;ii<Off_MAXRECORD;ii++)
	{		
		Addr=(ulong)offline_UpIndex*RECORD_SIZE;	
		Flash_Rd_Bytes(Addr,DatasBuffer,21);
		if(DatasBuffer[0]==0xa0 && (DatasBuffer[20]==CalCheckSum(DatasBuffer+1,19)) )
		{
			status=1;
			HaveOffRecord =1;
			break;
			
		}
		else
		{
			HaveOffRecord = 0;
			offline_UpIndex++;
			offline_UpIndex = offline_UpIndex%Off_MAXRECORD;
		}
		
		if(!(ii%32))
		{
			Clr_WatchDog();
		}
	}
	
	OSMutexPost(SemFlashSpi);  //互斥资源释放
	
	if(status)//找到可以上传的记录
	{
		flag_reupcode=1; //主动上传脱机记录标志

//		DatasBuffer[19] = offline_UpIndex >> 8;
//		DatasBuffer[20] = offline_UpIndex & 0xFF;
		err = tcp_send_and_rec_packet(UpRecodCmd,DatasBuffer+1,19,1,recdata,&reclen,3);
		if(!err)
		{
			object=cJSON_Parse(recdata); //????????
			if(!object)
			{
				cJSON_Delete(object);	
				return ;
			}
			arrayItem = cJSON_GetObjectItem(object,"message"); //
			if(strcmp(arrayItem->valuestring,"0"))
			{
				cJSON_Delete(object);	
				return ;
			}
			else
			{	
				DatasBuffer[0] = 0;		
				OSMutexPend(SemFlashSpi,0,&err);  //申请互斥资源使用
				Addr=(ulong)offline_UpIndex*RECORD_SIZE;	
				Flash_Write_Bytes(Addr,DatasBuffer,10);
				offline_UpIndex++;
				offline_UpIndex = offline_UpIndex%Off_MAXRECORD;
				OSMutexPost(SemFlashSpi);  //互斥资源释放
				
			}
			cJSON_Delete(object);
		}
		else
		{
			Write_SOCK_Data_Buffer(5,Tx_Buffer,11);
		}	
		//Write_SOCK_Data_Buffer(0,Tx_Buffer,11);
	}
	else//没有记录
	{
		bitSerial_SendRequest=0;
		flag_first_over=1;//完成
		flag_reupcode=2;
		Off_TimeCount =1;
		NeedSendRecordFlag =0;
		
		OSTimeDlyHMSM(0, 0, 2, 0);//将任务挂起500ms	 		
	}
	
}
void   	Up_UpNetRe(void)
{
	uint   ii;
	uchar  status; 
	uint   Addr;
	uchar  DatasBuffer[32];

	
	if(flag_first_over)//启动机制已经完成
		return;
	if(UPD_TimeCount)//命令运行中
		return;
	if(bitSerial_SendRequest ||	bitSerial_ReceiveEnd || SerialReceiveLen )//有发送或接收的数据未处理
		return;
	if(recode_rdnum > 16383)
		recode_rdnum = 0;
	for(ii=0;ii<16384;ii++)
	{
		Addr=(ulong)recode_rdnum *RECORD_SIZE;
	
		Flash_Rd_Bytes(Addr,DatasBuffer,23);		
		if((DatasBuffer[0]==0x03) ||(DatasBuffer[1]==0x03))
		{
			status=1;
			recode_rdnum++;	
			break;
		}
		else
			recode_rdnum++;
		if(!(ii%32))
		{
			Clr_WatchDog();
		}
	}
	if(status)//找到可以上传的记录
	{
		flag_reupcode=0; 
		flag_first_over=1;
		bitSerial_SendRequest=1;
		SerialUnion.S_DatasBuffer[1]=0x03;
		if(recode_rdnum < 4096)
			memcpy(SerialUnion.S_DatasBuffer+2,DatasBuffer+1,20);
		else
			memcpy(SerialUnion.S_DatasBuffer+2,DatasBuffer+2,20);
		//---------------------------------------
		//---------------------------------------
		memcpy(ReCardDatas,NetCardDatas+16,6);
		UPD_TimeCount=UPD_DELAY_TIME;
	}
	else//没有记录
	{
		bitSerial_SendRequest=0;
		flag_first_over=1;//完成
		flag_reupcode=2;
	}
	 	
}
void 	Up_AllRe(void)
{
	
	if(!Consum_Status && recode_rdnum < 16384 )	
	  flag_first_over=0;
	while(1)
	{
		
		if(!Consum_Status)//联网模式下
		{	
		  	Up_UpNetRe();
			if(recode_rdnum > 16383)
			{
				break;
			}
			if(!UPD_TimeCount)
			{
			   Consum_Status=1;
			}
			memcpy(DispBuffer,"\x77\x38\x38\x40\x5e\x3e\x73\x80\x80\x80",10);//off-d up...
			LED_DispDatas_all(DispBuffer);
			
			//udp_server_send_data(SerialUnion.S_DatasBuffer+1);
		}
		else
		{
			memcpy(DispBuffer,"\x00\x37\x5c\x00\x00\x00\x37\x79\x78\x00",10);//no net
			LED_DispDatas_all(DispBuffer);
			break;	
		}
		Clr_WatchDog();
	}
}


void InitUdp_SerNum(void)
{
	uint   ii;
	uchar   find;
	uint   Addr;
	uchar  DatasBuffer[32];

	find=0;	
	Udp_SerialNum=0;
	for(ii=0;ii<Off_MAXRECORD;ii++)
	{		
		Addr=(ulong)offline_UpIndex*RECORD_SIZE;
	
		Flash_Rd_Bytes(Addr,DatasBuffer,23);
		if(DatasBuffer[0]==0xa0 && (DatasBuffer[22]==CalCheckSum(DatasBuffer+1,21)) )
		{
			find=1;//找到第一条未传记录
			break;	
		}
		
		if(!(ii%32))
		{
			Clr_WatchDog();
		}
	}

	if(find)//有脱机记录，以第一条未传记录为序列号
		Udp_SerialNum=(uchar)ii;
	else//无记录，以下一条记录指针为序列号
		Udp_SerialNum=(uchar)SaveRecordIndex;
}
void	FindPosConsumCountSub(void)//查找最后一条记录的流水号
{

	uint	Addr;
	uint	ii;
	uint    jj;
	uint    PosConsumCount1,PosConsumCount2;
	uchar	Buffer[32];
	if (!SaveRecordIndex)
		ii=MAXRECORD_NUM-1;
	else
		ii=SaveRecordIndex-1;
	Addr=(ulong)ii*RECORD_SIZE;

	Flash_Rd_Bytes(Addr,Buffer,22);
	if (Buffer[0]!=0xff && (Buffer[21] == CalCheckSum(Buffer,21)))
		memcpy((uchar *)&PosConsumCount1,Buffer+16,2);
	else
		PosConsumCount1=0;//新flash流水号从0开始记
	if (!offline_ReIndex)
		jj=MAXRECORD_NUM-1;
	else
		jj=offline_ReIndex-1;
	Addr =(ulong)jj*RECORD_SIZE;
		
	Flash_Rd_Bytes(Addr,Buffer,23);
	if (Buffer[0]!=0xff && (Buffer[22] == CalCheckSum(Buffer+1,21)))
		memcpy((uchar *)&PosConsumCount2,Buffer+17,2);
	else
		PosConsumCount2=0;//新flash流水号从0开始记
	if((PosConsumCount2==65535)||(PosConsumCount1==65535))
	{
		if(PosConsumCount2<PosConsumCount1)
			PosConsumCount = PosConsumCount2;
		else
			PosConsumCount = PosConsumCount1;		
	}
	else
	{
		if(PosConsumCount2>PosConsumCount1)
			PosConsumCount = PosConsumCount2;
		else
			PosConsumCount = PosConsumCount1;
	}	
	PosConsumCount++;
}


