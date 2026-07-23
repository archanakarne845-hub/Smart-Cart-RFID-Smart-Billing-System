// String handling functions
#include <string.h>
#include "interrupt.h"

//-------UART0 Pin Definitions------
// P0.0 configured as UART0 Transmit (TXD0)
#define TX_PIN 0x00000001

 // P0.1 configured as UART0 Receive (RXD0)
#define RX_PIN 0x00000004

//------ Clock Configuration ---------
// Crystal oscillator frequency = 12 MHz
#define FOSC 12000000

// CPU Clock = 60 MHz (PLL ×5)
#define CCLK (FOSC * 5)

// Peripheral Clock = 15 MHz
#define PCLK (CCLK / 4)

// UART communication speed = 9600 bps
#define BAUDRATE 9600

// Baud rate divisor value
// Formula: Divisor = PCLK / (16 × Baud Rate)
#define DIVISOR (PCLK / (16 * BAUDRATE))

//---- UART0 Line Control Register (U0LCR) -----//
#define WORD_LEN 3   // 8-bit data length (Bits 1:0 = 11)
#define DLAB 7       // 8-bit data length (Bits 1:0 = 11)

//---- UART0 Line Status Register (U0LSR) -----//
#define DR 0     // Data Ready bit (Bit 0)
#define TEMT 6   // Transmitter Empty bit (Bit 6)

// Initializes UART0 with the selected baud rate
void Init_uart0(void);

// Transmits a single character through UART0
void tx_uart0(unsigned char data);

// Receives a single character from UART0
unsigned char rx_uart0(void);

// Transmits a null-terminated string
void uart0_txstring(char *str);

// Receives characters until '$' is encountered
void uart0_rxstring(char *str);



