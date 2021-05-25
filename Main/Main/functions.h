#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

// Definitions

#define GRABBER_LENGTH 75
#define ARM_LENGTH     110
#define BODY_LENGTH    110
#define SWING_TIME 1000


// Function prototypes

void control_motor (unsigned char, int);
void moveMotor	   (unsigned char, int, int, unsigned long);
void rotateJMotor  (unsigned char, int, int);
void openGrabbers  (unsigned char,unsigned long);
void closeGrabbers (unsigned char,unsigned long);

void start_c_brachiation  (void);
void c_brachiation        (int);
void start_r_brachiation  (void);
void r_brachiation        (int);
void finish_r_brachiation (void);

int BarDetected();
int conv_j30(double); // A function to convert rps to speed for the code. (Joint motor)

double detectBarGrabbers (void);
double readUltrasonic    (unsigned int);
double readAcceleration  (char);

#endif
