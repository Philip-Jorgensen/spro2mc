// Libraries

#include "functions.h"
#include <util/delay.h>
#include "PCA9685_ext.h"

// Function definitions

void unlockGrabbers(unsigned char motor_id){
	unsigned char leftSolenoid = 0, rightSolenoid = 0;
	
	// Depending on what grabbers we want to close ("green" ones or "purple" ones)
	// we need to choose different solenoids (the top ones or bottom ones)
	
	switch(motor_id){
		case (M1): // Green grabbers
		leftSolenoid  = 0;
		rightSolenoid = 1;
		break;
		case (M6): // Purple Grabbers
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
		case (M1): // Green grabbers
			leftSolenoid  = 0;
			rightSolenoid = 1;
			break;
		case (M6): // Purple Grabbers
			leftSolenoid  = 2;
			rightSolenoid = 3;
			break;
	}
	
	// We activate the two solenoids, setting their pins to one
	
	PORTD |= (1 << leftSolenoid) | (1 << rightSolenoid);
}

void closeGrabbers(unsigned char motor_id, unsigned long millis){
	
	static unsigned int timestamp = 0;

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
	
	static unsigned int timestamp = 0;
	
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
void moveMotor(unsigned char motor_id, int on_value, int time_on, unsigned long millis){

	static unsigned int timestamp = 0;
	int motionInProcess = 0;

	if (motionInProcess == 0){
		control_motor(motor_id, on_value);
		motionInProcess = 1;
	}

	if (millis - timestamp > time_on){
		timestamp = millis;
		motor_set_pwm(motor_id, 0, 0);
		motionInProcess = 0;
	}
}

void control_motor(unsigned char motor_id, int on_value){
	if (on_value >= 0){ // If the run value (speed?) is greater than 0, make it run clockwise.
		motor_set_state(motor_id, CW);
		motor_set_pwm(motor_id, on_value, 0);
	}
	if (on_value < 0)
	{
		motor_set_state(motor_id, CCW);				 // If speed is less than 0, make it run counter clockwise.
		motor_set_pwm(motor_id, (-1) * on_value, 0); // Since 'on_value' is below 0, it is multiplied by (-1) to make it positive.
	}
}
double readUltrasonic(unsigned int pulse)
{
	double distance = 0;
	distance = ((double)pulse) * 0.0000000625 * 342.2 / 2; //pulse*time for one tick (1/16mhz)*speed of sound(20C)/2
	return distance;
}
void distancesToBar(double distance,double tilt_angle, double *p_ZheightToBar, double *p_XdistanceToBar){
	double angle=180.0-90.0-tilt_angle;
	angle*=0.01745;//degrees to radians
	*p_ZheightToBar=sin(angle)*distance;
	*p_XdistanceToBar=cos(angle)*distance;
}
double readAccleration(char axis)
{ //axis is 'y'or'x'or'z'
	double acceleration;
	if (axis == 'x')
	{
		// Reading the acceleration in the x direction.
	}
	if (axis == 'y')
	{
		// Reading the acceleration in the y direction.
	}
	if (axis == 'z')
	{
		// Reading the acceleration in the z direction.
	}
	return acceleration;
}
// A function for converting rps to the speed value the motor library needs.
// This is for the 30 RPM Joint motor.
int conv_j30(double rps)
{
	if (rps > 0 && rps <= 0.51) // The motor can't go faster than 0.51 RPS
	{
		// This function can be found in notion.
		return (int)(-7930 * rps + 4090);
	}
	else
	{
		return 0;
	} // The function only works when it's a positive number (not 0).
}

void c_brachiation(int barDistance)
{

	float angleOfRotation;
	int grabBar = SWING_TIME - 40;

	//We calculate the angle that the elbows have to rotate using maffs

	angleOfRotation = asin((barDistance - BODY_LENGTH) / 2 / ARM_LENGTH);

	//We either swing the "arms" or the "legs"

	//Start counter

	if (readPSensor(0) == 1)
	{
		openGrabbers(GClaws, millis);

		rotatebigMotor(GELBOWS, angleOfRotation, SWING_TIME);
		rotatebigMotor(GSHOULDERS, angleOfRotation, SWING_TIME);
		rotatebigMotor(PELBOWS, angleOfRotation, SWING_TIME);
		rotatebigMotor(PSHOULDERS, angleOfRotation / 2, SWING_TIME);

		if (generic_counter > grabBar)
		{
			closeGrabbers(GClaws, millis);
		}
	}

	if (readPSensor(2) == 1)
	{
		openGrabbers(PClaws, millis);

		rotatebigMotor(PELBOWS, angleOfRotation, SWING_TIME);
		rotatebigMotor(PSHOULDERS, angleOfRotation, SWING_TIME);
		rotatebigMotor(GELBOWS, angleOfRotation, SWING_TIME);
		rotatebigMotor(GSHOULDERS, angleOfRotation, SWING_TIME);

		if (generic_counter > grabBar)
		{
			closeGrabbers(PClaws, millis);
		}
	}
}

void r_brachiation(double Zacceleration, double Yacceleration, double tilt_angle){
	static int state=0;
	switch(state){
		case 0:
		openGrabbers(Gclaws,millis);//curling body up for first swing
		moveMotor(GELBOWS, conv_j30(-1), 1000, millis);
		moveMotor(PELBOWS, conv_j30(-1), 1000, millis);
		moveMotor(GSHOULDERS, conv_j30(1), 400, millis);
		state=1;
		
		break;
		case 1:
		//starting swing motion
		moveMotor(PELBOWS, conv_j30(1.9), 1000, millis);
		moveMotor(GELBOWS, conv_j30(2), 200, millis);
		moveMotor(GSHOULDERS, conv_j30(-2), 500, millis);
		moveMotor(PSHOULDERS, conv_j30(1.2), 300, millis);
		if(Yacceleration<1&&Zacceleration<1)
		state=2;
		
		break;
		case 2:
		if(Zacceleration>=5&&Yacceleration>=4){//establishing two thresholds for Z and Y acceleration
			distancesToBar(readUltrasonic(pulse),tilt_angle, &ZheightToBar, &XdistanceToBar)
			if(ZheightToBar<=0.4&&XdistanceToBar<=0.4)
			state=3;
		}
		
		break;
		case 3://Green grabbers on the next bar
		moveMotor(PELBOWS, conv_j30(-1.2), 150, millis);
		openGrabbers(GClaws,millis);
		if(readPsensor())
		closeGrabbers(GClaws,millis);
		state=4;
		
		break;
		
	}	
}

//very rough; change the variable names if you want to
void rotatebigMotor(int motor_id, int degrees, int timeInterval)
{

	int rps;
	rps = degrees / 360 / timeInterval;

	moveMotor(motor_id, conv_j30(rps), timeInterval, millis);
}
