/*
 * square.c
 *
 * Created: 02.01.2014 22:18:17
 *  Author: Stanislav Zhelnio
 */ 

#include "common.h"

// square

// ��������� �������� �������� �������� �������
typedef enum { prsclr1 = 1, prsclr8 = 2, prsclr64 = 3, prsclr256 = 4, prsclr1024 = 5 } PrescalerValue;

uint16_t SquarePeriod = 0;						//������ �������
uint16_t SquareTop = 0;							//����������������� ��������, ��� �������� 0 ����������������� �������� = ����������������� �����
PrescalerValue SquarePrescaler = prsclr1;		//��������� �������� �������� �������
ParameterMode SquarePeriodMode = modeView;		//������ ���������/�������������� ����������
ParameterMode SquareTopMode = modeView;
ParameterMode SquarePrescalerMode = modeView;

//EEPROM

uint16_t SquarePeriodEeprom	EEMEM;				// ��������� ��� �������� � ����������������� ������
uint16_t SquareTopEeprom	EEMEM;
PrescalerValue SquarePrescalerEeprom EEMEM;

// ����������������� �������� �� ����� ���� ������ ������� �������
// ������� ��������� ����������������� �������� ��� ������������� �������
inline void checkSquareTopValue()
{
	if(SquareTop !=0 && SquareTop > SquarePeriod)
	{
		SquareTop = SquarePeriod;
		eeprom_write_word(&SquareTopEeprom, SquareTop);
	}
}

// ������� ������ � ��������
inline void SquarePeriodInc()
{
	switch (SquarePeriodMode)
	{
		case modeEditOrder0:		SquarePeriod++;		break;
		case modeEditOrder1:		SquarePeriod+=0xFF;	break;
		default:	break;
	}
	
	eeprom_write_word(&SquarePeriodEeprom, SquarePeriod);
	
	checkSquareTopValue();
}

inline void SquarePeriodDec()
{
	switch (SquarePeriodMode)
	{
		case modeEditOrder0:		SquarePeriod--;		break;
		case modeEditOrder1:		SquarePeriod-=0xFF;	break;
		default:	break;
	}
	
	eeprom_write_word(&SquarePeriodEeprom, SquarePeriod);
	
	checkSquareTopValue();
}

inline void SquarePeriodModeInc()
{
	if(SquarePeriodMode == modeView)
	SquarePeriodMode = modeEditOrder1;
	else
	SquarePeriodMode--;
}

// ������� ������ � ������������������ ��������
inline void SquareTopInc()
{
	uint16_t newValue = 0xFFFF;
	switch (SquareTopMode)
	{
		case modeEditOrder0:		newValue = SquareTop + 1;		break;
		case modeEditOrder1:		newValue = SquareTop + 0xFF;	break;
		default:	break;
	}
	if(newValue <= SquarePeriod)
	{
		SquareTop = newValue;
		eeprom_write_word(&SquareTopEeprom, SquareTop);
	}
}

inline void SquareTopDec()
{
	uint16_t newValue = 0xFFFF;
	switch (SquareTopMode)
	{
		case modeEditOrder0:		newValue = SquareTop - 1;		break;
		case modeEditOrder1:		newValue = SquareTop - 0xFF;	break;
		default:	break;
	}
	if(newValue <= SquarePeriod)
	{
		SquareTop = newValue;
		eeprom_write_word(&SquareTopEeprom, SquareTop);
	}
}

inline void SquareTopModeInc()
{
	if(SquareTopMode == modeView)
	SquareTopMode = modeEditOrder1;
	else
	SquareTopMode--;
}

// ������� ������ � ��������� �������� �������
inline void SquarePrescalerInc()
{
	if(SquarePrescaler == prsclr1024)
		SquarePrescaler = prsclr1;
	else
		SquarePrescaler++;
		
	eeprom_write_byte(&SquarePrescalerEeprom, SquarePrescaler);
}

inline void SquarePrescalerDec()
{
	if(SquarePrescaler == prsclr1)
		SquarePrescaler = prsclr1024;
	else
		SquarePrescaler--;
		
	eeprom_write_byte(&SquarePrescalerEeprom, SquarePrescaler);
}

inline void SquarePrescalerModeInc()
{
	if(SquarePrescalerMode == modeView)
		SquarePrescalerMode = modeEditOrder0;
	else
		SquarePrescalerMode = modeView;
}

//��������� ��� �������������� ������ � �� � ���
typedef struct 
{
	uint16_t minValue;		//����������� ������
	uint16_t stepValue;		//���������� �������
	uint8_t pointPosition;	//������� "�����" � ������ �������� ���������
	uint8_t measureUnit;	//������� ��������� (���, ��)
	uint8_t numPartLength;	//���-�� �������� � �������� ������������� ���������
} SquareConvertInfo;

//���� ��� �������� ���������� ��� 16��� ������
SquareConvertInfo convertInfo[5] = {{1250,625,4,'u',9},{10,5,5,'u',7},{8,4,3,'m',7},{32,16,4,'m',8},{128,64,4,'m',8}};

//������� �������������� �������� ���������� � ��������� �������������
uint8_t SquarePeriodToString(char target[])
{
	//���� ��������
	if(SquarePeriod == 0)
		return  ui_strcpy(target, "Offline");
	
	//���� ������� - ������������ � �� � ���
	SquareConvertInfo *info = &convertInfo[SquarePrescaler-1];
	
	uint32_t temp = (uint32_t)info->minValue + ((uint32_t)SquarePeriod - 1) * (uint32_t)info->stepValue;
	
	for(int8_t i = info->numPartLength - 1; i >= 0; i--)
	{
		if(i == info->pointPosition)
		{
			target[i] = '.';
			continue;
		}
		
		target[i] = '0' + temp % 10;
		temp = temp / 10;
	}
	
	//��������� ������� ���������
	int8_t i = info->numPartLength;
	
	target[i++] = info->measureUnit;
	target[i++] = 's';
	
	return i;
	
	/*
	const uint8_t digitCount = 5;
	uint16_t temp = SquarePeriod;
	
	for(int8_t i = digitCount - 1; i >= 0; i--)
	{
		target[i] = '0' + temp % 10;
		temp = temp / 10;
	}
	
	return digitCount;
	*/
}

uint8_t SquareTopToString(char target[])
{
	//���� ����� ����, �� ������� �������� �������� �������
	if(SquareTop == 0)
		return ui_strcpy(target, "half");

	//���� ��� - ���������
	SquareConvertInfo *info = &convertInfo[SquarePrescaler-1];
	
	uint32_t temp = ((uint32_t)SquareTop + 1) * (uint32_t)info->stepValue;
	
	for(int8_t i = info->numPartLength - 1; i >= 0; i--)
	{
		if(i == info->pointPosition)
		{
			target[i] = '.';
			continue;
		}
		
		target[i] = '0' + temp % 10;
		temp = temp / 10;
	}
	
	//��������� ������� ���������
	int8_t i = info->numPartLength;
	
	target[i++] = info->measureUnit;
	target[i++] = 's';
	
	return i;
	
	/*
	const uint8_t digitCount = 5;
	uint16_t temp = SquareTop;
	
	for(int8_t i = digitCount - 1; i >= 0; i--)
	{
		target[i] = '0' + temp % 10;
		temp = temp / 10;
	}
	
	return digitCount;
	*/
}

uint8_t SquarePrescalerToString(char target[])
{
	//������� ������ ��������������� �������� � enum
	char ValueName[][5] = {"1", "8", "64", "256", "1024"};
	
	uint8_t strnum = SquarePrescaler - 1;
	
	return ui_strcpy(target, ValueName[strnum]);
}

// public ���������� �������������� � ������
void SquareItemToString(void *menuItem, char target[])
{
	MenuItem *item = (MenuItem *) menuItem;
	uint8_t i = 0;
	
	switch(item->Id)
	{
		case mSquare:
			i += MainItemWithStateToString(menuItem, target, (SquarePeriod != 0));
			break;
		case mSquareTs:
			i += PrepareItemValueString(menuItem,target);
			i += SquarePeriodToString(&target[i]);
			i += AddModeToItemValueString(SquarePeriodMode, &target[i]);
			break;
		case mSquareTt:
			i += PrepareItemValueString(menuItem,target);
			i += SquareTopToString(&target[i]);
			i += AddModeToItemValueString(SquareTopMode, &target[i]);
			break;
		case mSquareSp:
			i += PrepareItemValueString(menuItem,target);
			i += SquarePrescalerToString(&target[i]);
			i += AddModeToItemValueString(SquarePrescalerMode, &target[i]);
			break;
			
		default:;
	}
	target[i] = '\0';
}

// public ���������� ������� �� ������� ���������� (������)
ActionProcessed doSquareButtonAction (void *menuItem, eButton button)
{
	MenuItem *item = (MenuItem *) menuItem;
	switch(item->Id)
	{
		case mSquareTs:
			switch(button)
			{
				case buttonRight:	SquarePeriodModeInc(); return aProcessedAll;
				case buttonLeft:	if(SquarePeriodMode == modeView) return aProcessedNone;
									SquarePeriodMode = modeView; return aProcessedAll;
				case buttonUp:		if(SquarePeriodMode == modeView) return aProcessedNone;
									SquarePeriodInc(); return aProcessedAll;
				case buttonDown:	if(SquarePeriodMode == modeView) return aProcessedNone;
									SquarePeriodDec(); return aProcessedAll;
				default: return aProcessedNone;
			}
		case mSquareTt:
			switch (button)
			{
				case buttonRight:	SquareTopModeInc(); return aProcessedAll;
				case buttonLeft:	if(SquareTopMode == modeView) return aProcessedNone;
									SquareTopMode = modeView; return aProcessedAll;
				case buttonUp:		if(SquareTopMode == modeView) return aProcessedNone;
									SquareTopInc(); return aProcessedAll;
				case buttonDown:	if(SquareTopMode == modeView) return aProcessedNone;
									SquareTopDec(); return aProcessedAll;
				default: return aProcessedNone;
			}
			
		case mSquareSp:
			switch (button)
			{
				case buttonRight:	SquarePrescalerModeInc(); return aProcessedAll;
				case buttonLeft:	if(SquarePrescalerMode == modeView) return aProcessedNone;
									SquarePrescalerMode = modeView; return aProcessedAll;
				case buttonUp:		if(SquarePrescalerMode == modeView) return aProcessedNone;
									SquarePrescalerInc(); return aProcessedAll;
				case buttonDown:	if(SquarePrescalerMode == modeView) return aProcessedNone;
									SquarePrescalerDec(); return aProcessedAll;
				default: return aProcessedNone;
			}
		default: return aProcessedNone;
	}
}

// ��������� ���������������� ��������� �������� ������ 16-���������� �������
inline void SquareModeUpdateSettings()
{
	if(SquarePeriod == 0 && TCCR1A != 0)
	{
		TCCR1A = 0;
		TCCR1B = 0;
	}
	
	if(SquarePeriod != 0 && TCCR1A == 0)
	{
		TCCR1A = (1 << COM1B1) | (1 << WGM10) | (1 << WGM11);
		TCCR1B = (1 << WGM12) | (1 << WGM13);
	}
	
	if(OCR1A != SquarePeriod)
		OCR1A = SquarePeriod;
	
	uint16_t newTopValue = (SquareTop == 0) ? (SquarePeriod / 2) : SquareTop;
	if(OCR1B != newTopValue)
		OCR1B = newTopValue;

	uint8_t prsclrValue = TCCR1B & 0b00000111;
	if(prsclrValue != SquarePrescaler)
	{
		TCCR1B &= 0b11111000;
		TCCR1B |= SquarePrescaler;
	}
}

// public ���������� ������� ��������� ������ ������
void doSquareModeAction(void *menuItem, eAction action)
{
	if(action == onChange)
		SquareModeUpdateSettings();
}

// public �������� �������� ������ ��� ��������� ����������
void loadSquareSettings()
{
	SquarePeriod = eeprom_read_word(&SquarePeriodEeprom);
	
	SquareTop = eeprom_read_word(&SquareTopEeprom);
	if(SquareTop > SquarePeriod)
	SquareTop = 0;
	
	SquarePrescaler = eeprom_read_byte(&SquarePrescalerEeprom);
	if(SquarePrescaler > prsclr1024 || SquarePrescaler == 0)
	SquarePrescaler = prsclr1;
	
	SquareModeUpdateSettings();
}