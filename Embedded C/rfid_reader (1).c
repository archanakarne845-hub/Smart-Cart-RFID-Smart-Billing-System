
#include <lpc21xx.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "interrupt.h"
#include "rf_reader.h"


//===============================================================
// Global Variables
//===============================================================
// 1 -> Entry mode active
// 0 -> Entry mode inactive
int entry_mode = 0;

// 1 -> Delete mode active
// 0 -> Delete mode inactive
int delete_mode = 0;

// 1 -> Exit mode active
// 0 -> Exit mode inactive
int exit_mode = 0;
// General purpose variable
int value = 0;

// This flag is used to switch the system to cash payment.
extern int switch_to_cash;
// Stores the actual PIN received from the PC/database
// during card payment.
int actual_pin = 0;
char buf[50];
// Stores the PIN entered by the user through keypad.
int entered_pin;


//===============================================================
// Function Declarations
//===============================================================

void cash_payment_frontend(void);
void card_payment_frontend(void);

void int_to_str(char *buf, int size, int num)
{
    // Clear the buffer before storing the converted value.
    memset(buf, 0, size);

    // Convert integer to string.
    sprintf(buf, "%d", num);
}

void RFID(void)
{

    //===========================================================
    // Welcome Screen
    //===========================================================

    // If none of the modes are active, display the welcome
    // message on the LCD.
    if((entry_mode || delete_mode || exit_mode) == 0)
    {
        // Clear LCD.
        cmdLCD(0x01);

        // Move cursor to first line.
        cmdLCD(0x80);

        // Display welcome message.
        strLCD("   Welcome To   ");

        // Move cursor to second line.
        cmdLCD(0xC0);

        // Display shop name.
        strLCD("   LUCKY MART   ");

        // Keep the welcome message visible for 10 seconds.
        delay_s(10);
    }


    //===========================================================
    // ENTRY MODE
    //===========================================================

    // Check whether Entry mode is activated.
    if(entry_mode == 1)
    {
        int status = -1;

        // Clear LCD.
        cmdLCD(0x01);

        // Display Entry mode.
        cmdLCD(0x80);
        strLCD("Entry mode");

        // Ask the user to scan the RFID card.
        cmdLCD(0xC0);
        strLCD("Scan RFID Card");


        // Continue Entry mode until it is disabled.
        while(entry_mode == 1)
        {

            // Check whether new RFID data has been received
            // through UART1.
            if(uart1_ready)
            {
                // Clear the receive flag after processing
                // the received RFID data.
                uart1_ready = 0;
                // Remove carriage return and newline characters
                // from the received RFID string.
                uart1_buf[strcspn(uart1_buf,"\r\n")] = 0;

                //================================================
                // Manager RFID Card
                //================================================

                // Compare received RFID ID with the predefined
                // manager card ID.
                if(strcmp(uart1_buf, "12603355") == 0)
                {
                    // Display manager card message.
                    cmdLCD(0x01);
                    cmdLCD(0x80);
                    strLCD("Manager card");

                    // Inform the user that stock is being updated.
                    cmdLCD(0xC0);
                    strLCD("Update Stock...");

                    // Send 'M' command to the PC.
                    // 'M' indicates Manager operation.
                    tx_uart0('M');

                    // Send manager RFID ID to PC.
                    uart0_txstring(uart1_buf);

                    // '$' is used as a termination character.
                    tx_uart0('$');

                    // Wait for PC/database response.
                    delay_s(2);

                    // Clear receive buffer.
                    memset(buf, 0, sizeof(buf));

                    // Receive response from PC.
                    uart0_rxstring(buf);

                    // Remove termination/newline characters.
                    buf[strcspn(buf,"$\r\n")] = '\0';

                    // Convert received response from string
                    // to integer.
                    status = atoi(buf);

                    // If status is 0, manager operation is completed.
                    if(status == 0)
                    {
                        entry_mode = 0;
                    }
                    else
                    {
                        // Otherwise remain in Entry mode.
                        entry_mode = 1;
                    }
                }

                //================================================
                // Customer RFID Card
                //================================================

                else
                {
                    // Send 'C' command to PC.
                    // 'C' indicates customer operation.
                    tx_uart0('C');

                    // Send customer RFID ID.
                    uart0_txstring(uart1_buf);

                    // Terminate the message.
                    tx_uart0('$');

                    // Display customer card message.
                    cmdLCD(0x01);
                    cmdLCD(0x80);
                    strLCD("customer card");

                    // Inform the customer that items can be added.
                    cmdLCD(0xC0);
                    strLCD("Adding Items..");

                    delay_s(2);
                }
            }

            // If Delete mode or Exit mode is activated,
            // leave Entry mode.
            if(delete_mode || exit_mode)
            {
                break;
            }
        }

        // Entry operation completed.
        entry_mode = 0;

        // Clear LCD.
        cmdLCD(0x01);
    }

    //===========================================================
    // DELETE MODE
    //===========================================================

    // Check whether Delete mode is activated.
    if(delete_mode == 1)
    {
        // Display Delete mode.
        cmdLCD(0x01);
        cmdLCD(0x80);
        strLCD("Delete_mode");

        // Ask the user to scan the product RFID card.
        cmdLCD(0xC0);
        strLCD("Scan Card..");

        // Continue Delete mode until disabled.
        while(delete_mode == 1)
        {
            // Check whether RFID data is received.
            if(uart1_ready)
            {
                // Clear UART receive flag.
                uart1_ready = 0;

                // Send 'D' command to PC.
                // 'D' indicates Delete operation.
                tx_uart0('D');

                // Send scanned RFID ID to PC.
                uart0_txstring(uart1_buf);

                // Terminate the message.
                tx_uart0('$');

                // Display confirmation message.
                cmdLCD(0x01);
                cmdLCD(0x80);
                strLCD("Item deleted");

                delay_s(2);
            }

            // If Entry or Exit mode is activated,
            // leave Delete mode.
            if(entry_mode || exit_mode)
            {
                break;
            }
        }

        // Delete operation completed.
        delete_mode = 0;

        // Clear LCD.
        cmdLCD(0x01);
    }

    //===========================================================
    // EXIT MODE
    //===========================================================

    // Check whether Exit mode is activated.
    if(exit_mode == 1)
    {
        char choice;

        // Local buffer used to receive cart information.
        char buf[20];
        // Send 'T' command to PC.
        // 'T' indicates that the customer wants to
        // terminate shopping and proceed to billing.
        tx_uart0('T');

        // Terminate the command.
        tx_uart0('$');

        // Clear the receive buffer.
        memset(buf, 0, sizeof(buf));

        // Receive cart status from PC.
        uart0_rxstring(buf);

        // Remove unwanted characters.
        buf[strcspn(buf, "\r\n")] = '\0';

        //=======================================================
        // Check whether Cart is Empty
        //=======================================================

        if(strcmp(buf, "EMPTY") == 0)
        {
            // Display empty cart message.
            cmdLCD(0x01);
            cmdLCD(0x80);
            strLCD("cart was empty");

            delay_s(5);

            // Exit mode completed.
            exit_mode = 0;
            return;
        }

        //=======================================================
        // Cart Contains Items
        //=======================================================

        else
        {
            // Display payment options.
            cmdLCD(0x01);
            cmdLCD(0x80);
            strLCD("1.cash 2.online");

            // Wait for the user to select a payment method.
            while(exit_mode == 1)
            {
                // If another mode is activated,
                // cancel Exit mode.
                if(entry_mode == 1 || delete_mode == 1)
                {
                    exit_mode = 0;
                    break;
                }

                // Read keypad input.
                choice = keyscan();
                // Ignore invalid/no key input.
                if(choice == '0')
                {
                    delay_ms(10);
                    continue;
                }
                //================================================
                // Cash Payment
                //================================================

                if(choice == '1')
                {
                    // Start cash payment process.
                    cash_payment_frontend();

                    // Exit payment selection.
                    exit_mode = 0;

                    break;
                }
                //================================================
                // Online/Card Payment
                //================================================

                else if(choice == '2')
                {
                    // Start card payment process.
                    card_payment_frontend();
                    // Exit payment selection.
                    exit_mode = 0;

                    break;
                }
            }

            // Clear LCD after payment selection.
            cmdLCD(0x01);
        }
    }
}

// Function : cash_payment_frontend()
void cash_payment_frontend(void)
{
    char buf[50];

    int total = 0;
    int entered_money = 0;

    // Send 'H' command to PC.
    // This requests the total bill amount.
    tx_uart0('H');

    // Terminate the command.
    tx_uart0('$');

    // Clear receive buffer.
    memset(buf, 0, sizeof(buf));

    // Receive total bill from PC.
    uart0_rxstring(buf);

    // Remove unwanted characters.
    buf[strcspn(buf,"$\r\n")] = '\0';

    // If no bill amount is received, return.
    if(strlen(buf) == 0)
        return;

    // Convert bill amount from string to integer.
    total = atoi(buf);

    //===========================================================
    // Display Total Bill
    //===========================================================

    cmdLCD(0x01);
    cmdLCD(0x80);
    strLCD("total bill");

    // Display total amount on second line.
    cmdLCD(0xC0);
    U32LCD(total);

    delay_s(5);

    //===========================================================
    // Accept Money from Customer
    //===========================================================

label1:

    cmdLCD(0x01);
    cmdLCD(0x80);
    strLCD("enter money");

    // Read amount entered through keypad.
    entered_money = ReadNum();

    //===========================================================
    // Exact Payment
    //===========================================================

    if(entered_money == total)
    {
        // Send '1' to PC indicating successful payment.
        tx_uart0('1');
        tx_uart0('$');

        // Display successful payment.
        cmdLCD(0x01);
        cmdLCD(0x80);
        strLCD("paid successfully");

        delay_s(5);

        // Display thank-you message.
        cmdLCD(0x01);
        cmdLCD(0x80);
        strLCD("Thank you ");
        cmdLCD(0xC0);
        strLCD("    VISIT AGAIN!!   ");
        delay_s(5);
        return;
    }

    //===========================================================
    // Extra Money / Return Change
    //===========================================================

    else if(entered_money > total)
    {
        // Calculate amount to return.
        int return_money = entered_money - total;

        // Inform PC that payment is successful.
        tx_uart0('1');
        tx_uart0('$');

        // Display payment success.
        cmdLCD(0x01);
        cmdLCD(0x80);
        strLCD("payment success");

        // Display return amount.
        cmdLCD(0xC0);
        strLCD("returns:");

        cmdLCD(0xC0 + 8);
        U32LCD(return_money);

        delay_s(5);
        // Clear LCD.
        cmdLCD(0x01);
        return;
    }

    //===========================================================
    // Insufficient Money
    //===========================================================

    else
    {
        // Inform PC that payment failed.
        tx_uart0('0');
        tx_uart0('$');

        // Display insufficient payment message.
        cmdLCD(0x01);
        cmdLCD(0x80);
        strLCD("payment failed");

        cmdLCD(0xC0);
        strLCD("less money");
        delay_s(5);
        // Ask the customer to enter the amount again.
        goto label1;
    }
}

// Function : card_payment_frontend()

void card_payment_frontend(void)
{
    char buf[50];

    //===========================================================
    // Scan ATM Card
    //===========================================================

    cmdLCD(0x01);
    cmdLCD(0x80);
    strLCD("Scan ATM card");

    // Wait until the RFID reader receives card data.
    while(uart1_ready == 0);

    // Clear UART receive flag.
    uart1_ready = 0;

    // Send 'B' command to PC.
    // 'B' indicates bank/card transaction.
    tx_uart0('B');

    // Send the received card ID.
    uart0_txstring(uart1_buf);

    // Terminate message.
    tx_uart0('$');

    // Clear response buffer.
    memset(buf, 0, sizeof(buf));

    // Receive card validation response from PC.
    uart0_rxstring(buf);

    // Remove unwanted characters.
    buf[strcspn(buf,"$\r\n")] = '\0';

    //===========================================================
    // Invalid Card
    //===========================================================

    if(strcmp(buf,"INVALID") == 0)
    {
        // Display invalid card message.
        cmdLCD(0x01);
        cmdLCD(0x80);
        strLCD("invalid card");
        delay_s(2);
        return;
    }

    //===========================================================
    // Valid Card
    //===========================================================

    else if(strcmp(buf, "CARD_OK") == 0)
    {
        // Clear buffer before receiving PIN.
        memset(buf, 0, sizeof(buf));

        // Receive actual PIN from PC/database.
        uart0_rxstring(buf);
        // Remove unwanted characters.
        buf[strcspn(buf,"$\r\n")] = '\0';
        // Convert PIN from string to integer.
        actual_pin = atoi(buf);
        // Display received PIN.
        // Note: In a real banking system, the PIN should
        // NEVER be displayed on the LCD.
        cmdLCD(0x01);
        cmdLCD(0x80);
        strLCD("received PIN:");
        cmdLCD(0xC0);
        U32LCD(actual_pin);
        delay_s(2);
        //=======================================================
        // Ask Customer to Enter PIN
        //=======================================================

label2:

        cmdLCD(0x01);
        cmdLCD(0x80);
        strLCD("Enter pin:");
        cmdLCD(0xC0);
        // Read PIN entered through keypad.
        entered_pin = ReadNum();
        delay_s(2);
        //=======================================================
        // Correct PIN
        //=======================================================

        if(entered_pin == actual_pin)
        {
            // Inform PC that the PIN is correct.
            tx_uart0('1');
            tx_uart0('$');
            // Clear buffer.
            memset(buf, 0, sizeof(buf));

            // Receive payment result from PC.
            uart0_rxstring(buf);

            // Remove unwanted characters.
            buf[strcspn(buf,"$\r\n")] = '\0';

            //===================================================
            // Payment Successful
            //===================================================

            if(strcmp(buf,"SUCCESS") == 0)
            {
                // Display successful payment.
                cmdLCD(0x01);
                cmdLCD(0x80);
                strLCD("payment OK");
                delay_s(2);
                // Display thank-you message.
                cmdLCD(0x01);
                cmdLCD(0x80);
                strLCD("Thank you");
                cmdLCD(0xC0);
                strLCD("    VISIT AGAIN!!   ");
                delay_s(5);
                return;
            }

            //===================================================
            // Insufficient Balance
            //===================================================

            else if(strcmp(buf,"LOW_BAL") == 0)
            {
                // Display insufficient balance message.
                cmdLCD(0x01);
                cmdLCD(0x80);
                strLCD("Less Bal");
                delay_s(2);
                return;
            }
        }

        //=======================================================
        // Incorrect PIN
        //=======================================================

        else if(entered_pin != actual_pin)
        {
            // Inform PC that the PIN is incorrect.
            tx_uart0('0');
            tx_uart0('$');
            // Clear buffer.
            memset(buf, 0, sizeof(buf));
            // Receive response from PC.
            uart0_rxstring(buf);
            // Remove unwanted characters.
            buf[strcspn(buf,"$\r\n")] = '\0';
            // Display wrong PIN message.
            cmdLCD(0x01);
            cmdLCD(0x80);
            strLCD("wrong Pin");
            delay_s(2);

            //===================================================
            // Allow PIN Retry
            //===================================================

            if(strcmp(buf,"RETRY") == 0)
            {
                // Ask customer to enter PIN again.
                goto label2;
            }
            //===================================================
            // Card Blocked
            //===================================================

            else if(strncmp(buf,"CASH",4) == 0)
            {
                // Display card blocked message.
                cmdLCD(0x01);
                cmdLCD(0x80);
                strLCD("Card Blocked");
                cmdLCD(0xC0);
                strLCD("use CASH");
                delay_s(2);
                // Inform customer to switch to cash mode.
                cmdLCD(0x01);
                cmdLCD(0x80);
                strLCD("switch to");
                cmdLCD(0xC0);
                strLCD("CASH MODE");
                delay_s(2);
                // Clear buffer.
                memset(buf,0,sizeof(buf));
                // Set flag to switch the system to cash payment.
                switch_to_cash = 1;
                // Clear RFID receive flag.
                uart1_ready = 0;

                // Cash payment is handled later by the main loop.
                // cash_payment_frontend();

                return;
            }
        }
    }
}
