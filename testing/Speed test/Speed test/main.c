/*
 * Speed test.c
 *
 * Created: 5/21/2021 12:26:22 PM
 * Author : Philip J
 */ 

#include <avr/io.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

int main(void)
{
	// Setup
    double time_spent = 0.0;
	
	clock_t begin = clock();
	
	// Code to time.
	sin(1);
	
	// Getting the time elapsed for the desired code.
	clock_t end =clock();
	
	time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
	
	printf("Time elapsed %f", time_spent);
	
	return 0;
}

