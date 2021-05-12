// Include libraries here
#include "functions.h"
#include <util/delay.h>

// Function definitions below.
void closeGrabbers(){
	//rotate grabbers
	_delay_ms(10);
	//PORTD|=1<<PIND4;//lock grabbers
	
}
void openGrabbers(){
	//PORTD&=~(1<<PIND4);//unlock grabbers
	_delay_ms(10)
	//rotate grabbers to open
}
double detectBarGrabbers(){
	double distanceToBar;
	//read the sensors for the grabbers
	return distanceToBar;
}
void moveMotor(unsigned char motor_id, int on_value, int time_on){
	 control_motor(unsigned char motor_id, int on_value)
	 _delay_ms(time_on);//we probably have throw the delay out
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
	}
	if (axis=='y')
	{
	}
	if (axis=='z')
	{
	}
	return acceleration;
}
