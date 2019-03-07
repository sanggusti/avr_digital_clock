/*
 * digital_7_segment_clock.c
 *
 * Created: 25/02/2019 16:35:38
 * Author : Muhammad Rafly Arya Putra (13216064) & Muhammad Irfaan S. (13216047)
 */ 

#define CLOCK 0
#define  1
#define CLOCK 2
#define CLOCK 3

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


volatile int state = CLOCK;
int clk_hour = 21;
volatile int clk_minute = 42;
volatile int clk_second = 00;

void Init_Ext_Int(void);
void init_int(void);
void clockLimitter(void);
void showClock (int hh, int mm, int ss);
void showDigit (int number, int digit);
void digitOn(int digit);
void digitOff(int digit);
void segmentDisplay(int number);

ISR(TIMER1_COMPA_vect)
{
	clk_second++;
}

ISR (INT0_vect)
{
	/* interrupt code here */
	clk_minute++;
}

int main(void)
{
    /* inisialisasi output pin */
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFF;
    /* inisialisasi interrupt */
    Init_Ext_Int();
    init_int();

	while (1) {
		clockLimitter();
		showClock(clk_hour,clk_minute,clk_second);
	}
	
	return 0;
}

void init_int(void)
{
    OCR1A = 0x3D08;
    TCCR1B |= (1 << WGM12); //Mode 4, CTC on OCR1A
    TIMSK1 |= (1 << OCIE1A); //Mengatur interrupt untuk terpanggil ketika compare match
    TCCR1B |= (1 << CS12) | (1 << CS10); //Mengatur prescaler menjadi 1024 dan memulai timer
    sei(); // Mengaktifkan interrupt
}

void Init_Ext_Int(void)
{
	DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
	// PD2 (PCINT0 pin) is now an input

	PORTD |= (1 << PORTD2);    // turn On the Pull-up
	// PD2 is now an input with pull-up enabled

	EICRA |= (1 << ISC01);    // set INT0 to trigger on falling edge
	EIMSK |= (1 << INT0);     // Turns on INT0
}

void clockLimitter(void) {
    if (clk_second > 59) {
	    clk_second -= 60;
	    clk_minute++;
	    if (clk_minute > 59) {
		    clk_minute -= 60;
		    clk_hour++;
		    if (clk_hour > 23) {
			    clk_hour = 0;
		    }
	    }
    }
}

void showClock (int hh, int mm, int ss) {
	for (int digit= 6; digit >=5; digit--)
	{
		showDigit(ss % 10, digit);
		ss= ss/10;
	}
	for (int digit= 4; digit >=3; digit--)
	{
		showDigit(mm % 10, digit);
		mm= mm/10;
	}
	for (int digit= 2; digit >=1; digit--)
	{
		showDigit(hh % 10, digit);
		hh= hh/10;
	}
}

void showDigit (int number, int digit) {
	digitOn(digit);
	segmentDisplay(number);
	_delay_ms(3);
	digitOff(digit);
}

void digitOn(int digit) {
	if (digit==1)
		PORTC|=0x4;
	else if (digit==2)
		PORTD|=0x10;
	else if (digit==3)
		PORTB|=0x4;
	else if (digit==4)
		PORTB|=0x8;
	else if (digit==5)
		PORTB|=0x10;
	else if (digit==6)
		PORTB|=0x2;
}

void digitOff(int digit) {
	if (digit==1)
		PORTC&=~0x4;
	else if (digit==2)
		PORTD&=~0x10;
	else if (digit==3)
		PORTB&=~0x4;
	else if (digit==4)
		PORTB&=~0x8;
	else if (digit==5)
		PORTB&=~0x10;
	else if (digit==6)
		PORTB&=~0x2;
}

void segmentDisplay(int number) {
	if (number==0) {
		PORTD|=0x40;
		PORTB&=~0x1;
		PORTC&=~0b00111000;
		PORTD&=~0b10100000;
	}
	else if (number==1) {
		PORTD|=0b11100000;
		PORTC|=0b00110000;
		PORTC&=~0x8;
		PORTB&=~0x1;
	}
	else if (number==2) {
		PORTB|=0x1;
		PORTC|=0x10;
		PORTC&=~0b00101000;
		PORTD&=~0b11100000;
	}
	else if (number==3)	{
		PORTD|=0x80;
		PORTC|=0x10;
		PORTB&=~0x1;
		PORTC&=~0b00101000;
		PORTD&=~0b01100000;
	}
	else if (number==4) {
		PORTC|=0x20;
		PORTD|=0b10100000;
		PORTB&=~0x1;
		PORTC&=~0b00011000;
		PORTD&=~0x40;
	}
	else if (number==5) {
		PORTC|=0x8;
		PORTD|=0x80;
		PORTB&=~0x1;
		PORTC&=~0b00110000;
		PORTD&=~0b01100000;
	}
	else if (number==6) {
		PORTC|=0x8;
		PORTB&=~0x1;
		PORTC&=~0b00110000;
		PORTD&=~0b11100000;
	}
	else if (number==7) {
		PORTC|=0x10;
		PORTD|=0b11100000;
		PORTB&=~0X1;
		PORTC&=~0b00101000;
	}
	else if (number==8) {
		PORTB&=~0x1;
		PORTC&=~0b00111000;
		PORTD&=~0b11100000;
	}
	else if (number==9) {
		PORTD|=0x80;
		PORTB&=~0x1;
		PORTC&=~0b00111000;
		PORTD&=~0b01100000;
	}
}