#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "char.H"
#include  "ucos_ii.h"

extern OS_EVENT *SemEthMac;  //�����ź���
extern OS_EVENT *HaveReceiveData_Semp;//�Ѿ����յ�����
extern OS_EVENT *NeedSendData_Semp;//��Ҫ��������


extern uchar bitSerial_ReceiveEnd;//���ڽ�����ɱ�־λ
extern uchar SerialReceiveLen;//���ڶ�������ݳ���
extern uchar bitSerial_SendRequest;
extern uchar SerialReCase;//���ڽ���״̬
extern uchar		bitEven;
extern uchar SerialReceiveTimeCount;
extern uint		SerialReCheckSum;
extern uchar		SerialReCheckSumCount;
extern uchar		rChar;

extern ulong BeepDelayTimeCount ;
extern uchar	DispBuffer[11];//��ʾ����
extern uint		MainCode;//վ���
extern uchar	Disp0_9String[10];
extern const uchar	DefaultNetDatas[22];
extern ulong		KeyValue;
extern uchar		InputBuffer[8];
extern unsigned char	  	Forbidden;
extern unsigned char 		keybuff[4];
extern ulong ResetW5500Num  ;

extern	unsigned char		bitUpdateParameter;
extern	union	sDatasUnion   SerialUnion;
extern uchar	ReSerialComd;
extern uint	ReMainCode;
extern union	uTimeUnion		SysTimeDatas;
extern union	uTimeUnion		SysTimeDatasBak;
extern uchar  	Pos_SysDate[6];//ϵͳ����
extern uchar TypeA_Sort;
extern uchar		CardType[2];
extern uchar		CardSerialNum[4];
extern uchar		CardSerialNumBak[4];
extern uchar    *CPU_RcLen;
extern uchar    CPU_CommBuf[50];

extern	unsigned char		CardIdentity;
extern	unsigned char  	Card_Rebate;
extern	unsigned char		InputCase;
extern	unsigned char		InputNum;
extern	unsigned long		SingleMoney;
extern	unsigned char		bitInPutPoint;
extern	unsigned char		InputMaxNum;
extern	uint						MulData;
extern 	unsigned char		BatModeFlag;
extern	unsigned char		bitHaveReadBalance; 

extern uchar     fristLook;
extern uchar     LookOk;
extern uchar     LookMony[8];
extern uchar		bitDispFlash;
extern uchar		bitLookSysDatas;
extern uchar	Resend_Num;
extern uchar		ConsumModeEnable;//���ѷ�ʽ����
extern ulong		SumConsumMoney;//�����Ѷ�
extern ulong		SumPrintMoney;//��ӡ�������Ѷ�
extern uchar   NeedDispFlag ;//��Ҫ��ʾ

//���ѱ���
extern uchar CardPID[4];//��Ƭ��Ψһ���к�
extern uchar CardPrintNum[4];//��Ƭ��ӡˢ���
extern uchar	ConsumeMode;//����ģʽ
extern uchar CurrentConsumeMoney[4];//���ѽ��	
extern ulong		CurrentConsumMoney;//�������Ѷ�
extern uchar		ConsumMode;//���ѷ�ʽ
extern  uchar	DispHardeErrorString[7*16];
extern uchar	DispErrorString[27*16];
extern  uchar	DispModeChar[5];
extern uchar	PosVerString[9];//F012.F00.07
extern uchar  	Purse_num;
extern uchar   Consum_Status;//1--��ʾ�ѻ�
extern uchar		bitAddStatus;
extern ulong BeepTimes;
extern uchar   Num_MulValue;
extern uchar	MoneyMulValue;
extern uchar		InputCount;
extern ulong   Falsh_TimeCount;
extern uchar		LedWattingChar;
extern uchar   OutTimeFlag;
extern uchar  Consum_Datas[8];
extern uchar  HaveQueryConDat;
extern uchar   HeartBeatNum;
extern uchar 	CardPrinterNum[6];//ӡˢ���
extern uchar  LimtConsumeData_CPU[4];
extern uchar	  ConsumSndInfo[60];//CPU��������Ϣ
extern u16 TimerCount;
extern uchar   PsamRcvBufCount;
extern	unsigned char   PsamRcvBuf[50];//psam
extern uchar		NoNetRecord[32];//�ѻ���¼
extern uchar   flag_off_full;//�ѻ���¼��
extern uint   	offline_ReIndex;
extern uint   	offline_UpIndex;
extern uchar	flag_first_over;//��һ���ϴ���ɱ�־
extern uint Off_TimeCount ;
extern uint Off_TimeCount ;
extern uchar 	flag_reupcode;//���ɼ�¼
extern uint  	UPD_TimeCount;
extern uchar		NeedSendRecordFlag ;
extern uchar		ReCardDatas[6];
extern uchar 	NetCardDatas[22];
extern uint   	recode_rdnum;
extern uchar   Udp_SerialNum;//���к�
extern ulong   NoNetLimetMoney;
extern uchar LimtConsumeBuf[4];
extern uchar		bitHaveInputDatas;
extern ulong   LimtMoney;
extern uchar  HaveOffRecord ;
extern uchar CanNoNetConsume ;
extern uchar		bitHaveDispBalance;
extern uchar ComsumeDate[3];//��������
extern uchar HaveSendDataFlag ;//
extern uchar     ThreeRecord[24];

extern uchar		CardKeyCode[6];//��������
extern uchar		CalCardKey[8];//����Կ
extern uchar		CardSector;//�������������� 
extern ulong stationcode;//�շѵ�վ���

extern uchar	customerId[20];//�ͻ�ID
extern uchar	customerkey[50];//�ͻ���Կ
extern uchar  posStyle;//�豸����
extern uchar	posVersion[50];//�̼��汾��
extern uchar tokenBuf[50];
extern uchar		usesectornum;//�ѻ�������		
extern ulong		LimtConsumeMoney;