
	
extern void	SetCommModeSub(void);
extern void	SetConsumModeEnableSub(void);//�������ѷ�ʽ����λ
extern void	LookSysPatameter(void);//�쿴ϵͳ����,0-������,�׽����޶ʹ�õ������ţ��汾�ţ����Ѽ�¼������
extern void	SetSysDateTime(unsigned long);//��������ʱ��
extern void	SetConsumMode(unsigned long);//��������ģʽ
extern void	SetSysPrince(unsigned char,unsigned long);//���õ���
extern void	LookRecordDatas(unsigned long);//�쿴��������Ѽ�¼	
extern void	LookRecordNum(unsigned char Sort);//�쿴���Ѽ�¼��///
extern unsigned char	SelectDate(unsigned char * ptr);//ѡ����������
extern void	ChgInputToLedBuffer(unsigned char LedX,unsigned char * ptr,unsigned char Num);//��������ַ�ת��Ϊ��ʾ�ַ�
extern void	ChgUlongToLongDispBuffer(unsigned long,unsigned char *);
extern void	ReadSysParameter(unsigned char bbit);
extern void	SetMainCodeSub(void);//�ֶ�����վ���
extern void	SetConsumStatusCode(void);//�����ѻ�����

void writeSysParameterToFlash(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);



