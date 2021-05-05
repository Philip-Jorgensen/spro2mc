/*
 * Motor_driver.c
 *
 * Created: 5/4/2021 1:47:22 PM
 * Author : Philip J
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "PCA9685_ext.h"
#include "i2cmaster.h"
void control_motor(unsigned char , unsigned char , unsigned int , unsigned int );

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
void control_motor(unsigned char motor_id, unsigned char state, unsigned int on_value, unsigned int off_value){
	motor_set_state(motor_id, state);
	motor_set_pwm(motor_id,on_value,off_value);
}
