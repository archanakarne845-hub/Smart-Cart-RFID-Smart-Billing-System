#include <lpc21xx.h>      // LPC2148 register definitions
#include "uart1.h"        // UART1 macros and function declarations

// Function to initialize UART1
void Init_uart1(void)
{
    // Clear PINSEL0 bits corresponding to P0.8 and P0.9
    PINSEL0 &= ~(0xF << 16);

    // Configure P0.8 as TXD1 and P0.9 as RXD1
    PINSEL0 |= (0x5 << 16);

    // Enable access to Divisor Latch Registers (DLL & DLM)
    // and configure UART for 8-bit data, no parity, 1 stop bit
    U1LCR = (1 << DLAB) | WORD_LEN;

    // Load the upper 8 bits of the baud rate divisor
    U1DLM = DIVISOR >> 8;

    // Load the lower 8 bits of the baud rate divisor
    U1DLL = DIVISOR;

    // Disable DLAB to access normal UART registers
    U1LCR &= ~(1 << DLAB);

    // Enable and reset both Transmit and Receive FIFOs
    //U1FCR = 0x07;

    // Enable UART1 Receive Data Available interrupt
    U1IER = 0x01;
}

