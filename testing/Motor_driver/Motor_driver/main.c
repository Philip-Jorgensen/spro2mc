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

// Function prototypes
void control_motor(unsigned char, int);

int main(void)
{
	
	uart_init(); // Open the communication to the micro controller
	i2c_init(); // Initialize the i2c communication.
	io_redirect(); // Redirect the input/output to the computer.
	
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
	int spe, motor_n;
	motor_n = 1;
	printf("Motor number:\n"); // Ask for what motor you want to test.
	scanf("%d", &motor_n);
	
	while(1){
		
		
		// Asking the user to control the motor.
		printf("Motor speed:\n"); // Asking for the speed
		scanf("%d", &spe);
		
		// Runs motor (number: motor_n) with speed 'spe'
		control_motor(motor_n, spe);
		
		
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
		motor_set_state(motor_id,CW);
		motor_set_pwm(motor_id,on_value,0);
	}
	if(on_value<0){
		motor_set_state(motor_id,CCW); // If speed is less than 0, make it run counter clockwise.
		motor_set_pwm(motor_id,(-1)*on_value,0); // Since 'on_value' is below 0, it is multiplied by (-1) to make it positive.
	}
}
