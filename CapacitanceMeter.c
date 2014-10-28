// ECE 4760 Lab 1 - Capacitor Measurement Device
// Written by William Milne (wmm46) and Dustin Franco (dmfxx)

#define F_CPU 16000000UL
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h" //Why is this different from the <> method of inclusion?
#include <util/delay.h> // needed for lcd_lib
#include "lcd_lib.h"
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

//time for task 1
#define t1_thresh 200 //Sets the time threshold equal to 200 ms
//Define Resistor Value
#define resistor 10000
//task subroutine
void task1(void);

void initialize(void); //Taken from Professor Land's TimersGCC644.c

void init_lcd(void);

//float capeq(int t, int resistor);

float capac = 0.0;

int LEDcount = 0;

int CompTime = 0;

volatile int t1_curr = 0;

char capmes[20];

//float epon = .00000001;
float epon = 10;
//lcd ththings
const int8_t LCD_initialize[] PROGMEM = "LCD Initialized\0";
const int8_t LCD_line[] PROGMEM = "line 1\0";
const int8_t LCD_number[] PROGMEM = "Number=\0";

int8_t lcd_buffer[17]; // LCD display buffer

//timer 0 overflow ISR - Compare-match timer.  Counts until TCNT0 is matched (I think)
ISR (TIMER0_COMPA_vect)
{
	//Keeps track of how long task1 has been running
	if(t1_curr < 199)
	{
	t1_curr++;
	}
//	fprintf(stdout,"%d\n\r",t1_curr);
}

ISR (TIMER1_CAPT_vect)
{
	CompTime=(float) ICR1;
}

ISR(TIMER2_COMPA_vect)
{
//PORTD= PORTD^=0b00000100;
}

//Task 1


int main(void)
{
	
	LCDinit(); //initialize the display
	LCDcursorOFF();
	LCDclr(); //clear the display
	initialize();
	
	while(1)
	{
	//fprintf(stdout,"Left Initialize\n\r");
		//After 200 Milliseconds, run task 1, reset t1_curr counter
		if (t1_curr==199)
		{
		//fprintf(stdout,"Starting Task 1\n\r");
			
			task1();
			t1_curr=0;
		}

	}
}



void task1(void)
{
	//fprintf(stdout,"In Task 1/n/r");
	DDRB =  0b11110111;
	PORTB= 0b00000000;
	//Converts ICR1 to Seconds and does calculation
	capac = CompTime*6.25*epon/(resistor);
	if(CompTime<100)
	{
		sprintf(capmes, "No Cap Detected");
	}
	if(CompTime>=100)
	{
		sprintf(capmes, "Cap = %3.1f nF", capac);
	}
	LCDGotoXY(0,0);
	LCDclr();
	LCDstring(capmes, strlen(capmes));
	//fprintf(stdout,"/n/r");
	TCNT1=0;
	DDRB = 0b11110011;




	//turn off led


	PORTD= PORTD^=0b00000100;
	//turn on led
	
	

}









void initialize(void)
{
DDRD=0b11111111;
//set up timer 0 for 1 mSec timebase
//Turn on timer 0 cmp match ISR
TIMSK0 = (1<<OCIE0A); //Timer Counter 0 Interrupt Mask Register || Output Compare Match Interrupt Enable
OCR0A = 249; //set the compare re to 250 time ticks || Output Compare Register
//set prescalar to divide by 64
TCCR0B = 3; //0b00000011;     //Only the last three bits count here.
//turn on clear-on-match
TCCR0A = (1<<WGM01); //Timer/Counter Control Register ||Waveform Generation Mode

//Set up timer1 for full speed and
//caputer an edge on analog comparator pin B.3
//Set capture to positive Edge, full counting rate
//NOTE: having +1 in our TCCR#B registers USUALLY sets the timers to no prescalar (x/64 prescalar is 3)
TCCR1B = (1<<ICES1) + 1; //TCCR1B || Interrupt Capture Edge Select (high means cap on positive edge)
//Turn on timer1 interrupt-on-capture
TIMSK1 = (1<<ICIE1); //Timer Counter 1 Interrupt Mask Register

// Set analog comp to connect to timer capture input
// and turn on the band gap reference on the positive input
ACSR =  (1<<ACIC) ; //CHANGE- removed BG compare enable || Analog Comparatoror control and Status Register || Analog Comparator Input Capture Enable
// Comparator negative input is B.3
DDRB = 0;


TIMSK2 = (1<<OCIE2A); //Timer Counter 0 Interrupt Mask Register || Output Compare Match Interrupt Enable
OCR2A = 124500; //set the compare re to 250 time ticks || Output Compare Register
//set prescalar to divide by 64
TCCR2B = 3; //0b00000011;     //Only the last three bits count here.
//turn on clear-on-match
TCCR2A = (1<<WGM01); //Timer/Counter Control Register ||Waveform Generation Mode






//init the task timer
//time1=t1;

//init the UART -- uart_init() is in uart.c
uart_init();
stdout = stdin = stderr = &uart_str;


//crank up the ISRs
sei();
fprintf(stdout,"Starting timers...\n\r");
}
