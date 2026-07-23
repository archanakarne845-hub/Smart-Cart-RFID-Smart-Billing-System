// LPC2148 register definitions
#include <lpc21xx.h> 
// UART0 communication functions
#include "uart0.h"
// UART1 communication functions (RFID Reader)
#include "uart1.h" 
// Interrupt initialization and ISR functions
#include "interrupt.h"
// LCD interface functions
#include "lcd.h" 
// Delay functions
#include "delay.h" 
// 4×4 Matrix Keypad functions
#include "keypad.h"  
// RFID processing and billing functions
#include "rf_reader.h"  
// Buzzer control functions
//#include "buzzer.h"       
// String handling functions
#include <string.h>       
#include <stdlib.h>      

//---------------------------------------------------------------
// Function : main()
// Purpose  :
//      Entry point of the Smart Cart RFID Billing System.
//      Initializes all peripherals and continuously processes
//      RFID card operations.
//---------------------------------------------------------------
 int switch_to_cash=0;
int main()
{
	
    // Initialize UART0 for communication with PC/database
    Init_uart0();

    // Initialize UART1 for RFID reader communication
    Init_uart1();

     // Initialize 4×4 Matrix Keypad
    InitKPM();

	 // Initialize external interrupts and UART interrupts
    Init_interrupt();
    // Initialize 16x2 LCD
    InitLCD();

	 cmdLCD(CLEAR_LCD);
	 strLCD("smart Billing");
	 cmdLCD(0xC0);
	 strLCD("system");
	 delay_s(3); 
    // Infinite loop
    while(1)
    {
	if(switch_to_cash==1)
	{
	switch_to_cash=0;
	cash_payment_frontend();
	}
        RFID();
    }
}

