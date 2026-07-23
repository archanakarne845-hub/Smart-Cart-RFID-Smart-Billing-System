//------ Clock Configuration -------//

// Crystal oscillator frequency = 12 MHz
#define FOSC      12000000 

// CPU Clock = 60 MHz (PLL ×5)
#define CCLK      (FOSC * 5)   

// Peripheral Clock = 15 MHz
#define PCLK      (CCLK / 4)        

//------- UART Configuration -------//
 // UART communication speed = 9600 bps
#define BAUDRATE  9600             

// Baud rate divisor
// Formula: Divisor = PCLK / (16 × Baud Rate)
#define DIVISOR   (PCLK / (16 * BAUDRATE))

//----- UART Line Control Register (UxLCR) ------//
// Configure UART for 8-bit data 
#define WORD_LEN  3 

// Divisor Latch Access Bit (Bit 7)
#define DLAB      7 

//---- UART Line Status Register (UxLSR) -----//
// Data Ready bit (Bit 0)
#define DR        0 

// Transmitter Empty bit (Bit 6)
#define TEMT      6                 

//----- Function Prototype ------//

// Initializes UART1 with the configured baud rate (9600 bps)
void Init_uart1(void);

