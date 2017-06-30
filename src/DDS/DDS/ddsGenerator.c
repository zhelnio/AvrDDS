/*
 * dds.c
 *
 * Created: 07.01.2014 0:52:53
 *  Author: Stanislav
 */ 

#include "common.h"

// буфер для текущей табличной функции (заполнен подобием меандра для упрощения отладки)
uint8_t ddsBuffer[256] __attribute__((aligned(0x100))) = {
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
	0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// табличные функции, хранятся в памяти программ, подгружаются в буфер при изменении настроек

const uint8_t sine[256] PROGMEM = {
	0x80,0x83,0x86,0x89,0x8c,0x8f,0x92,0x95,0x98,0x9c,0x9f,0xa2,0xa5,0xa8,0xab,0xae,
	0xb0,0xb3,0xb6,0xb9,0xbc,0xbf,0xc1,0xc4,0xc7,0xc9,0xcc,0xce,0xd1,0xd3,0xd5,0xd8,
	0xda,0xdc,0xde,0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xed,0xef,0xf0,0xf2,0xf3,0xf5,
	0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfc,0xfd,0xfe,0xfe,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xfe,0xfd,0xfc,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,
	0xf6,0xf5,0xf3,0xf2,0xf0,0xef,0xed,0xec,0xea,0xe8,0xe6,0xe4,0xe2,0xe0,0xde,0xdc,
	0xda,0xd8,0xd5,0xd3,0xd1,0xce,0xcc,0xc9,0xc7,0xc4,0xc1,0xbf,0xbc,0xb9,0xb6,0xb3,
	0xb0,0xae,0xab,0xa8,0xa5,0xa2,0x9f,0x9c,0x98,0x95,0x92,0x8f,0x8c,0x89,0x86,0x83,
	0x80,0x7c,0x79,0x76,0x73,0x70,0x6d,0x6a,0x67,0x63,0x60,0x5d,0x5a,0x57,0x54,0x51,
	0x4f,0x4c,0x49,0x46,0x43,0x40,0x3e,0x3b,0x38,0x36,0x33,0x31,0x2e,0x2c,0x2a,0x27,
	0x25,0x23,0x21,0x1f,0x1d,0x1b,0x19,0x17,0x15,0x13,0x12,0x10,0x0f,0x0d,0x0c,0x0a,
	0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x03,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08,
	0x09,0x0a,0x0c,0x0d,0x0f,0x10,0x12,0x13,0x15,0x17,0x19,0x1b,0x1d,0x1f,0x21,0x23,
	0x25,0x27,0x2a,0x2c,0x2e,0x31,0x33,0x36,0x38,0x3b,0x3e,0x40,0x43,0x46,0x49,0x4c,
	0x4f,0x51,0x54,0x57,0x5a,0x5d,0x60,0x63,0x67,0x6a,0x6d,0x70,0x73,0x76,0x79,0x7c
};

const uint8_t tits[256] PROGMEM = {
	0,  0,  0,  0,  0,  0,  0,  0,	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,	0,  0,  0,  0,  0,  0,  0,  0,
	32, 36, 40, 44, 48, 52, 54, 56,	56, 57, 58, 59, 60, 61, 62, 63,
	63, 64, 64, 65, 65, 66, 67, 68,	68, 68, 68, 69, 69, 69, 70, 70,
	70, 74, 78, 82, 83, 83, 83, 83,	82, 82, 78, 74, 70, 70, 70, 70,
	69, 69, 69, 68, 68, 68, 67, 67,	67, 67, 66, 66, 66, 65, 65, 65,
	65, 64, 63, 62, 60, 59, 58, 57,	57, 55, 53, 51, 48, 45, 43, 41,
	35, 41, 43, 45, 48, 51, 53, 55,	57, 57, 58, 59, 60, 62, 63, 64,
	65, 65, 65, 65, 66, 66, 66, 67,	67, 67, 67, 68, 68, 68, 69, 69,
	69, 70, 70, 70, 70, 74, 78, 82,	82, 83, 83, 83, 83, 82, 78, 74,
	70, 70, 70, 69, 69, 69, 68, 68,	68, 68, 67, 66, 65, 64, 64, 63,
	63, 62, 61, 60, 59, 58, 57, 56,	56, 54, 52, 48, 44, 40, 36, 32,
	0,  0,  0,  0,  0,  0,  0,  0,	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,	0,  0,  0,  0,  0,  0,  0,  0
};

const uint8_t ecg[256] PROGMEM = {
	73, 74, 75, 75, 74, 73, 73, 73, 73, 72, 71, 69, 68, 67, 67, 67,
	68, 68, 67, 65, 62, 61, 59, 57, 56, 55, 55, 54, 54, 54, 55, 55,
	55, 55, 55, 55, 54, 53, 51, 50, 49, 49, 52, 61, 77, 101,132,169,
	207,238,255,254,234,198,154,109,68, 37, 17, 5, 	0,  1,  6,  13,
	20, 28, 36, 45, 52, 57, 61, 64, 65, 66, 67, 68, 68, 69, 70, 71,
	71, 71, 71, 71, 71, 71, 71, 72, 72, 72, 73, 73, 74, 75, 75, 76,
	77, 78, 79, 80, 81, 82, 83, 84, 86, 88, 91, 93, 96, 98, 100,102,
	104,107,109,112,115,118,121,123,125,126,127,127,127,127,127,126,
	125,124,121,119,116,113,109,105,102,98, 95, 92, 89, 87, 84, 81,
	79, 77, 76, 75,	74, 73, 72, 70, 69, 68, 67, 67, 67, 68, 68, 68,
	69, 69, 69, 69, 69, 69, 69, 70, 71, 72, 73, 73, 74, 74, 75, 75,
	75, 75, 75, 75, 74, 74, 73, 73, 73, 73, 72, 72, 72, 71, 71, 71,
	71, 71, 71, 71,	70, 70, 70, 69, 69, 69, 69, 69, 70, 70, 70, 69,
	68, 68, 67, 67,	67, 67, 66, 66, 66, 65, 65, 65, 65, 65, 65, 65,
	65, 64, 64, 63,	63, 64, 64, 65, 65, 65, 65, 65, 65, 65, 64, 64,
	64, 64, 64, 64, 64, 64, 65, 65, 65, 66, 67, 68, 69, 71, 72, 73
};

const uint8_t kremlin[256] PROGMEM = {
	0,  0,  0,  0,  0,  0,  0,  55, 55, 64, 61, 61, 64,	55, 55, 64,
	61, 61, 64, 55, 55, 64, 61, 61, 64, 55, 55, 64, 61, 61, 64, 55,
	55, 64, 61, 61, 64, 55, 55, 64, 61, 61, 64,	55, 55, 64, 61, 61,
	64, 55, 55, 64, 61, 61, 64,	55, 55, 64, 61, 61, 64, 55, 55, 64,
	61, 61, 64,	55, 55, 64, 61, 61, 64, 55, 55, 64, 61, 61, 64,	55,
	55, 64, 61, 61, 64, 55, 55, 64, 61, 61, 64,	55, 55, 64, 61, 61,
	64,	135,135,138,144,149,155,150,145,140,135,135,137,140,142,145,
	147,182,175,203,200,205,209,214,219,224,228,232,236,241,245,255,
	245,241,236,232,228,224,219,214,209,205,200,203,175,182,147,145,
	142,140,137,135,135,140,145,150,155,149,144,138,135,135,64, 61,
	61, 64, 55, 55, 64, 61, 61, 64, 55, 55,	64, 61, 61, 64, 55, 55,
	64, 61, 61, 64, 55, 55,	64, 61, 61, 64, 55, 55, 64, 61, 61, 64,
	55, 55,	64, 61, 61, 64, 55, 55, 64, 61, 61, 64, 55, 55,	64, 61,
	61, 64, 55, 55, 64, 61, 61, 64, 55, 55,	64, 61, 61, 64, 55, 55,
	64, 61, 61, 64, 55, 55,	64, 61, 61, 64, 55, 55, 64, 61, 61, 64,
	55, 55,	64, 61, 61, 64, 55, 55, 0,  0,  0,  0,  0,  0,  0,  0
	
};

// перечень сигналов, доступных для генерации DDS
typedef enum { ddsSine, ddsTriangle, ddsSawTooth, ddsRevSawTooth, ddsTits, ddsECG, ddsKremlin, ddsNone =0xFF } DdsSignalType;

// настройки, хранящиеся в энергонезависимой памяти
uint16_t ddsPhaseIncrementDelayEeprom EEMEM;	// задержка перед переходом к следующей ячейке таблицы функции (буфера)
uint8_t ddsPhaseIncrementStepEeprom EEMEM;		// шаг, с которым осуществляется переход к следующей ячейке таблицы функции (буфера)
DdsSignalType ddsSignalTypeEeprom EEMEM;		// тип сигнала

// настройки работы модуля
register volatile uint8_t ddsStopFlag asm("r10");	// регистр, в котором храниться флаг остановки генерации
													// генерация выполняется в main, флаг устанавливается в прерывании
const uint8_t ddsStopFlagBit = 0;					// номер стоп-бита флага остановки

union
{
	uint16_t val16;
	uint8_t  val8[2];
} ddsPhaseIncrementDelay;							// задержка перед переходом к следующей ячейке таблицы функции (буфера)

uint8_t ddsPhaseIncrementStep;						// шаг, с которым осуществляется переход к следующей ячейке таблицы функции (буфера)
const uint8_t maxDdsPhaseIncrementStep = 64;		// его минимальное и максимальное значения
const uint8_t minDdsPhaseIncrementStep = 1;

DdsSignalType ddsSignalType;						// тип сигнала
const DdsSignalType maxDdsSignalType = ddsKremlin;	// его минимальное и максимальное значения
const DdsSignalType minDdsSignalType = ddsSine;

ParameterMode ddsPhaseIncrementMode = modeView;		// режимы просмотра/редактирования параметров (просмотр, грубая настройка, точная настройка)
ParameterMode ddsSignalTypeMode = modeView;
ParameterMode ddsPhaseIncrementStepMode = modeView;

// Функция непосредственно выполняеющая проход по буферу с текущей таблице функции.
// Использует глобальные параметры шага/задержки.
// Для считывания новых настроек в прерывании устанавливается флаг остановки, что позволяет выйти из цикла внутри функции.
// После чего эта фукнция снова вызывается из main.
// Во время вызова проверяется минимальная задержка, если она = 0, значит генерация отключена.
void checkStopFlagAndDoDdsSignalOut()
{
	volatile uint8_t temp1 = 0, temp2 = 0;
	
	ddsStopFlag = (ddsPhaseIncrementDelay.val16 > 0) ? 0 : 1;

	asm volatile(
				" sbrc %[BYTE_EXIT], %[BIT_EXIT] "	"\n\t" \
				" rjmp Exit_%= "					"\n\t" \
		"Enter_%=: "								"\n\t" \
				" ld __tmp_reg__, z " 				"; 2 цикла	\n\t" \
				" out %[PPORT], __tmp_reg__ "		"; 1 цикл	\n\t" \
				" mov %[TMP_0], %[DELAY_0] "		"; 1 цикл   \n\t"      \
				" mov %[TMP_1], %[DELAY_1] "		"; 1 цикл   \n\t"      \
		"Delay_%=: "
				" subi %[TMP_0], 1"					"; 1 цикл  \n\t"      \
				" sbci %[TMP_1], 0"					"; 1 цикл  \n\t"      \
				" brne Delay_%="					"; 2 цикла при переходе по метке, иначе 1 цикл\n\t"      \
				" add r30, %[STEP] " 				"; 1 цикл	\n\t" \
				" sbrs %[BYTE_EXIT], %[BIT_EXIT] "	"; 1 цикл (условие ложно, не пропускаем) \n\t" \
				" rjmp Enter_%= "					"; 2 цикла. Итого: 12 (+4) циклов. \n\t" \
		"Exit_%=: "									"\n\t" \
				" out %[PPORT], __zero_reg__ "		"\n\t" \
		:
		:	[PPORT] "I" (_SFR_IO_ADDR(PORTD)),			\
			[ZPOINTER_REG] "z" (ddsBuffer),				\
			[BYTE_EXIT] "r" (ddsStopFlag),				\
			[BIT_EXIT] "I" (ddsStopFlagBit),			\
			[TMP_0] "r" (temp1),						\
			[TMP_1] "r" (temp2),						\
			[DELAY_0] "r" (ddsPhaseIncrementDelay.val8[0]),		\
			[DELAY_1] "r" (ddsPhaseIncrementDelay.val8[1]),		\
			[STEP] "r" (ddsPhaseIncrementStep)					\
	);
}

// функции работы с параметром Шаг
inline void ddsPhaseIncrementStepModeInc()
{
	ddsPhaseIncrementStepMode = (ddsPhaseIncrementStepMode == modeView) ? modeEditOrder0 : modeView;
}

inline void ddsPhaseIncrementStepInc()
{
	if(ddsPhaseIncrementStep == maxDdsPhaseIncrementStep)
		ddsPhaseIncrementStep = minDdsPhaseIncrementStep;
	else
		ddsPhaseIncrementStep++;
	
	eeprom_write_byte(&ddsPhaseIncrementStepEeprom, ddsPhaseIncrementStep);
}

inline void ddsPhaseIncrementStepDec()
{
	if(ddsPhaseIncrementStep == minDdsPhaseIncrementStep)
		ddsPhaseIncrementStep = maxDdsPhaseIncrementStep;
	else
		ddsPhaseIncrementStep--;
	
	eeprom_write_byte(&ddsPhaseIncrementStepEeprom, ddsPhaseIncrementStep);
}

// функции работы с параметром Тип сиглана
inline void ddsSignalTypeModeInc()
{
	ddsSignalTypeMode = (ddsSignalTypeMode == modeView) ? modeEditOrder0 : modeView;
}

inline void ddsSignalTypeInc()
{
	if(ddsSignalType == maxDdsSignalType)
		ddsSignalType = minDdsSignalType;
	else
		ddsSignalType++;
	
	eeprom_write_byte(&ddsSignalTypeEeprom, ddsSignalType);
}

inline void ddsSignalTypeDec()
{
	if(ddsSignalType == minDdsSignalType)
		ddsSignalType = maxDdsSignalType;
	else
		ddsSignalType--;
	
	eeprom_write_byte(&ddsSignalTypeEeprom, ddsSignalType);
}

// функции работы с параметром Задержка
inline void ddsPhaseIncrementDelayModeInc()
{
	if(ddsPhaseIncrementMode == modeView)
		ddsPhaseIncrementMode = modeEditOrder1;
	else
		ddsPhaseIncrementMode--;	
}

inline void ddsPhaseIncrementDelayInc()
{
	switch(ddsPhaseIncrementMode)
	{
		case modeEditOrder0:
			ddsPhaseIncrementDelay.val16++;
			break;
		
		case modeEditOrder1:
			ddsPhaseIncrementDelay.val8[1]++;
			break;
		
		default:	break;
	}
	
	eeprom_write_word(&ddsPhaseIncrementDelayEeprom, ddsPhaseIncrementDelay.val16);
}

inline void ddsPhaseIncrementDelayDec()
{
	switch(ddsPhaseIncrementMode)
	{
		case modeEditOrder0:
			ddsPhaseIncrementDelay.val16--;
			break;
		
		case modeEditOrder1:
			ddsPhaseIncrementDelay.val8[1]--;
			break;
		
		default:	break;
	}
	
	eeprom_write_word(&ddsPhaseIncrementDelayEeprom, ddsPhaseIncrementDelay.val16);
}

//функции преобразования значения параметра в строку
//все формулы расчитаны для тактовой частоты 16 Мгц
uint8_t ddsPhaseIncrementDelayToString(char target[])
{
	const uint8_t digitCount = 6;
	uint16_t temp = ddsPhaseIncrementDelay.val16;
	
	for(int8_t i = digitCount - 1; i >= 0; i--)
	{
		target[i] = '0' + temp % 10;
		temp = temp / 10;
	}
	
	return digitCount;
}

uint8_t ddsPhaseIncrementDelayToStringMU(char target[])
{
	const uint8_t valueDigitCountWithPoint = 8;
	const uint8_t valuePointPosition = 4;
	
	uint32_t temp = (192ul + (ddsPhaseIncrementDelay.val16 - 1ul) * 64ul)/ ddsPhaseIncrementStep;
	
	for(int8_t i = valueDigitCountWithPoint - 1; i >= 0; i--)
	{
		if(i == valuePointPosition)
		{
			target[i] = '.';
			continue;
		}
		
		target[i] = '0' + temp % 10;
		temp = temp / 10;
	}
	
	target[valueDigitCountWithPoint] = 'm';
	target[valueDigitCountWithPoint+1] = 's';
	
	return valueDigitCountWithPoint+2;
}

uint8_t ddsPhaseIncrementStepToString(char target[])
{
	const uint8_t digitCount = 2;
	uint8_t temp = ddsPhaseIncrementStep;
	
	for(int8_t i = digitCount - 1; i >= 0; i--)
	{
		target[i] = '0' + temp % 10;
		temp = temp / 10;
	}
	
	return digitCount;
}

uint8_t ddsSignalTypeToString(char target[])
{
	//порядок должен соответствовать таковому в enum
	char ValueName[][12] = {"Sine", "Triangle", "SawTooth", "RevSawTooth", "Tits", "ECG", "Kremlin", "None"};
	
	uint8_t strnum = ddsSignalType;
	uint8_t i = 0;
	
	while (ValueName[strnum][i] != '\0')
	{
		target[i] = ValueName[strnum][i];
		i++;
	}
	
	return i;
}

// public обработчик преобразования в строку
void DdsItemToString(void *menuItem, char target[])
{
	MenuItem *item = (MenuItem *) menuItem;
	uint8_t i = 0;
	
	
	
	switch(item->Id)
	{
		case mDds:
			i += MainItemWithStateToString(menuItem, target, (ddsPhaseIncrementDelay.val16 != 0));
			break;
		
		case mDdsDelay:
			i += PrepareItemValueString(menuItem,target);
			if(ddsPhaseIncrementDelay.val16 == 0)
				i += ui_strcpy(&target[i], "Offline");
			else
				i += ddsPhaseIncrementDelayToStringMU(&target[i]);
				i += AddModeToItemValueString(ddsPhaseIncrementMode, &target[i]);
			break;
		
		case mDdsSignal:
			i += PrepareItemValueString(menuItem,target);
			i += ddsSignalTypeToString(&target[i]);
			i += AddModeToItemValueString(ddsSignalTypeMode, &target[i]);
			break;
		
		case mDdsStep:	
			i += PrepareItemValueString(menuItem,target);	
			i += ddsPhaseIncrementStepToString(&target[i]);
			i += AddModeToItemValueString(ddsPhaseIncrementStepMode, &target[i]);
			break;
		
		default:;
	}
	target[i] = '\0';
}

// public обработчик событий от органов управления (кнопок)
ActionProcessed doDdsButtonAction (void *menuItem, eButton button)
{
	MenuItem *item = (MenuItem *) menuItem;
	
	// поведение зависит от текущего пунта меню
	switch(item->Id)
	{
		case mDdsDelay:
		switch(button)
		{
			case buttonRight:	ddsPhaseIncrementDelayModeInc();
								return aProcessedAll;
			case buttonLeft:	if(ddsPhaseIncrementMode == modeView) return aProcessedNone;
								ddsPhaseIncrementMode = modeView; return aProcessedAll;
			case buttonUp:		if(ddsPhaseIncrementMode == modeView) return aProcessedNone;
								ddsPhaseIncrementDelayInc(); return aProcessedAll;
			case buttonDown:	if(ddsPhaseIncrementMode == modeView) return aProcessedNone;
								ddsPhaseIncrementDelayDec(); return aProcessedAll;
			default: return aProcessedNone;
		}
		
		case mDdsSignal:
		switch (button)
		{
			case buttonRight:	ddsSignalTypeModeInc(); return aProcessedAll;
			case buttonLeft:	if(ddsSignalTypeMode == modeView) return aProcessedNone;
								ddsSignalTypeMode = modeView; return aProcessedAll;
			case buttonUp:		if(ddsSignalTypeMode == modeView) return aProcessedNone;
								ddsSignalTypeInc(); return aProcessedAll;
			case buttonDown:	if(ddsSignalTypeMode == modeView) return aProcessedNone;
								ddsSignalTypeDec(); return aProcessedAll;
			default: return aProcessedNone;
		}
		
		case mDdsStep:
		switch (button)
		{
			case buttonRight:	ddsPhaseIncrementStepModeInc();
								return aProcessedAll;
			case buttonLeft:	if(ddsPhaseIncrementStepMode == modeView) return aProcessedNone;
								ddsPhaseIncrementStepMode = modeView; return aProcessedAll;
			case buttonUp:		if(ddsPhaseIncrementStepMode == modeView) return aProcessedNone;
								ddsPhaseIncrementStepInc(); return aProcessedAll;
			case buttonDown:	if(ddsPhaseIncrementStepMode == modeView) return aProcessedNone;
								ddsPhaseIncrementStepDec(); return aProcessedAll;
			default: return aProcessedNone;
		}
		
		default: return aProcessedNone;
	}
}

// выполяет загрузку таблицы значений функции (буфера) из памяти программ
// либо генерацию этих значений
void ddsLoadData(DdsSignalType signalType)
{
	switch (signalType)
	{
		case ddsSine:
			memcpy_PF(ddsBuffer,&sine,sizeof ddsBuffer);
			break;
		
		case ddsTits:
			memcpy_PF(ddsBuffer,&tits,sizeof ddsBuffer);
			break;
		
		case ddsECG:
			memcpy_PF(ddsBuffer,&ecg,sizeof ddsBuffer);
			break;
		
		case ddsKremlin:
			memcpy_PF(ddsBuffer,&kremlin,sizeof ddsBuffer);
			break;
		
		case ddsTriangle:
			for(int i=0, j=0 ; i <= 127 ; i++)
			{
				ddsBuffer[i] = j;
				ddsBuffer[0xFF - i] = j;
				j +=2;
			}
			break;
		
		case ddsSawTooth:
			for(int i=0 ; i <= 255 ; i++)
				ddsBuffer[i] = i;
			break;
		
		case ddsRevSawTooth:
			for(int i=0 ; i <= 255 ; i++)
				ddsBuffer[i] = 0xFF - i;
			break;
		
		default:;
	}
}

// выполняет изменение настроек работы и выставляет флаг, по которому
// функция генерации будет перезапущена с новыми параметрами
void doDdsModeUpdateSettings()
{
	static DdsSignalType currentSignalType = ddsNone;
	
	if(currentSignalType != ddsSignalType)
	{
		ddsLoadData(ddsSignalType);
		currentSignalType = ddsSignalType;
	}
	
	ddsStopFlag = 1;
}

// public обработчик событий изменения режима работы
void doDdsModeAction(void *menuItem, eAction action)
{	
	if(action == onChange)
		doDdsModeUpdateSettings();
}

// public загрузка настроек модуля при включении устройства
void loadDdsSettings()
{	
	ddsPhaseIncrementDelay.val16 = eeprom_read_word(&ddsPhaseIncrementDelayEeprom);
	
	ddsPhaseIncrementStep = eeprom_read_byte(&ddsPhaseIncrementStepEeprom);
	if(ddsPhaseIncrementStep > maxDdsPhaseIncrementStep || ddsPhaseIncrementStep < minDdsPhaseIncrementStep)
		ddsPhaseIncrementStep = minDdsPhaseIncrementStep;
		
	ddsSignalType = eeprom_read_byte(&ddsSignalTypeEeprom);
	if(ddsSignalType > maxDdsSignalType || ddsSignalType < minDdsSignalType)
		ddsSignalType = minDdsSignalType;

	doDdsModeUpdateSettings();
}
