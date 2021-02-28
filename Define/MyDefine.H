
//#include <absacc.h>
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"

#define devicetype	1


#define		ulong	unsigned 	long
#define		uint	unsigned   short
#define		uchar	unsigned  	char

#define		Off_MAXRECORD	    	3000//最多的存储记录数
#define		Off_RECORD_BLOCK_NUM	96 //用于存储消费记录的块数	
	
#define		UPD_DELAY_TIME      100

#define		MAXCARDPRINTERNUM	262144 //最大卡号

#define		RECORD_SIZE		    32 //每条记录占的字节数
#define		RECORD_BLOCK_NUM	60//60 //用于存储消费记录的块数
#define		RECORDNUM_BLOCK		128 //每块存储的记录数 (4096/RECORD_SIZE)
#define		MAXRECORD_NUM	    7680//最多的存储记录数
#define     SAVERECORD_BAK_USEBLOCK	   70//存储交易数据的块
//一共的块 128块
//程序备份区 90 到128
#define 	PRG_BACK_START		80
#define 	PRG_BACK_END			127


#define     FLASH_BLOCK_NUM	   128//4M的FLASH总共128个块每块32KB
//一下几个数据存在最后的8页中对应着原来存储程序的第三页存储的数据
#define		MenuName_Addr				(SAVERECORD_BAK_USEBLOCK+1)*0x1000//菜名
#define		SortPrince_Addr				(SAVERECORD_BAK_USEBLOCK+3)*0x1000//价格方案

#define		PurseKind_Addr				(SAVERECORD_BAK_USEBLOCK+2)*0x1000//身份钱包顺序
#define		PurseEnable_Addr			(SAVERECORD_BAK_USEBLOCK+4)*0x1000//身份钱包顺序
#define     RecordDatasToBuffer_Addr 	(SAVERECORD_BAK_USEBLOCK+5)*0x1000 //备份记录的指针
 

#define		CONSUM_SIMPLE		0 //简易
#define		CONSUM_NUM			1 //菜号
#define		CONSUM_RATION		2 //定额
#define		CONSUM_MONEY		3 //金额
#define		CONSUM_MENU			4 //菜单设置方式

#define		DEFAULT_CONSUM_MODE	CONSUM_MONEY //默认的消费方式

//通讯命令
#define		Clr_PosData				 0xc9//清空POS，根据数据不同处理不同
#define		Close_Pos				   0xcb//关闭POS
#define 	HeartBeatCmd     			   0xA0//心跳命令
#define 	GetTokenCmd     			   0xA1//获取token
#define 	GetBalanceCmd     			 0xA2//获取余额
#define 	ConsumeCmd     			 		 0xA3//消费接口
#define		UpRecodCmd							 0xA4//上传脱机记录
#define		QueryToalConsumeMoney		 0xA5//查询消费总额
#define 	QueryConsumeRecordCmd    0xAF//查询最近的三笔消费
//#define 	QueryDayConsumeCmd   		 0xF1//查询当日消费金额笔数
//#define 	AppointDayConsumeCmd   		 0xF2//指定某日的消费金额跟笔数
//#define 	QueryConsumeRecordCmd    0xAF//查询最近的三笔消费
//#define 	RequestConsumeCmd				 0xA4//请求消费
//#define		ADD_BLKNUM_COMD					 0x2C//增加黑名单
//#define		DEL_BLKNUM_COMD			     0x4C//删除黑名单
//#define		CLR_BLKNUM_COMD			     0x44//清除黑名单 Limt_Consume_COMD
//#define		Limt_Consume_COMD		     0x55//下载消费限额
//#define   Rd_Recode_Comd           0x32//脱机记录移指针

#define		Request_CardCon         0x22//卡消费0x00~0x5f
#define		Request_CardErr1        0x13//卡错误状态
#define		Request_CardErr2        0x14//卡错误状态
#define		Request_CardErr3        0x15//卡错误状态
#define		Request_CardErr4        0x16//卡错误状态
#define		Request_CardErr5        0x17//卡错误状态
#define		Request_CardErr6        0x18//密码效验
#define		PinCode_Error           0x19//密码效验错误


////错误信息定义
#define		SYS_OK					0
#define		CARD_OK					0
#define		SYS_RC500_ERROR			1//读卡模块 
#define		SYS_FLASH_ERROR			2//FLASH
#define		SYS_24C256_ERROR		3//24C256
#define		SYS_24C64_ERROR			4//24C64
#define		TCP_ERROR			      5//TCP模块错误
#define		PASM_ERROR			    6//PSAM模块错误
///上位机返回的错误
#define		CARD_NUMBER_ERROR				1//非本系统卡   
#define		CARD_OVER_DATE_ERROR		2//卡过期
#define		CARD_CANEL_ERROR				3//卡号     
#define		CARD_LOSS_ERROR		  		4//卡号挂失        
//#define		CARD_FREEZE_ERROR				5//卡号被冻结  
#define		CARD_FREEZE_ERROR				6//余额负数
#define		LESS_MONEY_ERROR	  7//余额不足       
#define		SINGLE_MONEY_ERROR	8//单次限额   
#define		CARD_PURSENUM_ERROR	11//脱机限额 
#define		CARD_TUOJINULL_ERROR	22//脱机未开启
    



//--------------------------------------------------

 

//不显示的状态
#define     MATCHCODE_ERR        19//PSAM初始化错误(1.复位2.PPS失败3.一卡通目录4.触点接触不良)
#define     CPU_SELFILE_FAIL        20//选文件失败(1.主目录2.一卡通应用3.钱包文件)
#define     CPU_READFILE_FAIL       21//读文件失败(1.公共信息2.累计文件3.记录)
#define     CPU_REDABALANCE_FAIL    22//读余额失败
#define			Uncert_State						23//写卡、交易失败
#define     CARD_SAME_ERROR	        24//非同一张卡错误(1.序列号2.卡印刷号)
#define 		GET_PROVE_FAIL		    	25//取交易认证失败
#define     PSAM_MAC1_FAIL		    	26//MAC1
#define     PSAM_CHECK_MAC2		    	27//MAC2
#define	    PSAM_RESET_ERROR          28//匹配字错
#define	    PSAM_FILEPARA_ERR       29//PSAM错误(1.一卡通目录2.参数读取3.站点为0)
#define			NO_PURSE_ERROR					30 //钱包没设置
#define			SYS_CONSUM_ESC					31 //钱包没设置
#define			No_Use					        32 //不允许此身份消费
#define			No_ip					        	33 //IP丢失#define		CARD_NOCARD				0xff
  
#define     PSAM_COM_ERROR          0xf1//PSAM通讯错误(1.长度2.无响应3.响应错误) 
#define     CPU_WRITEPURSE_FAIL     0xf2//写累计失败
#define     CONSUM_PROCE_FAIL	    	0xf3//交易失败
#define			CARD_NOCARD							0xff


///////////////////////////键盘按键定义//////////////////
//第一行:
	#define		KEY_SIMPLE		0x00000001
	#define		KEY_RATION		0x00010000 
	#define		KEY_NUM				0x00020000
	#define		KEY_MONEY			0x00001000	
//第二行:
	#define		KEY_1			0x10000000
	#define		KEY_2			0x01000000
	#define		KEY_3			0x00100000
	#define		KEY_MENU	0x00002000
//第三行:
	#define		KEY_4			0x20000000
	#define		KEY_5			0x02000000
	#define		KEY_6			0x00200000
	#define		KEY_ESC		0x00004000
//第四行:
	#define		KEY_7			0x40000000
	#define		KEY_8			0x04000000
	#define		KEY_9			0x00400000
	#define		KEY_MUL		0x00040000
//第五行:
	#define		KEY_0				0x80000000
	#define		KEY_POINT		0x08000000   //小数点,
	#define		KEY_ADD			0x00800000
	#define		KEY_ENTER		0x00008000
//其它功能键	
	#define		KEY_0_9			0xF7710000
	#define		KEY_ANY			0xffffffff
	#define		KEY_NO			0x00000000
		
///////////////////X25045地址定义////////////////////
#define		MainCode_Addr				4//站点号0-3
#define		ConsumStatus_Addr		7//是否脱机消费
#define		ReadCardCode_Addr		10 //读卡密码10-17
#define		UserSector_Addr			18 //公共区的扇区18-20
#define		LimtConsume_Addr		21//脱机限额地址
#define		stationcode_Addr		24//消费站点号

#define		ConsumModeEnable_Addr	0x1d //消费方式允许 0x1d 0x1e 0x1f
#define		TcpCardDatas_Addr		0x50 //网卡参数设置 
#define		CommModeChar_Addr		0x6b //通讯模式0－485，1-TCPIP
#define		MoneyMulValue_Addr		0x7d //金额的倍数,分为1,10,100
#define		SimplePrince_Addr		0x80 //简易消费单价(预留12个 空间12*5=60),0x70--0xaf
#define		MenuPrince_Addr         0x100 //菜号消费单价(最多50个，空间50*5=250)

#define   usesectorNum_Addr				0x500	//使用扇区地址
#define   useUsrID						0x505 //用户的用户ID 
#define 	LimitedMoney				0x510 //用户脱机限额
#define   UpdatePar						0x520 //升级参数
#define 	DeviceID						0x530 //设备ID
#define 	svrPort							0x53A //服务器端口
#define 	svraddr							0x540 //服务器地址
#define 	staticsum						0x590 //是否自动获取ＩＰ

#define		OffLineData_Page		2	//脱机地址页
#define		OffLineData_Addr		0   //脱机起始地址


//参数存储的flash地址
#define MainCodeAddress    				0x08061000
#define UserSectorFlashAdd  			MainCodeAddress+1*2//公共扇区号
#define UserCodeFlashAdd  				UserSectorFlashAdd+1*2//匹配字
#define CardKeyCodeFlashAdd				UserCodeFlashAdd+4*2//公共山区密码
#define CalCardKeyFlashAdd				CardKeyCodeFlashAdd+6*2//卡密钥
#define CardMinBalanceFlashAdd		CalCardKeyFlashAdd+8*2//卡底金
#define DayLimetMoneyFlashAdd   	CardMinBalanceFlashAdd+1*2//卡日限额
#define CardEnableMonthsFlasgAdd	DayLimetMoneyFlashAdd+2*2//黑名单有效月
#define ConsumModeEnableFlashAdd	CardEnableMonthsFlasgAdd+1*2//消费方式允许位
#define BatchEnableFlashAdd				ConsumModeEnableFlashAdd+1*2//批次
#define CommEncryptKeyFlasAdd			BatchEnableFlashAdd+32*2//传输密钥
#define CommModeCharFlashAdd			CommEncryptKeyFlasAdd+8*2//通讯方式
#define RemoteFlagFlashAdd				CommModeCharFlashAdd +1*2//远端绑定标记
#define BuTieFlashAdd							RemoteFlagFlashAdd +1*2//补贴标记
#define TcpCardDatasFlashAdd			BuTieFlashAdd +1*2


