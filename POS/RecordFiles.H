 #include "stm32f10x.h"
 #include "char.h"
 
extern uint		RecordSum;//��¼����
extern uint		NoCollectRecordSum;//û�вɼ��ļ�¼����
extern uint		NoCollectRecordIndex;//û�вɼ��ļ�¼ָ��
extern uint		ReCollectRecordIndex;//�Ѿ��ɼ��ļ�¼ָ��
extern uint		SaveRecordIndex;//�洢��¼ָ��
extern uint		ReCollectAddrBak;
extern uint		ConsumCount;//������ˮ��
extern uchar		SerialSendNoCollectNum;//�ϴδ��͵�δ�ɼ�¼����
extern uchar		SerialSendReCollectNum;//�ϴδ��͵ĸ��ɼ�¼����

extern uint		ReCollectCount;
extern uchar		bitReCollect;
extern uint		PosConsumCount;
 
 
extern void	InitSaveRecordSub(unsigned char);//��ʼ��ָ��
extern unsigned char	RecordBlockDiag(unsigned char);//00 -�޼�¼ ;11-��ȫ��δ��; 22-ȫ��Ϊ���ɼ�¼
extern void	SaveRecordSub(void);//�洢���Ѽ�¼
extern unsigned char		RecordFullDiag(void);//��¼�洢�Ƿ��������
extern unsigned char		RecordDatasToBuffer(unsigned char *);//�洢��¼������д�뻺��
extern void	ChgTimeToRecordDatas(unsigned char * ,unsigned char * );
extern void	ChgRecordDatasToTime(unsigned char * ,unsigned char * );
extern void	SaveRecordBakSub(void);//��������¼�ı���
extern void	ClrConsumDatasBak(void);//���������¼
extern void	Save_OffRecode(void);//�洢�ѻ���¼
extern void InitOffLineRecord(void);//��ʼ���ѻ���¼ָ��
extern void	FindPosConsumCountSub(void);//�������һ����¼����ˮ��
extern void	Active_UpOffRe(void);//�ϴ��ѻ���¼




