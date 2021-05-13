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
#include "../../libraries/usart.h"
#include "../../libraries/PCA9685_ext.h"
#include "../../libraries/i2cmaster.h"
#include "../../functions/functions.h"

// Function Prototypes
void closeGrabbers(unsigned char);
void openGrabbers(unsigned char);
void control_motor(unsigned char, int);
double detectBarGrabbers();
double readUltrasonic(double);
void moveMotor(unsigned char, int, int);
double readAccleration(char);
double readUltrasonic();
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
	
	EICRA |= 1<<ISC00;//set INT0(PD2) to trigger on any logic change
	EIMSK |=1<<INT0;//turn on interrupt
	sei();//enable global interrups
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
  while(1){
    //start with the movement
	moveMotor(motors.GR_Elbow_L,2500,500);//example move the left green elbow motor counterclockwise with signal of 50 for 500ms
	moveMotor(motors.GR_Elbow_R,2500,500);
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
