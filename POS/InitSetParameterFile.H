
	
extern void	SetCommModeSub(void);
extern void	SetConsumModeEnableSub(void);//设置消费方式允许位
extern void	LookSysPatameter(void);//察看系统参数,0-波特率,底金，日限额，使用的扇区号，版本号，消费记录条数等
extern void	SetSysDateTime(unsigned long);//设置日期时间
extern void	SetConsumMode(unsigned long);//设置消费模式
extern void	SetSysPrince(unsigned char,unsigned long);//设置单价
extern void	LookRecordDatas(unsigned long);//察看最近的消费记录	
extern void	LookRecordNum(unsigned char Sort);//察看消费记录条///
extern unsigned char	SelectDate(unsigned char * ptr);//选择消费日期
extern void	ChgInputToLedBuffer(unsigned char LedX,unsigned char * ptr,unsigned char Num);//将输入的字符转换为显示字符
extern void	ChgUlongToLongDispBuffer(unsigned long,unsigned char *);
extern void	ReadSysParameter(unsigned char bbit);
extern void	SetMainCodeSub(void);//手动设置站点号
extern void	SetConsumStatusCode(void);//设置脱机消费

void writeSysParameterToFlash(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);




