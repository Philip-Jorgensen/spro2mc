/*
SPRO2MC
main.c
2021

Remember to add proper comments and explanations when you make changes.
*/

// Definitions
#define F_CPU 16000000UL

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

// Function Prototypes
void closeGrabbers(unsigned char,unsigned long);
void openGrabbers(unsigned char,unsigned long);
void control_motor(unsigned char, int);
double detectBarGrabbers();
double readUltrasonic(unsigned int);
void moveMotor(unsigned char, int, int,unsigned long);
double readAccleration(char);
int BarDetected();

struct Motors 
{
	unsigned char GR_Elbow_L;//motor ids for the equivalent motors in the simulation
	unsigned char PR_Elbow_L;
	unsigned char GR_Shoulder_L;
	unsigned char PR_Shoulder_L;
	unsigned char GR_Grabber;
	unsigned char PR_Grabber;
	unsigned char GR_Elbow_R;
	unsigned char PR_Elbow_R;
	unsigned char GR_Shoulder_R;
	unsigned char PR_Shoulder_R;
};
//For ultrasonic
volatile unsigned int pulse=0;//time echo pin signal is high
volatile int i=0;//used for identifying edge type

volatile unsigned long millis=0;

int main(void){
	
	uart_init(); // Open the communication to the micro controller
	i2c_init(); // Initialize the i2c communication.
	io_redirect(); // Redirect the input/output to the computer.
	
	//SENSOR and INTERRUPT setup
	DDRB = 0x00;
	DDRD=0xFB;//set PD2 input, rest as output
	double distance=0;//distance measured
	unsigned long counter=0;//used with printfs to avoid delay
	unsigned long lstC=0;//used with printfs to avoid delay
	
	//for ultrasonic
	EICRA |= 1<<ISC00;//set INT0(PD2) to trigger on any logic change
	EIMSK |=1<<INT0;//turn on interrupt
	
	//for 1ms counter
	TCCR0A|=(1<<WGM01);//set timer to ctc
	OCR0A=0xF9;//set value to count to
	TIMSK0|=(1<<OCIE0A);//enable interrupt for on compare a for timer 0
	
	sei();//enable global interrups
	TCCR0B|=(1<<CS01)|(1<<CS00);//set prescaler to 64
	PORTD|=1<<PIND4;//trig pin output to ultrasonic, set PD4 high
	_delay_us(10);//needs 10us pulse to start
	PORTD&=~(1<<PIND4);//set PD4 to low
	
	//---------------------------------------------------
	
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
	
	 millis=0;
  while(1){
    //start with the movement
	moveMotor(motors.GR_Elbow_L,-1,700,millis);// move the left green elbow motor counterclockwise with signal of 1 for 500ms
	moveMotor(motors.GR_Elbow_R,-1,700,millis);
	if(millis>1500)
		openGrabbers(motors.GR_Grabber,millis);
	if(millis>2000){
		moveMotor(motors.GR_Elbow_L,1,200,millis);
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
	
  }
}
ISR(INT0_vect){//interrupt routine for ultrasonic sensor
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
ISR(TIMER0_COMPA_vect){
	millis++;
}
