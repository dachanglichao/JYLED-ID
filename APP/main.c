/**************************************************************************************

**************************************************************************************/
#include  "cpu.h"
#include  "os_cpu.h"
#include  "app_cfg.h"
#include  "ucos_ii.h"
#include  "include.h"
#include  "variable.h"
#include "stdio.h"
#include "dhcp.h"
#include "cJSON.h"

OS_EVENT *SemEthMac;  //互斥信号量
OS_EVENT *SemEthMacSENDREC;  //互斥信号量
OS_EVENT *SemFlashSpi;  //互斥信号量
OS_EVENT *mutDESCODE;

OS_EVENT *SemHandleETH; //以太网中断信号量

extern void	TCP_ReceiveSub(void);//接收数据处理
extern void	TCP_SendSub(uchar Cmd);//发送数据处理
extern void	ConsumSub(void);//消费主程序
void App_TaskDownLoadFile(void* p_arg);
extern uchar		ConsumCase;
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK App_TaskStartStk[APP_TASK_START_STK_SIZE];  //任务创建
static  OS_STK AppTaskEHTRECDATAStk[APP_TASK_EHTRECDATA_STK_SIZE];
static  OS_STK AppTaskCARDStk[APP_TASK_CARD_STK_SIZE];
static  OS_STK AppTaskDOWNStk[APP_TASK_DOWN_STK_SIZE];
static  OS_STK AppTaskUPLOADStk[APP_TASK_UPLOAD_STK_SIZE];


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void App_TaskCreate(void);
extern void App_TaskUploadRecord(void* p_arg);
static  void App_TaskStart(void* p_arg);




bool startFlag = FALSE;
/****************************************************************************
* 名    称：int main(void)
* 功    能：主函数入口
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
extern u8 isneedupdate;
int main(void)
{
   //CPU_INT08U os_err;
	 /* 硬件平台初始化 */
		BSP_Init();	
   /* 禁止所有中断 */
   CPU_IntDis();
   
   /* ucosII 初始化 */
   OSInit();                                                                                  
	
   //建立主任务， 优先级最高  建立这个任务另外一个用途是为了以后使用统计任务
   OSTaskCreate((void (*) (void *)) App_TaskStart,               		    //指向任务代码的指针
                          (void *) 0,												//任务开始执行时，传递给任务的参数的指针
													(OS_STK *) &App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],	//分配给任务的堆栈的栈顶指针   从顶向下递减
                         (INT8U) APP_TASK_START_PRIO);								//分配给任务的优先级
             
   OSTimeSet(0);			 //ucosII的节拍计数器清0    节拍计数器是0-4294967295  
   OSStart();                //启动ucosII内核   
   return (0);
}




/****************************************************************************
* 名    称：static  void App_TaskStart(void* p_arg)
* 功    能：开始任务建立
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
int tcp_send_and_rec_packet(int cmd,u8 * data,int sendLength,int entType,u8 * dataRec,int * length,int timeout);
extern void Handle_ETH_DATA(void* p_arg);
extern void Handle_Card(void* p_arg);
extern char keyDes[8];

const char datatempKey[] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38};
unsigned char		CheckPF8563(void);

u8 isReg = 0;
u8 snedData[15+6+10+2] = {0x02,0x00,0x00,0x12,0x27,0x24,0x17,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
int16_t dns_makequery(uint16_t op, char * name, uint8_t * buf, uint16_t len);
int32_t sendto(uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port);

extern uint8_t  DNS_SOCKET;    // SOCKET number for DNS
extern uint16_t DNS_MSGID;     // DNS message ID

u8 isgetdns = 0;
u8 svraddrip[4];
int svraddrport = 0;
u8 svraddrsdfsf[100];
extern u8 sendver[10];
void W5500_NVIC_Configuration_DIASABNLE(void);
void W5500_Init_DHCP(void);
void InitW5500Net(void);
u8 isneeddhcp = 1;

u8 check_DHCP_state(SOCKET s) ;
void Set_Sysdate(unsigned char * ptr);
	
void startupdate()
{
	OSTaskCreateExt(App_TaskDownLoadFile,
							(void *)0,
							(OS_STK *)&AppTaskDOWNStk[APP_TASK_DOWN_STK_SIZE-1],
							APP_TASK_DOWN_FILE_PRIO,
							APP_TASK_DOWN_FILE_PRIO,
							(OS_STK *)&AppTaskDOWNStk[0],
							APP_TASK_DOWN_STK_SIZE,
							(void *)0,
							OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);
}
extern u8 scidcode[6];
void App_TaskStart(void* p_arg)
{
	u8 dhcpret=STATE_DHCP_READY;
	u8 recDataTemp[500];
	u8 TempBuf[500];
	int recDataLength = 0;
  OS_CPU_SR cpu_sr;
	u8 err;	
	u16 i;
	u32 addr;
	int len;
	u8 diani = 0;
	char *ptr;
	u8 needdns = 1;
	int year;
	u8 status = -1,Buffer[10];
	cJSON *object;
	cJSON *arrayItem;
	
  (void) p_arg;

   //初始化ucosII时钟节拍
   OS_CPU_SysTickInit();
	

   //使能ucos 的统计任务
#if (OS_TASK_STAT_EN > 0)
   
   OSStatInit();                //----统计任务初始化函数                                 
#endif
	
	SemEthMac = OSMutexCreate(1,&err);	//创建互斥信号量	
	SemFlashSpi = OSMutexCreate(3,&err);	//创建互斥信号量
	mutDESCODE = OSMutexCreate(2,&err);	//创建互斥信号量
	SemEthMacSENDREC = OSMutexCreate(4,&err);	//创建互斥信号量
	
	SemHandleETH = OSSemCreate(0);
	
	InitW5500Net();//W5500工作模式配置 
	/*   建立任务一 */
	OSTaskCreateExt(Handle_ETH_DATA,
				(void *)0,
				(OS_STK *)&AppTaskEHTRECDATAStk[APP_TASK_EHTRECDATA_STK_SIZE-1],
				APP_TASK_EHTRECDATA_PRIO,
				APP_TASK_EHTRECDATA_PRIO,
				(OS_STK *)&AppTaskEHTRECDATAStk[0],
				APP_TASK_EHTRECDATA_STK_SIZE,
				(void *)0,
				OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);
				//Socket_Init(3);	
	/*   建立任务一 */
	OSTaskCreateExt(Handle_Card,
				(void *)0,
				(OS_STK *)&AppTaskCARDStk[APP_TASK_CARD_STK_SIZE-1],
				APP_TASK_CARD_PRIO,
				APP_TASK_CARD_PRIO,
				(OS_STK *)&AppTaskCARDStk[0],
				APP_TASK_CARD_STK_SIZE,
				(void *)0,
				OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);
	/*   建立任务一 */
	OSTaskCreateExt(App_TaskUploadRecord,
				(void *)0,
				(OS_STK *)&AppTaskUPLOADStk[APP_TASK_UPLOAD_STK_SIZE-1],
				APP_TASK_UPLOAD_PRIO,
				APP_TASK_UPLOAD_PRIO,
				(OS_STK *)&AppTaskUPLOADStk[0],
				APP_TASK_UPLOAD_STK_SIZE,
				(void *)0,
				OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);
									
	if(isneedupdate == 1)
	{
		startupdate();
	}

	Socket_Init(5);		//指定Socket(0~7)初始化,初始化端口0	
	Socket_Init(7);		//指定Socket(0~7)初始化,初始化端口0	
	while (1)
	{ 
		err = 1;
		recDataLength = sizeof(snedData);
		if(!isReg )
		{				
			err = tcp_send_and_rec_packet(GetTokenCmd,snedData,sizeof(snedData),1,recDataTemp,&recDataLength,3);//连接后台获取token
			if(!err)
			{				
				object=cJSON_Parse(recDataTemp); //????????
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
				
				arrayItem = cJSON_GetObjectItem(object,"resultData"); //????????
				memset(tokenBuf,0,50);
				memcpy(tokenBuf,cJSON_GetObjectItem(arrayItem,"token")->valuestring,(strlen(cJSON_GetObjectItem(arrayItem,"token")->valuestring)));
				isReg = 1;//连接到后台
				Write_SOCK_Data_Buffer(5,tokenBuf,strlen(tokenBuf));
				cJSON_Delete(object);	
			}
			if(err)
			{
				OSMutexPend(SemEthMac,0,&err);  //申请互斥资源使用
				Write_W5500_SOCK_1Byte(7,Sn_CR,CLOSE);// 关闭端口,等待重新打开连接 
				Socket_Init(7);		//指定Socket(0~7)初始化,初始化端口0	
				OSMutexPost(SemEthMac);
			}
		}	
		if(isReg)
		{
		//	OSTimeDlyHMSM(0, 0, 2, 0);//将任务挂起500ms	 	
			err = tcp_send_and_rec_packet(HeartBeatCmd,snedData,sizeof(snedData),1,recDataTemp,&recDataLength,3);//定时获取时间
			if(!err)
			{
				object=cJSON_Parse(recDataTemp); //
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
				Consum_Status=0;	//联网模式			
				arrayItem = cJSON_GetObjectItem(object,"resultData"); //
				if(startFlag==FALSE)
				{
					startFlag =TRUE;
					//获取时间
					memcpy(TempBuf,cJSON_GetObjectItem(arrayItem,"txdate")->valuestring,(strlen(cJSON_GetObjectItem(arrayItem,"txdate")->valuestring)));
				//	isReg = 0;//连接到后台
					//设置系统时间
					HexStringToHexGroup(TempBuf+2,SysTimeDatas.TimeString,12);
					Set_Sysdate(SysTimeDatas.TimeString);
					Read_Sysdate(SysTimeDatas.TimeString);
					
					//脱机扇区号
					memcpy(TempBuf,cJSON_GetObjectItem(arrayItem,"sector")->valuestring,(strlen(cJSON_GetObjectItem(arrayItem,"sector")->valuestring)));
					HexStringToHexGroup(TempBuf,&usesectornum,2);
					usesectornum = 10;//测试使用
					//脱机限额
					//memcpy(TempBuf,cJSON_GetObjectItem(arrayItem,"limitMoney")->valuestring,(strlen(cJSON_GetObjectItem(arrayItem,"limitMoney")->valuestring)));
					LimtConsumeMoney = atoi(cJSON_GetObjectItem(arrayItem,"limitMoney")->valuestring);
				
					//扇区密码
					memcpy(TempBuf,cJSON_GetObjectItem(arrayItem,"cardPwd")->valuestring,(strlen(cJSON_GetObjectItem(arrayItem,"cardPwd")->valuestring)));
					HexStringToHexGroup(TempBuf,scidcode,6);
					
					Buffer[0]=0xa0;
					Buffer[1]=BCDToHex(usesectornum);
					Buffer[2]=CalCheckSum(Buffer,2);
					WrBytesToAT24C64(usesectorNum_Addr,Buffer,3);//
				
					Buffer[0]=0xa0;
					memcpy(Buffer+1,scidcode,6);
					Buffer[7]=CalCheckSum(Buffer,7);
					WrBytesToAT24C64(useUsrID,Buffer,8);//

					Buffer[0]=0xa0;
					Buffer[4]=LimtConsumeMoney>>24;
					Buffer[3]=LimtConsumeMoney>>16;
					Buffer[2]=LimtConsumeMoney>>8;
					Buffer[1]=LimtConsumeMoney;
					Buffer[5]=CalCheckSum(Buffer,5);
					WrBytesToAT24C64(LimitedMoney,Buffer,6);//
					memcpy(LimtConsumeBuf,Buffer+1,4);
				}
				
				//	Write_SOCK_Data_Buffer(5,TempBuf,(strlen(cJSON_GetObjectItem(arrayItem,"txdate")->valuestring)));	
				cJSON_Delete(object);	
			}
			else
			{
				isReg =0;	
				Consum_Status=1;	//脱机模式	
				OSMutexPend(SemEthMac,0,&err);  //申请互斥资源使用
				Write_W5500_SOCK_1Byte(7,Sn_CR,CLOSE);// 关闭端口,等待重新打开连接 
				Socket_Init(7);		//指定Socket(0~7)初始化,初始化端口0	
				OSMutexPost(SemEthMac);
			}		
		}
	
		OSTimeDlyHMSM(0, 0, 5, 0);//将任务挂起500ms	 		
	}
}

#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument : ptcb   is a pointer to the task control block of the task being created.
*
* Note     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void App_TaskCreateHook(OS_TCB* ptcb)
{
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void App_TaskDelHook(OS_TCB* ptcb)
{
   (void) ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument : none.
*
* Note     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void App_TaskIdleHook(void)
{
}
#endif

/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument : none.
*********************************************************************************************************
*/

void App_TaskStatHook(void)
{
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument : none.
*
* Note     : 1 Interrupts are disabled during this call.
*
*            2  It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void App_TaskSwHook(void)
{
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument : ptcb    is a pointer to the TCB of the task being created.
*
* Note     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void App_TCBInitHook(OS_TCB* ptcb)
{
   (void) ptcb;
}
#endif

#endif
