#include  "cpu.h"
#include  "os_cpu.h"
#include  "app_cfg.h"
#include  "ucos_ii.h"
#include  "include.h"
#include "w5500.h"
#include "descode.h"
#include "dhcp.h"
#include "cJSON.h"
#include "stdbool.h"

extern OS_EVENT *SemHandleETH; //��̫���ж��ź���
extern OS_EVENT *SemEthMac;  //�����ź���

extern int svraddrport;
void Handle_Eth_data(u8 *data,uint16 len);
u8 filebufferdata[20+30];
u8 filebufferdata2[20+30];
u8 dataupdafdf[512];
void handledatafile(u8 data);
void handleFUDP(u8 data);
void Write_W5500_SOCK_4Byte(SOCKET s, unsigned short reg, unsigned char *dat_ptr);
extern unsigned char S5_DIP[4];	//�˿�0Ŀ��IP��ַ ;
int S5_DPort;
void Write_W5500_SOCK_2Byte(SOCKET s, unsigned short reg, unsigned short dat);
int8_t parseDNSMSG(struct dhdr * pdhdr, uint8_t * pbuf, uint8_t * ip_from_dns);
uint8_t Domain_IP[4]  = {0};
extern u8 isgetdns;
extern u8 svraddrip[4];
extern u8 snedData[15+6];// = {0x02,0x00,0x00,0x12,0x27,0x24,0x17,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
void W5500_Init_DHCP(void);
void W5500_NVIC_Configuration_DIASABNLE(void);
uint8 check_DHCP_state(SOCKET s) ;
long dtatlen = 0;
unsigned short Only_Read_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr);
ulong  BatchNum;
uchar tokenBuf[50];

/*********************************************************
//�����������
*********************************************************/

static uint analyzePostdata(char *buf,char *Json_data)
{
 char *p = buf;
 int tk,i;
 
 if(strlen(p)<15)
 {
  return 0;
 }
// p = memchr(p,'\r',strlen(p));
 tk = memcmp(p, "HTTP/1.1 200" ,12);
 if( tk != 0)
 {
  return 0;
 }
 for(i=0;i<strlen(p);i++)
 {
  //if((p[i] == '\r')&p[i+1]=='\n'&p[i+2]=='\r'&p[i+3]=='\n')
	 if(p[i] =='{')
  {
		//memcpy(Json_data,p+i+4,(strlen(p)-i-4));
		memcpy(Json_data,p+i,(strlen(p)-i));

		break;
  }
 }
 return 200;
}

void Handle_ETH_DATA(void* p_arg)
{
	unsigned char i,j,err;
	u8 k = 0;
	int size,d,size2;
	u8 Buffer[20];
	int svraddrlen = 0;
	int ret;
	struct dhdr dhp;
	u8 dhcpret;
	
	while(1)
	{
		OSSemPend(SemHandleETH,0,&err);
		OSMutexPend(SemEthMac,0,&err);  //���뻥����Դʹ��
		
		while(1)
		{
			W5500_Interrupt=0;//�����жϱ�־
			i = Read_W5500_1Byte(IR);//��ȡ�жϱ�־�Ĵ���
			Write_W5500_1Byte(IR, (i&0xf0));//��д����жϱ�־

			if((i & CONFLICT) == CONFLICT)//IP��ַ��ͻ�쳣����
			{
				 //�Լ���Ӵ���
			}

			if((i & UNREACH) == UNREACH)//UDPģʽ�µ�ַ�޷������쳣����
			{
				//�Լ���Ӵ���
			}

			i=Read_W5500_1Byte(SIR);//��ȡ�˿��жϱ�־�Ĵ���	
			for(k=0;k<8;k++)
			{
				if((i & (1<<k)) == (1<<k))
				{
					j=Read_W5500_SOCK_1Byte(k,Sn_IR);//��ȡSocket0�жϱ�־�Ĵ���
					Write_W5500_SOCK_1Byte(k,Sn_IR,j);
					
					if(j&IR_DISCON)//��TCPģʽ��Socket�Ͽ����Ӵ���
					{
						Write_W5500_SOCK_1Byte(k,Sn_CR,CLOSE);//�رն˿�,�ȴ����´����� 
						Socket_Init(k);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
					}
					if(j&IR_SEND_OK)//Socket0���ݷ������,�����ٴ�����S_tx_process()������������ 
					{
					}
					if(j&IR_RECV)//Socket���յ�����,��������S_rx_process()���� 
					{
						switch(k)
						{
							case 0x00:
							case 0x01:
							case 0x02:
							case 0x03:
								break;
							case 0x04:
								size=Read_SOCK_Data_Buffer(4, filebufferdata);
								ret = parseDNSMSG(&dhp, filebufferdata+8, Domain_IP);
							
								if(ret == 1)
								{
									memcpy(svraddrip,Domain_IP,4);
									
									isgetdns = 1;
								}
													
								break;
							case 0x05:
//								size=Read_SOCK_Data_Buffer(5, dataupdafdf);
//								/* Extract Destination IP */
//								S5_DIP[0]=dataupdafdf[0];
//								S5_DIP[1]=dataupdafdf[1];
//								S5_DIP[2]=dataupdafdf[2];
//								S5_DIP[3]=dataupdafdf[3];
//								Write_W5500_SOCK_4Byte(5, Sn_DIPR, S5_DIP);

//								/* Extract Destination Port Number */
//								S5_DPort=dataupdafdf[4]*0x100+dataupdafdf[5];
//								Write_W5500_SOCK_2Byte(5, Sn_DPORTR, S5_DPort);
//									
//								if(!BytesCheckSum(dataupdafdf+8,size-8))
//								{
//								
//									Buffer[0]=0xa0;
//									memcpy(Buffer+1,dataupdafdf+8,6);
//									Buffer[7]=CalCheckSum(Buffer,7);
//									WrBytesToAT24C64(DeviceID,Buffer,8);//
//								
//									Buffer[0]=0xa0;
//									memcpy(Buffer+1,dataupdafdf+14,2);
//									Buffer[3]=CalCheckSum(Buffer,3);
//									WrBytesToAT24C64(svrPort,Buffer,4);//
//									
//									svraddrlen = dataupdafdf[16];
//									
//									Buffer[0]=0xa0;
//									memcpy(Buffer+1,dataupdafdf+16,svraddrlen+1);
//									Buffer[svraddrlen+2]=CalCheckSum(Buffer,svraddrlen+2);
//									WrBytesToAT24C64(svraddr,Buffer,svraddrlen+3);//
//									
//									Write_SOCK_Data_Buffer(5,snedData+1,6);
//								}
															
								break;
							case 0x06:
								while(1)
								{
									size=Only_Read_SOCK_Data_Buffer(6, filebufferdata);
									size2 = Only_Read_SOCK_Data_Buffer(6, filebufferdata2);
									if(size == size2 && (memcmp(filebufferdata,filebufferdata2,size) == 0))
									{
										size = Read_SOCK_Data_Buffer(6, filebufferdata);
										break;
									}
								}
								dtatlen += size;
								for(d=0;d<size;d++)
								{
									handledatafile(filebufferdata[d]);
								}
								break;
							case 0x07:
								size=Read_SOCK_Data_Buffer(7, Rx_Buffer);			
								Handle_Eth_data(Rx_Buffer,size);
							//	Write_SOCK_Data_Buffer(5,Rx_Buffer,size);
								//S0_Data |= S_RECEIVE;
								break;
						}
					}
					if(j&IR_TIMEOUT)//Socket���ӻ����ݴ��䳬ʱ���� 
					{
						Write_W5500_SOCK_1Byte(k,Sn_CR,CLOSE);// �رն˿�,�ȴ����´����� 
						Socket_Init(k);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
					}
				}
			}		
			if(Read_W5500_1Byte(SIR) == 0) break; 
		}
		OSMutexPost(SemEthMac);		
	}
}

int RecCase,getDataLength;

char sendTempcnt = 0;
int wantRecDataCmd = 0;

u8 wantRecDataData[1000];
int wantRecDataLenght = 0;

char keyDes[8] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38};
void UDP_Write_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr, unsigned short size);

extern u8 isReg;
extern u8 snedData[15+6];
void tcp_send_data_packet(u8 status,int cmd,u8 * data,int sendLength,int entype);
u8 scidcode[6];
extern  u8 usesectornum;
u8 isstartupdate = 0;
void startupdate();

void HandleCMD(u8 * data,u16 cmd)
{
	u8 status = -1,Buffer[10];
	
	switch(cmd)
	{
		case 0x1002:  //����������
			Buffer[0]=0xa0;
			Buffer[1]=BCDToHex(*data);
			Buffer[2]=CalCheckSum(Buffer,2);
			WrBytesToAT24C64(usesectorNum_Addr,Buffer,3);//
			status = 0;
			usesectornum = Buffer[1];
			break;
		case 0x1004: //�����û�id
			Buffer[0]=0xa0;
			memcpy(Buffer+1,data,6);
			Buffer[7]=CalCheckSum(Buffer,7);
			WrBytesToAT24C64(useUsrID,Buffer,8);//
			status = 0;
			memcpy(scidcode,data,6);
		
			break;
		case 0x1006: //�����ѻ��޶�
			Buffer[0]=0xa0;
			memcpy(Buffer+1,data,4);
			Buffer[5]=CalCheckSum(Buffer,5);
			WrBytesToAT24C64(LimitedMoney,Buffer,6);//
			memcpy(LimtConsumeBuf,Buffer+1,4);
			status = 0;
			break;
		case 0x1008:  //Զ�̿�ʼ����
			if(isstartupdate == 0)
			{
				isstartupdate = 1;
				startupdate();
			}
			status = 0;
			
			break;
	}
	
	tcp_send_data_packet(status,cmd,data,0,1);
}

void Handle_Eth_data(u8 *data,uint16 len)
{
	memcpy(wantRecDataData,data,len);
	wantRecDataLenght = len;
	wantRecDataCmd = 0;
}
/***************************************************
//��ȡTOKEN
***************************************************/
static void HttpGetToken(void)
{
	char	*out;
	cJSON *root;
	uchar end[2];
	uchar buf[100];
	uchar buf1[100];
	u8 sendTempp[500]={0};
	uint totalsize = 0,request_len = 0,header_len = 0;
	
	root=cJSON_CreateObject();	

	//��ӽӿ�����
	cJSON_AddItemToObject(root,"iterfaceName",cJSON_CreateString("token"));	
	//վ���	
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"dev_id",cJSON_CreateString(buf));
	//�豸���
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"deviceSerial",cJSON_CreateString(buf));	
	//�ͻ�ID
	HexGroupToHexString(customerId,buf,4);
	cJSON_AddItemToObject(root,"customerId",cJSON_CreateString(buf));
	//�ͻ���Կ
	HexGroupToHexString(customerkey,buf,20);
	cJSON_AddItemToObject(root,"customerPwd",cJSON_CreateString(buf));
	
	out=cJSON_PrintUnformatted(root);	
	request_len = strlen(out);
	memcpy(sendTempp,out,request_len);
	end[0] = 0x0d;
	end[1] = 0x0a;
	strcat(sendTempp,end);
	request_len +=2;
//	sprintf(buf1,"%d.%d.%.d.%d:%d",S1_DIP[0],S1_DIP[1],S1_DIP[2],S1_DIP[3],svraddrport);
//	header_len = snprintf(header,1024,HTTP_HEAD,buf1,request_len,NULL);
//	strcat(header,out);
	cJSON_Delete(root);	
	free(out);	/* Print to text, Delete the cJSON, print it, release the string. */
	//request_len = strlen(header);

	Write_SOCK_Data_Buffer(7,sendTempp,request_len);
	Write_SOCK_Data_Buffer(5,sendTempp,request_len);
}

/***************************************************
//����
***************************************************/
static void HttpCheckTime(void)
{
	char	*out;
	cJSON *root;
	uchar buf[100];
	uchar buf1[100];
	uchar end[2];
	uchar sendTempp[500]={0};
	uint totalsize = 0,request_len = 0,header_len = 0;
	
	root=cJSON_CreateObject();	
	
	//��ӽӿ�����
	cJSON_AddItemToObject(root,"iterfaceName",cJSON_CreateString("deviceHeartbeat"));	
	//���token
	cJSON_AddItemToObject(root,"Token",cJSON_CreateString(tokenBuf));	
	//վ���	
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"dev_id",cJSON_CreateString(buf));	
	//�豸���
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"deviceSerial",cJSON_CreateString(buf));
	//�ͻ�ID
	HexGroupToHexString(customerId,buf,4);
	cJSON_AddItemToObject(root,"customerId",cJSON_CreateString(buf));
	//�豸����
	HexGroupToHexString(&posStyle,buf,1);
	cJSON_AddItemToObject(root,"deviceType",cJSON_CreateString(buf));
	//�̼��汾��
	HexGroupToHexString(posVersion,buf,10);
	cJSON_AddItemToObject(root,"deviseVersion",cJSON_CreateString(buf));
	
	out=cJSON_PrintUnformatted(root);	
	request_len = strlen(out);

	memcpy(sendTempp,out,request_len);
	end[0] = 0x0d;
	end[1] = 0x0a;
	strcat(sendTempp,end);
	request_len +=2;
//	sprintf(buf1,"%d.%d.%.d.%d:%d",S1_DIP[0],S1_DIP[1],S1_DIP[2],S1_DIP[3],svraddrport);
//	header_len = snprintf(header,1024,HTTP_GETTIME_HEAD,buf1,request_len);
	//strcat(header,out);
	cJSON_Delete(root);	
	free(out);	/* Print to text, Delete the cJSON, print it, release the string. */
	//request_len = strlen(header);

	Write_SOCK_Data_Buffer(7,sendTempp,request_len);
	//Write_SOCK_Data_Buffer(5,sendTempp,request_len);
}

/***************************************************
//��ȡ���
***************************************************/
static void HttpGetBalance(void)
{
	char	*out;
	cJSON *root;
	uchar buf[100];
	uchar buf1[100];
	uchar end[2];
	u8 sendTempp[500]={0};
	uint totalsize = 0,request_len = 0,header_len = 0;
	
	root=cJSON_CreateObject();	
	
	//��ӽӿ�����
	cJSON_AddItemToObject(root,"iterfaceName",cJSON_CreateString("getBalance"));	
	//���token
	cJSON_AddItemToObject(root,"Token",cJSON_CreateString(tokenBuf));	
	//վ���	
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"dev_id",cJSON_CreateString(buf));	
	//�豸���
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"deviceSerial",cJSON_CreateString(buf));
	//�ͻ�ID
	HexGroupToHexString(customerId,buf,4);
	cJSON_AddItemToObject(root,"customerId",cJSON_CreateString(buf));
	//����
	HexGroupToHexString(CardSerialNum,buf,4);
	cJSON_AddItemToObject(root,"cardId",cJSON_CreateString(buf));
	
	out=cJSON_PrintUnformatted(root);	
	request_len = strlen(out);

	memcpy(sendTempp,out,request_len);
	end[0] = 0x0d;
	end[1] = 0x0a;
	strcat(sendTempp,end);
	request_len +=2;
//	sprintf(buf1,"%d.%d.%.d.%d:%d",S1_DIP[0],S1_DIP[1],S1_DIP[2],S1_DIP[3],svraddrport);
//	header_len = snprintf(header,1024,HTTP_GETBALANCE_HEAD,buf1,request_len);
	//strcat(header,out);
	cJSON_Delete(root);	
	free(out);	/* Print to text, Delete the cJSON, print it, release the string. */
	//request_len = strlen(header);

	Write_SOCK_Data_Buffer(7,sendTempp,request_len);
	//Write_SOCK_Data_Buffer(5,sendTempp,request_len);
}

/***************************************************
//���ѽӿ�
***************************************************/
static void HttpGetConsume(void)
{
	char	*out;
	cJSON *root;
	uchar buf[100];
	uchar buf1[100];
	uchar end[2];
	u8 sendTempp[500]={0};
	uint totalsize = 0,request_len = 0,header_len = 0;
	
	root=cJSON_CreateObject();
	//��ӽӿ�����
	cJSON_AddItemToObject(root,"iterfaceName",cJSON_CreateString("getConsume"));		
	//���token
	cJSON_AddItemToObject(root,"Token",cJSON_CreateString(tokenBuf));	
	//վ���	
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"dev_id",cJSON_CreateString(buf));	
	//�豸���
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"deviceSerial",cJSON_CreateString(buf));
	//�ͻ�ID
	HexGroupToHexString(customerId,buf,4);
	cJSON_AddItemToObject(root,"customerId",cJSON_CreateString(buf));
	//����ʱ��
	Read_Sysdate(SysTimeDatas.TimeString);
	buf1[0] = 0x20;
	memcpy(buf1+1,SysTimeDatas.TimeString,6);
	HexGroupToHexString(buf1,buf,7);
	cJSON_AddItemToObject(root,"time",cJSON_CreateString(buf));
	//��ˮ��
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	ChgUlongToBCDString(BatchNum,Tx_Buffer+2,4);
	HexGroupToHexString(Tx_Buffer,buf1,12);
	memcpy(Tx_Buffer,buf1,4);
	Tx_Buffer[4] = '-';
	memcpy(Tx_Buffer+5,buf,14);
	memcpy(Tx_Buffer+19,buf1+4,8);
	
	cJSON_AddItemToObject(root,"orderNo",cJSON_CreateString(Tx_Buffer));
	//���׽��
	memset(buf,0,20);
	ChgUlongToBCDString(CurrentConsumMoney,Tx_Buffer,4);
	HexGroupToHexString(Tx_Buffer,buf,4);
	cJSON_AddItemToObject(root,"conAmount",cJSON_CreateString(buf));
	//����
	HexGroupToHexString(CardSerialNum,buf,4);
	cJSON_AddItemToObject(root,"cardId",cJSON_CreateString(buf));
	//�������ģʽ�ӿ�
	if(ConsumMode==CONSUM_RATION)
		cJSON_AddItemToObject(root,"conType",cJSON_CreateString("2"));	
	else
		cJSON_AddItemToObject(root,"conType",cJSON_CreateString("1"));	
	out=cJSON_PrintUnformatted(root);	
	request_len = strlen(out);
	memcpy(sendTempp,out,request_len);
	end[0] = 0x0d;
	end[1] = 0x0a;

	strcat(sendTempp,end);
	request_len +=2;
//	sprintf(buf1,"%d.%d.%.d.%d:%d",S1_DIP[0],S1_DIP[1],S1_DIP[2],S1_DIP[3],svraddrport);
//	header_len = snprintf(header,1024,HTTP_GETCONSUME_HEAD,buf1,request_len);
//	strcat(header,out);
	cJSON_Delete(root);	
	free(out);	/* Print to text, Delete the cJSON, print it, release the string. */
	//request_len = strlen(header);

	Write_SOCK_Data_Buffer(7,sendTempp,request_len);
//	Write_SOCK_Data_Buffer(5,sendTempp,request_len);
}

/***************************************************
//�ϴ��ѻ���¼
***************************************************/
static void HttpUpRecord(uint8_t *data)
{
	char	*out;
	cJSON *root;
	uchar buf[100];
	uchar buf1[100];
	uchar end[2];
	u8 sendTempp[500]={0};
	uint totalsize = 0,request_len = 0,header_len = 0;
	
	root=cJSON_CreateObject();
	//��ӽӿ�����
	cJSON_AddItemToObject(root,"iterfaceName",cJSON_CreateString("offlineRecord"));		
	//���token
	cJSON_AddItemToObject(root,"Token",cJSON_CreateString(tokenBuf));	
	//վ���	
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"dev_id",cJSON_CreateString(buf));	
	//�豸���
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"deviceSerial",cJSON_CreateString(buf));
	//�ͻ�ID
	HexGroupToHexString(customerId,buf,4);
	cJSON_AddItemToObject(root,"customerId",cJSON_CreateString(buf));
	//����ʱ��
	//Read_Sysdate(SysTimeDatas.TimeString);
	buf1[0] = 0x20;
	memcpy(buf1+1,data+9,6);
	HexGroupToHexString(buf1,buf,7);
	cJSON_AddItemToObject(root,"time",cJSON_CreateString(buf));
	//��ˮ��
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	BatchNum = *(volatile u32*)(&data +15);
	ChgUlongToBCDString(BatchNum,Tx_Buffer+2,4);
	HexGroupToHexString(Tx_Buffer,buf1,12);
	memcpy(Tx_Buffer,buf1,4);
	Tx_Buffer[4] = '-';
	memcpy(Tx_Buffer+5,buf,14);
	memcpy(Tx_Buffer+19,buf1+4,8);
	
	cJSON_AddItemToObject(root,"orderNo",cJSON_CreateString(Tx_Buffer));
	//���׽��
	memset(buf,0,20);
	
	//ChgUlongToBCDString(CurrentConsumMoney,Tx_Buffer,4);
	memcpy(Tx_Buffer,data+5,4);
	HexGroupToHexString(Tx_Buffer,buf,4);
	cJSON_AddItemToObject(root,"conAmount",cJSON_CreateString(buf));
	//����
	memcpy(CardSerialNum,data+1,4);
	HexGroupToHexString(CardSerialNum,buf,4);
	cJSON_AddItemToObject(root,"cardId",cJSON_CreateString(buf));
	
	out=cJSON_PrintUnformatted(root);	
	request_len = strlen(out);
	memcpy(sendTempp,out,request_len);
	end[0] = 0x0d;
	end[1] = 0x0a;

	strcat(sendTempp,end);
	request_len +=2;
//	sprintf(buf1,"%d.%d.%.d.%d:%d",S1_DIP[0],S1_DIP[1],S1_DIP[2],S1_DIP[3],svraddrport);
//	header_len = snprintf(header,1024,HTTP_GETCONSUME_HEAD,buf1,request_len);
//	strcat(header,out);
	cJSON_Delete(root);	
	free(out);	/* Print to text, Delete the cJSON, print it, release the string. */
	//request_len = strlen(header);

	Write_SOCK_Data_Buffer(7,sendTempp,request_len);
//	Write_SOCK_Data_Buffer(5,sendTempp,request_len);
}

/***************************************************
//��ѯ�����ܶ�
***************************************************/
static void QueryConsumeMoney(uint8_t *data)
{
	char	*out;
	cJSON *root;
	uchar buf[100];
	uchar buf1[100];
	uchar end[2];
	u8 sendTempp[500]={0};
	uint totalsize = 0,request_len = 0,header_len = 0;
	
	root=cJSON_CreateObject();
	//��ӽӿ�����
	cJSON_AddItemToObject(root,"iterfaceName",cJSON_CreateString("getConsumeToalMoney"));		
	//���token
	cJSON_AddItemToObject(root,"Token",cJSON_CreateString(tokenBuf));	
	//վ���	
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"dev_id",cJSON_CreateString(buf));	
	//�豸���
	ChgUlongToBCDString(MainCode,Tx_Buffer,2);
	HexGroupToHexString(Tx_Buffer,buf,2);
	cJSON_AddItemToObject(root,"deviceSerial",cJSON_CreateString(buf));
	//�ͻ�ID
	HexGroupToHexString(customerId,buf,4);
	cJSON_AddItemToObject(root,"customerId",cJSON_CreateString(buf));
	//��ʼ����
	//Read_Sysdate(SysTimeDatas.TimeString);
	buf1[0] = 0x20;
	buf1[1] = SysTimeDatas.S_Time.YearChar;
	memcpy(buf1+2,data,2);
	HexGroupToHexString(buf1,buf,4);
	cJSON_AddItemToObject(root,"startTime",cJSON_CreateString(buf));
	//��ֹ����
	buf1[0] = 0x20;
	buf1[1] = SysTimeDatas.S_Time.YearChar;
	memcpy(buf1+2,data+2,2);
	HexGroupToHexString(buf1,buf,4);
	cJSON_AddItemToObject(root,"endTime",cJSON_CreateString(buf));

	out=cJSON_PrintUnformatted(root);	
	request_len = strlen(out);
	memcpy(sendTempp,out,request_len);
	end[0] = 0x0d;
	end[1] = 0x0a;

	strcat(sendTempp,end);
	request_len +=2;

	cJSON_Delete(root);	
	free(out);	/* Print to text, Delete the cJSON, print it, release the string. */
	//request_len = strlen(header);

	Write_SOCK_Data_Buffer(7,sendTempp,request_len);
	//Write_SOCK_Data_Buffer(5,sendTempp,request_len);
}
static void tcp_send_data_packet(u8 status,int cmd,u8 * data,int sendLength,int entype)
{
	int sendLengthtrue;
	
	switch (cmd)
	{
		case GetTokenCmd://token
			HttpGetToken();
		break;
		
		case HeartBeatCmd://����
			HttpCheckTime();
		break;
		
		case GetBalanceCmd://��ȡ���
			HttpGetBalance();
		break;
		
		case ConsumeCmd://���ѽӿ�
			HttpGetConsume();
		break;
		
		case UpRecodCmd://�ϴ��ѻ���¼
			HttpUpRecord(data);
		break;
		
		case QueryToalConsumeMoney://��ѯ�����ܶ�
			QueryConsumeMoney(data);
		break;
	}
}

extern u8 wantRecDataData[1000];
extern int wantRecDataLenght;

extern OS_EVENT *SemEthMacSENDREC;  //�����ź���

int tcp_send_and_rec_packet(int cmd,u8 * data,int sendLength,int entType,u8 * dataRec,int * length,int timeout)
{
	u8 err;

//	Write_W5500_SOCK_1Byte(7,Sn_CR,CLOSE);// �رն˿�,�ȴ����´����� 
//	Socket_Init(7);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0	
	//Socket_Connect(7);
	OSMutexPend(SemEthMacSENDREC,0,&err);  //���뻥����Դʹ��
	
	wantRecDataCmd = cmd;
	
	OSMutexPend(SemEthMac,0,&err);  //���뻥����Դʹ��
	tcp_send_data_packet(0,cmd,data,sendLength,entType);
	OSMutexPost(SemEthMac);
	
	timeout*=100;
	while(--timeout)
	{
		OSTimeDlyHMSM(0, 0, 0, 10);//
		if(wantRecDataCmd == 0)
		{
			break;
		}
	}	
	if(timeout != 0)
	{
		memcpy(dataRec,wantRecDataData,wantRecDataLenght);
	//	analyzePostdata(wantRecDataData,dataRec);
		//Write_SOCK_Data_Buffer(5,wantRecDataData,strlen(wantRecDataData));
		*length = strlen(dataRec);
		//Write_SOCK_Data_Buffer(5,dataRec,*length);
		OSMutexPost(SemEthMacSENDREC);
		return 0;
	}else 
	{
		OSMutexPost(SemEthMacSENDREC);
		return -1;
	}	
}

