/*
 * Motor_driver.c
 *
 * Created: 5/4/2021 1:47:22 PM
 * Author : Philip J
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "PCA9685_ext.c"
#include "twimaster.c"


int main(void)
{
    motor_init_pwm(PWM_FREQUENCY_1500);
	
	// Making sure that all the motors are stopped when it starts.
	motor_set_state(M1, STOP);
	motor_set_state(M2, STOP);
	motor_set_state(M3, STOP);
	motor_set_state(M4, STOP);
	
	// Starting the first motor
	motor_set_state(M1, CCW); // CCCW -> Counter clockwise
	motor_set_pwm(M1, 0, 0x1ff); // PWM: stop = 0, start = 511 (0x1ff)
	
    while (1) 
    {
		
    }
}

