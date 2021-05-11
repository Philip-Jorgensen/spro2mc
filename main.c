/*
SPRO2MC
main.c
2021

Remember to add proper comments and explanations when you make changes.
*/

// Definitions
#define F_CPU 16E6

// Imports
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"
#include "PCA9685_ext"

#include "functions/functions.h"

// Function Prototypes
void closeGrabbers(void);
void openGrabbers(void);
void control_motor(unsigned char, int);
double detectBarGrabbers(void);
double readUltrasonic(void);
void moveMotor(unsigned char, int, int);
double readAccleration(char);

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

int main(void){
	
	uart_init(); // Open the communication to the micro controller
	i2c_init(); // Initialize the i2c communication.
	io_redirect(); // Redirect the input/output to the computer.
	
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
	moveMotor(motors.GR_Elbow_L,-50,500);//example move the left green elbow motor counterclockwise with signal of 50 for 500ms
  }
}
