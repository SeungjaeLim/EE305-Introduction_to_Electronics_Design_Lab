#define F_CPU 		16000000UL
#define MCU_XTAL 	F_CPU

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "firmware.h"

void
Beep (void)
{
	DDRE |= _BV(3);
	
	int k;
	for (k = 0; k < 100; k++)
	{
		PORTE |=  _BV(3);	_delay_us(250);
		PORTE &= ~_BV(3);	_delay_us(250);
	}
}

// RS: PC0 -> PB5
// RW: PC1 -> PB6
// E:  PC2 -> PB7
// D4-D7: PC4-PC7

#define LCD_DELAY	10
#define LCD_RS_1	({PORTB |=  _BV(5); _delay_us(LCD_DELAY);})		// Register select
#define LCD_EN_1	({PORTB |=  _BV(7); _delay_us(LCD_DELAY);})		// Enable
#define LCD_RS_0	({PORTB &= ~_BV(5); _delay_us(LCD_DELAY);})		// Register select
#define LCD_EN_0	({PORTB &= ~_BV(7); _delay_us(LCD_DELAY);})		// Enable
#define LCD_D4_1	({PORTC |=  _BV(4); _delay_us(LCD_DELAY);})		// Data bits
#define LCD_D5_1	({PORTC |=  _BV(5); _delay_us(LCD_DELAY);})		// Data bits
#define LCD_D6_1	({PORTC |=  _BV(6); _delay_us(LCD_DELAY);})		// Data bits
#define LCD_D7_1	({PORTC |=  _BV(7); _delay_us(LCD_DELAY);})		// Data bits
#define LCD_D4_0	({PORTC &= ~_BV(4); _delay_us(LCD_DELAY);})		// Data bits
#define LCD_D5_0	({PORTC &= ~_BV(5); _delay_us(LCD_DELAY);})		// Data bits
#define LCD_D6_0	({PORTC &= ~_BV(6); _delay_us(LCD_DELAY);})		// Data bits
#define LCD_D7_0	({PORTC &= ~_BV(7); _delay_us(LCD_DELAY);})		// Data bits
#define LCD_STROBE	((LCD_EN_1),(LCD_EN_0))

/* write a byte to the LCD in 4 bit mode */
void
_lcd_write (unsigned char c)
{
	if(c & 0x80) LCD_D7_1; else LCD_D7_0;
	if(c & 0x40) LCD_D6_1; else LCD_D6_0;
	if(c & 0x20) LCD_D5_1; else LCD_D5_0;
	if(c & 0x10) LCD_D4_1; else LCD_D4_0;
	LCD_STROBE;
	if(c & 0x08) LCD_D7_1; else LCD_D7_0;
	if(c & 0x04) LCD_D6_1; else LCD_D6_0;
	if(c & 0x02) LCD_D5_1; else LCD_D5_0;
	if(c & 0x01) LCD_D4_1; else LCD_D4_0;
	LCD_STROBE;
	_delay_us(40);
}

/* initialise the LCD - put into 4 bit mode */
void
lcd_init (void)
{
	DDRB |= 0xf0;
	DDRC |= 0xf0;
	
	LCD_RS_0;			// write control bytes
	_delay_ms(15);		// power on delay

	LCD_D4_1;			// init!
	LCD_D5_1;
	LCD_STROBE;
	_delay_ms(5);

	LCD_STROBE;			// init!
	_delay_us(100);

	LCD_STROBE;			// init!
	_delay_ms(5);

	LCD_D4_0;			// set 4 bit mode
	LCD_STROBE;
	_delay_us(40);
	
	_lcd_write(0x28);	// 4 bit mode, 1/16 duty, 5x8 font, 2lines
	_lcd_write(0x0D);	// display on, cursor off, blinking on
	_lcd_write(0x06);	// entry mode advance cursor
	_lcd_write(0x01);	// clear display and reset cursor
}

/* Clear and home the LCD */
void
_lcd_clear (void)
{
	LCD_RS_0;
	_lcd_write(0x1);
	_delay_ms(2);
}

/* write a string of chars to the LCD */
void
_lcd_puts (const char * s)
{
	LCD_RS_1;	// write characters
	while(*s) _lcd_write(*s++);
}

/* write one character to the LCD */
void
_lcd_putch (char c)
{
	LCD_RS_1;	// write characters
	_lcd_write(c);
}

/* Go to the specified position */
void
_lcd_goto (unsigned char pos, unsigned char line)
{
	LCD_RS_0;
	if (line==0)		_lcd_write(0x80 + pos);
	else if (line==1)	_lcd_write(0x80 + pos+ 0x40);
	else if (line==2)	_lcd_write(0x80 + pos+ 20);
	else if (line==3)	_lcd_write(0x80 + pos+ 0x40 + 20);
}

#define SCR_MAX 256

char scr[SCR_MAX];	// screen buffer (circular)

int cmd_start = 0;
int p = 0;	// position for a new character
int ls[20];	// start of line addresses (-1 if not initialized)

int x = 0;	// cursor position
int y = 0;

void 
ClearLCD (void)
{
	int i;
	for (i = 0; i < SCR_MAX; i++) scr[i] = 0;
	for (i = 0; i < 20; i++) ls[i] = -1;
	ls[0] = 0;

	x = y = p = cmd_start = 0;

	_lcd_clear();
	_lcd_goto(0, 0);
}

int 
_scroll_up (void)
{
	if (ls[3] >= 0 && ls[2] >= 0 && ls[1] >= 0 && ls[0] >= 0)
	{
		_lcd_clear();

		int i, j;
		char c;
		for (j = 3; j >= 0; j--)
		{
			_lcd_goto(0, 3 - j);
			for (i = ls[j]; i < ls[j] + 20; i++)
			{
				c = scr[i % SCR_MAX];
				if (c == '\n') break;
				_lcd_putch (c);
			}
		}
		return 1;
	} 
	else 
	{
		return 0;
	}
}

void 
_scroll_down (void)
{
	int i, j;
	char c;

	_lcd_clear();

	for (j = 2; j >= 0; j--)
	{
		_lcd_goto(0, 2 - j);
		if (ls[j] >= 0)
		{
			for (i = ls[j]; i < ls[j] + 20; i++)
			{
				c = scr[i % SCR_MAX];
				if (c == '\n') break;
				_lcd_putch (c);
			}
		}
	}
}

void 
_back_space (void)
{
	int i;
	if (p != cmd_start)
	{
		p--;
		if (p < 0)
		{
			p = SCR_MAX - 1;
		}
		scr[p] = 0;
		x--;
		if (x < 0)
		{
			x = 19;
			for (i = 0; i < 19; i++)
			{
				ls[i] = ls[i + 1];
			}
			ls[19] = -1;

			if (!_scroll_up())
			{
				y--;	// y < 0 can't happen since p != cmd_start
			}
		}
		_lcd_goto(x, y);
		_lcd_putch(' ');
		_lcd_goto(x, y);
	}
	else
	{
		Beep();
	}
}

void 
_nextline (void)
{
	x = 0; y++;
	if (y == 4)
	{
		y = 3;
		_scroll_down();
	}

	int i;
	for (i = 19; i > 0; i--)
	{
		ls[i] = ls[i - 1];
	}
	ls[0] = p;

	_lcd_goto(x, y);
}

void 
lcd_putchar(char c, FILE* stream)
{
	if (c == '\b')
	{
		_back_space();
	}
	else if ((c >= 32 && c <= 127) || (c == '\n'))
	{
		if ((c == '*') && (cmd_start != p) && (stream == 569))
		{
			int q = p;
			q--;
			if (q < 0) q = SCR_MAX - 1;
			if (scr[q] == '*')
			{
				scr[q] = '^';
				int x0 = x;
				int y0 = y;
				x0--;
				if (x0 < 0) 
				{
					x0 = 19;
					y0--;
				}
				_lcd_goto(x0, y0);
				_lcd_putch('^');
				_lcd_goto(x, y);
				return;
			}
		}

		if (c != '\n')
		{
			int size = (p >= cmd_start) ? (p - cmd_start) : (p + SCR_MAX - cmd_start);
			if (size >= SCR_MAX - 1)
			{
				Beep();
				return;
			}
		}

		scr[p++] = c;
		if (p == SCR_MAX) p = 0;
		scr[p] = 0;

		if (c == '\n')
		{
			cmd_start = p;
			_nextline();
		}
		else
		{
			if (stream != 569) cmd_start = p;
			
			_lcd_goto(x, y);
			_lcd_putch(c);

			x++;
			if (x >= 20) _nextline();
		}
	}
}

/*---------------------------------------------------------------------------*/
static FILE lcd_stdout = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

void 
InitCalculator (void)
{
	lcd_init();
	ClearLCD();

	stdout = &lcd_stdout; 
	stderr = &lcd_stdout;
}
