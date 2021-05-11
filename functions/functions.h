#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
// Add the function prototypes from the "functions.c" file here.
void closeGrabbers(void);
void openGrabbers(void);
void control_motor(unsigned char motor_id, int on_value);
double detectBarGrabbers(void);
double readUltrasonic(void);
void moveMotor(unsigned char motor_id, int on_value, int time_on);
double readAccleration(char axis);
#endif
