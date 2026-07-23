#include <lpc21xx.h>      // LPC2148 register definitions

//---- User Header Files ----//

#include "lcd.h"          // LCD interface functions
#include "keypad.h"       // 4×4 matrix keypad functions
#include "delay.h"        // Delay functions
#include "uart0.h"        // UART0 communication functions
#include "uart1.h"        // UART1 communication functions
#include "interrupt.h"    // Interrupt configuration and ISRs
#include "buzzer.h"       // Buzzer control functions

//---- Standard Library Headers ----//
// String handling functions
#include <string.h>       
#include <stdlib.h>       
/*
//---- Function Prototypes -----//

// Reads a multi-digit amount entered from the keypad
// str : LCD cursor position
// i   : Cursor offset
int get_amount(int str, int i);

// Clears the buffer and receives a string through UART0
// buff : Destination buffer
// size : Buffer size
void clr_buff_and_recv_uart0_str(char *buff, int size);

// Clears the buffer and converts an integer into a string
// buff  : Destination buffer
// size  : Buffer size
// value : Integer to convert
void clr_buff_and_int_conv_str(char *buff, int size, int value);

// Processes RFID card data and performs required operations
void processRFID(void);

// Receives data through UART0 using interrupt mode
// Waits until the UART0 ISR completes reception
void uart0_recv_intr(char *buff);

*/

void card_payment_frontend(void);
void cash_payment_frontend(void);
void int_to_str(char *buf, int size, int num);
void RFID(void);
