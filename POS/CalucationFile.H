#include "char.h"
#include "MyDefine.h"
		 
extern	uchar   	BytesCheckSum(uchar *,uchar);//Ð£ÑéºÍ±È½Ï
extern	uchar     BytesComp( uchar * ,uchar *,uchar);//×Ö·û´®±È½Ï£¬ÕýÈ··µ»Ø0
extern	uchar	    BCD_String_Diag(uchar * ,uchar);//BCDÂë×Ö·û´®Õï¶Ï
extern	uchar   BCDToHex(uchar  );
extern	uchar   HexToBCD(uchar  );
extern	ulong	ChgBCDStringToUlong(uchar * ,uchar);//BCDÂë×Ö·û´®×ª»»ÎªÕûÐÍÊý
extern	uchar	CalCheckSum(uchar * ,uchar);//¼ÆËãÐ£ÑéºÍ£
extern	void	ChgUlongToBCDString( ulong ,uchar * ,uchar );
extern	void	ChgIntToStrings(uint	ii,uchar * ptr);
extern	ulong	ChgInputToUlong(uchar * ,uchar);//ÊäÈëµÄÊý×Ö×ª»»Îª³¤ÕûÐÎ
extern	ulong 	GetU32_HiLo(uchar *);
extern	uint 	GetU16_HiLo(uchar *);
extern	void 	PutU32_HiLo(uchar *,ulong);
extern	void 	PutU16_HiLo(uchar *,uint);
extern	void	FormatBuffer(uchar,uchar * ,uchar *);
extern unsigned long	ChgKeyStringToUlong(uchar * ptr ,uchar Len);
extern ulong	ChgStringsToInt(uchar * ptr);
extern uint16_t DoubleBigToSmall(uint16_t a);
extern void HexGroupToHexString(uchar *data,uchar *dst,uchar len);
extern int HexStringToHexGroup(uchar *,uchar *,uint);
extern unsigned char AddQuFan(uchar *str,uchar len);//ÀÛ¼ÓºÍÈ¡·´



