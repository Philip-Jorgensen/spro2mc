/*
 * Test.c
 *
 * Created: 5/27/2021 10:10:40 AM
 * Author : Philip J
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    DDRD = 0xFF;
	
    while (1) 
    {
		PORTD |= (1<<7);
		_delay_ms(500);
		PORTD &= ~(1<<7);
		_delay_ms(500);
    }
}

