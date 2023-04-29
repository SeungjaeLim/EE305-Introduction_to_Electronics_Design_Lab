#define F_CPU       16000000UL
#define MCU_XTAL    F_CPU

#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "firmware.h"
/*

   External and Timer Interrupt Service Routine

   [Interrupt Vectors]
      INT1_vect         : External Interrupt 1
      INT2_vect         : External Interrupt 2
      INT3_vect         : External Interrupt 3
      INT4_vect         : External Interrupt 4
      INT5_vect         : External Interrupt 5
      INT6_vect         : External Interrupt 6
      INT7_vect         : External Interrupt 7
      
      TIMER0_OVF_vect      : Timer0 Overflow
      TIMER0_COMP_vect    : Timer0 Compare Match
      TIMER2_OVF_vect      : Timer2 Overflow
      TIMER2_COMP_vect    : Timer2 Compare Match

      TIMER1_OVF_vect      : Timer1 Overflow
      TIMER1_COMPA_vect    : Timer1 Compare A Match
      TIMER3_OVF_vect      : Timer3 Overflow
      TIMER3_COMPA_vect    : Timer3 Compare A Match

*/

/* Print ticks to time */
void tick2time(long tick);

/* ticks for stopwatch */
long ticks = 0;

/* identify running status
   true  -> stopwatch running
   false -> stopwatch stop
*/
bool isRunning = false;

ISR(TIMER1_COMPA_vect) // Timer1 Compare A
{
   /* overflow preventing */
   TCNT1 = 0;

   /* Increase tick */
   ticks++;

   /* Print ticks to time */
   tick2time(tick);
}

ISR(INT6_vect) //reset button
{
   /* off clock pulse */
   TCCR1B&=~((1<<2)|(1<<1)|(1<<0));

   /* reset ticks */
   ticks = 0;
   TCNT1 = 0;

   /* setup running status (stop) */
   isRunning = false; 

   /* Print ticks to time */
   tick2time(tick);
   
}

ISR(INT7_vect) // start,stop button
{
   if(!isRunning)
   { 
      /* when stopwatch is stopped, make it run with clk/64 prescaling */
      TCCR1B |= ((0<<2)|(1<<1)|(1<<0));
      isRunning = true;
   }
   else
   {
      /* when stopwatch is running, stop it*/
      TCCR1B &= ~((1<<2)|(1<<1)|(1<<0));
      isRunning = false;
   }
}
/*----------------------------------------------------------*/
int main(void)
{
   InitCalculator();

   cli();   // Disable Global Interrupt

/* Interrupt control setup */
   /* Enable 6, 7 External interrupt */
   EIMSK |= (1<<7) | (1<<6);

   /* Set interrupt sense control bit for falling edge */
   EICRB &= ~((1<<7)|(1<<6)|(1<<5)|(1<<4));
   EICRB |= (1<<7)|(1<<5);
   
/* Button input setup */
   /* set A4 output low */
   DDRA |= 1<<4;
   PORTA &= ~(1<<4);

   /* E6, E7 set 0 for input mode */
   DDRE &= ~(1<<7|1<<6);

   /* pull-up Circuit */
   PORTE |= (1<<7|1<<6); 
   
/* Timer/Counter Interrrupt setup */
   /* initially, off clock */
   TCCR1B &= ~((1<<2)|(1<<1|(1<<0)));
   TCNT1 = 0; 

   /* 1/64 prescaling value */
   OCR1A = 25000;  

   /* enable interrupt */
   TIMSK |= 1<<4;

   sei();   // Enable Global Interrupt
   
   while (1);

   return 0;
}

/* Print ticks to time */ 
void tick2time(long tick)
{
   /* transform ticks to time format */
	long hour = t / 600;
	long minute = (t%600) / 10;
	long second = t%10

   /* Clear LCD and print time */
   ClearLCD();
   printf("%02ld:%02ld:%ld",hour, minute, second);
}