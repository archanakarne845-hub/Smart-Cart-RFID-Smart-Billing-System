#ifndef interrupt_h
#define interrupt_h
// LPC2148 register definitions
#include <lpc21xx.h>      

//----- External Interrupt Pin Numbers ------//
// P0.16 connected to External Interrupt 0 (Entry switch)
#define EINT0 0x15400001
// P0.3 connected to External Interrupt 1 (Delete switch)
#define EINT1 0x000000C0  
// P0.7 connected to External Interrupt 2 (Exit switch)
#define EINT2 0x0000C000        

//----- UART Interrupt -------//
// UART1 interrupt number in VIC
#define UART1 7            

//----- VIC Interrupt Channel Numbers ------//

// VIC channel number for EINT0
#define EINT0_CHANNEL 14 
// VIC channel number for EINT1
#define EINT1_CHANNEL 15 
// VIC channel number for EINT2
#define EINT2_CHANNEL 16      

//---- External Variables ----//

// UART1 receive status
// Set when complete UART1 data is received
extern volatile int uart1_ready; 
// UART1 receive buffer
extern char uart1_buf[20];        

// UART0 receive status
// UART0 receive buffer
extern char uart0_buffer[20];
// Set when complete UART0 data is received
extern volatile int uart0_ready;      

//---- Function Prototypes ----//

// Initializes UART and external interrupts
void Init_interrupt(void);

// UART1 Interrupt Service Routine (RFID Receiver)
void uart1_isr(void) __irq;

// UART0 Interrupt Service Routine (Serial Communication)
//void uart0_isr(void) __irq;

// External Interrupt 0 ISR (Entry Button)
void entry_isr(void) __irq;

// External Interrupt 1 ISR (Delete Button)
void delete_isr(void) __irq;

// External Interrupt 2 ISR (Exit Button)
void exit_isr(void) __irq;
#endif
