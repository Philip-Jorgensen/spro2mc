#ifndef ACCELEROMETER_H INCLUDED
#define ACCELEROMETER_H INCLUDED
// Add the function prototypes from the "functions.c" file here.

#define SENSITIVITY_2G    4096   
#define PI 3.14159265

void accel_init(void);
void MMA8451_init(void);
void main_accel(void);
void i2c_write_register8(unsigned char reg, unsigned char value);
unsigned char i2c_read_register8(unsigned char reg);
unsigned char getOrientation(void);
void Gravity(void);
void getPosition(void);

#endif
