


#define F_CPU 16000000UL

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"
#include <avr/interrupt.h>
#include "PCA9685_ext.h"
#include "i2cmaster.h"

int main(void)
{
	uart_init();
	i2c_init();
	io_redirect();
	int n = 0;
	
	DDRB = 0x00;
	while(1)
	{
	if (PINB == 0b0001000)
	{
		printf("I am working\n");
		printf("%d \n", n);
		_delay_ms(1000);
		n ++;
		PINB = 0b0000000;
	}	
	}
	
}