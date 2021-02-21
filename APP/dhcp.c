#include  "cpu.h"
#include  "os_cpu.h"
#include  "app_cfg.h"
#include  "ucos_ii.h"
#include  "include.h"
//#include  "variable.h"
#include "stdio.h"
#include "w5500.h"
#include "dhcp.h"

#include  "include.h"
#include "stdio.h"
#include "cJSON.h"
#include "stdlib.h"

int32_t sendto(uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port);
#define DEVICE_ID "W5200"
//#define DHCP_DEBUG
extern u8 snedData[15+6+10];
extern unsigned char Gateway_IP[4];//网关IP地址 
extern unsigned char Sub_Mask[4];	//子网掩码 
extern unsigned char Phy_Addr[6];	//物理地址(MAC) 
extern unsigned char IP_Addr[4];	//本机IP地址 
unsigned char DNS_IP[4];
//unsigned char DNS_HOST_NAME[40];

uint8*  SRC_MAC_ADDR   =  snedData +1;    // Local MAC address
uint8*   GET_SN_MASK    = Sub_Mask;     // Subnet mask received from the DHCP server
uint8*   GET_GW_IP      = Gateway_IP;     // Gateway ip address received from the DHCP server
uint8*   GET_DNS_IP     = DNS_IP;    // DNS server ip address received from the DHCP server
//uint8*   DHCP_HOST_NAME = DNS_HOST_NAME;   // HOST NAME
uint8*   GET_SIP        = IP_Addr;    // Local ip address received from the DHCP server
uint8    DHCP_SIP[4] = {0,};      // DNS server ip address is discovered
uint8    DHCP_REAL_SIP[4] = {0,}; // For extract my DHCP server in a few DHCP servers
uint8    OLD_SIP[4];        // Previous local ip address received from DHCP server

uint8  dhcp_state       = STATE_DHCP_READY;          // DHCP client status
uint8  dhcp_retry_count = 0;    // retry count
uint8  DHCP_timeout     = 0;        // DHCP Timeout flag
uint32  dhcp_lease_time;    // Leased time
uint32  dhcp_time       = 0;
uint32  next_dhcp_time  = 0;      // DHCP Time 1s
uint32  dhcp_tick_cnt   = 0;                    // 1ms
uint8  DHCP_timer;

uint32  DHCP_XID        = DEFAULT_XID;				
//uint8 EXTERN_DHCPBUF[1024];
RIP_MSG EXTERN_DHCPBUF;
RIP_MSG*  pRIPMSG = &EXTERN_DHCPBUF;         // Pointer for the DHCP message


void  send_DHCP_DISCOVER(void);// Send the discovery message to the DHCP server
void  send_DHCP_REQUEST(void);// Send the request message to the DHCP server
void  send_DHCP_RELEASE_DECLINE(char msgtype);// send the release message to the DHCP server
uint8 parseDHCPMSG(uint16 length);// Receive the message from DHCP server and parse it.
void  reset_DHCP_time(void);        // Initialize DHCP Timer
void  DHCP_timer_handler(void);     // DHCP Timer handler
uint8 check_leasedIP(void);         // Check the leased IP address
void  check_DHCP_Timeout(void);     // Check DHCP Timeout 

void reset_DHCP_time(void)
{
	dhcp_time = 0;
	dhcp_tick_cnt = 0;
	next_dhcp_time = dhcp_time + DHCP_WAIT_TIME;
	dhcp_retry_count = 0;
}


uint16 swaps(uint16 i);

uint16 htons( 
	uint16 hostshort	/**< A 16-bit number in host byte order.  */
	)
{
	return swaps(hostshort);	
}

uint32 swapl(uint32 l)
{
  uint32 ret=0;
  ret = (l & 0xFF) << 24;
  ret |= ((l >> 8) & 0xFF) << 16;
  ret |= ((l >> 16) & 0xFF) << 8;
  ret |= ((l >> 24) & 0xFF);
  return ret;
}
uint16 swaps(uint16 i)
{
  uint16 ret=0;
  ret = (i & 0xFF) << 8;
  ret |= ((i >> 8)& 0xFF);
  return ret;	
}
unsigned long htonl(
	unsigned long hostlong		/**< hostshort  - A 32-bit number in host byte order.  */
	)
{
	return swapl(hostlong);
}

void send_DHCP_DISCOVER(void)
{
  uint8 ip[4] = {255,255,255,255};
  uint16 i=0;
  //the host name modified
  uint8 host_name[12];
	u16 siziofpip = sizeof(RIP_MSG);
  //*((uint32*)DHCP_SIP)=0;
  //*((uint32*)DHCP_REAL_SIP)=0;
  memset((char*)pRIPMSG,0,siziofpip);
 
  pRIPMSG->op = DHCP_BOOTREQUEST;
  pRIPMSG->htype = DHCP_HTYPE10MB;
  pRIPMSG->hlen = DHCP_HLENETHERNET;
  pRIPMSG->hops = DHCP_HOPS;
  pRIPMSG->xid = htonl(DHCP_XID);
  pRIPMSG->secs = htons(DHCP_SECS);
  pRIPMSG->flags =htons(DHCP_FLAGSBROADCAST);
  pRIPMSG->chaddr[0] = SRC_MAC_ADDR[0];
  pRIPMSG->chaddr[1] = SRC_MAC_ADDR[1];
  pRIPMSG->chaddr[2] = SRC_MAC_ADDR[2];
  pRIPMSG->chaddr[3] = SRC_MAC_ADDR[3];
  pRIPMSG->chaddr[4] = SRC_MAC_ADDR[4];
  pRIPMSG->chaddr[5] = SRC_MAC_ADDR[5];    
  
  
  /* MAGIC_COOKIE */
  pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 24)& 0xFF);
  pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 16)& 0xFF);
  pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 8)& 0xFF);
  pRIPMSG->OPT[i++] = (uint8)(MAGIC_COOKIE& 0xFF);
  
  /* Option Request Param. */
  pRIPMSG->OPT[i++] = dhcpMessageType;
  pRIPMSG->OPT[i++] = 0x01;
  pRIPMSG->OPT[i++] = DHCP_DISCOVER;
  
  // Client identifier
  pRIPMSG->OPT[i++] = dhcpClientIdentifier;
  pRIPMSG->OPT[i++] = 0x07;
  pRIPMSG->OPT[i++] = 0x01;
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[0];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[1];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[2];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[3];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[4];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[5];
  
  // host name
  pRIPMSG->OPT[i++] = hostName;
  // set the host name
  sprintf((char*)host_name,"%.4s-%02X%02X%02X",DEVICE_ID,SRC_MAC_ADDR[3],SRC_MAC_ADDR[4],SRC_MAC_ADDR[5]);
  
  
  pRIPMSG->OPT[i++] = (uint8)strlen((char*)host_name);
  
  strcpy((char*)(&(pRIPMSG->OPT[i])),(char*)host_name);
 
  
  i+=(uint16)strlen((char*)host_name);	
 
  pRIPMSG->OPT[i++] = dhcpParamRequest;
  pRIPMSG->OPT[i++] = 0x06;
  pRIPMSG->OPT[i++] = subnetMask;
  pRIPMSG->OPT[i++] = routersOnSubnet;
  pRIPMSG->OPT[i++] = dns;
  pRIPMSG->OPT[i++] = domainName;
  pRIPMSG->OPT[i++] = dhcpT1value;
  pRIPMSG->OPT[i++] = dhcpT2value;
  pRIPMSG->OPT[i++] = endOption;


  sendto(SOCK_DHCP, (uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);	

}


void send_DHCP_REQUEST(void)
{
  uint8  ip[4];
  uint16 i = 0;
  //uint16 len=0;
  
  uint8  host_name[12];

  memset((void*)pRIPMSG,0,sizeof(RIP_MSG));
  
  pRIPMSG->op = DHCP_BOOTREQUEST;
  pRIPMSG->htype = DHCP_HTYPE10MB;
  pRIPMSG->hlen = DHCP_HLENETHERNET;
  pRIPMSG->hops = DHCP_HOPS;
  pRIPMSG->xid = htonl(DHCP_XID);
  pRIPMSG->secs = htons(DHCP_SECS);
  
  if(dhcp_state < STATE_DHCP_LEASED)
    pRIPMSG->flags = htons(DHCP_FLAGSBROADCAST);
  else
  {
    pRIPMSG->flags = 0;		// For Unicast
    memcpy(pRIPMSG->ciaddr,GET_SIP,4);
  }		

  memcpy(pRIPMSG->chaddr,SRC_MAC_ADDR,6);
  
  /* MAGIC_COOKIE */
  pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 24) & 0xFF);
  pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 16) & 0xFF);
  pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 8) & 0xFF);
  pRIPMSG->OPT[i++] = (uint8)(MAGIC_COOKIE & 0xFF);
  
  /* Option Request Param. */
  pRIPMSG->OPT[i++] = dhcpMessageType;
  pRIPMSG->OPT[i++] = 0x01;
  pRIPMSG->OPT[i++] = DHCP_REQUEST;
  
  pRIPMSG->OPT[i++] = dhcpClientIdentifier;
  pRIPMSG->OPT[i++] = 0x07;
  pRIPMSG->OPT[i++] = 0x01;
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[0];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[1];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[2];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[3];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[4];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[5];	
  
  
  if(dhcp_state < STATE_DHCP_LEASED)
  {
    pRIPMSG->OPT[i++] = dhcpRequestedIPaddr;
    pRIPMSG->OPT[i++] = 0x04;
    pRIPMSG->OPT[i++] = GET_SIP[0];
    pRIPMSG->OPT[i++] = GET_SIP[1];
    pRIPMSG->OPT[i++] = GET_SIP[2];
    pRIPMSG->OPT[i++] = GET_SIP[3];
  
    pRIPMSG->OPT[i++] = dhcpServerIdentifier;
    pRIPMSG->OPT[i++] = 0x04;
    pRIPMSG->OPT[i++] = DHCP_SIP[0];
    pRIPMSG->OPT[i++] = DHCP_SIP[1];
    pRIPMSG->OPT[i++] = DHCP_SIP[2];
    pRIPMSG->OPT[i++] = DHCP_SIP[3];
  }
  
  // host name
  pRIPMSG->OPT[i++] = hostName;
  //set the host name
  sprintf((char*)host_name,(char*)"%.4s-%02X%02X%02X",DEVICE_ID,SRC_MAC_ADDR[3],SRC_MAC_ADDR[4],SRC_MAC_ADDR[5]);
  pRIPMSG->OPT[i++] = (uint8)strlen((char*)host_name);

  strcpy((char*)&(pRIPMSG->OPT[i]),(char*)host_name);

  i+=strlen((char*)host_name);
	
  
  pRIPMSG->OPT[i++] = dhcpParamRequest;
  pRIPMSG->OPT[i++] = 0x08;
  pRIPMSG->OPT[i++] = subnetMask;
  pRIPMSG->OPT[i++] = routersOnSubnet;
  pRIPMSG->OPT[i++] = dns;
  pRIPMSG->OPT[i++] = domainName;
  pRIPMSG->OPT[i++] = dhcpT1value;
  pRIPMSG->OPT[i++] = dhcpT2value;
  pRIPMSG->OPT[i++] = performRouterDiscovery;
  pRIPMSG->OPT[i++] = staticRoute;
  pRIPMSG->OPT[i++] = endOption;
  
  /* send broadcasting packet */
  if(dhcp_state < STATE_DHCP_LEASED) memset(ip,0xFF,4);
  else 
    memcpy(ip,DHCP_SIP,4);
  //printf("send dhcp request\r\n");
    sendto(SOCK_DHCP, (uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
  //if(len==0)
    //printf("send request error\r\n");
}

void send_DHCP_RELEASE_DECLINE(char msgtype)
{
  uint16 i =0;
  uint8  ip[4];
  
  memset((void*)pRIPMSG,0,sizeof(RIP_MSG));
  
  pRIPMSG->op = DHCP_BOOTREQUEST;
  pRIPMSG->htype = DHCP_HTYPE10MB;
  pRIPMSG->hlen = DHCP_HLENETHERNET;
  pRIPMSG->hops = DHCP_HOPS;
  pRIPMSG->xid = htonl(DHCP_XID);
  pRIPMSG->secs = htons(DHCP_SECS);
  pRIPMSG->flags =0;// DHCP_FLAGSBROADCAST;
  
  memcpy(pRIPMSG->chaddr,SRC_MAC_ADDR,6);
  
  /* MAGIC_COOKIE */
  pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 24) & 0xFF);
  pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 16) & 0xFF);
  pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 8) & 0xFF);
  pRIPMSG->OPT[i++] = (uint8)(MAGIC_COOKIE & 0xFF);
  
  /* Option Request Param. */
  pRIPMSG->OPT[i++] = dhcpMessageType;
  pRIPMSG->OPT[i++] = 0x01;
  pRIPMSG->OPT[i++] = ((!msgtype) ? DHCP_RELEASE : DHCP_DECLINE);
  
  pRIPMSG->OPT[i++] = dhcpClientIdentifier;
  pRIPMSG->OPT[i++] = 0x07;
  pRIPMSG->OPT[i++] = 0x01;
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[0];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[1];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[2];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[3];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[4];
  pRIPMSG->OPT[i++] = SRC_MAC_ADDR[5];	
  
  pRIPMSG->OPT[i++] = dhcpServerIdentifier;
  pRIPMSG->OPT[i++] = 0x04;
  pRIPMSG->OPT[i++] = DHCP_SIP[0];
  pRIPMSG->OPT[i++] = DHCP_SIP[1];
  pRIPMSG->OPT[i++] = DHCP_SIP[2];
  pRIPMSG->OPT[i++] = DHCP_SIP[3];
  
  if(msgtype)
  {
    pRIPMSG->OPT[i++] = dhcpRequestedIPaddr;
    pRIPMSG->OPT[i++] = 0x04;
    pRIPMSG->OPT[i++] = GET_SIP[0];
    pRIPMSG->OPT[i++] = GET_SIP[1];
    pRIPMSG->OPT[i++] = GET_SIP[2];
    pRIPMSG->OPT[i++] = GET_SIP[3];		
  }
  
  pRIPMSG->OPT[i++] = endOption;	
  
  
  if(msgtype) memset(ip,0xFF,4);
  else 
    memcpy(ip,DHCP_SIP,4);
  //printf("send dhcp decline\r\n");
  sendto(SOCK_DHCP, (uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
}

unsigned long ntohl(unsigned long netlong)
{
	return htonl(netlong);	
}

u8 bufferreadfrom[1024];
uint16 recvfrom(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16 *port)
{
	int size;
	
	size=Read_SOCK_Data_Buffer(s, bufferreadfrom);
	memcpy(addr,bufferreadfrom,4);
	*port    = bufferreadfrom[4];
  *port    = (*port << 8) + bufferreadfrom[5];
	
	memcpy(buf,bufferreadfrom+8,size-8);
	return size-8;
}


uint8 parseDHCPMSG(uint16 length)
{
  uint8  svr_addr[6];
  uint16 svr_port;
  uint16 len;
  uint8  * p;
  uint8  * e;
  uint8  type;
  uint8  opt_len = 0;
  
  len = recvfrom(SOCK_DHCP, (uint8 *)pRIPMSG, length, svr_addr, &svr_port);
  
  if(pRIPMSG->op != DHCP_BOOTREPLY || svr_port != DHCP_SERVER_PORT)
  {	
    return 0;
  }
  if(memcmp(pRIPMSG->chaddr,SRC_MAC_ADDR,6) != 0 || pRIPMSG->xid != htonl(DHCP_XID))
  {
    return 0;
  }
  
  if( *((uint32*)DHCP_SIP) != 0x00000000 )
  {
    if( *((uint32*)DHCP_REAL_SIP) != *((uint32*)svr_addr) && 
            *((uint32*)DHCP_SIP) != *((uint32*)svr_addr) ) 
    {		
      return 0;								
    }
  }
  
  memcpy(GET_SIP,pRIPMSG->yiaddr,4);
  
  type = 0;
  p = (uint8 *)(&pRIPMSG->op);
  p = p + 240;
  e = p + (len - 240);
 
  while ( p < e ) 
  {
    switch ( *p++ ) 
    {
    case endOption :
      return type;
              
      case padOption :
      break;
    case dhcpMessageType :
      opt_len = *p++;
      type = *p;	

      break;
    case subnetMask :
      opt_len =* p++;
      memcpy(GET_SN_MASK,p,4);

      break;
    case routersOnSubnet :
      opt_len = *p++;
      memcpy(GET_GW_IP,p,4);
		
      break;
    case dns :
      opt_len = *p++;
      memcpy(GET_DNS_IP,p,4);
      break;
    case dhcpIPaddrLeaseTime :
      opt_len = *p++;
      dhcp_lease_time = ntohl(*((uint32*)p));
		
      break;
    case dhcpServerIdentifier :
      opt_len = *p++;
			
      if( *((uint32*)DHCP_SIP) == 0 || 
          *((uint32*)DHCP_REAL_SIP) == *((uint32*)svr_addr) || 
          *((uint32*)DHCP_SIP) == *((uint32*)svr_addr) )
      {
        memcpy(DHCP_SIP,p,4);
        memcpy(DHCP_REAL_SIP,svr_addr,4);	// Copy the real ip address of my DHCP server					
      }
      else
      {		
      }
      break;
    default :
      opt_len = *p++;		
      break;
    } // switch
    p+=opt_len;
  } // while
  
  return 0;
}


/*
*********************************************************************************************************
*              CHECK DHCP STATE
*
* Description: This function checks the state of DHCP.
* Arguments  : None.
* Returns    : None.
* Note       : 
*********************************************************************************************************
*/

unsigned short Read_W5500_SOCK_2Byte(SOCKET s, unsigned short reg);


uint8 check_DHCP_state(SOCKET s) 
{
  uint16 len;
  uint8  type;
  
  type = 0;
  if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_CLOSED)
  {
    if ((len = Read_W5500_SOCK_2Byte(s,Sn_RX_RSR)) > 0)
    {
      type = parseDHCPMSG(len);
    }
  }
  else
  {
    if(dhcp_state == STATE_DHCP_READY)
    {	   	
    }
    
    Socket_Init(SOCK_DHCP);
  }
  
  switch ( dhcp_state )
  {
    case STATE_DHCP_READY:
      DHCP_timeout = 0;
      reset_DHCP_time();
      send_DHCP_DISCOVER();
      
      DHCP_timer = 0;//set_timer0(DHCP_timer_handler);   	
      dhcp_state = STATE_DHCP_DISCOVER;
      break;	   
    case STATE_DHCP_DISCOVER :
      if (type == DHCP_OFFER) 
      {
        reset_DHCP_time();
        send_DHCP_REQUEST();
        dhcp_state = STATE_DHCP_REQUEST;			
      }
      else 
        check_DHCP_Timeout();
      break;
    
    case STATE_DHCP_REQUEST :
      if (type == DHCP_ACK) 
      {
        reset_DHCP_time();
        if (check_leasedIP()) 
        {	
          dhcp_state = STATE_DHCP_LEASED;
          return DHCP_RET_UPDATE;
        } 
        else 
        {			
          dhcp_state = STATE_DHCP_DISCOVER;
          return DHCP_RET_CONFLICT;
        }
      }
      else if (type == DHCP_NAK) 
      {
        reset_DHCP_time();
        dhcp_state = STATE_DHCP_DISCOVER;	
      }
      else 
        check_DHCP_Timeout();
      break;
    
    case STATE_DHCP_LEASED :
      if ((dhcp_lease_time != 0xffffffff) && (dhcp_time>(dhcp_lease_time/2))) 
      {
        type = 0;
        memcpy(OLD_SIP,GET_SIP,4);
        DHCP_XID++;
        send_DHCP_REQUEST();
        dhcp_state = STATE_DHCP_REREQUEST;
        reset_DHCP_time();
      }
      break;
    
    case STATE_DHCP_REREQUEST :
      if (type == DHCP_ACK) 
      {
        if(memcmp(OLD_SIP,GET_SIP,4)!=0)	
        {
          return DHCP_RET_UPDATE;
        }
        reset_DHCP_time();
        dhcp_state = STATE_DHCP_LEASED;
      } 
      else if (type == DHCP_NAK) 
      {
        reset_DHCP_time();
        dhcp_state = STATE_DHCP_DISCOVER;
      } 
      else 
        check_DHCP_Timeout();
      break;
    default :
    dhcp_state = STATE_DHCP_READY;
    break;
  }
  
  if (DHCP_timeout == 1)
  {
    //kill_timer0(DHCP_timer);
    dhcp_state = STATE_DHCP_READY;
    return DHCP_RET_TIMEOUT;
  }
  return DHCP_RET_NONE;
}

/*
*********************************************************************************************************
*              CHECK TIMEOUT
*
* Description: This function checks the timeout of DHCP in each state.
* Arguments  : None.
* Returns    : None.
* Note       : 
*********************************************************************************************************
*/
void check_DHCP_Timeout(void)
{
  if (dhcp_retry_count < MAX_DHCP_RETRY) 
  {
    if (dhcp_time > next_dhcp_time) 
    {
      dhcp_time = 0;
      next_dhcp_time = dhcp_time + DHCP_WAIT_TIME;
      dhcp_retry_count++;
      switch ( dhcp_state ) 
      {
      case STATE_DHCP_DISCOVER :		
        send_DHCP_DISCOVER();
        break;
      case STATE_DHCP_REQUEST :			
        send_DHCP_REQUEST();
        break;

      case STATE_DHCP_REREQUEST :			
        send_DHCP_REQUEST();
        break;
      default :
        break;
      }
    }
  } 
  else 
  {
    reset_DHCP_time();
    DHCP_timeout = 1;
    
    send_DHCP_DISCOVER();
    dhcp_state = STATE_DHCP_DISCOVER;	
  }
}


/*
*********************************************************************************************************
* Description: check if a leased IP is valid
* Arguments  : None.
* Returns    : None.
* Note       : 
*********************************************************************************************************
*/
uint8 check_leasedIP(void)
{
  // sendto is complete. that means there is a node which has a same IP.
  
  if ( sendto(SOCK_DHCP, (uint8*)"CHECK_IP_CONFLICT", 17, GET_SIP, 5000))
  {
    send_DHCP_RELEASE_DECLINE(1);
    return 0;
  }
  return 1;

}	




