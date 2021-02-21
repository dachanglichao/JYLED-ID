#include "485.H"
#include "GPIO.H"
#include "USART.H"

//���ڵ�4052ѡ��ܽ�����
#define 	COM_SEL0_GPIO     		PE
#define 	COM_SEL0_BIT      		GPIO_Pin_15
#define 	COM_SEL0_Type_Out     	Out_PP

#define 	COM_SEL1_GPIO     		PE
#define 	COM_SEL1_BIT      		GPIO_Pin_14
#define 	COM_SEL1_Type_Out     	Out_PP

#define 	COM1_SEL0_GPIO     		PE
#define 	COM1_SEL0_BIT      		GPIO_Pin_13
#define 	COM1_SEL0_Type_Out     	Out_PP

#define 	COM1_SEL1_GPIO     		PE
#define 	COM1_SEL1_BIT      		GPIO_Pin_12
#define 	COM1_SEL1_Type_Out     	Out_PP
 
//IO��������
#define COM_SEL0_OUT()  DK_GPIO_Init(COM_SEL0_GPIO,COM_SEL0_BIT,COM_SEL0_Type_Out);
#define COM_SEL1_OUT()  DK_GPIO_Init(COM_SEL1_GPIO,COM_SEL1_BIT,COM_SEL1_Type_Out);
#define COM1_SEL0_OUT() DK_GPIO_Init(COM1_SEL0_GPIO,COM1_SEL0_BIT,COM1_SEL0_Type_Out);
#define COM1_SEL1_OUT() DK_GPIO_Init(COM1_SEL1_GPIO,COM1_SEL1_BIT,COM1_SEL1_Type_Out);
//IO��������	 
#define COM_SEL0    PEout(15) //
#define COM_SEL1    PEout(14) //
#define COM1_SEL0    PEout(13) //
#define COM1_SEL1    PEout(12) //

/***********************************************************************
*����������
*�������ܣ�
*************************************************************************/
void u485_GPIO_INT(void)
{
	u485_OUT  ;
	u485_RE;
}
//���ڵ�4052ѡ��ܽų�ʼ��
void USART_COMSEL_GPIO_INIT(void)
{
	COM_SEL0_OUT();
	COM_SEL1_OUT();
	COM1_SEL0_OUT();
	COM1_SEL1_OUT();
}
//485ģʽ4052ѡ��
void COMSEL_485Mode(void)
{
	COM_SEL0 = 0;
	COM_SEL1 = 0;	
}
//232ģʽ4052ѡ��
void COMSEL_232Mode(void)
{
	COM_SEL0 = 1;
	COM_SEL1 = 0;	
}
//����3����
void COMSEL_NoMode(void)
{
	COM_SEL0 = 1;
	COM_SEL1 = 1;	
}
//232ģʽ4052ѡ��
void SelWifiMode(void)
{
	COM_SEL0 = 0;
	COM_SEL1 = 1;	
}
 //����2�����ڴ�ӡ��ģʽ
 void Usart2ToPrint(void)
 {
 	COM1_SEL0 = 0;
	COM1_SEL1 = 0;
 }
  //����2������MP3ģʽ
 void Usart2ToMP3(void)
 {
 	COM1_SEL0 = 1;
	COM1_SEL1 = 0;
 }
   //����2�����ڶ�ά��
 void Usart2ToErMa(void)
 {
 	COM1_SEL0 = 0;
	COM1_SEL1 = 1;
 }