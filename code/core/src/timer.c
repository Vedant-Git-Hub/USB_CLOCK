#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"


static timer_callback_func_ptr timer_callback = NULL;
static volatile uint32_t get_ticks_ms = 0;


ISR(TIMER0_COMPA_vect)
{
    if(timer_callback != NULL)
    {
        timer_callback();
    }

    get_ticks_ms++;
}


void timer_init(timer_callback_func_ptr callback)
{
    //Normal timer operation mode with OC)A disconnected.
    TCCR0A &= ~(1 << COM0A1);
    TCCR0A &= ~(1 << COM0A0);
    //CTC mode on.
    TCCR0A |= (1 << WGM01);
    TCCR0A &= ~(1 << WGM00);
    TCCR0B &= ~(1 << WGM02);
    //Select Prescalar
    TCCR0B &= ~(1 << CS02);
    TCCR0B |= (1 << CS01);
    TCCR0B |= (1 << CS00); 

    timer_callback = callback;
}

void timer_start_1_ms()
{
    TCNT0 = 0; //Start timer at this value.
    OCR0A = 249; //Generates a 1ms delay with this count.
    TIMSK0 |= (1 << OCIE0A); //Enable interrupt 
}

uint32_t timer_getTicks()
{
    uint32_t ticks = 0;

    cli();
    ticks = get_ticks_ms;
    sei();

    return ticks;
}

void timer_stop_1_ms()
{
    TCNT0 = 0;
    TIMSK0 &= ~(1 << OCIE0A); //Disable interrupt 
}