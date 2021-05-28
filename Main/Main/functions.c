// Libraries
#define F_CPU 16000000UL
#include "functions.h"
#include "PCA9685_ext.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include "math.h"
#include <stdlib.h>
#include <util/delay.h>
#include "usart.h"
#include "i2cmaster.h"


// Definitions

#define JOINT_MOTOR 0 // 30RPS Worm gear motor

#define SWING_TIME    1000 // Time between each swing in the continuous brachiation
#define GRABBERS_TIME  100 // The time it takes for the grabbers to open or close

#define CENTRE_TO_SHOULDER    40 // Distance in millimetres from the centre of the body to the shoulder
#define SHOULDER_TO_SHOULDER  80 // Distance in millimetres from shoulder to shoulder
#define SHOULDER_TO_ELBOW    110 // Distance in millimetres from shoulder to elbow
#define ELBOW_TO_GRABBER     110 // Distance in millimetres from elbow to grabber
#define GRABBER_TO_BAR        75 // Distance in millimetres from grabber to bar

#define g 9.81

typedef struct{

	// Motor IDs
	
	unsigned char G_Grabbers;
	unsigned char G_Elbows;
	unsigned char G_Shoulders;
	
	unsigned char P_Elbows;
	unsigned char P_Shoulders;
	unsigned char P_Grabbers;
} Motors;

Motors motors = {M1,M2,M3,M4,M5,M6};

// Function definitions

int readPSensor(unsigned int motor_id){
	switch (motor_id)
	{
	case M1:
		if ((PINC&(1<<PINC0))==0||(PINC&(1<<PINC1))==0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
		break;
	case M6:
		if ((PINC&(1<<PINC2))==0||(PINC&(1<<PINC3))==0)
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

void unlockGrabbers(unsigned char motor_id){

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

void lockGrabbers(unsigned char motor_id){

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

void closeGrabbers(unsigned char motor_id, unsigned long millis){

	static unsigned long timestamp = 0;

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

void openGrabbers(unsigned char motor_id, unsigned long millis){

	static unsigned int timestamp = 0;

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



// Only moves the motor for a given time, 'time_on', and then stops the motor again.
void timebasedRotation(unsigned char motor_id, int on_value, int time_on, unsigned long millis){

	static unsigned int timestamp = 0;

	// We turn on the motor
	control_motor(motor_id, on_value);

	// We turn off the motor when the given time has passed
	if (millis - timestamp > time_on)
	{
		timestamp = millis;
		motor_set_pwm(motor_id, 0, 0);
	}
}

void control_motor(unsigned char motor_id, int on_value){
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

double readUltrasonic(unsigned int pulse){

	double distance = 0;

	distance = ((double)pulse) * 0.0000000625 * 342.2 / 2; // pulse (number of "ticks") * time for one tick (1/16mhz) * speed of sound(20ºC) / 2

	return distance;
}

void distancesToBar(double distance, double tilt_angle, double *p_ZheightToBar, double *p_XdistanceToBar){

	double angle = 180.0 - 90.0 - tilt_angle;

	angle *= 0.01745; //degrees to radians
	*p_ZheightToBar = sin(angle) * distance;
	*p_XdistanceToBar = cos(angle) * distance;
}

void trackArmPosition(float x_accelerometer, float y_accelerometer, float angle_accelerometer){
	float x_arm[4], y_arm[4], A = SHOULDER_TO_ELBOW/20.0;
	float x_shoulder[2], y_shoulder[2];
	
	x_shoulder[0] = x_accelerometer + SHOULDER_TO_SHOULDER/20*cos(angle_accelerometer);
	y_shoulder[0] = y_accelerometer + tan(angle_accelerometer) * x_accelerometer;
	x_shoulder[1] = x_accelerometer - SHOULDER_TO_SHOULDER/20*cos(angle_accelerometer);
	y_shoulder[1] = y_accelerometer + tan(angle_accelerometer) * x_accelerometer;

	x_arm[1] = x_accelerometer + A*cos(angle_accelerometer);
	y_arm[1] = y_accelerometer + tan(angle_accelerometer) * (x_accelerometer - x_shoulder[0]);

	x_arm[0] = x_accelerometer + A*cos(angle_accelerometer);
	y_arm[0] = y_accelerometer + tan(angle_accelerometer) * (x_accelerometer - x_arm[1]);

	x_arm[2] = x_accelerometer - A*cos(angle_accelerometer);
	y_arm[2] = y_accelerometer + tan(angle_accelerometer) * (x_accelerometer + x_shoulder[1]);

	x_arm[3] = x_accelerometer - A*cos(angle_accelerometer);
	y_arm[3] = y_accelerometer + tan(angle_accelerometer) * (x_accelerometer + x_arm[2]);
}

double readAccleration(char axis){ //axis is 'y'or'x'or'z'

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

int rps_to_speedValue(double rps, int motor_type){
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

void start_c_brachiation(int barDistance, int direction, int *bar_number, unsigned long millis){
	
	// Motion of the robot from the start position (Green grabbers holding the first bar with the arms fully extended 
	// and the body in a vertical position) to the next bar

	// Offsets for later fine adjustments.
	float grabbingArms = 1.0, swingingArms = 1.0;
	
	// FROM THE SIMULATION:
	//  1. Grab the first bar with the green grabbers.
	// 		1.1 Pull the body up behind, to make the robot ready for brachiating. (This can be seen in the c_simulation)
	// 				The shoulders don't rotate in this first part.
	int time_1_1 = 1;
	anglebasedRotation(motors.P_Elbows, -111, time_1_1, swingingArms, millis) // Purple elbow to -90 degrees. (delta = (-111))
	anglebasedRotation(motors.G_Elbows, -109, time_1_1, swingingArms, millis) // Green elbow to -130 degrees. (delta = -109)

	// 		1.2 Open the green grabbers
	openGrabbers(motors.G_Grabbers, millis);

	// 		1.3 Release the G_grabbers from Bar 0 and reach and grab Bar 1 with G_grabbers
	

	// 	2. Grab the second bar with the purple grabbers.
	// 		2.1 Open the purple grabbers

	// 		2.2 Swing the robot to the next bar (Bar 2) and close the P_grabbers

}

void c_brachiation(int barDistance, int direction, int *bar_number, unsigned long millis){

	static unsigned int timestamp = 0;
	int offset = 10;

	float angleOfRotation;
	float grabbingArms = 1.000, swingingArms = 1.000;

	// We calculate the angle that the elbows have to rotate using maffs

	angleOfRotation = (asin((barDistance - SHOULDER_TO_SHOULDER) / 2 / SHOULDER_TO_ELBOW) + offset) * direction;

	// We have to decide to either swing the "arms" or the "legs"

	if (*bar_number % 2 == 0){ // If the bar number is even, we grab the next bar with the purple grabbers
		openGrabbers(motors.P_Grabbers, millis);

		anglebasedRotation(motors.P_Elbows   ,  angleOfRotation, SWING_TIME, grabbingArms, millis);
		anglebasedRotation(motors.P_Shoulders,  angleOfRotation, SWING_TIME, grabbingArms, millis);
		anglebasedRotation(motors.G_Elbows   , -angleOfRotation, SWING_TIME, swingingArms, millis);
		anglebasedRotation(motors.G_Shoulders, -angleOfRotation, SWING_TIME, swingingArms, millis);

		if(millis - timestamp > SWING_TIME- GRABBERS_TIME){ // We need to begin the opening protocol a little bit before the end of the swing
			closeGrabbers(motors.P_Grabbers, millis);
		}
	}
	
	if ((*bar_number % 2) == 1){ // If the bar number is uneven, we grab the next bar with the green grabbers
		openGrabbers(motors.G_Grabbers, millis);

		anglebasedRotation(motors.G_Elbows   ,  angleOfRotation, SWING_TIME, grabbingArms, millis);
		anglebasedRotation(motors.G_Shoulders,  angleOfRotation, SWING_TIME, grabbingArms, millis);
		anglebasedRotation(motors.P_Elbows   , -angleOfRotation, SWING_TIME, swingingArms, millis);
		anglebasedRotation(motors.P_Shoulders, -angleOfRotation, SWING_TIME, swingingArms, millis);

		if(millis - timestamp > SWING_TIME - GRABBERS_TIME){ // We need to begin the closing protocol a little bit before the end of the swing
			closeGrabbers(motors.G_Grabbers, millis);
		}
	}
	
	if(millis - timestamp > SWING_TIME + GRABBERS_TIME){ // We have progressed to the next bar
		*bar_number++;
	}
}

void start_r_brachiation(){
	openGrabbers(motors.G_Grabbers, millis); //curling body up for first swing
	timebasedRotation(motors.G_Elbows, conv_j30(-1), 1000, millis);
	timebasedRotation(motors.P_Elbows, conv_j30(-1), 1000, millis);
	timebasedRotation(motors.G_Shoulders, conv_j30(1), 400, millis);
	
	timebasedRotation(motors.G_Elbows, conv_j30(2), 200, millis);
}
=======

void start_r_brachiation(int barDistance, int direction, int *bar_number, unsigned long millis){
	
	/* Motion of the robot from the end position of the continuous brachiation (both arms holding the bars
	   and the body in a horizontal position) to the start position of the first swing of the ricochetal
	   brachiation (look at the simulation). */
	
}


void r_brachiation(double Z_acceleration, double Y_acceleration, double Y_velocity, double tilt_angle,int bar_number, unsigned long millis, unsigned int pulse){//y is the forward axis here

	static int state = 0;
	double ZheightToBar;
	double XdistanceToBar;
	int barbuffer;
	switch (state)
	{
	case 0:
		//starting the swing
		barbuffer=bar_number;
		int A_PElbows=65,A_GShoulders=90,A_PShoulders=75;
		if (bar_number%2==0){
			A_PElbows=-A_PElbows;
			A_GShoulders=-A_GShoulders;
			A_PShoulders=-A_PShoulders;
		}
		anglebasedRotation(motors.P_Elbows, A_PElbows, 400, 1.0, millis);//at ca. 1.6s in NX motion
		anglebasedRotation(motors.G_Shoulders,A_GShoulders,350,1.0,millis);//ca. 1.7s-2.05s in motion
		anglebasedRotation(motors.P_Shoulders,A_PShoulders,400,1.0,millis);
		state = 1;

		break;
	case 1://check acceleration, velocity 
		if (Z_acceleration >= 5 && Y_acceleration >= 4 && Y_velocity>=0.8)//in m/s and m/s2
		{ //establishing two thresholds for Z and Y acceleration
			distancesToBar(readUltrasonic(pulse), tilt_angle, &ZheightToBar, &XdistanceToBar);
			if (ZheightToBar <= 0.4 && XdistanceToBar <= 0.4)
			state = 2;
		}

		break;
	case 2://close/open grabbers
		openGrabbers(motors.G_Grabbers, millis);
		openGrabbers(motors.P_Grabbers,millis);
		if(readPSensor(1)==1){//check for bar
			if (bar_number%2==1)
			closeGrabbers(motors.G_Grabbers, millis);
			else
			closeGrabbers(motors.P_Grabbers, millis);
			//maybe check if we it#s falling or not
			state=3;
		}

		break;
	case 3: //start the next swing
		int A_GElbows=-115,A_GShoulders=-55,A_PShoulders=-75,A_PElbows=-60;
		if (bar_number%2==0){
			A_PElbows=-A_PElbows;
			A_GShoulders=-A_GShoulders;
			A_PShoulders=-A_PShoulders;
			A_GElbows=-A_GElbows;
		}
		anglebasedRotation(motors.G_Elbows,A_GElbows, 450, 1.0, millis);//at ca. 2.2s in NX motion
		anglebasedRotation(motors.P_Shoulders,A_PShoulders,350,1.0,millis);//at ca. 2.2s
		anglebasedRotation(motors.G_Shoulders,A_GShoulders,300,1.0,millis);//ca. 2.3s in motion
		anglebasedRotation(motors.P_Elbows,A_PElbows,350,1.0,millis);//ca. 2.3s
		//ready for next swing
		if(barbuffer!=bar_number) 
		state=0;
		break;
	}
}

void finish_r_brachiation(double Z_acceleration, double Y_acceleration, double Y_velocity, double tilt_angle, unsigned long millis, unsigned int pulse){ //y is the forward axis here

	//  Make the robot stabilise itself after the completing the ricochetal brachiation
}

void anglebasedRotation(unsigned char motor_id, int degrees, int time_interval, float coefficient, unsigned long millis){

	int speedValue = 0;
	float rps = 0;

	rps = degrees / 360 / time_interval;								 // We calculate the rps
	speedValue = rps_to_speedValue(JOINT_MOTOR, (int)(rps * coefficient)); // We convert the rps into a speed value

	timebasedRotation(motor_id, speedValue, time_interval, millis);
}
