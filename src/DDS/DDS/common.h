/*
 * common.h
 *
 * Created: 01.01.2014 17:40:15
 *  Author: Stanislav Zhelnio
 */ 

#ifndef COMMON_H_
#define COMMON_H_

#define  F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "lib/LCD.h"

// обработчик события для пункта меню должен возвращать статус отработки
// он может сообщить, что обработка не произведена. 
// В этом случае doAction выполняет необходимые действия "на свое усмотрение"
typedef enum { aProcessedAll, aProcessedNone } ActionProcessed;

// перечень кнопок, от которых поступают внешние события
typedef enum { buttonUp, buttonDown, buttonLeft, buttonRight } eButton;
	
// перечень событий изменения режимов работы
// вход в пункт меню, изменение параметра в пункте меню, выход из пункта меню
typedef enum { onEnter, onChange, onExit } eAction;
	
// перечень пунктов меню (порядок в этом enum должен соответствовать их порядку при определении массива пунктов меню)
typedef enum {	mSquare, mDds, mSquareTs, mSquareTt, mSquareSp, mDdsSignal, mDdsDelay, mDdsStep, mNothing = 0xFF } eItem;
	
// режимы редактирования настроек (просмотр, точная настройка, грубая настройка)
typedef enum { modeView = 0xFF, modeEditOrder0 = 0, modeEditOrder1 = 1 } ParameterMode;


typedef void (*ModeAction) (void *menuItem, eAction action);				// тип-указатель на функцио обоработке изменения режима работы
typedef ActionProcessed (*ButtonAction) (void *menuItem, eButton button);	// тип-указатель на функцию обработки нажатия на кнопку
typedef void (*PutToString) (void *menuItem, char target[]);				// тип-указатель на функцию преобразования в строку

// структура определяющая элемент меню
typedef struct 
{
	eItem		Id;					//идентификатор, дожен соответствовать номеру элемента в массиве пунктов меню
	
	eItem		NextId;				//номера связанных элементов (нумерация = нумерации массива пунктов меню)
	eItem		PreviousId;
	eItem		ParentId;
	eItem		ChildId;

	ButtonAction	buttonAction;	// обработчик нажатий на кнопки
	ModeAction		modeAction;		// обработчик изменения режимов работы
	
	char		*Caption;			// наименование пунта меню
	PutToString toString;			// обработчик преобразования в строку
} MenuItem;

// main.c

// массив пунктов меню
extern MenuItem			globalMenuData[];

// action manager

void doAction(eButton button);				// обработчик всех событий поступающих от органов управления
void initMenu(MenuItem *startMenuItem);		// выполняет начальную инициализацию меню

// набор общих функций работы с именами пунктов меню
uint8_t MainItemWithStateToString(void *menuItem, char target[], uint8_t status);	// формирует наименования основных режимов работы (включая статус - работает или нет)
uint8_t PrepareItemValueString(void *menuItem, char target[]);			// формирует подстроку "параметр="
uint8_t AddModeToItemValueString(ParameterMode mode, char target[]);	// для режимов редактирования формирует вторую строчку 
																		//    LCD с кратким именем режима
uint8_t ui_strcpy(char target[], char source[]);	//обычное посимвольное копирование
													//  не копирует \0, возвращает текущий индекс в массиве - удобно для отладки

// square
void SquareItemToString(void *menuItem, char target[]);					// обработчик преобразования в строку
ActionProcessed doSquareButtonAction (void *menuItem, eButton button);	// обработчик событий от органов управления (кнопок)
void doSquareModeAction(void *menuItem, eAction action);				// обработчик событий изменения режима работы
void loadSquareSettings();												// загрузка настроек модуля при включении устройства

// dds

void DdsItemToString(void *menuItem, char target[]);					// обработчик преобразования в строку
ActionProcessed doDdsButtonAction (void *menuItem, eButton button);		// обработчик событий от органов управления (кнопок)
void doDdsModeAction(void *menuItem, eAction action);					// обработчик событий изменения режима работы
void loadDdsSettings();													// загрузка настроек модуля при включении устройства

void checkStopFlagAndDoDdsSignalOut();									// непосредственно формирует сигнал на порт, подключенный к ЦАП

#endif /* COMMON_H_ */
