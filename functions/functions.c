// Include libraries here
#include "functions.h"
#include <util/delay.h>
#include "PCA9685_ext.h"

// Function definitions below.
void closeGrabbers(unsigned char motor_id){
	// Unlock the solenoid.
	// code.
	
	//rotate grabbers
	control_motor(motor_id, -1);
	_delay_ms(130);
	// Tries to stop the motor fast by making it go in reverse shortly and then stop.
	control_motor(motor_id, 1); 
	_delay_ms(20);
	control_motor(motor_id, 0); // Stops the motor
	
	_delay_ms(10);
	// The code for locking the grabbers with the solenoid.
	//PORTD|=1<<PIND4;//lock grabbers
	
}
void openGrabbers(unsigned char motor_id){
	//PORTD&=~(1<<PIND4);//unlock the solenoid.
	
	_delay_ms(10)
	//rotate grabbers
	control_motor(motor_id, 1);
	_delay_ms(130);
	// Tries to stop the motor fast by making it go in reverse shortly and then stop.
	control_motor(motor_id, -1);
	_delay_ms(20);
	control_motor(motor_id, 0); // Stops the motor
	
	// Lock the solenoid.
}
double distanceBarGrabbers(){
	double distanceToBar;
	//read the sensors for the grabbers
	return distanceToBar;
}

// Only moves the motor for a given time, 'time_on', and then stops the motor again.
void moveMotor(unsigned char motor_id, int on_value, int time_on){
	 control_motor(unsigned char motor_id, int on_value)
	 _delay_ms(time_on);//we probably have to throw the delay out
	 motor_set_pwm(motor_id,0,0);
}
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
double readUltrasonic(){
	double distance=0;
	distance=((double)pulse)*0.0000000625*342.2/2;//pulse*time for one tick (1/16mhz)*speed of sound(20C)/2 
	return distance;
}
double readAccleration(char axis){//axis is 'y'or'x'or'z'
	double acceleration;
	if (axis=='x')
	{
		// Reading the acceleration in the x direction.
		
	}
	if (axis=='y')
	{
		// Reading the acceleration in the y direction.
		
	}
	if (axis=='z')
	{
		// Reading the acceleration in the z direction.
		
	}
	return acceleration;
}
