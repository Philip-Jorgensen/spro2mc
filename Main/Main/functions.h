#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

<<<<<<< HEAD
void control_motor	    (unsigned char, int);
void timebasedRotation  (unsigned char, int, int, unsigned long);
void anglebasedRotation (unsigned char, int, int, float);
void openGrabbers       (unsigned char, unsigned long);
void closeGrabbers      (unsigned char, unsigned long);

void start_c_brachiation  (void);
void c_brachiation        (int);
void start_r_brachiation  (void);
void r_brachiation        (int);
void finish_r_brachiation (void);
void distancesToBar		  (double, double , double*, double*);

int BarDetected();

double detectBarGrabbers (void);
double readUltrasonic    (unsigned int);
double readAcceleration  (char);
=======
// Add the function prototypes from the "functions.c" file here.
void closeGrabbers(unsigned char,unsigned long);
void openGrabbers(unsigned char,unsigned long);
void control_motor(unsigned char, int);
double distanceBarGrabbers(void);
double readUltrasonic(unsigned int);
void distancesToBar(double ,double, double*, double*);
void timebasedRotation(unsigned char, int, int,unsigned long);
double readAccleration(char);
int BarDetected(void);
int rps_to_speedValue(double); // A function to convert rps to speed for the code. (Joint motor 30RPM)

// Definitions

#define JOINT_MOTOR 0; // 30RPS Worm gear motor

#define SWING_TIME 1000; // Time between each swing in the continuous brachiation

// Motor IDs
#define G_Grabbers 1;
#define G_Elbows 2;
#define G_Shoulders 3;

#define P_Elbows 4;
#define P_Shoulders 5;
#define P_Grabbers 6;
>>>>>>> 9325505ff2cd408024b6a0459496042ec2223e6c

#endif
