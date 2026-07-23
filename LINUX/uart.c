#include "uart.h"

// Function: uart_init
// Purpose: Initialize UART communication by opening the device and configuring settings
void uart_init()
{
    // Open UART device (/dev/ttyUSB0) in read/write mode, no controlling terminal
    uart_fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);

    // Check whether UART opened successfully
    if(uart_fd < 0)
    {
        printf("UART OPEN FAILED\n");
        exit(1); // Exit program if UART cannot be opened
    }

    struct termios options; // Structure to hold UART configuration

    // Get current UART settings
    tcgetattr(uart_fd, &options);

    // Set baud rate to 9600
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    // Enable receiver and ignore modem control lines
    options.c_cflag |= (CLOCAL | CREAD);

    // Configure frame format: 8 data bits, no parity, 1 stop bit
    options.c_cflag &= ~PARENB; // Disable parity
    options.c_cflag &= ~CSTOPB; // Use 1 stop bit
    options.c_cflag &= ~CSIZE;  // Clear current data size settings
    options.c_cflag |= CS8;     // Set 8-bit data frame

    // Apply new UART settings immediately
    tcsetattr(uart_fd, TCSANOW, &options);
}

// Function: uart_rx
// Purpose: Receive a single character from UART
char uart_rx(void)
{
    char ch;
    // Keep reading until a character is successfully received
    while(read(uart_fd, &ch, 1) <= 0);
    return ch;
}

// Function: uart_tx
// Purpose: Transmit a single character over UART
void uart_tx(char ch)
{
    write(uart_fd, &ch, 1);
}

// Function: uart_str_rx
// Purpose: Receive a string from UART until '$' is encountered
// Skips newline and carriage return characters
void uart_str_rx(char *buf, int size)
{
    char ch;
    int i = 0;

    while(1)
    {
        ch = uart_rx(); // Read one character

        if(ch == '$')   // End of string marker
        {
            break;
        }
        if(ch == '\n' || ch == '\r') // Ignore newline/carriage return
        {
            continue;
        }
        if(i <= size - 1) // Store character if buffer has space
        {
            buf[i++] = ch;
        }
    }
    buf[i] = '\0'; // Null-terminate the string
}

// Function: uart_str_tx
// Purpose: Transmit a string over UART
void uart_str_tx(char *str)
{
    write(uart_fd, str, strlen(str)); // Send string using write()
}

