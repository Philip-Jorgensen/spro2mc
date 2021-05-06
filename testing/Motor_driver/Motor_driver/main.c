/*
 * Motor_driver.c
 *
 * Created: 5/4/2021 1:47:22 PM
 * Author : Philip J
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdio.h>
#include "PCA9685_ext.h"
#include "i2cmaster.h"
#include "usart.h"
#include <util/delay.h>

void control_motor(unsigned char , unsigned char , unsigned int , unsigned int );

int main(void)
{
	
	uart_init(); // Open the communication to the micro controller
	i2c_init(); // Initialize the i2c communication.
	io_redirect(); // Redirect the input/output to the computer.
	
	// Make sure all the motors are stopped from the beginning (Initialization)
	motor_init_pwm(PWM_FREQUENCY_50);
	
	printf("Adafruit 1438");
	
	// M1,..,M4 are ports on the "Adafruit 1438"
	motor_set_state(M1, STOP);
	motor_set_state(M2, STOP);
	motor_set_state(M3, STOP);
	motor_set_state(M4, STOP);
	
	/* This piece of code does make the motor turn, but very slow.
	motor_set_state(M1, CW);
	motor_set_pwm(M1, 0, 0x1ff);
	*/
	
	// Initializing variables
	int spe, flag;
	flag = 0;
	
	while(1){
		
		
		// Asking the user to control the motor.
		printf("Motor speed:\n"); // Asking for the speed
		scanf("%d", &spe);
		
		if(spe>=0 && flag==0){
			motor_set_state(M1, CW);
			flag = 1;
		}
		if(spe<0 && flag==1){
			motor_set_state(M1, CCW);
			spe = spe * (-1);
			flag = 0;
		}
		motor_set_pwm(M2, spe, 0);
		
		
		
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
void control_motor(unsigned char motor_id, unsigned char state, unsigned int on_value, unsigned int off_value){
	motor_set_state(motor_id, state);
	motor_set_pwm(motor_id,on_value,off_value);
}
