// LPC2148 register definitions
#include <lpc21xx.h> 
// LCD macros and function declarations
#include "lcd.h" 
// Delay functions
#include "delay.h"        

// Function : writeLCD()
// Purpose  : Sends data/command to LCD
void writeLCD(unsigned char data)
{
    // Select Write operation (RW = 0)
    IOCLR0 = 1 << RW;

    // Clear previous data and place new data on LCD data pins
    //IOPIN1 = (IOPIN1 & ~(0xFF << pin)) | (data << pin);
	IOCLR1 = (0xFFUL<<pin);
	IOSET1 = (data<<pin);
    // Generate Enable pulse
    IOSET0 = 1 << EN;
    delay_us(1);

    IOCLR0 = 1 << EN;

    // Wait for LCD to complete the operation
    delay_ms(2);
}

// Function : cmdLCD()
// Purpose  : Sends a command to the LCD
void cmdLCD(unsigned char cmd)
{
    // Select Command Register (RS = 0)
    IOCLR0 = 1 << RS;

    // Send command
    writeLCD(cmd);
}
						                
// Function : charLCD()
// Purpose  : Displays a single character on the LCD
void charLCD(unsigned char ascii)
{
    // Select Data Register (RS = 1)
    IOSET0 = 1 << RS;

    // Send ASCII character
    writeLCD(ascii);
}

// Function : InitLCD()
// Purpose  : Initializes the LCD in 8-bit mode
void InitLCD(void)
{
    // Configure LCD data pins and control pins as outputs
    IODIR1 |= (0xFFUL << pin) ;
	IODIR0|= (1 << RS) | (1 << RW) | (1 << EN);
    // Wait for LCD power-up
    delay_ms(15);

    // LCD initialization sequence
    cmdLCD(MODE_8BIT_1LINE);
    delay_ms(5);

    cmdLCD(MODE_8BIT_1LINE);
    delay_us(100);

    cmdLCD(MODE_8BIT_1LINE);

    // Configure LCD for 8-bit, 2-line mode
    cmdLCD(MODE_8BIT_2LINE);

    // Display ON, Cursor ON, Cursor Blinking
    cmdLCD(DISP_ON_CUR_BLINK);

    // Clear LCD display
    cmdLCD(CLEAR_LCD);

    // Move cursor to the right after each character
    cmdLCD(SHIFT_CUR_RIGHT);
}

// Function : strLCD()
// Purpose  : Displays a null-terminated string on the LCD
void strLCD(char *p)
{
    while (*p)
        charLCD(*p++);
}

// Function : U32LCD()
// Purpose  : Displays an unsigned integer on the LCD
void U32LCD(unsigned int num)
{
    unsigned char a[10];
    signed int i = 0;

    // Special case for zero
    if (num == 0)
    {
        charLCD('0');
        return;
    }
    else
    {
        // Convert number into ASCII digits (stored in reverse order)
        while (num)
        {
            a[i++] = (num % 10) + '0';
            num /= 10;
        }

        // Display digits in correct order
        for (--i; i >= 0; i--)
            charLCD(a[i]);
    }
}
