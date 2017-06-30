
/* �����:              Papandopala Papandopalavich
 * ��� �����:          Atmega16_LCD_LIB.c
 * ��� ��:			   Atmega16, Flash 16 Kbytes, SRAM 1 Kbytes, EEPROM 512 bytes.
 * �������� �������:   F_CPU 8000000 ��
 * ����:               28.06.2013 21:17:29
 * ������ ��:          ATMEL STUDIO VERSION 6.1.2562
 * FUSES:              HIGH xx, LOW xx;
 * ��������:           ��������� ���������� ��� ������ � LCD ��������� ������������� �� HD44780 
					   ������������. ������ ���������� �������� �� 4� � 8�� ������ ����� �����������
					   LCD, ������������ ������ ����� ������ �� �������*/ 


#define  F_CPU 16000000UL         
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "LCD.h"

//��������� �������, �� �� �������, ��� ��� ������ �������.
void Send_byte(uint8_t);	//�������� ������ LCD, ���������� ��������� Send_command � Send_data.
void Busy_flag(void);		//�������� ����� ���������.
void HiPin(void);			//��������� ���� � ��������� HI.
void OutPin(void);			//��������� ���� � ��������� "�����".
void InPin(void);			//��������� ���� � ��������� "����".
void Strob(void);			//���/���� ������ �


//���������������� �������, ��� ���������� � ���������.
void LCDinit(void)							//�������������� �������
{
	//����� �������������� LCD, ���� � init �������� � 100 ���� �������, ���� ������� ������������. 
	_delay_ms(100);
	CDDR |=  (1<<RS)|(1<<E)|(1<<RW);   //��������� ������ 
	CPORT&=~((1<<RS)|(1<<E)|(1<<RW));  //��������� ������ 
	OutPin();						   //��������� ������
	
	uint8_t i=0;
	while (i!=3)
	{
		#ifdef  LCD_8BIT       //�������� ������� 0x30.
		        DPORT|=(0<<DB7)|(0<<DB6)|(1<<DB5)|(1<<DB4)|(0<<DB3)|(0<<DB2)|(0<<DB1)|(0<<DB0);
		#else
				DPORT|=(0<<DB7)|(0<<DB6)|(1<<DB5)|(1<<DB4);
		#endif
		
		Strob();
		_delay_ms(5);
		i++;
	}
	
	//����������� ���� ��������, ����� ��������� � �������� �����������.
	
	#ifdef  LCD_8BIT
			LCDcommand(0b00111000);//8�� ������ ���������, ��� ������, 5x8 �����.
	#else   //������ ��� ���������� ������ ��� ������� �������
			Busy_flag();
			OutPin();
			DPORT|=(0<<DB7)|(0<<DB6)|(1<<DB5)|(0<<DB4);//4� ������ ��������� 
			Strob();
			LCDcommand(0b00101000);//��� ������, 5x8 �����.
	#endif
	
	LCDcommand(0b1100);  //�������� ������� + ��� ����������� ��������.
	LCDcommand(0b110);   //������� ������ ������ ����� ��������� �� n+1
	LCDcommand(0b10);    //������ � ������� 0,0 + ����� ���� �������
	LCDcommand(0b1);     //������� ������� 
	//�������� ���������� ���������. ��������� � ����������.
}
void LCDcommand(uint8_t i)					//�������� �������, ��������� �������.
{
	Busy_flag();	//�������� ������ ���� ���������, � �������� �� �������?
	Send_byte(i);
}
void LCDdata(uint8_t i)						//�������� ������� ��� ����������� �� �������.
{
	Busy_flag();	//�������� ������ ���� ���������, � �������� �� �������?
	CPORT|=(1<<RS); //RS=1 �������� ������ � LCD
	Send_byte(i);
	CPORT&=~(1<<RS);//RS=0
}
void LCDGotoXY(uint8_t x,uint8_t y)			//������������� ������ � X, Y �������
{
	 uint8_t Address;
	
	switch(y)
	{
		case 0: Address = LINE0+x; 
				break;
		case 1: Address = LINE1+x; 
				break;
		default: Address = LINE0+x;
	}
	
	LCDcommand(1<<7 | Address);
}
void LCDstring(char *i,uint8_t x,uint8_t y) //������� ������ �� �������.
{
	LCDGotoXY(x,y);
	while( *i )
	{
		LCDdata(*i++ );
	}
}
void LCDstring_of_sram(uint8_t* data,uint8_t nBytes,uint8_t x, uint8_t y)
{
	uint8_t i;
	LCDGotoXY(x,y);
	if (!data) 
	{
		return;
	}
	
	for(i=0; i<nBytes; i++)
	{
		LCDdata(data[i]);
	}
	
	
}
void LCDstring_of_flash(const uint8_t *FlashLoc,uint8_t x,uint8_t y)
{
	uint8_t i;
	LCDGotoXY(x,y);
	for(i=0;(uint8_t)pgm_read_byte(&FlashLoc[i]);i++)
	{
		LCDdata((uint8_t)pgm_read_byte(&FlashLoc[i]));
	}
}
void LCDset(void)			//������������ ������� 5x8 �����
{
	LCDcommand(0b101000);
}
void LCDblank(void)			//������� ��������� ���� �� �������
{
	LCDcommand(0b1000);
}
void LCDnblank(void)		//������� ������� ���� �� ������� + ��������� ������� �������.
{
	LCDcommand(0b1100);
}
void LCDclear(void)			//������� ������� + ������ �� ������� 0,0
{
	LCDcommand(0b1);
}
void LCDcursor_bl(void)		//�������� �������� ������
{
	LCDcommand(0b1101);
}
void LCDcursor_on(void)		//�������� �������������� ������
{
	LCDcommand(0b1110);
}
void LCDcursor_vi(void)		//�������� ��� �������
{
	LCDcommand(0b1111);
}
void LCDcursorOFF(void)		//��������� ������
{
	LCDcommand(0b1100);
}
void LCDacr(void)			//C������ ������ ������ ����� ��������� �� n+1
{
	LCDcommand(0b110);
}
void LCDacl(void)			//C������ ������ ������ ����� ��������� �� n-1
{
	LCDcommand(0b100);
}
void LCDcursorl(void)		//�������� ������ ����� �� 1
{
	LCDcommand(0b10000);
}
void LCDcursorr(void)		//�������� ������ ������ �� 1
{
	LCDcommand(0b10100);
}
void LCDcursorln(uint8_t n)	//�������� ������ ����� �� n ��������
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDcommand(0b10000);
	}
}
void LCDcursorrn(uint8_t n)	//�������� ������ ������ �� n ��������
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDcommand(0b10100);
	}
}
void LCDscreenl(void)		//�������� ����� ����� �� 1
{
	LCDcommand(0b11000);
}
void LCDscreenr(void)		//�������� ����� ������ �� 1
{
	LCDcommand(0b11100);
}
void LCDscreenln(uint8_t n)	//�������� ����� ����� �� n ��������
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDcommand(0b11000);
	}
}
void LCDscreenrn(uint8_t n)	//�������� ����� ������ �� n ��������
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDcommand(0b11100);
	}
}
void LCDscreenL(void)		//� ������ ����� �������� ����� ����� ��������� �����
{
	LCDcommand(0b101);
}
void LCDscreenR(void)		//� ������ ����� �������� ����� ����� ��������� ������
{
	LCDcommand(0b111);
}
void LCDresshift(void)      //���������� ������ � ������� 0,0 + ����� ���� �������, ����������� ��������
{
	LCDcommand(0b10);
}


//��������� �������, �� �� �������, ��� ��� ������ �������.
void Send_byte(uint8_t i)	//�������� ������ LCD, ���������� ��������� Send_command � Send_data.
{
	OutPin();
	
	#ifdef  LCD_8BIT
			if ((i&1)==1)
			{
				DPORT|=(1<<DB0);
			}
			i=i>>1;
			
			if ((i&1)==1)
			{
				DPORT|=(1<<DB1);
			}
			i=i>>1;
			
			if ((i&1)==1)
			{
				DPORT|=(1<<DB2);
			}
			i=i>>1;
			
			if ((i&1)==1)
			{
				DPORT|=(1<<DB3);
			}
			i=i>>1;
			
			if ((i&1)==1)
			{
				DPORT|=(1<<DB4);
			}
			i=i>>1;
			
			if ((i&1)==1)
			{
				DPORT|=(1<<DB5);
			}
			i=i>>1;
			
			if ((i&1)==1)
			{
				DPORT|=(1<<DB6);
			}
			i=i>>1;
			
			if ((i&1)==1)
			{
				DPORT|=(1<<DB7);
			}
			
		    Strob();
			
	#else
			
			uint8_t higb = ((i&0b11110000)>>4);
			uint8_t lowb =  (i&0b00001111);
	
			if ((higb&1)==1)
			{
				DPORT|=(1<<DB4);
			}
			higb=higb>>1;
			if ((higb&1)==1)
			{
				DPORT|=(1<<DB5);
			}
			higb=higb>>1;
			if ((higb&1)==1)
			{
				DPORT|=(1<<DB6);
			}
			higb=higb>>1;
			if ((higb&1)==1)
			{
				DPORT|=(1<<DB7);
			}
	
			Strob();
			OutPin();
	
			if ((lowb&1)==1)
			{
				DPORT|=(1<<DB4);
			}
			lowb=lowb>>1;
			if ((lowb&1)==1)
			{
				DPORT|=(1<<DB5);
			}
			lowb=lowb>>1;
			if ((lowb&1)==1)
			{
				DPORT|=(1<<DB6);
			}
			lowb=lowb>>1;
			if ((lowb&1)==1)
			{
				DPORT|=(1<<DB7);
			}
	
			Strob();
	#endif
	
	HiPin();
}
void Busy_flag(void)		//�������� ����� ���������
{
	InPin();
	CPORT|=(1<<RW);	  //R/W=1 ������ �� LCD
	
	#ifdef  LCD_8BIT
			uint8_t i=1;
			while(i==1)
			{
				CPORT|=(1<<E);//�������� �����.
				asm("nop");
				asm("nop");
				
				if (!(DPIN&(1<<DB7)))//������� � ��������� ���� ���������.
				{
					i=0;
				}
				
				CPORT&=~(1<<E);//��������� �����.
				asm("nop");
				asm("nop");						
			}
	#else
			uint8_t i=1;
			while(i==1)
			{
				CPORT|=(1<<E);//�������� �����.
				asm("nop");
				asm("nop");
		
				if (!(DPIN&(1<<DB7)))//������� � ��������� ���� ���������.
				{
					i=0;
				}
		
				CPORT&=~(1<<E);//��������� �����.
				asm("nop");
				asm("nop");
				Strob();
				asm("nop");
				asm("nop");
		
			}
	#endif
	
	CPORT&=~(1<<RW);   //R/W=0 
}
void HiPin(void)			//��������� ���� � ��������� Hi.
{
	#ifdef  LCD_8BIT
			DDDR =0;
			DPORT=0;
	#else
			DDDR &=~((1<<DB7)|(1<<DB6)|(1<<DB5)|(1<<DB4));
			DPORT&=~((1<<DB7)|(1<<DB6)|(1<<DB5)|(1<<DB4));
    #endif
}
void OutPin(void)			//��������� ���� � ��������� "�����".
{
	#ifdef  LCD_8BIT
			DDDR =0xFF;
			DPORT=0;
	#else
			DDDR |=  (1<<DB7)|(1<<DB6)|(1<<DB5)|(1<<DB4);
			DPORT&=~((1<<DB7)|(1<<DB6)|(1<<DB5)|(1<<DB4));
	#endif
}
void InPin(void)			//��������� ���� � ��������� "����".
{
	#ifdef  LCD_8BIT
			DDDR =0;
			DPORT=0xFF;
	#else
			DDDR &=~((1<<DB7)|(1<<DB6)|(1<<DB5)|(1<<DB4));
			DPORT|=  (1<<DB7)|(1<<DB6)|(1<<DB5)|(1<<DB4);
	#endif
}
void Strob(void)            //�������� ��������� ��������.
{
	CPORT|=(1<<E);//�������� �����.
	_delay_us(1);
	CPORT&=~(1<<E);//��������� �����.
}

