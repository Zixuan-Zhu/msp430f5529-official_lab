/*
 * IIC.c
 *
 *  Created on: 2014-7-15
 *      Author: dean
 */
#include <msp430.h>
#include "IIC.h"
uchar pdin_data=0;


/***************************************************************************
���ж��ݵ���ʱ��ԭ����MSP430���ٶȱȽϿ졣ʹ���߿��Ը���ʱ��Ƶ�����е�����ʱ����
***************************************************************************/
void delay()
{
	uchar q0;
	for(q0=0;q0<50;q0++)
	{
		_NOP();
	}
}
/***************************************************************************
������ʱ10MS��ԭ����MSP430���ٶȱȽϿ졣ʹ���߿��Ը���ʱ��Ƶ�����е�����ʱ����
***************************************************************************/
void DelayMs(uint ms)
{
	uint iq0, iq1;
	for (iq0 = ms; iq0 > 0; iq0--)
	for (iq1 = LOOPCNT; iq1 > 0; iq1--)
	;
}
/***************************************************************************
����IIC����
***************************************************************************/
void Start()
{
	SDA_OUT;
	S_SDA;
	delay();
	S_SCL;
	delay();
	C_SDA;
	delay();
	C_SCL;
	delay();
}
/***************************************************************************
ֹͣ�������ͷ�IIC����
***************************************************************************/
void Stop()
{
	SDA_OUT;
	C_SCL;
	delay();
	C_SDA;
	delay();
	S_SCL;
	delay();
	S_SDA;
	delay();
}
/***************************************************************************
IIC����Ӧ��
***************************************************************************/
void Ack()
{
	SDA_OUT;
	C_SDA;
	delay();
	S_SCL;
	delay();
	C_SCL;
	delay();
	S_SDA;
}
/***************************************************************************
IIC������Ӧ��
***************************************************************************/
void NoAck()
{
	SDA_OUT;
	S_SDA;
	delay();
	S_SCL;
	delay();
	C_SCL;
	delay();
}
/***************************************************************************
IIC���߼���Ӧ��
����ֵ��IICӦ��λ��ֵ0��Ӧ��   1����Ӧ��
***************************************************************************/
uchar TestAck()
{
	uchar ack;
	delay();
	SDA_IN;
	delay();
	S_SCL;
	delay();
	ack=READ_SDA;
	delay();
	C_SCL;
	delay();
        if(ack==0x00)
	return 0;
        else return 1;
}
/***************************************************************************
IIC����д8λ����
input��Ҫд��8λ����
***************************************************************************/
void Write8Bit(uchar input)
{
	uchar temp,q0;
	SDA_OUT;
	for(temp=8;temp!=0;temp--)
	{
		q0=input&0x80;
		if(q0==0x80)
			S_SDA;
		else
			C_SDA;
      delay();
		S_SCL;
		delay();
		C_SCL;
		delay();
		input=input<<1;
	}
}
/***************************************************************************
IIC���߶�8λ����
����ֵ��������8λ����
***************************************************************************/
uchar Read8Bit()
{
	uchar temp,q0,rbyte=0;
	SDA_IN;
	for(temp=8;temp!=0;temp--)
	{
		S_SCL;
		delay();
		rbyte=rbyte<<1;
		SDA_IN;
		q0=READ_SDA;
		if(q0==IIC_SDA)
			rbyte=rbyte|0x1;
		delay();
		C_SCL;
		delay();
	}
	return(rbyte);
}

/***************************************************************************
��ʼ��IIC
***************************************************************************/
void InitIIC()
{
	IIC_DIR |= IIC_SCL;  //SCL�ܽ�Ϊ���
	IIC_DIR |= IIC_SDA; //SDA�ܽ�Ϊ����
	C_SCL;
	Stop();
}
/***************************************************************************
д����ֽ�
psrc_data��ָ��Ҫд�����������ָ��
adr��Ҫд�����ݵ��׵�ַ
nbyte��д����ֽ���
����ֵ:  0��ִ����ϣ�1��ִ�г��ִ���
***************************************************************************/
uchar W_I2C(uint device,uint adr,uchar data)
{

		Start();					//����IIC����
		Write8Bit(device);	//дTCA6416A��оƬ��ַ�Լ�ȷ������ִ��д����
		if(TestAck()==1)			//����Ӧ��
		return 1;					//��Ӧ��������˳����������ش���

		Write8Bit((uchar)adr);			//д��TCA6416A�����ĵ�ַ
		if(TestAck()==1)			//����Ӧ��
			return 1;				//��Ӧ��������˳����������ش���

		Write8Bit(data);		//��TCA6416A��д����
		if(TestAck()==1)
			return 1;

		Stop();						//ֹͣIIC����
		DelayMs(10);				//д����ʱ
	return 0;
}
/***************************************************************************
��һ���ֽ�
pdin_data��ָ��Ҫ����������ݵı�����ָ��
adr��Ҫ�������ݵĵ�ַ
����ֵ:  0��ִ����ϣ�1��ִ�г��ִ���
***************************************************************************/
uchar R_I2C(uint device,uint adr)
{
	pdin_data=0;
	Start();						//����IIC����
	Write8Bit(device-1);		//дTCA6416A��оƬ��ַ�Լ�ȷ������ִ��д����
	if(TestAck()==1)
		return 1;

	Write8Bit((uchar)(adr));			//д��TCA6416A�����ĵ�ַ
	if(TestAck()==1)
		return 1;

	Start();						//�ٴ�����IIC����
	Write8Bit(device);		//дTCA6416A��оƬ��ַ�Լ�ȷ������ִ�ж�����
	if(TestAck()==1)
	        return 1;
	pdin_data=Read8Bit();			//��TCA6416A�ж����ݣ�����pdin_data��ָ�Ĵ洢����

//	Ack();							//IICӦ�����

//	pdin_data=Read8Bit();			//��TCA6416A�ж����ݣ�����pdin_data��ָ�Ĵ洢����

	Stop();							//ֹͣIIC����
        return pdin_data;
}

void Clockint()
{
	 UCSCTL6 &= ~XT1OFF; //����XT1
	 P5SEL |= BIT2 + BIT3; //XT2���Ź���ѡ��
	 UCSCTL6 &= ~XT2OFF;          //��XT2
	 __bis_SR_register(SCG0);
	 UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
	 UCSCTL1 = DCORSEL_4;       //DCOƵ�ʷ�Χ��28.2MHZ����
	 UCSCTL2 = FLLD_5 + 1;       //D=16��N=1
	 UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLKʱ��ԴΪXT2CLK��DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
	 UCSCTL4 = SELA_4 + SELS_3 +SELM_3;    //ACLK��ʱ��ԴΪDCOCLKDIV,MCLK\SMCLK��ʱ��ԴΪDCOCLK
	 UCSCTL5 = DIVA_5 +DIVS_1;      //ACLK��DCOCLKDIV��32��Ƶ�õ���SMCLK��DCOCLK��2��Ƶ�õ�
	             //����MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
}
