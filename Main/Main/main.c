/*
SPRO2MC
main.c
2021

Remember to add proper comments and explanations when you make changes.
*/

// Definitions

#define F_CPU 16000000UL

#define RIGHT 1;
#define LEFT -1;

// Imports

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "usart.h"
#include "PCA9685_ext.h"
#include "i2cmaster.h"
#include "functions.h"

// Function prototypes are written in "functions.h"

struct Motors {

	// Motor IDs
	
	unsigned char G_Grabbers  = M1;
	unsigned char G_Elbows    = M2;
	unsigned char G_Shoulders = M3;
	
	unsigned char P_Elbows	  = M4;
	unsigned char P_Shoulders = M5;
	unsigned char P_Grabbers  = M6;
};

// ULTRASONIC SENSOR setup #1
volatile unsigned int pulse = 0; // time echo pin signal is high
volatile int i = 0;				 // used for identifying edge type
int bar_number = 0;

volatile unsigned long millis = 0;

int main(void){
	
	uart_init();   // open the communication to the micro controller
	i2c_init();    // initialize the i2c communication.
	io_redirect(); // redirect the input/output to the computer.
	
	// --------------------------- SENSOR and INTERRUPT setup ---------------------------
	
	DDRB = 0x00;
	DDRD = 0xFB;			 // set PD2 input, rest as output
	
	double distance=0;		 // distance measured
	unsigned long counter=0; // used with printfs to avoid delay
	unsigned long lstC=0;	 // used with printfs to avoid delay
	
	// ULTRASONIC SENSOR setup #2
	EICRA |= 1<<ISC00; // set INT0(PD2) to trigger on any logic change
	EIMSK |=1<<INT0;   // turn on interrupt
	
	// COUNTER (1 ms)
	TCCR0A|=(1<<WGM01);	 // set timer to ctc
	OCR0A=0xF9;			 // set value to count to
	TIMSK0|=(1<<OCIE0A); // enable interrupt for on compare a for timer 0
	
	sei();						 // enable global interrups
	TCCR0B|=(1<<CS01)|(1<<CS00); // set prescaler to 64
	PORTD|=1<<PIND4;			 // trig pin output to ultrasonic, set PD4 high
	_delay_us(10);				 // needs 10us pulse to start
	PORTD&=~(1<<PIND4);			 // set PD4 to low
	
	// --------------------------- MOTORS setup ---------------------------
	
	struct Motors motors;
	
	// Make sure all the motors are stopped from the beginning (Initialization)
	
	motor_init_pwm(PWM_FREQUENCY_1500);
	
	printf("Adafruit 1438\n");
	
	// M1,..,M4 are ports on the "Adafruit 1438"
	
	motor_set_state(M1, STOP);
	motor_set_state(M2, STOP);
	motor_set_state(M3, STOP);
	motor_set_state(M4, STOP);
	motor_set_state(M5, STOP);
	motor_set_state(M6, STOP);
	motor_set_state(M7, STOP);
	
	// ---------------------------------------------------------------------------------
	
	millis = 0;
	
    while(1){
		
		//Frederik's code (time-based motion)
		/*
		//start with the movement
		
		moveMotor(motors.G_Elbow_L,-1,700,millis);// move the left green elbow motor counterclockwise with signal of 1 for 500ms
		moveMotor(motors.GR_Elbow_R,-1,700,millis);
		if(millis>1500)
			openGrabbers(motors.GR_Grabber,millis);
		if(millis>2000){
			moveMotor(motors.G_Elbow_L,1,200,millis);
			moveMotor(motors.GR_Elbow_R,1,200,millis);
		if (millis>2300){
			moveMotor(motors.PR_Shoulder_L,-1,300,millis);//example move the left green elbow motor counterclockwise with signal of 1 for 500ms
			moveMotor(motors.PR_Shoulder_R,-1,300,millis);
			moveMotor(motors.GR_Shoulder_L,-1,300,millis);//example move the left green elbow motor counterclockwise with signal of 1 for 500ms
			moveMotor(motors.GR_Shoulder_R,-1,300,millis);
		}
		if (detectBarGrabbers())
			closeGrabbers(motors.PR_Grabber,millis);
		}
		//now the green grabbers should be on the second bar and the purple grabbers on the first bar
		
		*/
		
		//New code (angle-based motion which also relies a bit on time)
		
		switch(bar_number){
			case (0):
			start_c_brachiation();
			break;
			case (1):
			c_brachiation(135, RIGHT, bar_number);
			break;
			case (2):
			c_brachiation(135, RIGHT, bar_number);
			break;
			case (3):
			c_brachiation(231, RIGHT, bar_number);
			break;
			case (4):
			start_r_brachiation();
			r_brachiation(406);
			break;
			case (5):
			r_brachiation(406);
			break;
			case (6):
			r_brachiation();
			break;
			case (7):
			r_brachiation();
			finish_r_brachiation()
			break;																										
		}
	}
}

//INTERRUPT ROUTINE for ultrasonic sensor

ISR(INT0_vect){
	
	if(i==0){ // low to high
		TCCR1B |= (1<<CS10); // start timer 1 (16bits) with no prescaler
		i = 1;
	} else { //high to low
		
		TCCR1B = 0;     //stop timer1
		pulse  = TCNT1; //value from timer1
		TCNT1  = 0;     //reset
		i = 0;
	}
}

//INTERRUPT ROUTINE for the COUNTER (1ms)

ISR(TIMER0_COMPA_vect){
	millis++;
}
