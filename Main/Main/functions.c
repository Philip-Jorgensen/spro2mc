// Include libraries here
#include "functions.h"
#include <util/delay.h>
#include "PCA9685_ext.h"

// Function definitions below.
void closeGrabbers(unsigned char motor_id, unsigned long millis)
{

	//Reset and start counter

	pushSolenoid();

	if (generic_counter == startTime)
	{
		control_motor(motor_id, -1);
	}

	// Tries to stop the motor fast by making it go in reverse shortly and then stop.
	if (generic_counter == closeTime)
	{
		control_motor(motor_id, 1);
	}

	// Stops the motor
	if (generic_counter == closeTime + 10)
	{
		control_motor(motor_id, 0);
	}

	//PORTD|=1<<PIND4;//lock grabbers
}
void openGrabbers(unsigned char motor_id, unsigned long millis)
{
	pullSolenoid();
	//unlock the solenoid.
	static unsigned int timestamp = 0;
	switch (millis - timestamp)
	{
	case (10):
		control_motor(motor_id, 1);
		break;
	case (140):
		control_motor(motor_id, -1);
		break;
	case (160):
		control_motor(motor_id, 0);
		timestamp = millis;
		break;
	}

	// Locking the solenoid still needs to be added
}
double distanceBarGrabbers()
{
	double distanceToBar;
	//read the sensors for the grabbers
	return distanceToBar;
}

// Only moves the motor for a given time, 'time_on', and then stops the motor again.
<<<<<<< HEAD
void moveMotor(unsigned char motor_id, int on_value, int time_on, unsigned long millis){
	
	static unsigned int timestamp=0;
	
	control_motor(motor_id, on_value);
	
	if(millis-timestamp>time_on){
		timestamp=millis;
		motor_set_pwm(motor_id,0,0);
=======
void moveMotor(unsigned char motor_id, int on_value, int time_on, unsigned long millis)
{

	static unsigned int timestamp = 0;
	int motionInProcess = 0;

	if (motionInProcess == 0)
	{
		control_motor(motor_id, on_value);
		motionInProcess = 1;
	}

	if (millis - timestamp > time_on)
	{
		timestamp = millis;
		motor_set_pwm(motor_id, 0, 0);
		motionInProcess = 0;
>>>>>>> f395a819eb6c642af1e26ed2213dc65488362959
	}
}

void control_motor(unsigned char motor_id, int on_value)
{
	if (on_value >= 0)
	{ // If the run value (speed?) is greater than 0, make it run clockwise.
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
<<<<<<< HEAD
double readAcceleration(char axis){//axis is 'y'or'x'or'z'
=======
double readAccleration(char axis)
{ //axis is 'y'or'x'or'z'
>>>>>>> f395a819eb6c642af1e26ed2213dc65488362959
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
<<<<<<< HEAD
	
	angleOfRotation = asin((barDistance - BODY_LENGTH)/2/ARM_LENGTH);
	
	//We either swing the "arms" or the "legs"
=======
>>>>>>> f395a819eb6c642af1e26ed2213dc65488362959

	angleOfRotation = inversine((barDistance - BODY_LENGTH) / 2 / ARM_LENGTH);

	//We either swing the "arms" or the "legs"

	//Start counter

	if (readPSensor(0) == 1)
	{
		openGrabbers(GClaws, millis);
<<<<<<< HEAD
		
		rotateJMotor(GELBOWS,    angleOfRotation, SWING_TIME);
		rotateJMotor(GSHOULDERS, angleOfRotation, SWING_TIME);
		rotateJMotor(PELBOWS,    angleOfRotation, SWING_TIME);
		rotateJMotor(PSHOULDERS, angleOfRotation, SWING_TIME);
		
		if(generic_counter > grabBar){
=======

		rotatebigMotor(GELBOWS, angleOfRotation, SWING_TIME);
		rotatebigMotor(GSHOULDERS, angleOfRotation / 2, SWING_TIME);
		rotatebigMotor(PELBOWS, angleOfRotation, SWING_TIME);
		rotatebigMotor(PSHOULDERS, angleOfRotation / 2, SWING_TIME);

		if (generic_counter > grabBar)
		{
>>>>>>> f395a819eb6c642af1e26ed2213dc65488362959
			closeGrabbers(GClaws, millis);
		}
	}

	if (readPSensor(2) == 1)
	{
		openGrabbers(PClaws, millis);
<<<<<<< HEAD
				
		rotateJMotor(PELBOWS,    angleOfRotation, SWING_TIME);
		rotateJMotor(PSHOULDERS, angleOfRotation, SWING_TIME);
		rotateJMotor(GELBOWS,    angleOfRotation, SWING_TIME);
		rotateJMotor(GSHOULDERS, angleOfRotation, SWING_TIME);
		
		if(generic_counter > grabBar){
=======

		rotatebigMotor(PELBOWS, angleOfRotation, SWING_TIME);
		rotatebigMotor(PSHOULDERS, angleOfRotation / 2, SWING_TIME);
		rotatebigMotor(GELBOWS, angleOfRotation, SWING_TIME);
		rotatebigMotor(GSHOULDERS, angleOfRotation / 2, SWING_TIME);

		if (generic_counter > grabBar)
		{
>>>>>>> f395a819eb6c642af1e26ed2213dc65488362959
			closeGrabbers(PClaws, millis);
		}
	}
}

void r_brachiation()
{
}

//very rough; change the variable names if you want to
<<<<<<< HEAD
void rotateJMotor(unsigned char motor_id, int degrees, int timeInterval){
	
=======
void rotatebigMotor(int motor_id, int degrees, int timeInterval)
{

>>>>>>> f395a819eb6c642af1e26ed2213dc65488362959
	int rps;
	rps = degrees / 360 / timeInterval;

	moveMotor(motor_id, conv_j30(rps), timeInterval, millis);
}
