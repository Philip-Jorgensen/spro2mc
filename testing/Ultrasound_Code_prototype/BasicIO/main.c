


#define F_CPU 16000000UL

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"
#include <avr/interrupt.h>
#include "PCA9685_ext.h"
#include "i2cmaster.h"

volatile unsigned int pulse=0;//time echo pin signal is high
volatile int i=0;//used for identifying edge type

double AverageDistance(double );

int main(void) {
	uart_init();
	io_redirect();
	
	DDRD=0xFB;//set PD2 input, rest as output
	double distance=0;//distance measured
	unsigned long counter=0;//used with printfs to avoid delay
	unsigned long lstC=0;//used with printfs to avoid delay
	
	EICRA |= 1<<ISC00;//set INT0(PD2) to trigger on any logic change
	EIMSK |=1<<INT0;//turn on interrupt
	sei();//enable global interrups
	PORTD|=1<<PIND4;//trig pin output to ultrasonic, set PD4 high
	_delay_us(10);//needs 10us pulse to start
	PORTD&=~(1<<PIND4);//set PD4 to low
	
	while(1){
	
		distance=((double)pulse)*0.0000000625*342.2/2;//pulse*time for one tick (1/16mhz)*speed of sound(20C)/2 
		counter++;//just for printf
		if(counter-lstC>50000){//do the printfs every half a second or so
		lstC=counter;
		printf("distance %f \n",distance);
		printf("pulse %u \n\n",pulse);
		//printf("average: %f \n",AverageDistance(distance));
		}
	
	}
	return 0;
}

ISR(INT0_vect){//interrupt routine
	if(i==0)// low to high
	{
		
		TCCR1B |= (1<<CS10);//start timer 1 (16bits) with no prescaler
		i = 1;
	}
	else//high to low
	{
		TCCR1B = 0;//stop timer1
		pulse = TCNT1;//value from timer1
		TCNT1 = 0;//reset
		i = 0;
	}
}
double AverageDistance(double distance){//just sth that i copied from last semester to get an average value, can be ignored
	static double arrSpeedsForAvg[]={0,0,0,0,0,0,0,0,0,0};
	const int arrSpeedsLength = 10;
	static double arrAvg=0;
	
	for(int i=arrSpeedsLength;i>0;i--){
		arrSpeedsForAvg[i]=arrSpeedsForAvg[i-1];
	}
	arrSpeedsForAvg[0]=distance;
	
	for(int i=0;i<arrSpeedsLength;i++){
		arrAvg+=arrSpeedsForAvg[i];
	}
	arrAvg=arrAvg/(double)arrSpeedsLength;
	return arrAvg;
}
