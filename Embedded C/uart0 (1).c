// LPC2148 register definitions
#include <lpc21xx.h>

// Delay function declarations
#include "delay.h"

// UART0 function declarations and macros
#include "uart0.h"

// Function to initialize UART0
void Init_uart0(void)
{
	// Clear PINSEL0 bits for P0.0 and P0.1
	PINSEL0 &=~(15<<0);
	
	// Configure P0.0 as TXD0 and P0.1 as RXD0
	PINSEL0 |= 0x05;
	
	// Enable access to Divisor Latch Registers (DLL & DLM)
    // and set UART data format (8-bit, no parity, 1 stop bit)
	U0LCR = (1<<DLAB)|WORD_LEN;
	
	 // Load baud rate divisor (higher 8 bits)
	U0DLM = DIVISOR>>8;
	
	// Load baud rate divisor (lower 8 bits)
	U0DLL = DIVISOR;
	
	 // Disable DLAB to access normal UART registers
	U0LCR &=~(1<<DLAB);
}

// Function to transmit one character through UART0
void tx_uart0(unsigned char data)
{
	// Load character into Transmit Holding Register
	U0THR = data;
	
	// Wait until transmission is complete
	while(!((U0LSR>>TEMT)&1));
}

// Function to receive one character through UART0
unsigned char rx_uart0(void)
{
	// Wait until data is available in Receive Buffer
	while(!((U0LSR>>DR)&1));
	
	// Return the received character
	return U0RBR;
}

// Function to transmit a null-terminated string
void uart0_txstring(char *str)
{
	// Send characters one by one until '\0' is reached
	while(*str)
	{
		tx_uart0(*str++);
	}
}

// Function to receive a string ending with '$'
void uart0_rxstring(char *str)
{
	char ch;
	int i=0;
	while(1)
	{
		// Receive one character
		ch = rx_uart0();
		// Stop receiving when '$' is encountered
		if(ch == '$')
		{
			str[i] = '\0';// Null-terminate the string
			break;
		}
		// Store received character
		str[i++] = ch;
	}
}

