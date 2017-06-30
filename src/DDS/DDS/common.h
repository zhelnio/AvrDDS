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

// ���������� ������� ��� ������ ���� ������ ���������� ������ ���������
// �� ����� ��������, ��� ��������� �� �����������. 
// � ���� ������ doAction ��������� ����������� �������� "�� ���� ����������"
typedef enum { aProcessedAll, aProcessedNone } ActionProcessed;

// �������� ������, �� ������� ��������� ������� �������
typedef enum { buttonUp, buttonDown, buttonLeft, buttonRight } eButton;
	
// �������� ������� ��������� ������� ������
// ���� � ����� ����, ��������� ��������� � ������ ����, ����� �� ������ ����
typedef enum { onEnter, onChange, onExit } eAction;
	
// �������� ������� ���� (������� � ���� enum ������ ��������������� �� ������� ��� ����������� ������� ������� ����)
typedef enum {	mSquare, mDds, mSquareTs, mSquareTt, mSquareSp, mDdsSignal, mDdsDelay, mDdsStep, mNothing = 0xFF } eItem;
	
// ������ �������������� �������� (��������, ������ ���������, ������ ���������)
typedef enum { modeView = 0xFF, modeEditOrder0 = 0, modeEditOrder1 = 1 } ParameterMode;


typedef void (*ModeAction) (void *menuItem, eAction action);				// ���-��������� �� ������� ���������� ��������� ������ ������
typedef ActionProcessed (*ButtonAction) (void *menuItem, eButton button);	// ���-��������� �� ������� ��������� ������� �� ������
typedef void (*PutToString) (void *menuItem, char target[]);				// ���-��������� �� ������� �������������� � ������

// ��������� ������������ ������� ����
typedef struct 
{
	eItem		Id;					//�������������, ����� ��������������� ������ �������� � ������� ������� ����
	
	eItem		NextId;				//������ ��������� ��������� (��������� = ��������� ������� ������� ����)
	eItem		PreviousId;
	eItem		ParentId;
	eItem		ChildId;

	ButtonAction	buttonAction;	// ���������� ������� �� ������
	ModeAction		modeAction;		// ���������� ��������� ������� ������
	
	char		*Caption;			// ������������ ����� ����
	PutToString toString;			// ���������� �������������� � ������
} MenuItem;

// main.c

// ������ ������� ����
extern MenuItem			globalMenuData[];

// action manager

void doAction(eButton button);				// ���������� ���� ������� ����������� �� ������� ����������
void initMenu(MenuItem *startMenuItem);		// ��������� ��������� ������������� ����

// ����� ����� ������� ������ � ������� ������� ����
uint8_t MainItemWithStateToString(void *menuItem, char target[], uint8_t status);	// ��������� ������������ �������� ������� ������ (������� ������ - �������� ��� ���)
uint8_t PrepareItemValueString(void *menuItem, char target[]);			// ��������� ��������� "��������="
uint8_t AddModeToItemValueString(ParameterMode mode, char target[]);	// ��� ������� �������������� ��������� ������ ������� 
																		//    LCD � ������� ������ ������
uint8_t ui_strcpy(char target[], char source[]);	//������� ������������ �����������
													//  �� �������� \0, ���������� ������� ������ � ������� - ������ ��� �������

// square
void SquareItemToString(void *menuItem, char target[]);					// ���������� �������������� � ������
ActionProcessed doSquareButtonAction (void *menuItem, eButton button);	// ���������� ������� �� ������� ���������� (������)
void doSquareModeAction(void *menuItem, eAction action);				// ���������� ������� ��������� ������ ������
void loadSquareSettings();												// �������� �������� ������ ��� ��������� ����������

// dds

void DdsItemToString(void *menuItem, char target[]);					// ���������� �������������� � ������
ActionProcessed doDdsButtonAction (void *menuItem, eButton button);		// ���������� ������� �� ������� ���������� (������)
void doDdsModeAction(void *menuItem, eAction action);					// ���������� ������� ��������� ������ ������
void loadDdsSettings();													// �������� �������� ������ ��� ��������� ����������

void checkStopFlagAndDoDdsSignalOut();									// ��������������� ��������� ������ �� ����, ������������ � ���

#endif /* COMMON_H_ */
