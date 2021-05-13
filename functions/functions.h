#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
// Add the function prototypes from the "functions.c" file here.

void closeGrabbers(unsigned char);
void openGrabbers(unsigned char);
void control_motor(unsigned char, int);
double distanceBarGrabbers(void);
double readUltrasonic(void);
void moveMotor(unsigned char, int, int);
double readAccleration(char);
double readUltrasonic(void);

#endif
