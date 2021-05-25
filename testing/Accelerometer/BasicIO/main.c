
#define F_CPU 16E6
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <math.h>
#include "i2cmaster.h"	// Enable communication to sensor (i2c_init())							(twimaster.c)
#include "lcd.h"		// Enable the LCD (lcd_init(), lcd_clear(), lcd_gotoxy())				(lcd.c)
#include "MMA8451.h"


#define SENSITIVITY_2G    4096   
#define PI 3.14159265

void MMA8451_init();
void i2c_write_register8(unsigned char reg, unsigned char value);
unsigned char i2c_read_register8(unsigned char reg);
unsigned char getOrientation(void);
void Gravity(void);
void getPosition(void);
//void ignoreGravity(void);

//flag
unsigned char a = 0;

unsigned char x, y,z,Xoffset,Yoffset,Zoffset;
int x_val, y_val, z_val;
float x_val_float, y_val_float, z_val_float;
float x_angle, y_angle,z_angle;
float x_g, y_g,z_g;
float x_acceleration, y_acceleration, z_acceleration;// they store acceleration minus gravity
unsigned char data_array[10];
unsigned char orientation;
//variables used to subtract gravity acceleration from our results
float x_gravity, y_gravity, z_gravity;
//Actual coordinates
float X_Coordinate, Y_Coordinate, Z_Coordinate = 0;

int main(void)
{
	i2c_init();
	LCD_init();
	MMA8451_init();
	
	while (1)
	 {
		  //Reading the accelerometer's registers
		 data_array[0] = i2c_read_register8(MMA8451_REG_OUT_X_MSB); 
		 data_array[1] = i2c_read_register8(MMA8451_REG_OUT_X_LSB);
		 data_array[2] = i2c_read_register8(MMA8451_REG_OUT_Y_MSB);
		 data_array[3] = i2c_read_register8(MMA8451_REG_OUT_Y_LSB); 
		 data_array[4] = i2c_read_register8(MMA8451_REG_OUT_Z_MSB);
		 data_array[5] = i2c_read_register8(MMA8451_REG_OUT_Z_LSB);
		 //x-axis acceleration normalization
		 x_val = ((short) (data_array[0]<<8 | data_array[1])) >> 2; // Compute 14-bit X-axis output value
		 x_val_float = x_val/16384.0;// +/-2g range -> 1g = 16384/4 = 4096 counts
		 //y-axis acceleration normalization
		 y_val = ((short) (data_array[2]<<8 | data_array[3])) >> 2; // Compute 14-bit Y-axis output value
		 y_val_float = y_val/16384.0;// +/-2g range -> 1g = 16384/4 = 4096 counts
		 //z-axis acceleration normalization
		 z_val = ((short) (data_array[4]<<8 | data_array[5])) >> 2; // Compute 14-bit Z-axis output value
		 z_val_float = z_val/16384.0;// +/-2g range -> 1g = 16384/4 = 4096 counts
		 
		 Xoffset = x_val / 8 * (-1);        // Compute X-axis offset correction value
		 Yoffset = y_val / 8 * (-1);        // Compute Y-axis offset correction value
		 Zoffset = (x_val - SENSITIVITY_2G) / 8 * (-1);          // Compute Z-axis offset correction value
		 
		 x_val+=Xoffset;
		 y_val+=Yoffset;
		 z_val+=Zoffset;
		 
		 //Calculating the x-axis angle
		 x_angle = atan(x_val_float/z_val_float); 
		 x_angle = x_angle*(180.0/3.141592);
		 //Calculating the y-axis angle
		 y_angle = atan(y_val_float/z_val_float); 
		 y_angle = y_angle*(180.0/3.141592);
		 //Calculating the z-axis angle
		 z_angle = atan(x_val_float/y_val_float);
		 z_angle = z_angle*(180.0/3.141592);		
		 
		 Gravity();//subtracting gravity
		 orientation = getOrientation(); //reading the detected orientation
		 getPosition();
		 
		 x_g = ((float) x_val) / SENSITIVITY_2G; //Calculating the x-axis acceleration
		 y_g = ((float) y_val) / SENSITIVITY_2G; //Calculating the y-axis acceleration
		 z_g = ((float) z_val) / SENSITIVITY_2G; //Calculating the z-axis acceleration
		 
		 // ***Printing the x-axis angle/acceleration***
		 //LCD_set_cursor(0,0); printf("X:%6.2f [Deg]", x_angle);  // X ANGLE
		 //LCD_set_cursor(0,3); printf("X:%f g", x_g);  // X ACC
		 //LCD_set_cursor(0,0); printf("%7d", x_val);
		 //LCD_set_cursor(0,0); printf("%f", x_val_float);

		 // ***Printing the y-axis angle/acceleration***
		 //LCD_set_cursor(0,1); printf("Y:%6.2f [Deg]", y_angle); // Y ANGLE
		 //LCD_set_cursor(0,2); printf("Y:%f g", y_g);  // Y ACC
		 //LCD_set_cursor(0,1); printf("%d", y_val);
		 //LCD_set_cursor(0,1); printf("%f", y_val_float);
		 
		 // ***Printing the z-axis angle/acceleration***
		 LCD_set_cursor(0,3); printf("Z:%6.2f [Deg]", z_angle); // Z ANGLE
		 LCD_set_cursor(0,2); printf("Z:%f g", z_g);  // Z ACC
		 //LCD_set_cursor(0,2); printf("%7d", z_val);
		 
		 // ***Cycle counting***
		 //LCD_set_cursor(0,3); printf("%7d", iterations);
		 
		 // ***Printing the detected orientation***
		 //LCD_set_cursor(0,3); printf("%d",orientation);
		 		 
		 // ***Printing Registers***
		 //LCD_set_cursor(0,0); printf("%d", data_array[0]);
		 //LCD_set_cursor(2,0); printf("%d", data_array[1]);
		 //LCD_set_cursor(3,0); printf("%d", data_array[4]);
		 //LCD_set_cursor(4,0); printf("%d", data_array[5]);
		 
		 // Printing Coordinates
		 //LCD_set_cursor(0,0); printf("X: %f", X_Coordinate);
		 //LCD_set_cursor(0,1); printf("Y: %f", Y_Coordinate);
		 //LCD_set_cursor(0,2); printf("Z: %f", Z_Coordinate);
		 
		 // Printing acceleration minus gravity
		 LCD_set_cursor(0,0); printf("%f", x_acceleration);
		 LCD_set_cursor(0,1); printf("%f", y_acceleration);
		 //LCD_set_cursor(0,2); printf("%f", z_acceleration);
	 }
}

void MMA8451_init()
{
	//i2c_start(0x3A+I2C_WRITE);
	//i2c_write(MMA8451_REG_WHOAMI);
	//i2c_rep_start(0x3A+I2C_READ);
	//x = i2c_readNak();		//if x not 0x3A then false
	//i2c_stop();
	
	//i2c_write_register8(MMA8451_REG_CTRL_REG2, 0x40)
	
	x = i2c_read_register8(MMA8451_REG_WHOAMI);
	
	i2c_write_register8(MMA8451_REG_CTRL_REG2, 0x40);		// reset
	_delay_ms(10);											// very important
	// test returværdi fra i2c_start om device er busy
	
	y = i2c_read_register8(MMA8451_REG_CTRL_REG2);
	
	// enable 2G range
	i2c_write_register8(MMA8451_REG_XYZ_DATA_CFG, MMA8451_RANGE_2_G);
	// High res
	i2c_write_register8(MMA8451_REG_CTRL_REG2, 0x02);
	// DRDY on INT1
	i2c_write_register8(MMA8451_REG_CTRL_REG4, 0x01);
	i2c_write_register8(MMA8451_REG_CTRL_REG5, 0x01);

	// Turn on orientation config
	i2c_write_register8(MMA8451_REG_PL_CFG, 0x40);

	// Activate at max rate, low noise mode,
	i2c_write_register8(MMA8451_REG_CTRL_REG1, 0x20 | 0x10 | 0x08 | 0x04 | 0x01);
	
	
}

void i2c_write_register8(unsigned char reg, unsigned char value)
{
	i2c_start(MMA8451_DEFAULT_ADDRESS+I2C_WRITE);
	i2c_write(reg);
	i2c_write(value);
	i2c_stop();
}

unsigned char i2c_read_register8(unsigned char reg)
{
	unsigned char ret_val=0;
	i2c_start(MMA8451_DEFAULT_ADDRESS+I2C_WRITE);
	i2c_write(reg);
	i2c_rep_start(MMA8451_DEFAULT_ADDRESS+I2C_READ);
	ret_val = i2c_readNak();
	i2c_stop();
	return(ret_val);
}

unsigned char getOrientation(void) {
	return i2c_read_register8(MMA8451_REG_PL_STATUS) & 0x07;
}

void Gravity(void)
{
	float temp =  PI/180;
	
	x_gravity = sin(x_angle*temp);
	y_gravity = sin(y_angle*temp);
	
	if (x_g > 0)
	{
		if (x_gravity > 0)
		{
			if (((x_gravity - x_g) < 0.1) && ((x_gravity - x_g) > -0.1))
			{
				x_gravity = x_g;
			}
		}
		if (x_gravity < 0)
		{
			if (((x_gravity + x_g) < 0.1) && ((x_gravity + x_g) > -0.1))
			{
				x_gravity = x_g;
			}
		}
	}
	if (x_g < 0)
	{
		if (x_gravity > 0)
		{
			if (((x_gravity + x_g) < 0.1) && ((x_gravity + x_g) > -0.1))
			{
				x_gravity = x_g;
			}
		}
		if (x_gravity < 0)
		{
			if (((x_gravity - x_g) < 0.1) && ((x_gravity - x_g) > -0.1))
			{
				x_gravity = x_g;
			}
		}
	}
	if (y_g > 0)
	{
		if (y_gravity > 0)
		{
			if (((y_gravity - y_g) < 0.1) && ((y_gravity - y_g) > -0.1))
			{
				y_gravity = y_g;
			}
		}
		if (y_gravity < 0)
		{
			if (((y_gravity + y_g) < 0.1) && ((y_gravity + y_g) > -0.1))
			{
				y_gravity = y_g;
			}
		}
	}
	if (y_g < 0)
	{
		if (y_gravity > 0)
		{
			if (((y_gravity + y_g) < 0.1) && ((y_gravity + y_g) > -0.1))
			{
				y_gravity = y_g;
			}
		}
		if (y_gravity < 0)
		{
			if (((y_gravity - y_g) < 0.1) && ((y_gravity - y_g) > -0.1))
			{
				y_gravity = y_g;
			}
		}
	}
	
	if (x_gravity > 0)
	{
		if (x_g > 0)
		{
			x_acceleration = x_g - x_gravity;
		}
	
		if (x_g < 0)
		{
			x_acceleration = x_g + x_gravity;
		}
	}
	
	else
	{
		if (x_g > 0)
		{
			x_acceleration = x_g + x_gravity;
		}
		if (x_g < 0)
		{
			x_acceleration = x_g - x_gravity;
		}
	}
	
	if (y_gravity > 0)
	{
		if (y_g > 0)
		{
			y_acceleration = y_g - y_gravity;
		}
		if (y_g < 0)
		{
			y_acceleration = y_g + y_gravity;
		}
	}
	
	else
	{
		if (y_g > 0)
		{
			y_acceleration = y_g + y_gravity;
		}
		
		if (y_g < 0)
		{
			y_acceleration = y_g - y_gravity;
		}
	}
}



void getPosition(void)
{

	//calculating the distance traveled since last data received and changing the coordinate based on that change
	//80ms between data
	
	X_Coordinate = X_Coordinate + (0.5*x_acceleration*0.0064);
	Y_Coordinate = Y_Coordinate + (0.5*y_acceleration*0.0064);
	//Z_Coordinate = Z_Coordinate + (0.5*z_acceleration*0.0064);
}

//Old unused functions, ignore them
/*
void ignoreGravity(void)
{
	float saved_angle_x = 0;
	float saved_angle_y = 0;
	float saved_angle_z = 0;
	if ((x_angle > 30 && x_angle > saved_angle_x) | (x_angle < -30 && x_angle < saved_angle_x))
	{
		saved_angle_x = x_angle;
		x_gravity = x_g;
		y_gravity = y_g;
		z_gravity = z_g;
	}
	if ((y_angle > 30 && y_angle > saved_angle_y) | (y_angle < -30 && y_angle < saved_angle_y))
	{
		saved_angle_y = y_angle;
		x_gravity = x_g;
		y_gravity = y_g;
		z_gravity = z_g;
	}
	if ((z_angle > 30 && z_angle > saved_angle_z) | (z_angle < -30 && z_angle < saved_angle_z))
	{
		saved_angle_z = z_angle;
		x_gravity = x_g;
		y_gravity = y_g;
		z_gravity = z_g;
	}
}
*/