#include "MyDefine.h"
#include "char.h"
#include "CalucationFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


unsigned long	ChgKeyStringToUlong(uchar * ptr ,uchar Len)//BCD码字符串转换为整型数
{
	unsigned long		ii=0;
	ulong		jj=1;
	uchar		aa;
	do
	{
		aa=(*(ptr+Len-1));
		ii|=aa*jj;
		jj=jj*0x100;
	}while(--Len);
	return	ii;
}

ulong	ChgStringsToInt(uchar * ptr)
{
	uchar	i;
	uint	kk=1;
	ulong	ii=0;
	uchar	st_data;
	for (i=0;i<5;i++)
	{
		st_data= ptr[4-i];
		ii+=(ulong)kk*st_data;	
		kk=kk*10;
	}	
	return	ii;
}

uchar BytesComp( uchar * CharDptr1,uchar * CharDptr2,uchar CompNum)
{
	uchar 			aa;
	uchar			bb;
	do
	{
		aa=* CharDptr1++;
		bb=* CharDptr2++;
		if (aa!=bb)	return 1; 
	}while (--CompNum);
	return 0;
}
uchar	BytesCheckSum(uchar * ptr,uchar Len)
{
	uchar			aa=0;
	uchar 			bb;
	Len--;
	do
	{
		aa+=* ptr++;
	}while (--Len);	 
	bb= * ptr;
	aa=~aa;
	if (aa!=bb) 
		return 1;
	else
		return 0;
}
uchar	CalCheckSum(uchar  * Ptr,uchar charLen)
{
	uchar 	st_data=0;
	do
	{
		st_data+= * Ptr++;
	}while (--charLen);
	st_data=~st_data;
	return st_data;
}
uchar	BCD_String_Diag(uchar * ptr ,uchar Len)//BCD码字符串诊断
{
	uchar			aa,bb;
	if (!Len)
		return	0;
	do
	{
		aa=* ptr++;
		bb=aa>>4;
		if (bb>9)
			return	1;
		bb=aa & 15;
		if (bb>9)
			return	1;
	}while (--Len);
	return	0;
}
ulong	ChgBCDStringToUlong(uchar * ptr ,uchar Len)//BCD码字符串转换为整型数
{
	ulong		ii=0;
	ulong		jj=1;
	uchar		aa;
	do
	{
		aa=BCDToHex(* (ptr+Len-1));
		ii+=aa*jj;
		jj=jj*100;		
	}while (--Len);
	return	ii;
}
void	ChgUlongToBCDString( ulong iii,uchar * ptr,uchar Len)
{
	uchar 		i;
	uchar 		aa;
	ulong 		jj=1;
	for (i=0;i<Len-1;i++)
		jj=jj*100;
 	for (i=0;i<Len;i++)
 	{
 		aa=iii/jj;
		* ptr++=HexToBCD(aa);
		iii=iii%jj;
		jj=jj/100;
 	}	
}

uchar HexToBCD(uchar aa )
{
	return((aa/10)*16+aa%10);
}
uchar BCDToHex(uchar aa )
{
	return((aa/16)*10+aa%16);
}


void	ChgIntToStrings(uint	ii,uchar * ptr)
{
	uint	kk=10000;
	uchar	st_data;
	uchar	i;
	for (i=0;i<5;i++)
	{
		st_data=ii/kk;
	//	ptr[i]=0x30+st_data;
		ptr[i]=st_data;
		ii=ii%kk;
		kk=kk/10;
	}	
}


ulong	ChgInputToUlong(uchar * ptr,uchar Num)//输入的数字转换为长整形
{
	uchar	i,st_data,j;
	ulong	iii=0;
	ulong	jjj=100;
	uchar	SumNum=0;
//	uchar	PointX=0xff;
	uchar		bbit=0;
	for (i=0;i<Num;i++)
	{
		st_data=ptr[i];
		if (st_data!=0xff )
		{
			SumNum++;
			if ((st_data&0x80))
			{
				bbit=1;
				for (j=0;j<i;j++)
					jjj=jjj*10;	
			}
		}
		else
			break;
	}
	if (!bbit)
	{
		for (i=0;i<SumNum-1;i++)
			jjj=jjj*10;
	}
	for (i=0;i<SumNum;i++)
	{
		iii+=(ptr[i]&0x0f)*jjj;
		jjj=jjj/10;
		if (!jjj)
			break;
	}
	return	iii;
}
void	FormatBuffer(uchar	SLen,uchar * ptr ,uchar * Len)
{
	uchar i;
	uchar	j=0;
	uchar	aaa[10];
	uchar		bbit=0;
	memset(aaa,0xff,SLen);
	for (i=0;i<SLen;i++)
	{
		if (ptr[i] || bbit)
		{
			bbit=1;
			aaa[j++]=ptr[i];
		}		
	}
	memcpy(ptr,aaa,SLen);
	Len[0]=j;		
}

//取四字节数据(高位在前)
ulong GetU32_HiLo(uchar * lbuf)
{
	uchar * p_buf;
	ulong  r_buf;
	
	p_buf = lbuf;
	r_buf = (ulong)p_buf[3] + ((ulong)p_buf[2]<<8) + ((ulong)p_buf[1]<<16) + ((ulong)p_buf[0]<<24);
	return r_buf;
} 

//取双字节数据(高位在前)
uint GetU16_HiLo(uchar * lbuf)
{
	uchar * p_buf;
	uint  r_buf;
	
	p_buf = lbuf;
	r_buf = (uint)p_buf[1] + ((uint)p_buf[0]<<8);
	return r_buf;
} 
 
//设置四字节数据(高位在前)
void PutU32_HiLo(uchar * lbuf,ulong ldata)
{
	lbuf[0]=(uchar)(ldata>>24);
	lbuf[1]=(uchar)(ldata>>16);
	lbuf[2]=(uchar)(ldata>>8);
	lbuf[3]=(uchar)(ldata);
} 

//设置双字节数据(高位在前)
void PutU16_HiLo(uchar * lbuf,uint ldata)
{	
	lbuf[0]=(uchar)(ldata>>8);
	lbuf[1]=(uchar)(ldata);
}

//双字节大小端转换
uint16_t DoubleBigToSmall(uint16_t a)
{
	uint16_t c;
	unsigned char b[2];
	b[0] =(unsigned char) (a);
	b[1] = (unsigned char)(a>>8);
	c = (uint16_t)(b[0]<<8) + b[1];
	return c;
}

//四字节大小端转换
int32_t FourBigToSmall(uint32_t a)
{
	uint32_t c;
	unsigned char b[4];
	b[0] =(unsigned char) (a);
	b[1] = (unsigned char)(a>>8);
	b[2] = (unsigned char)(a>>16);
	b[3] = (unsigned char)(a>>24);
	c = (uint32_t)(b[0]<<24) +(uint32_t)(b[1]<<16)+(uint32_t)(b[2]<<8)+ b[3];
	return c;		
}
//16进制数组转字符串
void HexGroupToHexString(uchar *data,uchar *dst,uchar len)
{
	uchar i;
	uchar str[250];
	//uchar *dst;
	for(i=0;i<len;i++)
	{
		 str[2*i] = data[i]>>4;
     str[2*i+1] = data[i]&0xf;
	}
	for(i=0;i<len*2;i++)
	{
		sprintf(&dst[i],"%X",str[i]);
	}
}
//16进制字符串转16进制数组
int HexStringToHexGroup(uchar *psrc,uchar *buf,uint len)
	{
    int i,n = 0;

		uchar dst[300];
		
		for(i=0;i<len;i++)
		{
			strcpy(dst,"0X");
			strncat(dst,psrc,2);
			buf[i]= strtol(dst,NULL,16);
			psrc+=2;
			//printf("%#X ",buf[i]);
		}
		
    return n;
}
//累计额和取反	
unsigned char AddQuFan(uchar *str,uchar len)
{
	int i;
	uchar sum  ;
  for(i=0;i<len;i++)
	{
		 sum += str[i];
	}
	sum = ~sum;
	return sum;
}

//浮点数转字符串
void FToStr1(float fl , char *str, uint8_t d)  // fl:???    d:????
{
    int     integer = 0;     //????
    float   decimal  = 0;    //????
    int     temp;
    uint8_t i, n;
    //????
    integer = (int)(fl);
    temp = integer;
    n = 0;
    do
    {
        n++;
        temp = (int)(temp / 10);  //???????? n
    }
    while(temp != 0);
 
    temp = integer;
    for( i = 0; i < n ; i++)
    {
        *(str + n - 1 - i) = (uint8_t)(temp % 10) + '0'; //????
        temp = (int)(temp / 10);
    }
    if( d == 0 )
    {
        *(str + i) = '\0';
    }
    else
    {
        *(str + i) = '.';
    }
    //????
    decimal = fl - integer;
    for(i = n + 1; i < d + n + 1 ; i++)
    {
        decimal = decimal * 10;
        *(str + i) = (uint8_t)(decimal) + '0';
        decimal = decimal - (uint8_t)(decimal);
    }
    *(str + i) = '\0';
}
 
