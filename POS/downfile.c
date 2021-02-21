#include  "os_cpu.h"
#include  "app_cfg.h"
#include  "ucos_ii.h"
#include  "include.h"
#include "stdio.h"
#include "cJSON.h"
#include "stdlib.h"

u8 filesvrIP[4] = {192,168,2,224};
int filesvrPOTY = 413;
volatile int fileindex = 0;
volatile int isRecOK = 0;
u8 datacmp[10];
int indexnum = 0;
unsigned long baoindex;
u8 filedataread[512];
u8 filesendtemp[20];
void sendfileindex(int index);
void sendfileindex(int index);

//0x2108 //获取地址
int tcp_send_and_rec_packet(int cmd,u8 * data,int sendLength,int entType,u8 * dataRec,int * length,int timeout);
extern u8 isReg;
//status = tcp_send_and_rec_packet(0x2101,CardPrinterNum,6,1,recDataTemp,&recDataLength,3);
//Write_SOCK_Data_Buffer(7, Tx_Buffer, udp_len);//指定Socket(0~7)发送数据处理,端口0发送23字节数据
u8 recdatasvr[100];
extern u8 filesvrIP[4];// = {192,168,2,224};
extern int filesvrPOTY;// = 9093;
char getFile(char * url,long start);
char tmpfiledata[512];
int tmpfilelen = 0;
long allfilelen = 0;
struct resp_header//保持相应头信息
{
    int status_code;//HTTP/1.1 '200' OK
    char content_type[128];//Content-Type: application/gzip
    long content_length;//Content-Length: 11683079
    char file_name[256];
};

struct resp_header resp;//全剧变量以便在多个进程中使用

extern OS_EVENT *SemFlashSpi;  //互斥信号量
extern volatile char isOnKMD;

char getFileall(char * url,long start);

volatile char isGetFileLen = 0;
unsigned long allFileWantLen = 0;

void App_TaskDownLoadFile(void* p_arg)
{
	int i;
	u32 flashaddr = 0;
	int status;
	u8 senddatasvr[1];
	int reclen;
	cJSON * root,*rootfor;
	u8 datagetsvr[80];
	int portsvr;
	char * ptr;
	INT8U err;
	int timecnt = 0;
	unsigned long addrflash = 0;
	
	while(1)
	{
		if(isReg == 0)
		{
			OSTimeDlyHMSM(0, 0, 3, 10);//将任务挂起500ms	
		}else
		{
			status = tcp_send_and_rec_packet(0x2108,senddatasvr,1,1,recdatasvr,&reclen,3);
			if(status == 0)
			{
				root = cJSON_Parse((const char *)(recdatasvr+3)); 
				if(root != 0)
				{
					status = cJSON_GetObjectItem(root,"returnType")->valueint;  
					if(status == 0)
					{
						rootfor = cJSON_GetObjectItem(root,"data");  //balance1
						if(rootfor != 0)
						{
							memset(datagetsvr,0,80);
							memcpy(datagetsvr,cJSON_GetObjectItem(rootfor,"svrip")->valuestring,
								strlen(cJSON_GetObjectItem(rootfor,"svrip")->valuestring));
							i=0;
							ptr = strtok(datagetsvr, ".");
							filesvrIP[i++] = atoi(ptr);
							while(ptr != NULL)
							{
								ptr = strtok(NULL, ".");
								if(ptr == NULL) break;
								filesvrIP[i++] = atoi(ptr);
							}
							
							portsvr = cJSON_GetObjectItem(rootfor,"svrport")->valueint;
						}
					}
				}
				cJSON_Delete(root);
				break;
			}
		}
		
	}
	
	if(i==4 && portsvr !=0)
	{
		filesvrPOTY = 8082;
	}else
	{
		while(1)
		{
			OSTimeDlyHMSM(0, 0, 0, 10);//将任务挂起500ms	
		}
	}
	
	OSMutexPend(SemEthMac,0,&err);  //申请互斥资源使用
	Socket_Init(6);
	OSMutexPost(SemEthMac);
	
	while(1)
	{
		isRecOK = 0;
		allfilelen = 0;	 
		baoindex = 0;
		isGetFileLen = 1;
		while(1)
		{
			OSMutexPend(SemEthMac,0,&err);  //申请互斥资源使用
			Write_W5500_SOCK_1Byte(6,Sn_CR,CLOSE);// 关闭端口,等待重新打开连接 
			Socket_Init(6);
			OSMutexPost(SemEthMac);
			OSTimeDlyHMSM(0, 0, 5, 0);//将任务挂起500ms
			sprintf((char *)recdatasvr,"http://%s:8082/newupdate_%d.bin",datagetsvr,devicetype);
			status = getFileall((char *)recdatasvr,0);
			if(status == 0)
				break;
		}
		
		isRecOK = 0;
		allfilelen = 0;	 
		baoindex = 0;
		timecnt = 0;
		while(1)
		{
			if(isRecOK)
			{
				isRecOK = 0;
				
				if(isGetFileLen == 1)
				{
					isGetFileLen = 0;
					OSMutexPend(SemEthMac,0,&err);  //申请互斥资源使用
					Write_W5500_SOCK_1Byte(6,Sn_CR,CLOSE);// 关闭端口,等待重新打开连接 
					Socket_Init(6);
					OSMutexPost(SemEthMac);
					OSTimeDlyHMSM(0, 0, 5, 0);//将任务挂起500ms
				}
				
				status = getFile((char *)recdatasvr,allfilelen);
				timecnt = 0;
			}else
			{
				timecnt++;
				if(timecnt >= 1000*5)
				{
					

						break;
				}
			}
			if(status != 0)
			{
				break;
			}
			OSTimeDlyHMSM(0, 0, 0, 1);//将任务挂起500ms	 
		}
//		
		
		OSTimeDlyHMSM(0,0, 10, 0);//将任务挂起500ms	 	
	}
}

void parse_url(const char *url, char *domain, int *port, char *file_name)
{
    /*通过url解析出域名, 端口, 以及文件名*/
    int j = 0;
    int start = 0;
	int i = 0;
    char *pos;
		
	
    char *patterns[] = {"http://", "https://", ""};
		
		*port = 80;

//    for (i = 0; patterns[i]; i++)
//        if (strncmp(url, patterns[i], strlen(patterns[i])) == 0)
//            start = strlen(patterns[i]);
		start = 7;

    //解析域名, 这里处理时域名后面的端口号会保留
    for (i = start; url[i] != '/' && url[i] != '\0'; i++, j++)
        domain[j] = url[i];
    domain[j] = '\0';

    //解析端口号, 如果没有, 那么设置端口为80
    pos = strstr(domain, ":");
    if (pos)
        sscanf(pos, ":%d", port);

    //删除域名端口号

    for (i = 0; i < (int)strlen(domain); i++)
    {
        if (domain[i] == ':')
        {
            domain[i] = '\0';
            break;
        }
    }

    //获取下载文件名
    j = 0;
    for (i = start; url[i] != '\0'; i++)
    {
        if (url[i] == '/')
        {
            if (i !=  strlen(url) - 1)
                j = 0;
            continue;
        }
        else
            file_name[j++] = url[i];
    }
    file_name[j] = '\0';
}

struct resp_header get_resp_header(const char *response)
{
    /*获取响应头的信息*/
    struct resp_header resp;

    char *pos = strstr(response, "HTTP/");
    if (pos)
        sscanf(pos, "%*s %d", &resp.status_code);//返回状态码

    pos = strstr(response, "Content-Type:");//返回内容类型
    if (pos)
        sscanf(pos, "%*s %s", resp.content_type);

    pos = strstr(response, "Content-Length:");//内容的长度(字节)
    if (pos)
        sscanf(pos, "%*s %ld", &resp.content_length);

    return resp;
}


char header[512] = {0};
char domain[64] = {0};
char file_name[100] = {0};
char headerbak[512] = {0};
volatile int headerbaklen = 0,isgetheardok = 0;
extern u8 scidcode[6];

char getFile(char * url,long start)
{
	int port = 80;
	INT8U err;
	unsigned long endpos = 0;
	
	//设置http请求头信息
  memset(header,0,512);
	memset(headerbak,0,512);
	memset(domain,0,64);
	memset(file_name,0,100);
	
	headerbaklen = 0;
	isgetheardok = 0;
	
		
	parse_url(url, domain, &port, file_name);
	if(start + 2047 < allFileWantLen)
	{
		endpos = start + 2047;
	}else
	{
		endpos = allFileWantLen - 1;
	}
    sprintf(header, \
            "GET /HTTPFile/%s HTTP/1.1\r\n"\
            "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
            "User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537(KHTML, like Gecko) Chrome/47.0.2526Safari/537.36\r\n"\
            "Host:%s\r\n"\
            "Connection:Keep-Alive\r\n"\
						"Range:bytes=%ld-%ld\r\n"\
            "\r\n"\
        ,
				file_name, domain,start,endpos);
	
	OSMutexPend(SemEthMac,0,&err);  //申请互斥资源使用
	if(Read_W5500_SOCK_1Byte(6,Sn_SR)!=SOCK_ESTABLISHED)//如果socket未建立连接
	{
		OSMutexPost(SemEthMac);
		return 1;
	}
	
	Write_SOCK_Data_Buffer(6,(unsigned char *)header,strlen(header));
	
	OSMutexPost(SemEthMac);
	
	return 0;
	
}


char getFileall(char * url,long start)
{
	int port = 80;
	INT8U err;
	
	//设置http请求头信息
  memset(header,0,512);
	memset(headerbak,0,512);
	memset(domain,0,64);
	memset(file_name,0,100);
	headerbaklen = 0;
	isgetheardok = 0;
		
	parse_url(url, domain, &port, file_name);
	
    sprintf(header, \
            "GET /HTTPFile/%s HTTP/1.1\r\n"\
            "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
            "User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537(KHTML, like Gecko) Chrome/47.0.2526Safari/537.36\r\n"\
            "Host:%s\r\n"\
            "Connection:Keep-Alive\r\n"\
            "\r\n"\
        ,
				file_name, domain);
	
	OSMutexPend(SemEthMac,0,&err);  //申请互斥资源使用
	if(Read_W5500_SOCK_1Byte(6,Sn_SR)!=SOCK_ESTABLISHED)//如果socket未建立连接
	{
		OSMutexPost(SemEthMac);
		return 1;
	}
	
	Write_SOCK_Data_Buffer(6,(unsigned char *)header,strlen(header));
	
	OSMutexPost(SemEthMac);
	
	return 0;
	
}



void handledatafile(u8 data)
{
	INT8U err;
	u32 addrflash;
	u32 crcdd = 0;
	
	if(isgetheardok == 0)
	{
		headerbak[headerbaklen++] = data;
		if(headerbaklen > sizeof(headerbak))
		{
			headerbaklen = 0;
		}
		if(headerbaklen > 4)
		{
			if(memcmp(headerbak+headerbaklen-4,"\r\n\r\n",4) == 0)
			{
				tmpfilelen = 0;
				if(isGetFileLen)
				{
					resp = get_resp_header(headerbak);
					if(resp.status_code == 200)
					{
						allFileWantLen = resp.content_length;
						isRecOK = 1;
						return;
					}
				}else
				{
					isgetheardok = 1;
					resp = get_resp_header(headerbak);
					if(resp.status_code == 404)
					{
						NVIC_SystemReset();
						while(1);
					}
					tmpfilelen = 0;
				}
				//allfilelen = 0;
				
			}
		}
	}else
	{
		tmpfiledata[tmpfilelen++] = data;
		allfilelen++;
		
		if(tmpfilelen == 512 || allfilelen == allFileWantLen)
		//if(tmpfilelen == 512)
		{
			
			
			OSMutexPend(SemFlashSpi,0,&err);  //申请互斥资源使用
			addrflash = PRG_BACK_START * 4096 + baoindex * 512;
			
			
			if(addrflash % 4096 == 0)
			{
				Erase_One_Sector(addrflash);
			}
			if(baoindex * 512 == 0x10000)
			{
				addrflash = addrflash;
			}
			
			baoindex ++;

			Flash_Write_Bytes(addrflash,(unsigned char *)tmpfiledata,512);
			Flash_Rd_Bytes(addrflash,filedataread,512);
			OSMutexPost(SemFlashSpi);  //互斥资源释放
			
			
			if(memcmp(tmpfiledata,filedataread,512) != 0)
			{
				NVIC_SystemReset();
				while(1);
			}
			
			if(allfilelen % 2048 == 0)
			{
				//getFile2((char *)recdatasvr,allfilelen);
				isRecOK = 1;
			}
			if(allfilelen == allFileWantLen)
			{
				memcpy((unsigned char *)tmpfiledata,&allfilelen,4);

				{
					tmpfiledata[8] = 0xAA;
					tmpfiledata[9] = 0xCC;
					tmpfiledata[10]=CalCheckSum((unsigned char *)tmpfiledata,10);
					WrBytesToAT24C64(UpdatePar,(unsigned char *)tmpfiledata,11);//
				}

						Write_W5500_SOCK_1Byte(6,Sn_CR,CLOSE);// 关闭端口,等待重新打开连接 
						isOnKMD = 0;
						NVIC_SystemReset();
						while(1);	
			}		
			tmpfilelen = 0;
		}
	}
	
	
	
}


