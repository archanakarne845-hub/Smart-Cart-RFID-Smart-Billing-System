#include "defines.h"   // Brings in global variables like uart_fd

#ifndef UART_H         // Include guard: prevents multiple inclusion
#define UART_H

// --- UART initialization and communication functions ---

// Initialize UART (open device, configure baud rate, frame format)
void uart_init();                  

// Receive a single character from UART
char uart_rx(void);                   

// Transmit a single character over UART
void uart_tx(char ch);                

// Receive a string until '$' is encountered, store in buffer
void uart_str_rx(char *buf,int size); 

// Transmit a string over UART
void uart_str_tx(char *str);          

#endif

