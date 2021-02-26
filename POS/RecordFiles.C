#include "MyDefine.h"
#include "include.h"
#include "char.h"



ulong		Sys_SumConsumMoney=0;//Ïû·Ñ¶îÀÛ¼Æ
uint		RecordSum=0;//¼ÇÂ¼×ÜÊý
uint		NoCollectRecordSum=0;//Ã»ÓÐ²É¼¯µÄ¼ÇÂ¼×ÜÊý
uint		NoCollectRecordIndex=0;//Ã»ÓÐ²É¼¯µÄ¼ÇÂ¼Ö¸Õë
uint		ReCollectRecordIndex=0;//ÒÑ¾­²É¼¯µÄ¼ÇÂ¼Ö¸Õë
uint		SaveRecordIndex=0;//´æ´¢¼ÇÂ¼Ö¸Õë
uint		ReCollectAddrBak;
uint		ConsumCount=0;//Ïû·ÑÁ÷Ë®ºÅ
uchar		SerialSendNoCollectNum=0;//ÉÏ´Î´«ËÍµÄÎ´²É¼ÇÂ¼¸öÊý
uchar		SerialSendReCollectNum=0;//ÉÏ´Î´«ËÍµÄ¸´²É¼ÇÂ¼¸öÊý

uint		ReCollectCount=0;
uchar		bitReCollect=0;
uint		PosConsumCount=0;//POSµÄÏû·ÑÁ÷Ë®ºÅ
extern OS_EVENT *SemFlashSpi;  //»¥³âÐÅºÅÁ¿


void	InitSaveRecordSub(uchar);//³õÊ¼»¯Ö¸Õë
uchar	RecordBlockDiag(uchar);//00 -ÎÞ¼ÇÂ¼ ;11-ÂúÈ«²¿Î´²É; 22-È«²¿Îª¸´²É¼ÇÂ¼
void	SaveRecordSub(void);//´æ´¢Ïû·Ñ¼ÇÂ¼
uchar		RecordDatasToBuffer(uchar *);//´æ´¢¼ÇÂ¼µÄÄÚÈÝÐ´Èë»º´æ
void 	InitUdp_SerNum(void);
void	FindPosConsumCountSub(void);//²éÕÒ×îºóÒ»Ìõ¼ÇÂ¼µÄÁ÷Ë®ºÅ

void	SaveRecordSub(void)//´æ´¢Ïû·Ñ¼ÇÂ¼
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
	
	OSMutexPend(SemFlashSpi,0,&err);  //ÉêÇë»¥³â×ÊÔ´Ê¹ÓÃ
	if (RecordDatasToBuffer(Buffer_39SF020))	
	{
		OSMutexPost(SemFlashSpi);  //»¥³â×ÊÔ´ÊÍ·Å
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
				Flash_Write_Bytes(Addr,Buffer_39SF020,22);//Ð´Èë
				Flash_Rd_Bytes(Addr,Buffer,22);
				if (!memcmp(Buffer,Buffer_39SF020,22))
				{
					PosConsumCount++;
					status=0;
					break;
				}
			}
			if (!status)//³É¹¦
			{
				SaveRecordIndex++;                              //¼ÇÂ¼Ö¸ÕëÖ¸ÏòÏÂÒ»µ¥Ôª                
			  	SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;	//¿ØÖÆ×îºóÒ»ÌõÌø×ª»Ø³õÊ¼Î»ÖÃ
	          	if (!(SaveRecordIndex&0xff))//Ã¿¸öÉÈÇø¿ªÊ¼
			    {       
			  	   Curr_BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
				   
		           Addr=(ulong)Curr_BlockNum*0x1000;
		           Erase_One_Sector(Addr);  //²ÁÉÈÇø	
			   }
				break;
			}
			else   //Ð´´íÎó,µ±Ç°µ¥ÔªÐ´0Êý¾ÝÐ´ÈëÏÂ¸öµ¥Ôª
			{
			  memset(Buffer,0,22);
			  Flash_Write_Bytes(Addr,Buffer,22);
			  SaveRecordIndex++;
			  SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;
			  if (!(SaveRecordIndex&0xff))//Ã¿¸öÉÈÇø¿ªÊ¼
			    {       
			  	   Curr_BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
		           Erase_One_Sector(Curr_BlockNum);  //²ÁÉÈÇø	
//						OSMutexPost(SemFlashSpi);  //»¥³â×ÊÔ´ÊÍ·Å
			    }
			}		
		}
		else
		{
			SaveRecordIndex++;
			SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;  
		    if (!(SaveRecordIndex&0xff))//Èç¹û¼ÇÂ¼Âú£¬»Øµ½ÆðÊ¼µØÖ·£¬²Á³ýµÚ1¿é
			{       
			  	Curr_BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
		        Erase_One_Sector(Curr_BlockNum);  //²ÁÉÈÇø	
			}
		}
		Clr_WatchDog();	
	}
	InitSaveRecordSub(1);	
	OSMutexPost(SemFlashSpi);  //»¥³â×ÊÔ´ÊÍ·Å
}
uchar	RecordDatasToBuffer(uchar * ptr)//´æ´¢¼ÇÂ¼µÄÄÚÈÝÐ´Èë»º´æ
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


void	InitSaveRecordSub(uchar bbit )//³õÊ¼»¯Ö¸Õë
{
 	uchar  			i;
	uint			Addr,jj;
	uchar			st_data;
	uchar			status;
	uchar			EndBlock;
	uchar			StartBlock;
	uchar 	bitFindReIndex=0; //¸´²É¼ÇÂ¼Ö¸Õë
	uchar		bitFindNoIndex=0;//Î´²ÉÖ¸Õë
	uchar		bitFindSymbl=0; 
	RecordSum=0;//¼ÇÂ¼×ÜÊý
	NoCollectRecordSum=0;//Ã»ÓÐ²É¼¯µÄ¼ÇÂ¼×ÜÊý
	NoCollectRecordIndex=0;//Ã»ÓÐ²É¼¯µÄ¼ÇÂ¼Ö¸Õë
	if (!bbit)
		ReCollectRecordIndex=0;//ÒÑ¾­²É¼¯µÄ¼ÇÂ¼Ö¸Õë
	SaveRecordIndex=0;//´æ´¢¼ÇÂ¼Ö¸Õë
	for (i=0;i<RECORD_BLOCK_NUM;i++)
	{//ÕÒ³ö¼ÇÂ¼µÄÆðÊ¼¿é
		status=RecordBlockDiag(i);
		if ( (status&0xf0)&& !(status&0x0f))
		{//¸Ã¿éÊý¾ÝÓÐ¼ÇÂ¼µ«Î´Âú¡£ÏÂÒ»¿éÎªÆðÊ¼¼ÇÂ¼µÄÎ»ÖÃ
			bitFindSymbl=1;
			EndBlock=i;
			break;
		}
		else if ((status&0xf0) && (status&0x0f))
		{//Èç¹û¸Ã¿éÒÑÂú£¬Õï¶ÏÏÂÒ»¿éµÄ¼ÇÂ¼
			status=RecordBlockDiag((i+1)%RECORD_BLOCK_NUM);
			if (!status)
			{//Èç¹ûÏÂÒ»¸ö¼ÇÂ¼Îª¿Õ,ÔòÏÂÒ»¿éÎªÆðÊ¼¼ÇÂ¼ÆðÊ¼¿é
				bitFindSymbl=1;
				EndBlock=i;
				break;
			}
		}
	}
	if (!bitFindSymbl)//²»´æÔÚ¼ÇÂ¼£¬·µ»Ø
	{//Ã»ÓÐÕÒµ½ÓÐ¼ÇÂ¼µÄ¿é
		status=RecordBlockDiag(0);	
		if (!status)//Èç¹ûµÚÒ»¿éÎª¿Õ£¬ÔòÃ»ÓÐ´æ´¢¼ÇÂ¼
			return;//ÎÞ¼ÇÂ¼
		else//Èç¹ûµÚÒ»¿éÎªÂú, 
		{
			for (i=0;i<RECORD_BLOCK_NUM;i++)
			{//Èç¹ûÄ³¿éº¬ÓÐÎ´²É¼ÇÂ¼£¬ÏÂ¿éÎª²É¼¯¹ýµÄ¼ÇÂ¼£¬Ôò¸Ã¿ìÎªÆðÊ¼¿é
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
				EndBlock=RECORD_BLOCK_NUM-1;//Èç¹ûÈ«²¿ÎªÎ´²É¼ÇÂ¼»ò²É¼¯¹ýµÄ¼ÇÂ¼£¬µÚ1¿ìÎª¿ªÊ¼
		}		
	}
	//¼ÇÂ¼´æ´¢Ö¸Õë
	st_data=RecordBlockDiag(EndBlock);
	if ((st_data&0xf0) && (st_data&0x0f) )
	{//Èç¹û×îºóÒ»¿éµÄ¼ÇÂ¼ÒÑÂú£¬ÏÂ¿éµÄÆðÊ¼µØÖ·Îª´æ´¢Ö¸Õë
		if (EndBlock==RECORD_BLOCK_NUM-1)
			SaveRecordIndex=0;//Èç¹û¼ÇÂ¼³ö¿ÚÎª×îºóÒ»¿éµÄ×îºóÒ»Ìõ¼ÇÂ¼£¬Ôò¼ÇÂ¼´æ´¢Ö¸ÕëÎª0
		else
			SaveRecordIndex=(EndBlock+1)*RECORDNUM_BLOCK;//·ñÔòÎªÏÂÒ»¿éµÄµÚÒ»Ìõ		
	}
	else
	{//µ±Ç°¿éÃ»ÓÐÂú£¬ÕÒ³öÏÂ´Î´æ´¢¼ÇÂ¼µÄË÷ÒýºÅ
		SaveRecordIndex=EndBlock*RECORDNUM_BLOCK;	
		Addr= (ulong)EndBlock* 0x1000;
		for (jj=0;jj<RECORDNUM_BLOCK;jj++)
		{
			Flash_Rd_Bytes(Addr,&st_data,1);//¿éµÄµÚÒ»Ìõ¼ÇÂ¼
			Addr+=RECORD_SIZE;
			if (st_data==0xff)
			{//ÕÒµ½´æ´¢¼ÇÂ¼µÄ³ö¿Ú
				SaveRecordIndex+=jj;
				break;
			}
		}	
	}
	NoCollectRecordIndex=SaveRecordIndex;
///////////////////////ÔËÐÐµ½´Ë£¬ÕÒµ½¼ÇÂ¼µÄ³ö¿Ú£¬¼´ÏÂ´Î´æ´¢¼ÇÂ¼µÄÖ¸Õë//////////////////////¬
	StartBlock=(EndBlock+1)%RECORD_BLOCK_NUM;//ÏÂ¿éÎª¼ÇÂ¼µÄÈë¿Ú
	for (i=0;i<RECORD_BLOCK_NUM;i++)
	{//´Ó¼ÇÂ¼µÄ¿ªÊ¼Î»ÖÃ¼ìË÷
		status=RecordBlockDiag(StartBlock);	
	
		switch (status)
		{
			case	0x10:// ¿ªÊ¼Î´²É,ºóÎª¿Õ
				if (!bitFindReIndex)
				{//Èç¹ûÎ´ÕÒµ½¸´²ÉÈë¿Ú£¬ÖÃ¸ÃÎ»ÖÃÎª¸´²ÉÈë¿Ú
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				if (!bitFindNoIndex)
				{//Èç¹ûÎ´ÕÒµ½²É¼¯Èë¿Ú£¬ÖÃ¸ÃÎ»ÖÃÎª²É¼¯Èë¿Ú
					bitFindNoIndex=1;
					NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				Addr=(ulong)StartBlock*0x1000;
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//¿éµÄµÚÒ»Ìõ¼ÇÂ¼
					Addr+=RECORD_SIZE;
					if (st_data!=0xff)
					{
						RecordSum++;//¼ÇÂ¼×ÜÊý
						NoCollectRecordSum++;//Î´²É¼ÇÂ¼×ÜÊý	
					}
					else
						break;
				}
				break;
			case	0x11://¿ªÊ¼ºÍ×îºó¶¼ÊÇÎ´²É¼¯
				if (!bitFindReIndex)
				{//Èç¹ûÎ´ÕÒµ½¸´²ÉÈë¿Ú£¬ÖÃ¸ÃÎ»ÖÃÎª¸´²ÉÈë¿Ú
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				if (!bitFindNoIndex)
				{//Èç¹ûÎ´ÕÒµ½²É¼¯Èë¿Ú£¬ÖÃ¸ÃÎ»ÖÃÎª²É¼¯Èë¿Ú
					bitFindNoIndex=1;
					NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				RecordSum+=RECORDNUM_BLOCK;
				NoCollectRecordSum+=RECORDNUM_BLOCK;
				break;
			case	0x20: //¿ªÊ¼ÒÑ²É¼¯£¬×îºóÎÞ¼ÇÂ¼ 
				if (!bitFindReIndex)
				{//Èç¹ûÎ´ÕÒµ½¸´²ÉÈë¿Ú£¬ÖÃ¸ÃÎ»ÖÃÎª¸´²ÉÈë¿Ú
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				Addr=(ulong)StartBlock*0x1000;			
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//¿éµÄµÚÒ»Ìõ¼ÇÂ¼
					Addr+=RECORD_SIZE;
					if (st_data!=0xff)
					{
						RecordSum++;//¼ÇÂ¼×ÜÊý
						if (st_data==0xa0)
						{
							NoCollectRecordSum++;
							if (!bitFindNoIndex)
							{//Èç¹ûÎ´ÕÒµ½²É¼¯Èë¿Ú£¬ÖÃ¸ÃÎ»ÖÃÎª²É¼¯Èë¿Ú
								bitFindNoIndex=1;
								NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK+jj;	
							}	
						}
				   	}
					else
						break;
				}
				break;
			case	0x21://¿ªÊ¼ÒÑ²É¼¯£¬×îºóÎ´²É¼¯
				if (!bitFindReIndex)
				{//Èç¹ûÎ´ÕÒµ½¸´²ÉÈë¿Ú£¬ÖÃ¸ÃÎ»ÖÃÎª¸´²ÉÈë¿Ú
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}			
				RecordSum+=RECORDNUM_BLOCK;
				Addr=(ulong)StartBlock*0x1000;
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)	
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//¿éµÄµÚÒ»Ìõ¼ÇÂ¼
					Addr+=RECORD_SIZE;
					if (st_data==0xa0)
					{
						NoCollectRecordSum++;
						if (!bitFindNoIndex)
						{//Èç¹ûÎ´ÕÒµ½²É¼¯Èë¿Ú£¬ÖÃ¸ÃÎ»ÖÃÎª²É¼¯Èë¿Ú
							bitFindNoIndex=1;
							NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK+jj;	
						}
					}					
				}
				break;
			case	0x22://µÚÒ»ÌõºÍ×îºóÒ»Ìõ¶¼²É¼¯¹ý
				if (!bitFindReIndex)
				{//Èç¹ûÎ´ÕÒµ½¸´²ÉÈë¿Ú£¬ÖÃ¸ÃÎ»ÖÃÎª¸´²ÉÈë¿Ú
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
//ÒÔÉÏ³ÌÐòÕÒµ½ÁË¿ªÃÅ¼ÇÂ¼µÄ¼ÇÂ¼×ÜÊý£¬¸´²É¼ÇÂ¼³õÊÔÖ¸Õë£¬ºÍ×îºóÒ»ÌõÏû·Ñ¼ÇÂ¼
}	
uchar	RecordBlockDiag(uchar BlockNum)
//¸Ã¿é¼ÇÂ¼´æ´¢Çé¿ö ¶ÔÓÚ39SF020--Îª0-63¿é
//00 -ÎÞ¼ÇÂ¼ ;11-ÂúÈ«²¿Î´²É; 2-¸´²É¼ÇÂ¼
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
	Addr=Addr+0x1000-RECORD_SIZE;//×îºóÒ»Ìõ
	Flash_Rd_Bytes(Addr,&st_data,1);
	if (st_data==0xa0)
		status+=1;
	else if (st_data==0xff)
		status&=0xf0;
	else
		status+=2;
	return	status;	
}

void	Save_OffRecode(void)//´æ´¢ÍÑ»ú¼ÇÂ¼
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
				Flash_Write_Bytes(Addr,Buffer_39SF020,21);//Ð´Èë
				Flash_Rd_Bytes(Addr,Buffer,21);
				if (!memcmp(Buffer,Buffer_39SF020,21))
				{
					PosConsumCount++;
					status=0;
					break;
				}
			}
			if (!status)//³É¹¦
			{
				offline_ReIndex++; //¼ÇÂ¼Ö¸ÕëÖ¸ÏòÏÂÒ»µ¥Ôª                
				offline_ReIndex = offline_ReIndex%Off_MAXRECORD;
				flag_off_full=RecordFullDiag();
				offline_UpIndex = 0;
				break;
			}
		}
		if(status)  //Ð´´íÎó,µ±Ç°µ¥ÔªÐ´0Êý¾ÝÐ´ÈëÏÂ¸öµ¥Ôª
		{
			memset(Buffer,0,21);
			Flash_Write_Bytes(Addr,Buffer,21);

			offline_ReIndex++; //¼ÇÂ¼Ö¸ÕëÖ¸ÏòÏÂÒ»µ¥Ôª                
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
//¸Ã¿é¼ÇÂ¼´æ´¢Çé¿ö ¶ÔÓÚ39SF020--Îª0-63¿é
//00 -ÎÞ¼ÇÂ¼ ;11-ÂúÈ«²¿Î´²É; 2-¸´²É¼ÇÂ¼
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
	Addr=Addr+0x1000-32;//×îºóÒ»Ìõ
	Flash_Rd_Bytes(Addr,&st_data,1);
	if (st_data==0xa0)
		status+=1;
	else if (st_data==0xff)
		status&=0xf0;
	else
		status+=2;
	return	status;	
}

void InitOffLineRecord(void)//³õÊ¼»¯ÍÑ»ú¼ÇÂ¼Ö¸Õë
{//ÕâÊÇÒ»¸ö¼òµ¥µÄË³Ðò½á¹¹
	uchar  			i;
	uint			Addr,jj;
	uchar			st_data;
	uchar			status;
	uchar			EndBlock;
	uchar			bitFindSymbl=0; 

	offline_ReIndex=0;//´æ´¢¼ÇÂ¼Ö¸Õë
	for (i=0;i<Off_RECORD_BLOCK_NUM;i++)
	{//ÕÒ³ö¼ÇÂ¼µÄÆðÊ¼¿é
		status=RecordBlockDiag1(i);
		if ( (status&0xf0)&& !(status&0x0f))
		{//¸Ã¿éÊý¾ÝÓÐ¼ÇÂ¼µ«Î´Âú¡£ÏÂÒ»¿éÎªÆðÊ¼¼ÇÂ¼µÄÎ»ÖÃ
			bitFindSymbl=1;
			EndBlock=i;
			break;
		}
		else if ((status&0xf0) && (status&0x0f))
		{//Èç¹û¸Ã¿éÒÑÂú£¬Õï¶ÏÏÂÒ»¿éµÄ¼ÇÂ¼
			status=RecordBlockDiag1((i+1)%Off_RECORD_BLOCK_NUM);
			if (!status)
			{//Èç¹ûÏÂÒ»¸ö¼ÇÂ¼Îª¿Õ,ÔòÏÂÒ»¿éÎªÆðÊ¼¼ÇÂ¼ÆðÊ¼¿é
				bitFindSymbl=1;
				EndBlock=i;
				break;
			}
		}
	}
	if (!bitFindSymbl)//²»´æÔÚ¼ÇÂ¼£¬·µ»Ø
	{//Ã»ÓÐÕÒµ½ÓÐ¼ÇÂ¼µÄ¿é
		status=RecordBlockDiag1(0);	
		if (!status)//Èç¹ûµÚÒ»¿éÎª¿Õ£¬ÔòÃ»ÓÐ´æ´¢¼ÇÂ¼
			return;//ÎÞ¼ÇÂ¼
		else//Èç¹ûµÚÒ»¿éÎªÂú, 
		{
			for (i=0;i<Off_RECORD_BLOCK_NUM;i++)
			{//Èç¹ûÄ³¿éº¬ÓÐÎ´²É¼ÇÂ¼£¬ÏÂ¿éÎª²É¼¯¹ýµÄ¼ÇÂ¼£¬Ôò¸Ã¿ìÎªÆðÊ¼¿é
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
				EndBlock=15;//Èç¹ûÈ«²¿ÎªÎ´²É¼ÇÂ¼»ò²É¼¯¹ýµÄ¼ÇÂ¼£¬µÚ1¿ìÎª¿ªÊ¼
		}		
	}
	//¼ÇÂ¼´æ´¢Ö¸Õë
	st_data=RecordBlockDiag1(EndBlock);
	if ((st_data&0xf0) && (st_data&0x0f) )
	{//Èç¹û×îºóÒ»¿éµÄ¼ÇÂ¼ÒÑÂú£¬ÏÂ¿éµÄÆðÊ¼µØÖ·Îª´æ´¢Ö¸Õë
		if (EndBlock==15)
			offline_ReIndex=0;//Èç¹û¼ÇÂ¼³ö¿ÚÎª×îºóÒ»¿éµÄ×îºóÒ»Ìõ¼ÇÂ¼£¬Ôò¼ÇÂ¼´æ´¢Ö¸ÕëÎª0
		else
			offline_ReIndex=(EndBlock+1)*128;//·ñÔòÎªÏÂÒ»¿éµÄµÚÒ»Ìõ		
	}
	else
	{//µ±Ç°¿éÃ»ÓÐÂú£¬ÕÒ³öÏÂ´Î´æ´¢¼ÇÂ¼µÄË÷ÒýºÅ
		offline_ReIndex=EndBlock*RECORDNUM_BLOCK;	
		
		Addr= (ulong)EndBlock* 0x1000;
		for (jj=0;jj<128;jj++)
		{
			Flash_Rd_Bytes(Addr,&st_data,1);//¿éµÄµÚÒ»Ìõ¼ÇÂ¼
			Addr+=32;
			if (st_data==0xff)
			{//ÕÒµ½´æ´¢¼ÇÂ¼µÄ³ö¿Ú
				offline_ReIndex+=jj;
				break;
			}
		}	
	}
}

uchar	RecordFullDiag(void)//¼ÇÂ¼´æ´¢ÊÇ·ñÒÑÂúÕï¶Ï	
{
	uchar		st_data;
	uint		Addr;
	uchar		BlockNum;
	uint	    ii,jj;
	uchar		status;
	uchar		OffLineBlockNum;


	offline_ReIndex=offline_ReIndex%Off_MAXRECORD;

	st_data=* (1+(uchar *)&offline_ReIndex);//Ã¿Ò»ÉÈÇøµÄ¿ªÊ¼
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
	OSMutexPend(SemFlashSpi,0,&err);  //ÉêÇë»¥³â×ÊÔ´Ê¹ÓÃ
	status=0;
	//offline_UpIndex=0;//Ö÷¶¯ÉÏ´«ÍÑ»ú¼ÇÂ¼Ö¸Õë	

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
	
	OSMutexPost(SemFlashSpi);  //»¥³â×ÊÔ´ÊÍ·Å
	
	if(status)//ÕÒµ½¿ÉÒÔÉÏ´«µÄ¼ÇÂ¼
	{
		flag_reupcode=1; //Ö÷¶¯ÉÏ´«ÍÑ»ú¼ÇÂ¼±êÖ¾

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
				OSMutexPend(SemFlashSpi,0,&err);  //ÉêÇë»¥³â×ÊÔ´Ê¹ÓÃ
				Addr=(ulong)offline_UpIndex*RECORD_SIZE;	
				Flash_Write_Bytes(Addr,DatasBuffer,10);
				offline_UpIndex++;
				offline_UpIndex = offline_UpIndex%Off_MAXRECORD;
				OSMutexPost(SemFlashSpi);  //»¥³â×ÊÔ´ÊÍ·Å
				
			}
			cJSON_Delete(object);
		}
		else
		{
			Write_SOCK_Data_Buffer(5,Tx_Buffer,11);
		}	
		//Write_SOCK_Data_Buffer(0,Tx_Buffer,11);
	}
	else//Ã»ÓÐ¼ÇÂ¼
	{
		bitSerial_SendRequest=0;
		flag_first_over=1;//Íê³É
		flag_reupcode=2;
		Off_TimeCount =1;
		NeedSendRecordFlag =0;
		
		OSTimeDlyHMSM(0, 0, 2, 0);//½«ÈÎÎñ¹ÒÆð500ms	 		
	}
	
}
void   	Up_UpNetRe(void)
{
	uint   ii;
	uchar  status; 
	uint   Addr;
	uchar  DatasBuffer[32];

	
	if(flag_first_over)//Æô¶¯»úÖÆÒÑ¾­Íê³É
		return;
	if(UPD_TimeCount)//ÃüÁîÔËÐÐÖÐ
		return;
	if(bitSerial_SendRequest ||	bitSerial_ReceiveEnd || SerialReceiveLen )//ÓÐ·¢ËÍ»ò½ÓÊÕµÄÊý¾ÝÎ´´¦Àí
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
	if(status)//ÕÒµ½¿ÉÒÔÉÏ´«µÄ¼ÇÂ¼
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
	else//Ã»ÓÐ¼ÇÂ¼
	{
		bitSerial_SendRequest=0;
		flag_first_over=1;//Íê³É
		flag_reupcode=2;
	}
	 	
}
void 	Up_AllRe(void)
{
	
	if(!Consum_Status && recode_rdnum < 16384 )	
	  flag_first_over=0;
	while(1)
	{
		
		if(!Consum_Status)//ÁªÍøÄ£Ê½ÏÂ
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
			find=1;//ÕÒµ½µÚÒ»ÌõÎ´´«¼ÇÂ¼
			break;	
		}
		
		if(!(ii%32))
		{
			Clr_WatchDog();
		}
	}

	if(find)//ÓÐÍÑ»ú¼ÇÂ¼£¬ÒÔµÚÒ»ÌõÎ´´«¼ÇÂ¼ÎªÐòÁÐºÅ
		Udp_SerialNum=(uchar)ii;
	else//ÎÞ¼ÇÂ¼£¬ÒÔÏÂÒ»Ìõ¼ÇÂ¼Ö¸ÕëÎªÐòÁÐºÅ
		Udp_SerialNum=(uchar)SaveRecordIndex;
}
void	FindPosConsumCountSub(void)//²éÕÒ×îºóÒ»Ìõ¼ÇÂ¼µÄÁ÷Ë®ºÅ
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
		PosConsumCount1=0;//ÐÂflashÁ÷Ë®ºÅ´Ó0¿ªÊ¼¼Ç
	if (!offline_ReIndex)
		jj=MAXRECORD_NUM-1;
	else
		jj=offline_ReIndex-1;
	Addr =(ulong)jj*RECORD_SIZE;
		
	Flash_Rd_Bytes(Addr,Buffer,23);
	if (Buffer[0]!=0xff && (Buffer[22] == CalCheckSum(Buffer+1,21)))
		memcpy((uchar *)&PosConsumCount2,Buffer+17,2);
	else
		PosConsumCount2=0;//ÐÂflashÁ÷Ë®ºÅ´Ó0¿ªÊ¼¼Ç
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


