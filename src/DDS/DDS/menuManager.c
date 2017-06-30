/*
 * actionManager.c
 *
 * Created: 01.01.2014 22:33:17
 *  Author: Stanislav Zhelnio
 */ 

#include "common.h"

MenuItem *currentItem;

//многострочный вывод на LCD (распознает \n)
//возвращает номер строки экрана, на которой был последний вывод
//или -1 если строка вне экрана
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

//в зависимости от наличия для текущего пункта меню обработчика
//выдающего динамеческое наименование (например "параметр=значение")
//возвращает его, либо только наименование пункта меню
char* getMenuItemCaption(MenuItem *menuItem)
{
	static char buffer[33];
	if(menuItem->toString == NULL)
		return menuItem->Caption;
	
	menuItem->toString(menuItem,buffer);
		return buffer;
}

//непосредственно осуществляет вывод текущего пункта меню
//на экран и вызывает все необходимые функции
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

//запускается при старте и инициализирует меню
void initMenu(MenuItem *startMenuItem)
{
	//устанавливаем текущий пункт меню
	currentItem = startMenuItem;
	
	//при входе в пункт - вызваем "конструктор"
	if(currentItem->modeAction != NULL)
		currentItem->modeAction(currentItem, onEnter);
		
	//обновляем иформацию на экране
	showItemOnLCD(currentItem);
}

//основной обработчик нажатий на кнопки управления
void doAction(eButton button)
{
	//определяем обработчик для текущего действия
	ActionProcessed buttonActionResult = aProcessedNone;
	if(currentItem->buttonAction != NULL)
		buttonActionResult = currentItem->buttonAction(currentItem, button);
		
	//если обработчик есть и отработал не вхолостую - переходить на следующий пункт не нужно
	if(buttonActionResult == aProcessedAll)
	{
		//вызываем обработчик обновления
		if(currentItem->modeAction != NULL)
			currentItem->modeAction(currentItem, onChange);
		
		//обновляем иформацию на экране
		showItemOnLCD(currentItem);
		return;
	}

	//определяем следующий пункт меню для перехода
	uint8_t nextItemId = mNothing;
	switch (button)
	{
		case buttonUp:		nextItemId = currentItem->NextId; break;
		case buttonDown:	nextItemId = currentItem->PreviousId; break;
		case buttonLeft:	nextItemId = currentItem->ParentId; break;
		case buttonRight:	nextItemId = currentItem->ChildId; break;
	}
	
	//если переходить некуда
	if(nextItemId == mNothing)
		return;

	MenuItem *nextItem = &globalMenuData[nextItemId];
	
	//при выходе с текущего пункта (кроме перехода вниз) - вызываем "деструктор"
	if(nextItem->Id != currentItem->ChildId)
		if(currentItem->modeAction != NULL)
			currentItem->modeAction(currentItem, onExit);
	
	//непосредственно сам переход
	currentItem = nextItem;
	
	//при входе в пункт - вызваем "конструктор"
	if(currentItem->modeAction != NULL)
		currentItem->modeAction(currentItem, onEnter);
	
	showItemOnLCD(currentItem);
}

//////////////////////////////////////////////////////////////////////////
// набор общих функций для работы с элементами меню
// используются в различных обработчиках *toString

// формирует подстроку "параметр="
uint8_t PrepareItemValueString(void *menuItem, char target[])
{
	MenuItem *item = (MenuItem *)menuItem;
	uint8_t i = 0;
	
	i += ui_strcpy(target, item->Caption);
	
	target[i++]='=';
	
	return i;
}

// для режимов редактирования формирует вторую строчку LCD с кратким именем режима
uint8_t AddModeToItemValueString(ParameterMode mode, char target[])
{
	switch (mode)
	{
		case modeEditOrder0: return ui_strcpy(target, "\nfine tuning");
		case modeEditOrder1: return ui_strcpy(target, "\nflat tuning");
		
		case modeView: //в режиме просмотра детализацию не добавляем
		default: 
			return 0; 
	}
}

// формирует наименования основных режимов работы (включая статус - работает или нет)
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

//обычное посимвольное копирование
//не копирует \0, возвращает текущий индекс в массиве - удобно для отладки
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