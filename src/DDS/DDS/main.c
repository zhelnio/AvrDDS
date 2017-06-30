
/* �����:              Stanislav Zhelnio (https://twitter.com/zhelnio)
 * ��� ��:			   ATmega88, Flash 8 Kbytes, SRAM 1 Kbytes, EEPROM 512 bytes.
 * �������� �������:   F_CPU 16000000 MHz
 * ����:               07.01.2014
 * ������ ��:          Atmel Studio 6 (Version: 6.1.2730 - Service Pack 2)
 * FUSES:              HIGH 0xdf, LOW 0xff;
 * ��������:           Software for ATmega88-based DDS with LCD1602, PWM-out on PB2 (OC1B), DDS-out to ADC on PORTD 
 
					   Thanks to:
					   DI HALT for PINBOARD-II developement board and http://easyelectronics.ru/ web resource
					   Papandopala Papandopalavich for his LCD library
					   (http://we.easyelectronics.ru/AVR/biblioteka-na-si-dlya-lcd-displeev-na-baze-processora-hd44780-dlya-mikrokontrollerov-avr.html)
					   Steel.ne for idia of menu structure 
					   (http://easyelectronics.ru/organizaciya-drevovidnogo-menyu.html)
 */

#include "common.h"

//menu

//�������� ������ ��������� ����
//����������:
// - �������������� ������� ���� (������ ���� � ������� �� ���������� � enum eItem !)
// - ����� ����� ���������� (���������/����������/��������/������������),
// - ������� ����������� ������� (������/����-�����)
// - ������������ ������ ����/���������
// - ������� ��������������� � ������
// ��� �������� ������ � ���� �������� ��������� ��������� doAction

MenuItem globalMenuData[] = {
	// ������� ������������ ������ ��������������� �������� � enum eItem (!)
	//	ID,			NXT,		PRV,		PRN,		CHLD,		ButtonAction,	ModeAction,	CAPTION		ToString
	{	mSquare,	mDds,		mNothing,	mNothing,	mSquareTs,	NULL,	doSquareModeAction,	"Square",	SquareItemToString },
	{	mDds,		mNothing,	mSquare,	mNothing,	mDdsSignal,	NULL,	doDdsModeAction,	"DDS",		DdsItemToString },
		
	{	mSquareTs,	mSquareTt,	mNothing,	mSquare,	mNothing,	doSquareButtonAction,	doSquareModeAction,	"Ts",	SquareItemToString },
	{	mSquareTt,	mSquareSp,	mSquareTs,	mSquare,	mNothing,	doSquareButtonAction,	doSquareModeAction,	"Tt",	SquareItemToString },
	{	mSquareSp,	mNothing,	mSquareTt,	mSquare,	mNothing,	doSquareButtonAction,	doSquareModeAction,	"Sp",	SquareItemToString },
	
	{	mDdsSignal,	mDdsDelay,	mNothing,	mDds,		mNothing,	doDdsButtonAction,		doDdsModeAction,	"Type",	DdsItemToString },
	{	mDdsDelay,	mDdsStep,	mDdsSignal,	mDds,		mNothing,	doDdsButtonAction,		doDdsModeAction,	"Td",	DdsItemToString },
	{	mDdsStep,	mNothing,	mDdsDelay,	mDds,		mNothing,	doDdsButtonAction,		doDdsModeAction,	"Stp",	DdsItemToString }
};

int main(void)
{
	LCDinit();
	
	DDRD = 0xFF;
	DDRB |= (1<<PINB2);
	
	//��������� ���������� �� ����� ���������� �� �����
	PCICR |= (1 << PCIE0) | (1 << PCIE1);
	PCMSK0 |= (1 << PCINT4) | (1 << PCINT5);
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);
	
	//�������� ��������
	loadDdsSettings();
	loadSquareSettings();
	
	//��������� � ����� ����
	initMenu(&globalMenuData[mSquare]);
	
	sei();	
	
    while(1)
	{
		//��������������� ��������� ���������� �� ���� DDS
		checkStopFlagAndDoDdsSignalOut();
	}
}

//���������� �� ������ �����/������
ISR(PCINT0_vect)
{
	//��������� ������ �� ������ ���������� ���������
	static volatile uint8_t portbhistory = 0xFF;
	
	//���������� ������������ ����
	uint8_t changedbits = PINB ^ portbhistory;
	portbhistory = PINB;
	
	if( (changedbits & (1 << PINB4)) && (portbhistory & (1 << PINB4)) )
		doAction(buttonRight);
	
	if( (changedbits & (1 << PINB5)) && (portbhistory & (1 << PINB5)) )
		doAction(buttonLeft);
	
	_delay_ms(100);
}

//���������� �� �������� ����/�����
ISR(PCINT1_vect)
{
	static uint8_t EncState = 0x00;
	uint8_t newValue = PINC & 0x03;
	uint8_t fullState = EncState | newValue << 2;
	
	switch(fullState)
	{
		case 0xB: case 0xD: // case 0x2: case 0x4:
		doAction(buttonUp);
		break;
		case 0x7: case 0xE: // case 0x1:  case 0x8:
		doAction(buttonDown);
		break;
	}
	EncState = newValue;
}
