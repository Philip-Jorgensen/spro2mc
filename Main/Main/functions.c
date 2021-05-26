// Libraries

#include "functions.h"
#include <util/delay.h>
#include "PCA9685_ext.h"
#include <avr/io.h>

// Definitions

#define JOINT_MOTOR 0; // 30RPS Worm gear motor

#define SWING_TIME    1000; // Time between each swing in the continuous brachiation
#define GRABBERS_TIME  100; // The time it takes for the grabbers to open or close

#define CENTRE_TO_SHOULDER    40; // Distance in millimetres from the centre of the body to the shoulder
#define SHOULDER_TO_SHOULDER  80; // Distance in millimetres from shoulder to shoulder
#define SHOULDER_TO_ELBOW    110; // Distance in millimetres from shoulder to elbow
#define ELBOW_TO_GRABBER     110; // Distance in millimetres from elbow to grabber
#define GRABBER_TO_BAR        75; // Distance in millimetres from grabber to bar

struct Motors {

	// Motor IDs
	
	unsigned char G_Grabbers  = M1;
	unsigned char G_Elbows    = M2;
	unsigned char G_Shoulders = M3;
	
	unsigned char P_Elbows	  = M4;
	unsigned char P_Shoulders = M5;
	unsigned char P_Grabbers  = M6;
};

struct Motors motors;

// Function definitions

int readPSensor(unsigned int motor_id)
{
	switch (motor_id)
	{
	case M1:
		if (PINC == 0b00000011)
		{
			return 1;
		}
		else
		{
			return 0;
		}
		break;
	case M6:
		if (PINC == 0b00001100)
		{
			return 1;
		}
		else
		{
			return 0;
		}
		break;
	}
}

void unlockGrabbers(unsigned char motor_id)
{

	unsigned char leftSolenoid = 0, rightSolenoid = 0;

	// Depending on what grabbers we want to close ("green" ones or "purple" ones)
	// we need to choose different solenoids (the top ones or bottom ones)

	switch (motor_id)
	{
	case (1): // Green grabbers
		leftSolenoid = 0;
		rightSolenoid = 1;
		break;
	case (6): // Purple Grabbers
		leftSolenoid = 2;
		rightSolenoid = 3;
		break;
	}

	// To retract the solenoid we set the pin to one (setting)

	PORTD |= (1 << leftSolenoid) | (1 << rightSolenoid);
}

void lockGrabbers(unsigned char motor_id)
{

	unsigned char leftSolenoid = 0, rightSolenoid = 0;

	// Depending on what grabbers we want to close ("green" ones or "purple" ones)
	// we need to choose different solenoids (the top ones or bottom ones)

	switch (motor_id)
	{
	case (1): // Green grabbers
		leftSolenoid = 0;
		rightSolenoid = 1;
		break;
	case (6): // Purple Grabbers
		leftSolenoid = 2;
		rightSolenoid = 3;
		break;
	}

	// To extend the solenoid we set the pin to zero (clearing)

	PORTD &= ~((1 << leftSolenoid) | (1 << rightSolenoid));
}

void closeGrabbers(unsigned char motor_id, unsigned long millis)
{

	static unsigned int timestamp = millis;

	unlockGrabbers(motor_id);

	// Closing protocol

	switch (millis - timestamp)
	{
	case (10): // We close the grabbers
		control_motor(motor_id, -1);
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

void openGrabbers(unsigned char motor_id, unsigned long millis)
{

	static unsigned int timestamp = millis;

	unlockGrabbers(motor_id);

	// Opening protocol

	switch (millis - timestamp)
	{
	case (10): // We open the grabbers
		control_motor(motor_id, 1);
		break;
	case (140): // We counteract the inertia
		control_motor(motor_id, -1);
		break;
	case (160): // We stop the motors
		control_motor(motor_id, 0);
		timestamp = millis;
		break;
	}

	lockGrabbers(motor_id);
}

double distanceBarGrabbers()
{

	double distanceToBar;

	//read the sensors for the grabbers

	return distanceToBar;
}

// Only moves the motor for a given time, 'time_on', and then stops the motor again.
void timebasedRotation(unsigned char motor_id, int on_value, int time_on, unsigned long millis)
{

	static unsigned int timestamp = millis;

	// We turn on the motor
	control_motor(motor_id, on_value);

	// We turn off the motor when the given time has passed
	if (millis - timestamp > time_on)
	{
		timestamp = millis;
		motor_set_pwm(motor_id, 0, 0);
	}
}

void control_motor(unsigned char motor_id, int on_value)
{
	if (on_value >= -3800 && on_value <= 3800)
	{
		if (on_value >= 0)
		{ // If the run value (speed?) is greater than 0, make it run clockwise.
			motor_set_state(motor_id, CW);
			motor_set_pwm(motor_id, on_value, 0);
		}

		if (on_value < 0)
		{ // If speed is less than 0, make it run counter clockwise.
			motor_set_state(motor_id, CCW);
			motor_set_pwm(motor_id, (-1) * on_value, 0); // Since 'on_value' is below 0, it is multiplied by (-1) to make it positive.
		}
	}
	else
	{
		motor_set_pwm(motor_id, 0, 0);
	}
}

double readUltrasonic(unsigned int pulse)
{

	double distance = 0;

	distance = ((double)pulse) * 0.0000000625 * 342.2 / 2; // pulse (number of "ticks") * time for one tick (1/16mhz) * speed of sound(20ºC) / 2

	return distance;
}

void distancesToBar(double distance, double tilt_angle, double *p_ZheightToBar, double *p_XdistanceToBar)
{

	double angle = 180.0 - 90.0 - tilt_angle;

	angle *= 0.01745; //degrees to radians
	*p_ZheightToBar = sin(angle) * distance;
	*p_XdistanceToBar = cos(angle) * distance;
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

int rps_to_speedValue(double rps, int motor_type)
{
	// This function only works for the 30 RPM Joint motor.
	// A function for converting rps to the speed value the motor library needs.

	switch (motor_type)
	{ //The conversion is different depending on what motor we are using
	case JOINT_MOTOR:
		if (rps > 0 && rps <= 0.51)
		{ // The motor can't go faster than 0.51 RPS
			return (int)(-7930 * rps + 4090);
		}
		break;
	}

	if (rps == 0)
	{ // The function only works when it's a positive number (not 0).
		return 0;
	}
}

void c_brachiation(int barDistance, int direction, int *bar_number){

	static unsigned int timestamp = millis;
	int offset = 10;

	float angleOfRotation;
	float grabbingArms = 1.000, swingingArms = 1.000;

	// We calculate the angle that the elbows have to rotate using maffs

	angleOfRotation = (asin((barDistance - SHOULDER_TO_SHOULDER) / 2 / SHOULDER_TO_ELBOW) + offset) * direction;

	// We have to decide to either swing the "arms" or the "legs"

	if (bar_number % 2 == 0){ // If the bar number is even, we swing the arms
		openGrabbers(motors.G_Grabbers, millis);

		anglebasedRotation(motors.G_Elbows   , angleOfRotation, SWING_TIME, grabbingArms);
		anglebasedRotation(motors.G_Shoulders, angleOfRotation, SWING_TIME, grabbingArms);
		anglebasedRotation(motors.P_Elbows   , angleOfRotation, SWING_TIME, swingingArms);
		anglebasedRotation(motors.P_Shoulders, angleOfRotation, SWING_TIME, swingingArms);

		if(millis - timestamp > SWING_TIME - GRABBERS_TIME){ // We need to begin the closing protocol a little bit before the end of the swing
			closeGrabbers(motors.G_Grabbers, millis);
		}
	}

	if (bar_number % 2 == 1){ // If the bar number is uneven, we swing the arms
		openGrabbers(motors.P_Grabbers, millis);

		anglebasedRotation(motors.P_Elbows   , angleOfRotation, SWING_TIME, grabbingArms);
		anglebasedRotation(motors.P_Shoulders, angleOfRotation, SWING_TIME, grabbingArms);
		anglebasedRotation(motors.G_Elbows   , angleOfRotation, SWING_TIME, swingingArms);
		anglebasedRotation(motors.G_Shoulders, angleOfRotation, SWING_TIME, swingingArms);

		if(millis - timestamp > SWING_TIME- GRABBERS_TIME){ // We need to begin the opening protocol a little bit before the end of the swing
			closeGrabbers(motors.P_Grabbers, millis);
		}
	}
	
	if(millis - timestamp > SWING_TIME + GRABBERS_TIME){ // We have progressed to the next bar
		*bar_number++;
	}
}

void r_brachiation(double Z_acceleration, double Y_acceleration, double Y_velocity, double tilt_angle)//y is the forward axis here
{
	static int state = 0;
	switch (state)
	{
	case 0:
		openGrabbers(motors.G_Grabbers, millis); //curling body up for first swing
		moveMotor(motors.G_Elbows, conv_j30(-1), 1000, millis);
		moveMotor(motors.P_Elbows, conv_j30(-1), 1000, millis);
		moveMotor(motors.G_Shoulders, conv_j30(1), 400, millis);
		state = 1;

		break;
	case 1:
		//starting swing motion
		moveMotor(motors.P_Elbows, conv_j30(1.9), 1000, millis);
		moveMotor(motors.G_Elbows, conv_j30(2), 200, millis);
		moveMotor(motors.G_Shoulders, conv_j30(-2), 500, millis);
		moveMotor(motors.P_Shoulders, conv_j30(1.2), 300, millis);
		if (Y_acceleration < 1 && Z_acceleration < 1)
			state = 2;

		break;
	case 2:
		if (Z_acceleration >= 5 && Y_acceleration >= 4 && Y_velocity>=0.8)
		{ //establishing two thresholds for Z and Y acceleration
			distancesToBar(readUltrasonic(pulse), tilt_angle, &ZheightToBar, &XdistanceToBar) if (ZheightToBar <= 0.4 && XdistanceToBar <= 0.4)
				state = 3;
		}

		break;
	case 3: //Green grabbers on the next bar
		moveMotor(motors.P_Elbows, conv_j30(-1.2), 150, millis);
		openGrabbers(motors.G_Grabbers, millis);
		if (readPsensor())
			closeGrabbers(motors.G_Grabbers, millis);
		state = 4;

		break;
	}
}

void anglebasedRotation(unsigned char motor_id, int degrees, int time_interval, float coefficient)
{

	int speedValue = 0;
	float rps = 0;

	rps = degrees / 360 / time_interval;								 // We calculate the rps
	speedValue = rps_to_speedValue(JOINT_MOTOR, int(rps * coefficient)); // We convert the rps into a speed value

	timebasedRotation(motor_id, speedValue, time_interval, millis);
}
