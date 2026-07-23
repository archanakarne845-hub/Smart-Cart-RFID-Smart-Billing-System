// Include the header file containing function declarations
#include "delay.h"

// Function to generate a delay in microseconds
void delay_us(unsigned int dlyus)
{
	int i;
	// Empty loop to create the required delay
	for(i=dlyus*12; i>0; i--);
}

// Function to generate a delay in milliseconds
void delay_ms(unsigned int dlyms)
{
	int i;
	// Empty loop to create the required delay
	for(i=dlyms*12000; i>0; i--);
}

// Function to generate a delay in seconds
void delay_s(unsigned int dlys)
{
	int i;
	// Empty loop to create the required delay
	for(i=dlys*12000000; i>0; i--);
}

