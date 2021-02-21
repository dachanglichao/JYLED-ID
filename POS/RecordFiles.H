 #include "stm32f10x.h"
 #include "char.h"
 
extern uint		RecordSum;//记录总数
extern uint		NoCollectRecordSum;//没有采集的记录总数
extern uint		NoCollectRecordIndex;//没有采集的记录指针
extern uint		ReCollectRecordIndex;//已经采集的记录指针
extern uint		SaveRecordIndex;//存储记录指针
extern uint		ReCollectAddrBak;
extern uint		ConsumCount;//消费流水号
extern uchar		SerialSendNoCollectNum;//上次传送的未采记录个数
extern uchar		SerialSendReCollectNum;//上次传送的复采记录个数

extern uint		ReCollectCount;
extern uchar		bitReCollect;
extern uint		PosConsumCount;
 
 
extern void	InitSaveRecordSub(unsigned char);//初始化指针
extern unsigned char	RecordBlockDiag(unsigned char);//00 -无记录 ;11-满全部未采; 22-全部为复采记录
extern void	SaveRecordSub(void);//存储消费记录
extern unsigned char		RecordFullDiag(void);//记录存储是否已满诊断
extern unsigned char		RecordDatasToBuffer(unsigned char *);//存储记录的内容写入缓存
extern void	ChgTimeToRecordDatas(unsigned char * ,unsigned char * );
extern void	ChgRecordDatasToTime(unsigned char * ,unsigned char * );
extern void	SaveRecordBakSub(void);//保存错误记录的备份
extern void	ClrConsumDatasBak(void);//擦除错误记录
extern void	Save_OffRecode(void);//存储脱机记录
extern void InitOffLineRecord(void);//初始化脱机记录指针
extern void	FindPosConsumCountSub(void);//查找最后一条记录的流水号
extern void	Active_UpOffRe(void);//上传脱机记录





