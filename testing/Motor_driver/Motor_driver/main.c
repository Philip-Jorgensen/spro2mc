/*
 * Motor_driver.c
 *
 * Created: 5/4/2021 1:47:22 PM
 * Author : Philip J
 */ 

// Definitions
#define F_CPU 16000000UL

// Libraries
#include <avr/io.h>
#include <stdio.h>
#include "PCA9685_ext.h"
#include "i2cmaster.h"
#include "usart.h"
#include <util/delay.h>
#include <math.h>

// Function prototypes
void control_motor(unsigned char, int);
int grabber_rps_to_spe(float);
void grabber_open(void);
void grabber_close(void);
int joint30_rps_spe(float);

int main(void)
{
	
	uart_init(); // Open the communication to the micro controller
	i2c_init(); // Initialize the i2c communication.
	io_redirect(); // Redirect the input/output to the computer.
	
	DDRD = 0xFF; //I/O board:PD4…7 as outputs, for LEDs
	DDRC = 0xF0; //I/O board PC0…3 as inputs, for buttons
	PORTC = 0x3F; // Enable internal pull at PC0..3 inputs
	PORTD = 0x00; // Set output LEDs to off
	
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
	
	/* This piece of code does make the motor turn, but very slow.
	motor_set_state(M1, CW);
	motor_set_pwm(M1, 0, 0x1ff);
	*/
	
	// Initializing variables
	int spe, motor_n, flag, rps;
	motor_n = 1;
	printf("Motor number:\n"); // Ask for what motor you want to test.
	scanf("%d", &motor_n);
	
	spe = 0;
	
	// control_motor(5, grabber_rps_to_spe(1.5));
	while(1){
		/*
		flag = 0; // Checks whether a button has been pressed.
		
		if(PINC == 0b00111110){ // First button on the SDU Nano shield
			spe += 50;
			flag = 1;
			printf("Speed: %d\n", spe);
			_delay_ms(100);
		}
		if(PINC == 0b00111101){ // Second button on the SDU Nano shield 
			spe -= 50;
			flag = 1;
			printf("Speed: %d\n", spe);
			_delay_ms(100);
		}
		if(flag){
			control_motor(motor_n, spe);
		}
		*/
		
		/*
		// Testing the opening and closing of the grabber.
		int open;
		printf("Open (1) or Close (0)?\n"); // Asks if the user wants to open or close the grabber.
		scanf("%d", &open);
		if(open){
			grabber_open();
		}
		else{
			grabber_close();
		}
		*/
		
		
		/*
		 // Running it through the serial monitor
		// Asking the user to control the motor.
		printf("Motor speed:\n"); // Asking for the speed
		scanf("%d", &spe);
		
		// Runs motor (number: motor_n) with speed 'spe'
		control_motor(motor_n, spe);
		*/
		
		/*	
		// Sweeps through the speed values with increment of 50, when the user tells it to.
		printf("Start?\n");
		scanf("%d", &flag);
		for(int i=0; i<3501; i+=50){
			if(i==0){i=1;}
			control_motor(motor_n, i);
			printf("Speed: %d\n", i);
			if(i==1){i=0;}
			scanf("%d", &flag);
		}
		control_motor(motor_n, 0);
		printf("Speed: 0");
		*/
		
		
		// Controlling the motor using a desired RPS value.
		printf("Input an RPS value:\n");
		scanf("%d", &rps);
		
		printf("SPE: %d", rps);
		
		
		
		
		/*
		// Run the servo using RPS values
		// Asking the user for an RPS value.
		printf("Motor RPS:\n");
		scanf("%d", &rps);
		
		control_motor(motor_n, grabber_rps_to_spe(rps));
		*/
		
		/*
		// Sweeping through speed values.
		for(int i=0; i<5000; i += 50){
			if(i<2500){
				control_motor(M2, CW, i, 0);
			}
			if(i>2500){
				control_motor(M2, CCW, i-2500, 0);
			}
			printf("Speed: %d\r", i);
			_delay_ms(250);
		}
		*/
		
	}
}
// A control motor function.
void control_motor(unsigned char motor_id, int on_value){
	if(on_value>=0){ // If the run value (speed?) is greater than 0, make it run clockwise.
		motor_set_state(motor_id,CCW);
		motor_set_pwm(motor_id,on_value,0);
	}
	if(on_value<0){
		motor_set_state(motor_id,CW); // If speed is less than 0, make it run counter clockwise (It is opposite of what it does irl).
		motor_set_pwm(motor_id,(-1)*on_value,0); // Since 'on_value' is below 0, it is multiplied by (-1) to make it positive.
	}
}
int grabber_rps_to_spe(float rps){
	// To know how this function is derived look on notion.
	long long int out =2218*pow(rps,5)-14100*pow(rps,4)+3158*pow(rps,3)-32810*pow(rps,2)+15180*rps+1012;
	if(rps==0){return 0;} // The function doesn't work at 0, so if the input is 0, return 0.
	else{return out;} // Returns the value of the function.
}
void grabber_open(void){
	control_motor(5,-1);
	_delay_ms(130);
	control_motor(5,1);
	_delay_ms(20);
	control_motor(5,0);
}
void grabber_close(void){
	control_motor(5,1);
	_delay_ms(130);
	control_motor(5,-1);
	_delay_ms(20);
	control_motor(5,0);
}
int joint30_rps_spe(float rps){
	// To know how this funciton is derived look on notion
	long long int out = -7930*rps+4049;
	if(rps==0){return 0;} // The function doesn't work at 0, so if the input is 0, return 0.
	else{return out;} // Returns the value of the function.
}