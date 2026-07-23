// UART0 functio n declarations
#include "uart0.h"  

// Interrupt-related definitions and macros
#include "interrupt.h"      

// Maximum size of UART1 receive buffer
//#define MAX 20              

// UART1 receive buffer and status variables
char uart1_buf[20];

// Current index in UART1 buffer
volatile int uart1_index = 0; 

// Flag indicating complete UART1 data received
volatile int uart1_ready = 0;  

char ch;
int i=0;

// Maximum size of UART0 receive buffer
//#define RX_SIZE 100         

// UART0 receive buffer and status variables
char uart0_buffer[20];

// Current index in UART0 buffer
volatile int uart0_index = 0;   

// Flag indicating complete UART0 data received
volatile int uart0_ready = 0;      

// Flags updated by external interrupt service routines
extern int entry_mode;
extern int delete_mode;
extern int exit_mode;

/* Function: Init_interrupt()
Purpose : Configure UART and External Interrupts*/
void Init_interrupt(void)
{
    // Clear PINSEL bits for EINT0, EINT1, and EINT2 pins
    PINSEL0 &= ~((EINT1) | (EINT2));
    PINSEL1 &= ~(EINT0);

    // Configure pins as External Interrupt pins
    PINSEL0 |= (EINT1) | (EINT2);
    PINSEL1 |= (EINT0);

    // Enable UART0, UART1, EINT0, and EINT1 interrupts in VIC
    VICIntEnable |= (1 << EINT2_CHANNEL) | (1 << UART1) |
                    (1 << EINT0_CHANNEL) | (1 << EINT1_CHANNEL);

    // Assign UART1 ISR to VIC Slot 0
    VICVectCntl0 = (1 << 5) | UART1;
    VICVectAddr0 = (unsigned int)uart1_isr;

    // Assign EINT0 ISR (Entry Switch) to VIC Slot 1
    VICVectCntl1 = (1 << 5) | EINT0_CHANNEL;
    VICVectAddr1 = (unsigned int)entry_isr;

    // Assign EINT1 ISR (Delete Switch) to VIC Slot 2
    VICVectCntl2 = (1 << 5) | EINT1_CHANNEL;
    VICVectAddr2 = (unsigned int)delete_isr;

    // Assign EINT2 ISR (Exit Switch) to VIC Slot 3
    VICVectCntl3 = (1 << 5) | EINT2_CHANNEL;
    VICVectAddr3 = (unsigned int)exit_isr;

    // Configure EINT0, EINT1, and EINT2 as edge-triggered interrupts
    EXTMODE = (1 << 0) | (1 << 1) | (1 << 2);

    // Enable UART0 Receive interrupt
   // U0IER = 0x01;
}

/* UART1 Interrupt Service Routine
 Receives RFID data between STX (0x02) and ETX (0x03)*/
void uart1_isr(void) __irq
{
    // Check if Receive Data Available interrupt occurred
    if(U1IIR & 0x04)
    {
        // Read received character
        ch = U1RBR;

        // Start of RFID packet
        if (ch == 0x02)
        {
            uart1_index = 0;
        }
        // End of RFID packet
        else if (ch == 0x03)
        {
            uart1_buf[uart1_index] = '\0';   // Terminate string
            uart1_ready = 1;
			uart1_index = 0;                    // Data ready flag
        }
        else
        {
			if(uart1_index < 19)
            	// Store received character
            	uart1_buf[uart1_index++] = ch;
        }
    }
    /*else
    {
        // Dummy read to clear interrupt
        dummy = U1IIR;
    } */

    // Notify VIC that interrupt servicing is complete
    VICVectAddr = 0;
}


// External Interrupt 0 ISR (Entry Button)
void entry_isr(void) __irq
{
    entry_mode = 1;        // Enable entry mode

    EXTINT = 1 << 0;       // Clear EINT0 interrupt flag

    VICVectAddr = 0;       // End of interrupt
}

// External Interrupt 1 ISR (Delete Button)
void delete_isr(void) __irq
{
    delete_mode = 1;        // Enable delete mode

    EXTINT = 1 << 1;        // Clear EINT1 interrupt flag

    VICVectAddr = 0;        // End of interrupt
}

// External Interrupt 2 ISR (Exit Button)
void exit_isr(void) __irq
{
    exit_mode = 1;          // Enable exit mode

    EXTINT = 1 << 2;        // Clear EINT2 interrupt flag

    VICVectAddr = 0;        // End of interrupt
}

