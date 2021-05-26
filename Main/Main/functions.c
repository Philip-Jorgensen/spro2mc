// Libraries

#include "functions.h"
#include <util/delay.h>
#include "PCA9685_ext.h"

// Definitions

#define JOINT_MOTOR 0; // 30RPS Worm gear motor

#define SWING_TIME 1000; // Time between each swing in the continuous brachiation

// Function definitions

void unlockGrabbers(unsigned char motor_id){
	
	unsigned char leftSolenoid = 0, rightSolenoid = 0;
	
	// Depending on what grabbers we want to close ("green" ones or "purple" ones)
	// we need to choose different solenoids (the top ones or bottom ones)
	
	switch(motor_id){
		case (1): // Green grabbers
		leftSolenoid  = 0;
		rightSolenoid = 1;
		break;
		case (6): // Purple Grabbers
		leftSolenoid  = 2;
		rightSolenoid = 3;
		break;
	}
	
	// We deactivate the two solenoids, setting their pins to zero
	
	PORTD |= (1 << leftSolenoid) | (1 << rightSolenoid);
	
}

void lockGrabbers(unsigned char motor_id){
	
	unsigned char leftSolenoid = 0, rightSolenoid = 0;
	
	// Depending on what grabbers we want to close ("green" ones or "purple" ones)
	// we need to choose different solenoids (the top ones or bottom ones)
	
	switch(motor_id){
		case (1): // Green grabbers
			leftSolenoid  = 0;
			rightSolenoid = 1;
			break;
		case (6): // Purple Grabbers
			leftSolenoid  = 2;
			rightSolenoid = 3;
			break;
	}
	
	// We activate the two solenoids, setting their pins to one
	
	PORTD |= (1 << leftSolenoid) | (1 << rightSolenoid);
	
}

void closeGrabbers(unsigned char motor_id, unsigned long millis){
	
	static unsigned int timestamp = millis;

	unlockGrabbers(motor_id);
	
	// Closing protocol
	
	switch (millis - timestamp){
		case (10):  // We close the grabbers
			control_motor(motor_id,-1);
			break;
		case (140): // We counteract the inertia
			control_motor(motor_id, 1);
			break;
		case (160): // We stop the motors
			control_motor(motor_id, 0);
			timestamp = millis;
			break;
	}
	
	lockGrabbers(motor_id);
	
}

void openGrabbers(unsigned char motor_id, unsigned long millis){
	
	static unsigned int timestamp = millis;
	
	unlockGrabbers(motor_id);
	
	// Opening protocol

	switch (millis - timestamp){
	case (10):  // We open the grabbers
		control_motor(motor_id, 1);
		break;
	case (140): // We counteract the inertia
		control_motor(motor_id,-1);
		break;
	case (160): // We stop the motors
		control_motor(motor_id, 0);
		timestamp = millis;
		break;
	}
	
	lockGrabbers(motor_id);
	
}

double distanceBarGrabbers(){
	
	double distanceToBar;
	
	//read the sensors for the grabbers
	
	return distanceToBar;
	
}

// Only moves the motor for a given time, 'time_on', and then stops the motor again.
void timebasedRotation(unsigned char motor_id, int on_value, int time_on, unsigned long millis){

	static unsigned int timestamp = millis;
	
	// We turn on the motor
	control_motor(motor_id, on_value);
	
	// We turn off the motor when the given time has passed
	if (millis - timestamp > time_on){
		timestamp = millis;
		motor_set_pwm(motor_id, 0, 0);
	}
	
}

void control_motor(unsigned char motor_id, int on_value){
	
	if (on_value >= 0){ // If the run value (speed?) is greater than 0, make it run clockwise.
		motor_set_state(motor_id, CW);
		motor_set_pwm(motor_id, on_value, 0);
	}
	
	if (on_value < 0){ // If speed is less than 0, make it run counter clockwise.
		motor_set_state(motor_id, CCW);	
		motor_set_pwm(motor_id, (-1) * on_value, 0); // Since 'on_value' is below 0, it is multiplied by (-1) to make it positive.
	}
	
}

double readUltrasonic(unsigned int pulse){
	
	double distance = 0;
	
	distance = ((double)pulse) * 0.0000000625 * 342.2 / 2; // pulse (number of "ticks") * time for one tick (1/16mhz) * speed of sound(20ºC) / 2
	
	return distance;
	
}

void distancesToBar(double distance,double tilt_angle, double *p_ZheightToBar, double *p_XdistanceToBar){
	
	double angle=180.0-90.0-tilt_angle;
	
	angle*=0.01745;//degrees to radians
	*p_ZheightToBar=sin(angle)*distance;
	*p_XdistanceToBar=cos(angle)*distance;
	
}

double readAccleration(char axis){ //axis is 'y'or'x'or'z'
	
	double acceleration;
	
	if (axis == 'x'){
		// Reading the acceleration in the x direction.
	}
	if (axis == 'y'){
		// Reading the acceleration in the y direction.
	}
	if (axis == 'z'){
		// Reading the acceleration in the z direction.
	}
	
	return acceleration;
	
}

int rps_to_speedValue(double rps, int motor_type){
	// This function only works for the 30 RPM Joint motor.
	// A function for converting rps to the speed value the motor library needs.
	
	switch(motor_type){ //The conversion is different depending on what motor we are using
		case JOINT_MOTOR:
			if (rps > 0 && rps <= 0.51){  // The motor can't go faster than 0.51 RPS
				return (int)(-7930 * rps + 4090);
			}
		break;
	}
	
	if(rps == 0){ // The function only works when it's a positive number (not 0).
		return 0;
	}
	
}

void c_brachiation(int barDistance, int *progress){

	int grabBar = SWING_TIME - 40;
	static unsigned int timestamp = millis;
	
	float angleOfRotation;

	//We calculate the angle that the elbows have to rotate using maffs

	angleOfRotation = asin((barDistance - BODY_LENGTH) / 2 / ARM_LENGTH);

	//We either swing the "arms" or the "legs"

	//Start counter

	if (readPSensor(GClaws) == 1)
	{
		openGrabbers(GClaws, millis);

		anglebasedRotation(GELBOWS, angleOfRotation, SWING_TIME);
		anglebasedRotation(GSHOULDERS, angleOfRotation, SWING_TIME);
		anglebasedRotation(PELBOWS, angleOfRotation, SWING_TIME);
		anglebasedRotation(PSHOULDERS, angleOfRotation / 2, SWING_TIME);

		if (generic_counter > grabBar){
			closeGrabbers(GClaws, millis);
			if (generic_counter > grabBar+4){
		}
	}

	if (readPSensor(PClaws) == 1)
	{
		openGrabbers(PClaws, millis);

		anglebasedRotation(PELBOWS, angleOfRotation, SWING_TIME);
		anglebasedRotation(PSHOULDERS, angleOfRotation, SWING_TIME);
		anglebasedRotation(GELBOWS, angleOfRotation, SWING_TIME);
		anglebasedRotation(GSHOULDERS, angleOfRotation, SWING_TIME);

		if (generic_counter > grabBar)
		{
			closeGrabbers(PClaws, millis);
		}
	}
	
}

void r_brachiation(){
	
}

void anglebasedRotation(int motor_id, int degrees, int time_interval){

	int rps = 0, speedValue = 0;
	
	rps = degrees / 360 / time_interval; // We calculate the rps
	speedValue = rps_to_speedValue(JOINT_MOTOR, rps); // We convert the rps into a speed value

	timebasedRotation(motor_id, speedValue, time_interval, millis);
	
}
