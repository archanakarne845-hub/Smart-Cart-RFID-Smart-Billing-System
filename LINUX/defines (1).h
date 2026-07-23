#ifndef DEFINES_H
#define DEFINES_H

// --- Standard library includes ---
#include <stdio.h>     // For input/output functions (printf, FILE, etc.)
#include <stdlib.h>    // For memory allocation, exit(), etc.
#include <string.h>    // For string handling (strcmp, strcpy, etc.)
#include <ctype.h>     // For character classification (isdigit, isalpha, etc.)
#include <time.h>      // For time/date functions (time, localtime, strftime)
#include <unistd.h>    // For POSIX functions (sleep, close, etc.)
#include <fcntl.h>     // For file control (open, O_RDWR, etc.)
#include <termios.h>   // For UART configuration (baud rate, parity, etc.)

#define MAX 1000       // Maximum buffer size (used in some modules)

// --- Product structure ---
// Represents an item in the stock database or cart
struct product
{
    char name[20];     // Product name
    char id[10];       // RFID code
    int stock;         // Quantity available in stock
    float price;       // Price per unit
};

// --- Bank structure ---
// Represents a bank account/card record
struct bank
{
    char place[50];    // Branch/place
    char ifsc_code[20];// IFSC code
    char bank_name[20];// Bank name
    char card_id[20];  // Card ID (RFID)
    char acc_no[20];   // Account number
    char atm_pin[10];  // ATM PIN
    long int balance;  // Account balance
};

// --- Global variables (declared as extern here, defined in main.c) ---
extern int bill_no;                // Current bill number
extern struct product db[100];     // Product database
extern struct product cart[100];   // Shopping cart
extern struct bank b[100];         // Bank database
extern int qty[100];               // Quantity array for cart items
extern int db_size;                // Number of products in database
extern int cart_size;              // Number of items in cart
extern int bank_size;              // Number of bank records
extern int uart_fd;                // UART file descriptor

#endif // DEFINES_H

