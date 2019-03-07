/*
 * digital_7_segment_clock.c
 *
 * Initialized: 25/02/2019 16:35:38
 * Last edited: 19:19 Thursday,07 Maret 2019
 * Author : Muhammad Rafly Arya Putra (13216064) 
 			Muhammad Irfaan S. (13216047)
			Gusti Triandi Winata (13216091)
* development repository : https://github.com/sanggusti/avr_digital_clock
 */ 

#define CLOCK_DISPLAY 0
#define HOUR_SET 1
#define MINUTE_SET 2
#define SECOND_SET 3
#define MAX_STATE 4

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


volatile int state = CLOCK_DISPLAY;
volatile int clk_hour = 23;
volatile int clk_minute = 59;
volatile int clk_second = 30;

void Init_Ext_Int(void);
void init_int(void);
void clockLimitter(void);
//void showClock (int hh, int mm, int ss);
void showHour (int hh);
void showMinute (int mm);
void showSecond (int ss);
void showDigit (int number, int digit);
void digitOn(int digit);
void digitOff(int digit);
void segmentDisplay(int number);

ISR(TIMER1_COMPA_vect)
{
	if (state==CLOCK_DISPLAY)
		clk_second++;
}

ISR (INT1_vect)
{
	//function to change hour, minute, or second
	if (state==HOUR_SET) {
		clk_hour++;
		if (clk_hour>23)
			clk_hour=0;
	}
	else if (state==MINUTE_SET) {
		clk_minute++;
		if (clk_minute>59)
			clk_minute=0;
	}
	else if (state==SECOND_SET) {
		clk_second++;
		if (clk_second>59)
			clk_second=0;
	}
}

ISR (INT0_vect)
{
	//function to change the state of the clock
	state++;
	if (state>=MAX_STATE)
		state=CLOCK_DISPLAY;
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
		if (state==CLOCK_DISPLAY) {
			clockLimitter();
			showHour(clk_hour);
			showMinute(clk_minute);
			showSecond(clk_second);
		}
		else if (state==HOUR_SET)
			showHour(clk_hour);
		else if (state==MINUTE_SET)
			showMinute(clk_minute);
		else if (state==SECOND_SET)
			showSecond(clk_second);
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
	DDRD &= ~(1 << DDD3);     // Clear the PD3 pin
	// PD2 (PCINT0 pin) and PD3 (PCINT1) are now an input
	PORTD |= (1 << PORTD2);    // turn On the Pull-up
	PORTD |= (1 << PORTD3);    // turn On the Pull-up
	// PD2 and PD3 are now an input with pull-up enabled

	EICRA |= (1 << ISC01);    // set INT0 to trigger on falling edge
	EICRA |= (1 << ISC11);    // set INT1 to trigger on falling edge
	EIMSK |= (1 << INT0);     // Turns on INT0
	EIMSK |= (1 << INT1);     // Turns on INT1
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

void showHour (int hh) {
	for (int digit= 2; digit >=1; digit--)
	{
		showDigit(hh % 10, digit);
		hh= hh/10;
	}
}

void showMinute (int mm) {
	for (int digit= 4; digit >=3; digit--)
	{
		showDigit(mm % 10, digit);
		mm= mm/10;
	}
}

void showSecond (int ss) {
	for (int digit= 6; digit >=5; digit--)
	{
		showDigit(ss % 10, digit);
		ss= ss/10;
	}
}

/*void showClock (int *hh, int *mm, int *ss) {
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
}*/

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