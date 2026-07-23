#include "defines.h"
#include "uart.h"

#ifndef PAYMENT_H        // Start include guard
#define PAYMENT_H        // Define guard macro

// --- Payment-related functions ---
void transaction_card(char *card_no);              // Handles card payment
void transaction_cash(void);                       // Handles cash payment
void send_total_bill();                            // Sends total bill via UART
void display_bill();                               // Displays final bill on console
void save_sales(char *card_id, char *payment_mode);// Saves sales record to sales.csv

#endif                   // End include guard

