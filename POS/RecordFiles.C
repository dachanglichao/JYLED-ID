#include "MyDefine.h"
#include "include.h"
#include "char.h"



ulong		Sys_SumConsumMoney=0;//���Ѷ��ۼ�
uint		RecordSum=0;//��¼����
uint		NoCollectRecordSum=0;//û�вɼ��ļ�¼����
uint		NoCollectRecordIndex=0;//û�вɼ��ļ�¼ָ��
uint		ReCollectRecordIndex=0;//�Ѿ��ɼ��ļ�¼ָ��
uint		SaveRecordIndex=0;//�洢��¼ָ��
uint		ReCollectAddrBak;
uint		ConsumCount=0;//������ˮ��
uchar		SerialSendNoCollectNum=0;//�ϴδ��͵�δ�ɼ�¼����
uchar		SerialSendReCollectNum=0;//�ϴδ��͵ĸ��ɼ�¼����

uint		ReCollectCount=0;
uchar		bitReCollect=0;
uint		PosConsumCount=0;//POS��������ˮ��
extern OS_EVENT *SemFlashSpi;  //�����ź���


void	InitSaveRecordSub(uchar);//��ʼ��ָ��
uchar	RecordBlockDiag(uchar);//00 -�޼�¼ ;11-��ȫ��δ��; 22-ȫ��Ϊ���ɼ�¼
void	SaveRecordSub(void);//�洢���Ѽ�¼
uchar		RecordDatasToBuffer(uchar *);//�洢��¼������д�뻺��
void 	InitUdp_SerNum(void);
void	FindPosConsumCountSub(void);//�������һ����¼����ˮ��

void	SaveRecordSub(void)//�洢���Ѽ�¼
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
	
	OSMutexPend(SemFlashSpi,0,&err);  //���뻥����Դʹ��
	if (RecordDatasToBuffer(Buffer_39SF020))	
	{
		OSMutexPost(SemFlashSpi);  //������Դ�ͷ�
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
				Flash_Write_Bytes(Addr,Buffer_39SF020,22);//д��
				Flash_Rd_Bytes(Addr,Buffer,22);
				if (!memcmp(Buffer,Buffer_39SF020,22))
				{
					PosConsumCount++;
					status=0;
					break;
				}
			}
			if (!status)//�ɹ�
			{
				SaveRecordIndex++;                              //��¼ָ��ָ����һ��Ԫ                
			  	SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;	//�������һ����ת�س�ʼλ��
	          	if (!(SaveRecordIndex&0xff))//ÿ��������ʼ
			    {       
			  	   Curr_BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
				   
		           Addr=(ulong)Curr_BlockNum*0x1000;
		           Erase_One_Sector(Addr);  //������	
			   }
				break;
			}
			else   //д����,��ǰ��Ԫд0����д���¸���Ԫ
			{
			  memset(Buffer,0,22);
			  Flash_Write_Bytes(Addr,Buffer,22);
			  SaveRecordIndex++;
			  SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;
			  if (!(SaveRecordIndex&0xff))//ÿ��������ʼ
			    {       
			  	   Curr_BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
		           Erase_One_Sector(Curr_BlockNum);  //������	
//						OSMutexPost(SemFlashSpi);  //������Դ�ͷ�
			    }
			}		
		}
		else
		{
			SaveRecordIndex++;
			SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;  
		    if (!(SaveRecordIndex&0xff))//�����¼�����ص���ʼ��ַ��������1��
			{       
			  	Curr_BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
		        Erase_One_Sector(Curr_BlockNum);  //������	
			}
		}
		Clr_WatchDog();	
	}
	InitSaveRecordSub(1);	
	OSMutexPost(SemFlashSpi);  //������Դ�ͷ�
}
uchar	RecordDatasToBuffer(uchar * ptr)//�洢��¼������д�뻺��
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


void	InitSaveRecordSub(uchar bbit )//��ʼ��ָ��
{
 	uchar  			i;
	uint			Addr,jj;
	uchar			st_data;
	uchar			status;
	uchar			EndBlock;
	uchar			StartBlock;
	uchar 	bitFindReIndex=0; //���ɼ�¼ָ��
	uchar		bitFindNoIndex=0;//δ��ָ��
	uchar		bitFindSymbl=0; 
	RecordSum=0;//��¼����
	NoCollectRecordSum=0;//û�вɼ��ļ�¼����
	NoCollectRecordIndex=0;//û�вɼ��ļ�¼ָ��
	if (!bbit)
		ReCollectRecordIndex=0;//�Ѿ��ɼ��ļ�¼ָ��
	SaveRecordIndex=0;//�洢��¼ָ��
	for (i=0;i<RECORD_BLOCK_NUM;i++)
	{//�ҳ���¼����ʼ��
		status=RecordBlockDiag(i);
		if ( (status&0xf0)&& !(status&0x0f))
		{//�ÿ������м�¼��δ������һ��Ϊ��ʼ��¼��λ��
			bitFindSymbl=1;
			EndBlock=i;
			break;
		}
		else if ((status&0xf0) && (status&0x0f))
		{//����ÿ������������һ��ļ�¼
			status=RecordBlockDiag((i+1)%RECORD_BLOCK_NUM);
			if (!status)
			{//�����һ����¼Ϊ��,����һ��Ϊ��ʼ��¼��ʼ��
				bitFindSymbl=1;
				EndBlock=i;
				break;
			}
		}
	}
	if (!bitFindSymbl)//�����ڼ�¼������
	{//û���ҵ��м�¼�Ŀ�
		status=RecordBlockDiag(0);	
		if (!status)//�����һ��Ϊ�գ���û�д洢��¼
			return;//�޼�¼
		else//�����һ��Ϊ��, 
		{
			for (i=0;i<RECORD_BLOCK_NUM;i++)
			{//���ĳ�麬��δ�ɼ�¼���¿�Ϊ�ɼ����ļ�¼����ÿ�Ϊ��ʼ��
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
				EndBlock=RECORD_BLOCK_NUM-1;//���ȫ��Ϊδ�ɼ�¼��ɼ����ļ�¼����1��Ϊ��ʼ
		}		
	}
	//��¼�洢ָ��
	st_data=RecordBlockDiag(EndBlock);
	if ((st_data&0xf0) && (st_data&0x0f) )
	{//������һ��ļ�¼�������¿����ʼ��ַΪ�洢ָ��
		if (EndBlock==RECORD_BLOCK_NUM-1)
			SaveRecordIndex=0;//�����¼����Ϊ���һ������һ����¼�����¼�洢ָ��Ϊ0
		else
			SaveRecordIndex=(EndBlock+1)*RECORDNUM_BLOCK;//����Ϊ��һ��ĵ�һ��		
	}
	else
	{//��ǰ��û�������ҳ��´δ洢��¼��������
		SaveRecordIndex=EndBlock*RECORDNUM_BLOCK;	
		Addr= (ulong)EndBlock* 0x1000;
		for (jj=0;jj<RECORDNUM_BLOCK;jj++)
		{
			Flash_Rd_Bytes(Addr,&st_data,1);//��ĵ�һ����¼
			Addr+=RECORD_SIZE;
			if (st_data==0xff)
			{//�ҵ��洢��¼�ĳ���
				SaveRecordIndex+=jj;
				break;
			}
		}	
	}
	NoCollectRecordIndex=SaveRecordIndex;
///////////////////////���е��ˣ��ҵ���¼�ĳ��ڣ����´δ洢��¼��ָ��//////////////////////�
	StartBlock=(EndBlock+1)%RECORD_BLOCK_NUM;//�¿�Ϊ��¼�����
	for (i=0;i<RECORD_BLOCK_NUM;i++)
	{//�Ӽ�¼�Ŀ�ʼλ�ü���
		status=RecordBlockDiag(StartBlock);	
	
		switch (status)
		{
			case	0x10:// ��ʼδ��,��Ϊ��
				if (!bitFindReIndex)
				{//���δ�ҵ�������ڣ��ø�λ��Ϊ�������
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				if (!bitFindNoIndex)
				{//���δ�ҵ��ɼ���ڣ��ø�λ��Ϊ�ɼ����
					bitFindNoIndex=1;
					NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				Addr=(ulong)StartBlock*0x1000;
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//��ĵ�һ����¼
					Addr+=RECORD_SIZE;
					if (st_data!=0xff)
					{
						RecordSum++;//��¼����
						NoCollectRecordSum++;//δ�ɼ�¼����	
					}
					else
						break;
				}
				break;
			case	0x11://��ʼ�������δ�ɼ�
				if (!bitFindReIndex)
				{//���δ�ҵ�������ڣ��ø�λ��Ϊ�������
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				if (!bitFindNoIndex)
				{//���δ�ҵ��ɼ���ڣ��ø�λ��Ϊ�ɼ����
					bitFindNoIndex=1;
					NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				RecordSum+=RECORDNUM_BLOCK;
				NoCollectRecordSum+=RECORDNUM_BLOCK;
				break;
			case	0x20: //��ʼ�Ѳɼ�������޼�¼ 
				if (!bitFindReIndex)
				{//���δ�ҵ�������ڣ��ø�λ��Ϊ�������
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				Addr=(ulong)StartBlock*0x1000;			
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//��ĵ�һ����¼
					Addr+=RECORD_SIZE;
					if (st_data!=0xff)
					{
						RecordSum++;//��¼����
						if (st_data==0xa0)
						{
							NoCollectRecordSum++;
							if (!bitFindNoIndex)
							{//���δ�ҵ��ɼ���ڣ��ø�λ��Ϊ�ɼ����
								bitFindNoIndex=1;
								NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK+jj;	
							}	
						}
				   	}
					else
						break;
				}
				break;
			case	0x21://��ʼ�Ѳɼ������δ�ɼ�
				if (!bitFindReIndex)
				{//���δ�ҵ�������ڣ��ø�λ��Ϊ�������
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}			
				RecordSum+=RECORDNUM_BLOCK;
				Addr=(ulong)StartBlock*0x1000;
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)	
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//��ĵ�һ����¼
					Addr+=RECORD_SIZE;
					if (st_data==0xa0)
					{
						NoCollectRecordSum++;
						if (!bitFindNoIndex)
						{//���δ�ҵ��ɼ���ڣ��ø�λ��Ϊ�ɼ����
							bitFindNoIndex=1;
							NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK+jj;	
						}
					}					
				}
				break;
			case	0x22://��һ�������һ�����ɼ���
				if (!bitFindReIndex)
				{//���δ�ҵ�������ڣ��ø�λ��Ϊ�������
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
//���ϳ����ҵ��˿��ż�¼�ļ�¼���������ɼ�¼����ָ�룬�����һ�����Ѽ�¼
}	
uchar	RecordBlockDiag(uchar BlockNum)
//�ÿ��¼�洢��� ����39SF020--Ϊ0-63��
//00 -�޼�¼ ;11-��ȫ��δ��; 2-���ɼ�¼
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
	Addr=Addr+0x1000-RECORD_SIZE;//���һ��
	Flash_Rd_Bytes(Addr,&st_data,1);
	if (st_data==0xa0)
		status+=1;
	else if (st_data==0xff)
		status&=0xf0;
	else
		status+=2;
	return	status;	
}

void	Save_OffRecode(void)//�洢�ѻ���¼
{ 
	uint		Addr;
	uchar		i;
	uchar		Buffer_39SF020[24];
	uchar		Buffer[32];
	uchar		status;

	uchar	AllFF[32]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
						   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	Buffer_39SF020[0]=0xa0;
	memcpy(Buffer_39SF020+1,NoNetRecord,18);
	Buffer_39SF020[19]=CalCheckSum(Buffer_39SF020+1,18);

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
				Flash_Write_Bytes(Addr,Buffer_39SF020,20);//д��
				Flash_Rd_Bytes(Addr,Buffer,20);
				if (!memcmp(Buffer,Buffer_39SF020,20))
				{
					PosConsumCount++;
					status=0;
					break;
				}
			}
			if (!status)//�ɹ�
			{
				offline_ReIndex++; //��¼ָ��ָ����һ��Ԫ                
				offline_ReIndex = offline_ReIndex%Off_MAXRECORD;
				flag_off_full=RecordFullDiag();
				offline_UpIndex = 0;
				break;
			}
		}
		if(status)  //д����,��ǰ��Ԫд0����д���¸���Ԫ
		{
			memset(Buffer,0,20);
			Flash_Write_Bytes(Addr,Buffer,20);

			offline_ReIndex++; //��¼ָ��ָ����һ��Ԫ                
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
//�ÿ��¼�洢��� ����39SF020--Ϊ0-63��
//00 -�޼�¼ ;11-��ȫ��δ��; 2-���ɼ�¼
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
	Addr=Addr+0x1000-32;//���һ��
	Flash_Rd_Bytes(Addr,&st_data,1);
	if (st_data==0xa0)
		status+=1;
	else if (st_data==0xff)
		status&=0xf0;
	else
		status+=2;
	return	status;	
}

void InitOffLineRecord(void)//��ʼ���ѻ���¼ָ��
{//����һ���򵥵�˳��ṹ
	uchar  			i;
	uint			Addr,jj;
	uchar			st_data;
	uchar			status;
	uchar			EndBlock;
	uchar			bitFindSymbl=0; 

	offline_ReIndex=0;//�洢��¼ָ��
	for (i=0;i<Off_RECORD_BLOCK_NUM;i++)
	{//�ҳ���¼����ʼ��
		status=RecordBlockDiag1(i);
		if ( (status&0xf0)&& !(status&0x0f))
		{//�ÿ������м�¼��δ������һ��Ϊ��ʼ��¼��λ��
			bitFindSymbl=1;
			EndBlock=i;
			break;
		}
		else if ((status&0xf0) && (status&0x0f))
		{//����ÿ������������һ��ļ�¼
			status=RecordBlockDiag1((i+1)%Off_RECORD_BLOCK_NUM);
			if (!status)
			{//�����һ����¼Ϊ��,����һ��Ϊ��ʼ��¼��ʼ��
				bitFindSymbl=1;
				EndBlock=i;
				break;
			}
		}
	}
	if (!bitFindSymbl)//�����ڼ�¼������
	{//û���ҵ��м�¼�Ŀ�
		status=RecordBlockDiag1(0);	
		if (!status)//�����һ��Ϊ�գ���û�д洢��¼
			return;//�޼�¼
		else//�����һ��Ϊ��, 
		{
			for (i=0;i<Off_RECORD_BLOCK_NUM;i++)
			{//���ĳ�麬��δ�ɼ�¼���¿�Ϊ�ɼ����ļ�¼����ÿ�Ϊ��ʼ��
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
				EndBlock=15;//���ȫ��Ϊδ�ɼ�¼��ɼ����ļ�¼����1��Ϊ��ʼ
		}		
	}
	//��¼�洢ָ��
	st_data=RecordBlockDiag1(EndBlock);
	if ((st_data&0xf0) && (st_data&0x0f) )
	{//������һ��ļ�¼�������¿����ʼ��ַΪ�洢ָ��
		if (EndBlock==15)
			offline_ReIndex=0;//�����¼����Ϊ���һ������һ����¼�����¼�洢ָ��Ϊ0
		else
			offline_ReIndex=(EndBlock+1)*128;//����Ϊ��һ��ĵ�һ��		
	}
	else
	{//��ǰ��û�������ҳ��´δ洢��¼��������
		offline_ReIndex=EndBlock*RECORDNUM_BLOCK;	
		
		Addr= (ulong)EndBlock* 0x1000;
		for (jj=0;jj<128;jj++)
		{
			Flash_Rd_Bytes(Addr,&st_data,1);//��ĵ�һ����¼
			Addr+=32;
			if (st_data==0xff)
			{//�ҵ��洢��¼�ĳ���
				offline_ReIndex+=jj;
				break;
			}
		}	
	}
}

uchar	RecordFullDiag(void)//��¼�洢�Ƿ��������	
{
	uchar		st_data;
	uint		Addr;
	uchar		BlockNum;
	uint	    ii,jj;
	uchar		status;
	uchar		OffLineBlockNum;


	offline_ReIndex=offline_ReIndex%Off_MAXRECORD;

	st_data=* (1+(uchar *)&offline_ReIndex);//ÿһ�����Ŀ�ʼ
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
	uchar recdata[32];
	int reclen;
	u8 err;
	cJSON * root,*rootfor;

	//return;
	OSMutexPend(SemFlashSpi,0,&err);  //���뻥����Դʹ��
	status=0;
	//offline_UpIndex=0;//�����ϴ��ѻ���¼ָ��	

	for(ii=offline_UpIndex;ii<Off_MAXRECORD;ii++)
	{		
		Addr=(ulong)offline_UpIndex*RECORD_SIZE;	
		Flash_Rd_Bytes(Addr,DatasBuffer,20);
		if(DatasBuffer[0]==0xa0 && (DatasBuffer[19]==CalCheckSum(DatasBuffer+1,18)) )
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
	
	OSMutexPost(SemFlashSpi);  //������Դ�ͷ�
	
	if(status)//�ҵ������ϴ��ļ�¼
	{
		flag_reupcode=1; //�����ϴ��ѻ���¼��־

		DatasBuffer[19] = offline_UpIndex >> 8;
		DatasBuffer[20] = offline_UpIndex & 0xFF;
		tcp_send_and_rec_packet(0x2109,DatasBuffer+2,19,1,recdata,&reclen,3);
		
		root = cJSON_Parse((const char *)(recdata+3)); 
		if(root != 0)
		{
			status = cJSON_GetObjectItem(root,"returnType")->valueint;  
			if(status == 0)
			{
				DatasBuffer[0] = 0;
				
				OSMutexPend(SemFlashSpi,0,&err);  //���뻥����Դʹ��
				Addr=(ulong)offline_UpIndex*RECORD_SIZE;	
				Flash_Write_Bytes(Addr,DatasBuffer,10);
				offline_UpIndex++;
				offline_UpIndex = offline_UpIndex%Off_MAXRECORD;
				OSMutexPost(SemFlashSpi);  //������Դ�ͷ�
			}
		}
		cJSON_Delete(root);
		//Write_SOCK_Data_Buffer(0,Tx_Buffer,11);
	}
	else//û�м�¼
	{
		bitSerial_SendRequest=0;
		flag_first_over=1;//���
		flag_reupcode=2;
		Off_TimeCount =1;
		NeedSendRecordFlag =0;
		
		OSTimeDlyHMSM(0, 0, 2, 0);//���������500ms	 		
	}
	
}
void   	Up_UpNetRe(void)
{
	uint   ii;
	uchar  status; 
	uint   Addr;
	uchar  DatasBuffer[32];

	
	if(flag_first_over)//���������Ѿ����
		return;
	if(UPD_TimeCount)//����������
		return;
	if(bitSerial_SendRequest ||	bitSerial_ReceiveEnd || SerialReceiveLen )//�з��ͻ���յ�����δ����
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
	if(status)//�ҵ������ϴ��ļ�¼
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
	else//û�м�¼
	{
		bitSerial_SendRequest=0;
		flag_first_over=1;//���
		flag_reupcode=2;
	}
	 	
}
void 	Up_AllRe(void)
{
	
	if(!Consum_Status && recode_rdnum < 16384 )	
	  flag_first_over=0;
	while(1)
	{
		
		if(!Consum_Status)//����ģʽ��
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
			find=1;//�ҵ���һ��δ����¼
			break;	
		}
		
		if(!(ii%32))
		{
			Clr_WatchDog();
		}
	}

	if(find)//���ѻ���¼���Ե�һ��δ����¼Ϊ���к�
		Udp_SerialNum=(uchar)ii;
	else//�޼�¼������һ����¼ָ��Ϊ���к�
		Udp_SerialNum=(uchar)SaveRecordIndex;
}
void	FindPosConsumCountSub(void)//�������һ����¼����ˮ��
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
		PosConsumCount1=0;//��flash��ˮ�Ŵ�0��ʼ��
	if (!offline_ReIndex)
		jj=MAXRECORD_NUM-1;
	else
		jj=offline_ReIndex-1;
	Addr =(ulong)jj*RECORD_SIZE;
		
	Flash_Rd_Bytes(Addr,Buffer,23);
	if (Buffer[0]!=0xff && (Buffer[22] == CalCheckSum(Buffer+1,21)))
		memcpy((uchar *)&PosConsumCount2,Buffer+17,2);
	else
		PosConsumCount2=0;//��flash��ˮ�Ŵ�0��ʼ��
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

