
//#include <absacc.h>
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"

#define devicetype	1


#define		ulong	unsigned 	long
#define		uint	unsigned   short
#define		uchar	unsigned  	char

#define		Off_MAXRECORD	    	3000//���Ĵ洢��¼��
#define		Off_RECORD_BLOCK_NUM	96 //���ڴ洢���Ѽ�¼�Ŀ���	
	
#define		UPD_DELAY_TIME      100

#define		MAXCARDPRINTERNUM	262144 //��󿨺�

#define		RECORD_SIZE		    32 //ÿ����¼ռ���ֽ���
#define		RECORD_BLOCK_NUM	60//60 //���ڴ洢���Ѽ�¼�Ŀ���
#define		RECORDNUM_BLOCK		128 //ÿ��洢�ļ�¼�� (4096/RECORD_SIZE)
#define		MAXRECORD_NUM	    7680//���Ĵ洢��¼��
#define     SAVERECORD_BAK_USEBLOCK	   70//�洢�������ݵĿ�
//һ���Ŀ� 128��
//���򱸷��� 90 ��128
#define 	PRG_BACK_START		80
#define 	PRG_BACK_END			127


#define     FLASH_BLOCK_NUM	   128//4M��FLASH�ܹ�128����ÿ��32KB
//һ�¼������ݴ�������8ҳ�ж�Ӧ��ԭ���洢����ĵ���ҳ�洢������
#define		MenuName_Addr				(SAVERECORD_BAK_USEBLOCK+1)*0x1000//����
#define		SortPrince_Addr				(SAVERECORD_BAK_USEBLOCK+3)*0x1000//�۸񷽰�

#define		PurseKind_Addr				(SAVERECORD_BAK_USEBLOCK+2)*0x1000//����Ǯ��˳��
#define		PurseEnable_Addr			(SAVERECORD_BAK_USEBLOCK+4)*0x1000//����Ǯ��˳��
#define     RecordDatasToBuffer_Addr 	(SAVERECORD_BAK_USEBLOCK+5)*0x1000 //���ݼ�¼��ָ��
 

#define		CONSUM_SIMPLE		0 //����
#define		CONSUM_NUM			1 //�˺�
#define		CONSUM_RATION		2 //����
#define		CONSUM_MONEY		3 //���
#define		CONSUM_MENU			4 //�˵����÷�ʽ

#define		DEFAULT_CONSUM_MODE	CONSUM_MONEY //Ĭ�ϵ����ѷ�ʽ

//ͨѶ����
#define		Clr_PosData				 0xc9//���POS���������ݲ�ͬ������ͬ
#define		Close_Pos				   0xcb//�ر�POS
#define 	HeartBeatCmd     			   0xA0//��������
#define 	GetTokenCmd     			   0xA1//��ȡtoken
#define 	GetBalanceCmd     			 0xA2//��ȡ���
#define 	ConsumeCmd     			 		 0xA3//���ѽӿ�
#define 	QueryConsumeRecordCmd    0xAF//��ѯ�������������
//#define 	QueryDayConsumeCmd   		 0xF1//��ѯ�������ѽ�����
//#define 	AppointDayConsumeCmd   		 0xF2//ָ��ĳ�յ����ѽ�������
//#define 	QueryConsumeRecordCmd    0xAF//��ѯ�������������
//#define 	RequestConsumeCmd				 0xA4//��������
//#define		ADD_BLKNUM_COMD					 0x2C//���Ӻ�����
//#define		DEL_BLKNUM_COMD			     0x4C//ɾ��������
//#define		CLR_BLKNUM_COMD			     0x44//��������� Limt_Consume_COMD
//#define		Limt_Consume_COMD		     0x55//���������޶�
//#define   Rd_Recode_Comd           0x32//�ѻ���¼��ָ��

#define		Request_CardCon         0x22//������0x00~0x5f
#define		Request_CardErr1        0x13//������״̬
#define		Request_CardErr2        0x14//������״̬
#define		Request_CardErr3        0x15//������״̬
#define		Request_CardErr4        0x16//������״̬
#define		Request_CardErr5        0x17//������״̬
#define		Request_CardErr6        0x18//����Ч��
#define		PinCode_Error           0x19//����Ч�����


////������Ϣ����
#define		SYS_OK					0
#define		CARD_OK					0
#define		SYS_RC500_ERROR			1//����ģ�� 
#define		SYS_FLASH_ERROR			2//FLASH
#define		SYS_24C256_ERROR		3//24C256
#define		SYS_24C64_ERROR			4//24C64
#define		TCP_ERROR			      5//TCPģ�����
#define		PASM_ERROR			    6//PSAMģ�����
///��λ�����صĴ���
#define		CARD_NUMBER_ERROR				1//�Ǳ�ϵͳ��   
#define		CARD_OVER_DATE_ERROR		2//������
#define		CARD_CANEL_ERROR				3//����     
#define		CARD_LOSS_ERROR		  		4//���Ź�ʧ        
//#define		CARD_FREEZE_ERROR				5//���ű�����  
#define		CARD_FREEZE_ERROR				6//����
#define		LESS_MONEY_ERROR	  7//����       
#define		SINGLE_MONEY_ERROR	8//�����޶�   
#define		CARD_PURSENUM_ERROR	11//�ѻ��޶� 
#define		CARD_TUOJINULL_ERROR	22//�ѻ�δ����
    



//--------------------------------------------------

 

//����ʾ��״̬
#define     MATCHCODE_ERR        19//PSAM��ʼ������(1.��λ2.PPSʧ��3.һ��ͨĿ¼4.����Ӵ�����)
#define     CPU_SELFILE_FAIL        20//ѡ�ļ�ʧ��(1.��Ŀ¼2.һ��ͨӦ��3.Ǯ���ļ�)
#define     CPU_READFILE_FAIL       21//���ļ�ʧ��(1.������Ϣ2.�ۼ��ļ�3.��¼)
#define     CPU_REDABALANCE_FAIL    22//�����ʧ��
#define			Uncert_State						23//д��������ʧ��
#define     CARD_SAME_ERROR	        24//��ͬһ�ſ�����(1.���к�2.��ӡˢ��)
#define 		GET_PROVE_FAIL		    	25//ȡ������֤ʧ��
#define     PSAM_MAC1_FAIL		    	26//MAC1
#define     PSAM_CHECK_MAC2		    	27//MAC2
#define	    PSAM_RESET_ERROR          28//ƥ���ִ�
#define	    PSAM_FILEPARA_ERR       29//PSAM����(1.һ��ͨĿ¼2.������ȡ3.վ��Ϊ0)
#define			NO_PURSE_ERROR					30 //Ǯ��û����
#define			SYS_CONSUM_ESC					31 //Ǯ��û����
#define			No_Use					        32 //����������������
#define			No_ip					        	33 //IP��ʧ#define		CARD_NOCARD				0xff
  
#define     PSAM_COM_ERROR          0xf1//PSAMͨѶ����(1.����2.����Ӧ3.��Ӧ����) 
#define     CPU_WRITEPURSE_FAIL     0xf2//д�ۼ�ʧ��
#define     CONSUM_PROCE_FAIL	    	0xf3//����ʧ��
#define			CARD_NOCARD							0xff


///////////////////////////���̰�������//////////////////
//��һ��:
	#define		KEY_SIMPLE		0x00000001
	#define		KEY_RATION		0x00010000 
	#define		KEY_NUM				0x00020000
	#define		KEY_MONEY			0x00001000	
//�ڶ���:
	#define		KEY_1			0x10000000
	#define		KEY_2			0x01000000
	#define		KEY_3			0x00100000
	#define		KEY_MENU	0x00002000
//������:
	#define		KEY_4			0x20000000
	#define		KEY_5			0x02000000
	#define		KEY_6			0x00200000
	#define		KEY_ESC		0x00004000
//������:
	#define		KEY_7			0x40000000
	#define		KEY_8			0x04000000
	#define		KEY_9			0x00400000
	#define		KEY_MUL		0x00040000
//������:
	#define		KEY_0				0x80000000
	#define		KEY_POINT		0x08000000   //С����,
	#define		KEY_ADD			0x00800000
	#define		KEY_ENTER		0x00008000
//�������ܼ�	
	#define		KEY_0_9			0xF7710000
	#define		KEY_ANY			0xffffffff
	#define		KEY_NO			0x00000000
		
///////////////////X25045��ַ����////////////////////
#define		MainCode_Addr				4//վ���0-3
#define		ConsumStatus_Addr		7//�Ƿ��ѻ�����
#define		ReadCardCode_Addr		10 //��������10-17
#define		UserSector_Addr			18 //������������18-20
#define		LimtConsume_Addr		21//�ѻ��޶��ַ
#define		stationcode_Addr		24//����վ���

#define		ConsumModeEnable_Addr	0x1d //���ѷ�ʽ���� 0x1d 0x1e 0x1f
#define		TcpCardDatas_Addr		0x50 //������������ 
#define		CommModeChar_Addr		0x6b //ͨѶģʽ0��485��1-TCPIP
#define		MoneyMulValue_Addr		0x7d //���ı���,��Ϊ1,10,100
#define		SimplePrince_Addr		0x80 //�������ѵ���(Ԥ��12�� �ռ�12*5=60),0x70--0xaf
#define		MenuPrince_Addr         0x100 //�˺����ѵ���(���50�����ռ�50*5=250)

#define   usesectorNum_Addr				0x500	//ʹ��������ַ
#define   useUsrID						0x505 //�û����û�ID 
#define 	LimitedMoney				0x510 //�û��ѻ��޶�
#define   UpdatePar						0x520 //��������
#define 	DeviceID						0x530 //�豸ID
#define 	svrPort							0x53A //�������˿�
#define 	svraddr							0x540 //��������ַ
#define 	staticsum						0x590 //�Ƿ��Զ���ȡ�ɣ�

#define		OffLineData_Page		2	//�ѻ���ַҳ
#define		OffLineData_Addr		0   //�ѻ���ʼ��ַ


//�����洢��flash��ַ
#define MainCodeAddress    				0x08061000
#define UserSectorFlashAdd  			MainCodeAddress+1*2//����������
#define UserCodeFlashAdd  				UserSectorFlashAdd+1*2//ƥ����
#define CardKeyCodeFlashAdd				UserCodeFlashAdd+4*2//����ɽ������
#define CalCardKeyFlashAdd				CardKeyCodeFlashAdd+6*2//����Կ
#define CardMinBalanceFlashAdd		CalCardKeyFlashAdd+8*2//���׽�
#define DayLimetMoneyFlashAdd   	CardMinBalanceFlashAdd+1*2//�����޶�
#define CardEnableMonthsFlasgAdd	DayLimetMoneyFlashAdd+2*2//��������Ч��
#define ConsumModeEnableFlashAdd	CardEnableMonthsFlasgAdd+1*2//���ѷ�ʽ����λ
#define BatchEnableFlashAdd				ConsumModeEnableFlashAdd+1*2//����
#define CommEncryptKeyFlasAdd			BatchEnableFlashAdd+32*2//������Կ
#define CommModeCharFlashAdd			CommEncryptKeyFlasAdd+8*2//ͨѶ��ʽ
#define RemoteFlagFlashAdd				CommModeCharFlashAdd +1*2//Զ�˰󶨱��
#define BuTieFlashAdd							RemoteFlagFlashAdd +1*2//�������
#define TcpCardDatasFlashAdd			BuTieFlashAdd +1*2

