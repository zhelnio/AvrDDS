/*
 * actionManager.c
 *
 * Created: 01.01.2014 22:33:17
 *  Author: Stanislav Zhelnio
 */ 

#include "common.h"

MenuItem *currentItem;

//������������� ����� �� LCD (���������� \n)
//���������� ����� ������ ������, �� ������� ��� ��������� �����
//��� -1 ���� ������ ��� ������
uint8_t LCDPrint(char *string, uint8_t lineNum)
{
	const uint8_t maxLineNum = 1;
	uint8_t line = lineNum;
	
	if(line > maxLineNum)
		return -1;
	
	LCDGotoXY(0,line);
	
	while (*string != '\0')
	{
		if(*string != '\n')
			LCDdata(*string );
		else
		{
			line++;
			if(line > maxLineNum)
				return maxLineNum;
			LCDGotoXY(0,line);
		}
		
		string++;
	}
	return line;
}

//� ����������� �� ������� ��� �������� ������ ���� �����������
//��������� ������������ ������������ (�������� "��������=��������")
//���������� ���, ���� ������ ������������ ������ ����
char* getMenuItemCaption(MenuItem *menuItem)
{
	static char buffer[33];
	if(menuItem->toString == NULL)
		return menuItem->Caption;
	
	menuItem->toString(menuItem,buffer);
		return buffer;
}

//��������������� ������������ ����� �������� ������ ����
//�� ����� � �������� ��� ����������� �������
void showItemOnLCD(MenuItem *menuItem)
{
	LCDclear();

	char *caption = getMenuItemCaption(menuItem);
	uint8_t curline = LCDPrint(caption,0);
	
	if(curline == 0 && menuItem->NextId != mNothing)
	{
		caption = getMenuItemCaption(&globalMenuData[menuItem->NextId]);
		LCDPrint(caption,1);
	}
}

//����������� ��� ������ � �������������� ����
void initMenu(MenuItem *startMenuItem)
{
	//������������� ������� ����� ����
	currentItem = startMenuItem;
	
	//��� ����� � ����� - ������� "�����������"
	if(currentItem->modeAction != NULL)
		currentItem->modeAction(currentItem, onEnter);
		
	//��������� ��������� �� ������
	showItemOnLCD(currentItem);
}

//�������� ���������� ������� �� ������ ����������
void doAction(eButton button)
{
	//���������� ���������� ��� �������� ��������
	ActionProcessed buttonActionResult = aProcessedNone;
	if(currentItem->buttonAction != NULL)
		buttonActionResult = currentItem->buttonAction(currentItem, button);
		
	//���� ���������� ���� � ��������� �� ��������� - ���������� �� ��������� ����� �� �����
	if(buttonActionResult == aProcessedAll)
	{
		//�������� ���������� ����������
		if(currentItem->modeAction != NULL)
			currentItem->modeAction(currentItem, onChange);
		
		//��������� ��������� �� ������
		showItemOnLCD(currentItem);
		return;
	}

	//���������� ��������� ����� ���� ��� ��������
	uint8_t nextItemId = mNothing;
	switch (button)
	{
		case buttonUp:		nextItemId = currentItem->NextId; break;
		case buttonDown:	nextItemId = currentItem->PreviousId; break;
		case buttonLeft:	nextItemId = currentItem->ParentId; break;
		case buttonRight:	nextItemId = currentItem->ChildId; break;
	}
	
	//���� ���������� ������
	if(nextItemId == mNothing)
		return;

	MenuItem *nextItem = &globalMenuData[nextItemId];
	
	//��� ������ � �������� ������ (����� �������� ����) - �������� "����������"
	if(nextItem->Id != currentItem->ChildId)
		if(currentItem->modeAction != NULL)
			currentItem->modeAction(currentItem, onExit);
	
	//��������������� ��� �������
	currentItem = nextItem;
	
	//��� ����� � ����� - ������� "�����������"
	if(currentItem->modeAction != NULL)
		currentItem->modeAction(currentItem, onEnter);
	
	showItemOnLCD(currentItem);
}

//////////////////////////////////////////////////////////////////////////
// ����� ����� ������� ��� ������ � ���������� ����
// ������������ � ��������� ������������ *toString

// ��������� ��������� "��������="
uint8_t PrepareItemValueString(void *menuItem, char target[])
{
	MenuItem *item = (MenuItem *)menuItem;
	uint8_t i = 0;
	
	i += ui_strcpy(target, item->Caption);
	
	target[i++]='=';
	
	return i;
}

// ��� ������� �������������� ��������� ������ ������� LCD � ������� ������ ������
uint8_t AddModeToItemValueString(ParameterMode mode, char target[])
{
	switch (mode)
	{
		case modeEditOrder0: return ui_strcpy(target, "\nfine tuning");
		case modeEditOrder1: return ui_strcpy(target, "\nflat tuning");
		
		case modeView: //� ������ ��������� ����������� �� ���������
		default: 
			return 0; 
	}
}

// ��������� ������������ �������� ������� ������ (������� ������ - �������� ��� ���)
uint8_t MainItemWithStateToString(void *menuItem, char target[], uint8_t status)
{
	MenuItem *item = (MenuItem *)menuItem;
	uint8_t i = 0;
	
	i += ui_strcpy(target, item->Caption);
	
	while(i < 8)
		target[i++]=' ';
	
	i += ui_strcpy(&target[i], (status) ? "Online" : "Offline" );

	return i;
}

//������� ������������ �����������
//�� �������� \0, ���������� ������� ������ � ������� - ������ ��� �������
inline uint8_t ui_strcpy(char target[], char source[])
{
	uint8_t i = 0;
	while (source[i] != '\0')
	{
		target[i] = source[i];
		i++;
	}
	return i;
}