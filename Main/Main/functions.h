#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
// Add the function prototypes from the "functions.c" file here.

void closeGrabbers(unsigned char,unsigned long);
void openGrabbers(unsigned char,unsigned long);
void control_motor(unsigned char, int);
double distanceBarGrabbers(void);
double readUltrasonic(unsigned int);
void distancesToBar(double ,double, double*, double*);
void moveMotor(unsigned char, int, int,unsigned long);
double readAccleration(char);
int BarDetected(void);
int conv_j30(double); // A function to convert rps to speed for the code. (Joint motor)

struct Motors {

	// Motor IDs
	
	unsigned char G_Grabbers  = M1;
	unsigned char G_Elbows    = M2;
	unsigned char G_Shoulders = M3;
	
	unsigned char P_Elbows	  = M4;
	unsigned char P_Shoulders = M5;
	unsigned char P_Grabbers  = M6;
};

#endif
