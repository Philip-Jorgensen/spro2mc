#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
// Add the function prototypes from the "functions.c" file here.

void control_motor	    (unsigned char, int);
void timebasedRotation  (unsigned char, int, int, unsigned long);
void anglebasedRotation (unsigned char, int, int, float,unsigned long);
void unlockGrabbers		(unsigned char);
void lockGrabbers		(unsigned char);
void openGrabbers		(unsigned char, unsigned long);
void closeGrabbers		(unsigned char, unsigned long);

void start_c_brachiation  (int, int, int*, unsigned long);
void c_brachiation		  (int, int, int*, unsigned long);
void start_r_brachiation  (int, int, int*, unsigned long);
void r_brachiation		  (double, double, double, double,int, unsigned long, unsigned int);
void finish_r_brachiation (double, double, double, double, unsigned long, unsigned int);
void trackArmPosition     (float, float, float, float, float, unsigned long);

int BarDetected(void);
int readPSensor(unsigned int);
int rps_to_speedValue(double, int);

//double detectBarGrabbers (void);
double readUltrasonic    (unsigned int);
double readAcceleration  (char);

#endif
