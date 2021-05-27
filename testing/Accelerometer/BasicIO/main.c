
#define F_CPU 16E6
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <math.h>
#include <avr/interrupt.h>
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
double AverageDistance(double);
void Ultrasonic_Sensor(void);


//flag
unsigned char a = 0;
char low_pass_fix;
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

volatile unsigned int pulse=0;//time echo pin signal is high
volatile int i=0;//used for identifying edge type

double distance=0;//distance measured
unsigned long counter=0;//used with printfs to avoid delay
unsigned long lstC=0;//used with printfs to avoid delay
char Ultrasonic_Position[2]; // keeps coordinates of ultrasonic sensor itself [X,Y,Z]

typedef struct  
{
	char X_bars[1];
	char Y_bars[1];
	char Z_bars[1];
}Bars_t;
Bars_t Bars_position[7];

int main(void)
{
	uart_init(); // Open the communication to the micro controller
	i2c_init(); // Initialize the i2c communication.
	io_redirect(); // Redirect the input/output to the computer.
	/*
	i2c_init();
	LCD_init();
	MMA8451_init();
	*/
	DDRD=0xFB;//set PD2 input, rest as output
	EICRA |= 1<<ISC00;//set INT0(PD2) to trigger on any logic change
	EIMSK |=1<<INT0;//turn on interrupt
	sei();//enable global interrups
	PORTD|=1<<PIND4;//trig pin output to ultrasonic, set PD4 high
	_delay_us(10);//needs 10us pulse to start
	PORTD&=~(1<<PIND4);//set PD4 to low
	
	for (int i=1;i<=7;i++)
	{
		Bars_position[i].X_bars[0] = -15;
		Bars_position[i].Z_bars[0] = 33;
		Bars_position[i].X_bars[1] = 15;
		Bars_position[i].Z_bars[1] = 33;
	}
	
	Bars_position[0].X_bars[0] = -15;
	Bars_position[0].Y_bars[0] = 0;
	Bars_position[0].Z_bars[0] = 33;
	Bars_position[0].X_bars[1] = 15;
	Bars_position[0].Y_bars[1] = 0;
	Bars_position[0].Z_bars[1] = 33;

	Bars_position[1].Y_bars[0] = 15;
	Bars_position[1].Y_bars[1] = 15;

	Bars_position[2].Y_bars[0] = 30;
	Bars_position[2].Y_bars[1] = 30;

	Bars_position[3].Y_bars[0] = 54;
	Bars_position[3].Y_bars[1] = 54;

	Bars_position[4].Y_bars[0] = 78;
	Bars_position[4].Y_bars[1] = 78;

	Bars_position[5].Y_bars[0] = 78+41;
	Bars_position[5].Y_bars[1] = 78+41;

	Bars_position[6].Y_bars[0] = 78+41*2;
	Bars_position[6].Y_bars[1] = 78+41*2;

	Bars_position[7].Y_bars[0] = 78+41*3;
	Bars_position[7].Y_bars[1] = 78+41*3;

	
	while (1)
	 {
		 Ultrasonic_Sensor();
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
		 //LCD_set_cursor(0,3); printf("Z:%6.2f [Deg]", z_angle); // Z ANGLE
		 //LCD_set_cursor(0,2); printf("Z:%f g", z_g);  // Z ACC
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
		 LCD_set_cursor(0,0); printf("X: %.1f", X_Coordinate);
		 LCD_set_cursor(0,1); printf("Y: %.1f", Y_Coordinate);
		 LCD_set_cursor(0,2); printf("Z: %.1f", Z_Coordinate);
		 
		 // Printing acceleration minus gravity
		 //LCD_set_cursor(0,0); printf("%f", x_acceleration);
		 //LCD_set_cursor(0,1); printf("%f", y_acceleration);
		 //LCD_set_cursor(0,3); printf("%f", z_acceleration);

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
	z_gravity = 0.9 * z_gravity + 0.1 * z_g;//Low pass filter for Z axis
	
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
	
	if (z_gravity > 0)
	{
		if (z_g > 0)
		{
			z_acceleration = z_g - z_gravity;
		}
		if (z_g < 0)
		{
			z_acceleration = z_g + z_gravity;
		}
	}
		
	else
	{
		if (z_g > 0)
		{
			z_acceleration = z_g + z_gravity;
		}
			
		if (z_g < 0)
		{
			z_acceleration = z_g - z_gravity;
		}
	}
}



void getPosition(void)
{
	//calculating the distance traveled since last data received and changing the coordinate based on that change
	//80ms between data
	X_Coordinate = X_Coordinate + (0.5*x_acceleration*0.0064*981);
	Y_Coordinate = Y_Coordinate + (0.5*y_acceleration*0.0064*981);
	if (low_pass_fix == 50)
	{
		Z_Coordinate = Z_Coordinate + (0.5*z_acceleration*0.0064*981);
	}
	if (low_pass_fix < 50)
	{
		low_pass_fix++;
	}
	Ultrasonic_Position[0] = X_Coordinate - 52;
	Ultrasonic_Position[1] = Y_Coordinate;
	Ultrasonic_Position[2] = Z_Coordinate;
}

void Ultrasonic_Sensor(void)
{
	distance=((double)pulse)*0.0000000625*342.2/2;//pulse*time for one tick (1/16mhz)*speed of sound(20C)/2
	counter++;//just for printf
	
	//do the printfs every half a second or so
	if(counter-lstC>50000)
	{
		lstC=counter;
	}
	_delay_ms(60);
}

ISR(INT0_vect){//interrupt routine
	if(i==0)// low to high
	{
		
		TCCR1B |= (1<<CS10);//start timer 1 (16bits) with no prescaler
		i = 1;
	}
	else//high to low
	{
		TCCR1B = 0;//stop timer1
		pulse = TCNT1;//value from timer1
		TCNT1 = 0;//reset
		i = 0;
	}
}

double AverageDistance(double distance){//just sth that i copied from last semester to get an average value, can be ignored
	static double arrSpeedsForAvg[]={0,0,0,0,0,0,0,0,0,0};
	const int arrSpeedsLength = 10;
	static double arrAvg=0;
	
	for(int i=arrSpeedsLength;i>0;i--){
		arrSpeedsForAvg[i]=arrSpeedsForAvg[i-1];
	}
	arrSpeedsForAvg[0]=distance;
	
	for(int i=0;i<arrSpeedsLength;i++){
		arrAvg+=arrSpeedsForAvg[i];
	}
	arrAvg=arrAvg/(double)arrSpeedsLength;
	return arrAvg;
}