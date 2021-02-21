#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "char.H"
#include  "ucos_ii.h"

extern OS_EVENT *SemEthMac;  //互斥信号量
extern OS_EVENT *HaveReceiveData_Semp;//已经接收到数据
extern OS_EVENT *NeedSendData_Semp;//需要发送数据


extern uchar bitSerial_ReceiveEnd;//串口接受完成标志位
extern uchar SerialReceiveLen;//串口额接收数据长度
extern uchar bitSerial_SendRequest;
extern uchar SerialReCase;//串口接受状态
extern uchar		bitEven;
extern uchar SerialReceiveTimeCount;
extern uint		SerialReCheckSum;
extern uchar		SerialReCheckSumCount;
extern uchar		rChar;

extern ulong BeepDelayTimeCount ;
extern uchar	DispBuffer[11];//显示数据
extern uint		MainCode;//站点号
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
extern uchar  	Pos_SysDate[6];//系统日期
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
extern uchar		ConsumModeEnable;//消费方式允许
extern ulong		SumConsumMoney;//总消费额
extern ulong		SumPrintMoney;//打印的总消费额
extern uchar   NeedDispFlag ;//需要显示

//消费变量
extern uchar CardPID[4];//卡片的唯一序列号
extern uchar CardPrintNum[4];//卡片的印刷编号
extern uchar	ConsumeMode;//消费模式
extern uchar CurrentConsumeMoney[4];//消费金额	
extern ulong		CurrentConsumMoney;//本次消费额
extern uchar		ConsumMode;//消费方式
extern  uchar	DispHardeErrorString[7*16];
extern uchar	DispErrorString[27*16];
extern  uchar	DispModeChar[5];
extern uchar	PosVerString[9];//F012.F00.07
extern uchar  	Purse_num;
extern uchar   Consum_Status;//1--表示脱机
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
extern uchar 	CardPrinterNum[6];//印刷编号
extern uchar  LimtConsumeData_CPU[4];
extern uchar	  ConsumSndInfo[60];//CPU卡发送信息
extern u16 TimerCount;
extern uchar   PsamRcvBufCount;
extern	unsigned char   PsamRcvBuf[50];//psam
extern uchar		NoNetRecord[32];//脱机记录
extern uchar   flag_off_full;//脱机记录满
extern uint   	offline_ReIndex;
extern uint   	offline_UpIndex;
extern uchar	flag_first_over;//第一条上传完成标志
extern uint Off_TimeCount ;
extern uint Off_TimeCount ;
extern uchar 	flag_reupcode;//复采记录
extern uint  	UPD_TimeCount;
extern uchar		NeedSendRecordFlag ;
extern uchar		ReCardDatas[6];
extern uchar 	NetCardDatas[22];
extern uint   	recode_rdnum;
extern uchar   Udp_SerialNum;//序列号
extern ulong   NoNetLimetMoney;
extern uchar LimtConsumeBuf[4];
extern uchar		bitHaveInputDatas;
extern ulong   LimtMoney;
extern uchar  HaveOffRecord ;
extern uchar CanNoNetConsume ;
extern uchar		bitHaveDispBalance;
extern uchar ComsumeDate[3];//消费日期
extern uchar HaveSendDataFlag ;//
extern uchar     ThreeRecord[24];

extern uchar		CardKeyCode[6];//读卡密码
extern uchar		CalCardKey[8];//卡密钥
extern uchar		CardSector;//公共区的扇区号 
extern ulong stationcode;//收费的站点号

extern uchar	customerId[20];//客户ID
extern uchar	customerkey[50];//客户秘钥
extern uchar  posStyle;//设备类型
extern uchar	posVersion[50];//固件版本号
extern uchar tokenBuf[50];
extern uchar		usesectornum;//脱机扇区号		
extern ulong		LimtConsumeMoney;
