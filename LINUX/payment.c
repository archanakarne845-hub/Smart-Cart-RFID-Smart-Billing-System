#include "payment.h"
#include "uart.h"
#include "update.h"

// Function: transaction_card
// Purpose: Handles payment using a bank card. Validates card, checks PIN, deducts balance, and updates bank records.
void transaction_card(char *card_no)
{
    printf("Entered into payment mode..\n");

    char buf[50];        // Buffer for UART communication
    int chances = 3;     // Number of PIN attempts allowed
    int found = 0;       // Flag to indicate if card is found
    int total = 0;       // Total bill amount

    // --- Calculate total bill from cart ---
    for(int j=0; j<cart_size; j++)
    {
        total += qty[j] * cart[j].price;
    }

    // --- Search for card in bank database ---
    for(int i=0; i<bank_size; i++)
    {
        if(strcmp(card_no, b[i].card_id) == 0) // Match card ID
        {
            found = 1;
            printf("Card Found\n");
            printf("Bank Name=%s\n", b[i].bank_name);
            printf("Total Bill=%d\n", total);

            // Notify UART that card is valid
            uart_str_tx("CARD_OK");
            uart_tx('$');

            // Send stored PIN to UART for verification
            sprintf(buf, "%s", b[i].atm_pin);
            uart_str_tx(buf);
            uart_tx('$');

            // --- PIN verification loop ---
            while(chances > 0)
            {
chance:         memset(buf, 0, sizeof(buf));          // Clear buffer
                uart_str_rx(buf, sizeof(buf));         // Receive PIN status from UART
                buf[strcspn(buf,"$\r\n")] = '\0';      // Replace '$' or newline with '\0'
                printf("Received Status:%s", buf);

                // --- Correct PIN ---
                if(strcmp(buf,"1") == 0)
                {
                    if(b[i].balance >= total) // Check sufficient balance
                    {
                        b[i].balance -= total;   // Deduct bill amount
			for(int k = 0; k < cart_size; k++)
			{
				int db_idx = find_product(cart[k].id);
				if(db_idx != 1)
					db[db_idx].stock -= qty[k];
			}
			update_stock();
                        update_bank();           // Save updated balance to bank.csv
                        save_sales(card_no,"ONLINE"); // Record transaction in sales
                        cart_size = 0;           // Clear cart after payment

                        uart_str_tx("SUCCESSFUL");  // Notify UART
                        uart_tx('$');
                        printf("PAYMENT SUCCESSFULLY COMPLETED\n");
                        return;
                    }
                    else
                    {
                        printf("Payment failed due to less balance\n");
                        uart_str_tx("LOW_BAL");
                        uart_tx('$');
                        return;
                    }
                }
                // --- Wrong PIN ---
                else
                {
                    chances--; // Reduce remaining attempts
                    if(chances == 0)
                    {
                        uart_str_tx("CASH"); // Switch to cash payment
                        uart_tx('$');
                        printf("Card Blocked\n");
                        printf("Switch to Cash Payment\n");
                        sleep(1);
                        // transaction_cash(); // Optional: trigger cash payment
                        return;
                    }
                    printf("Payment failed\n");
		    printf("WRONG PIN\n");
                    uart_str_tx("RETRY"); // Ask for retry
                    uart_tx('$');
                    goto chance; // Retry PIN input
                }
            }
        }
    }

    // --- Invalid card case ---
    if(found == 0)
    {
        uart_str_tx("INVALID"); // Notify UART
        uart_tx('$');
    }
}

// Function: transaction_cash
// Purpose: Handles payment in cash mode. Sends total bill to UART, waits for confirmation, and records sale.
void transaction_cash(void)
{
    char buf[50];   // Buffer for UART communication
    int total = 0;  // Total bill amount

    // --- Calculate total bill from cart ---
    for(int i=0; i<cart_size; i++)
    {
        total += qty[i] * cart[i].price;
    }

    // --- Send total bill to UART ---
    sprintf(buf, "%d", total);   // Convert total to string
    uart_str_tx(buf);            // Transmit total amount
    uart_tx('$');                // End marker
    printf("Sending total amount:%s\n", buf);

    // --- Receive payment result ---
l1: memset(buf, 0, sizeof(buf)); // Clear buffer
    uart_str_rx(buf, sizeof(buf)); // Receive result from UART
    printf("Received Result:%s\n", buf);

    int result = atoi(buf); // Convert received string to integer

    // --- SUCCESS case ---
    if(result == 1)
    {
	for(int k = 0; k < cart_size; k++)
	{
		int db_idx = find_product(cart[k].id);
		if(db_idx != -1)
			db[db_idx].stock -= qty[k];
	}
	update_stock();
        save_sales("CASH","CASH"); // Record transaction as cash payment
        cart_size = 0;             // Clear cart after payment
        printf("PAID SUCCESSFULLY\n");
        return;
    }
    // --- FAILURE case ---
    else
    {
        printf("Payment Failed\n");
	printf("Due to Less amount\n");
        goto l1; // Retry until payment succeeds
    }
}


// Function: send_total_bill
// Purpose: Calculates the total bill from the cart and sends it via UART
void send_total_bill()
{
    char buf[20];       // Buffer to hold formatted total bill string
    float total = 0;    // Variable to accumulate total bill amount

    // --- Calculate total bill ---
    for(int i=0; i<cart_size; i++)
    {
        total += qty[i] * cart[i].price; // Multiply quantity by price for each item
    }

    // Format total as integer followed by '$' (end marker for UART)
    sprintf(buf, "%d$", (int)total);

    // Send formatted total bill string via UART
    uart_str_tx(buf);
}

// Function: display_bill
// Purpose: Prints the final bill to the console with itemized details
void display_bill()
{
    float total = 0; // Variable to accumulate total bill amount

    printf("\n=========== FINAL BILL ===========\n");

    // --- Print each item in the cart ---
    for(int i=0; i<cart_size; i++)
    {
        float item_total = qty[i] * cart[i].price; // Calculate total for this item
        printf("%s  Qty=%d  Total=%.2f\n",
               cart[i].name,   // Item name
               qty[i],         // Quantity purchased
               item_total);    // Total price for this item
        total += item_total;   // Add to overall total
    }

    // --- Print grand total ---
    printf("\nFINAL BILL = %.2f\n", total);
}


// Function: save_sales
// Purpose: Records completed sales transactions into "sales.csv" with full details.
// Includes bill number, date/time, customer card ID, payment mode, itemized cart, and grand total.
void save_sales(char *card_id, char *payment_mode)
{
    FILE *fp;
    time_t t;
    struct tm *tm_info;
    char date[20];      // Stores formatted date (DD-MM-YYYY)
    char day[20];       // Stores day of the week (e.g., Monday)
    char time_str[20];  // Stores formatted time (HH:MM AM/PM)
    float grand_total = 0;

    // Open sales.csv in append mode (adds new bills at the end)
    fp = fopen("sales.csv","a");
    if(fp == NULL) // Check whether file opened successfully
    {
        printf("Sales File Open Failed\n");
        return;
    }

    // --- Get current system date and time ---
    time(&t);
    tm_info = localtime(&t);
    strftime(date, sizeof(date), "%d-%m-%Y", tm_info);   // Format date
    strftime(day, sizeof(day), "%A", tm_info);           // Format day name
    strftime(time_str, sizeof(time_str), "%I:%M %p", tm_info); // Format time

    // --- Generate next bill number ---
    bill_no++;

    // --- Write bill header ---
    fprintf(fp,"\n====================================================\n");
    fprintf(fp,"BILL NO : %d\n", bill_no);
    fprintf(fp,"DATE    : %s\n", date);
    fprintf(fp,"DAY     : %s\n", day);
    fprintf(fp,"TIME    : %s\n", time_str);
    fprintf(fp,"CARD ID : %s\n", card_id);
    fprintf(fp,"PAYMENT : %s\n", payment_mode);
    fprintf(fp,"--------------------------------------------------------\n");
    fprintf(fp,"\nITEM,RFID,QTY,PRICE,TOTAL\n\n");

    // --- Write itemized cart details ---
    for(int i=0; i<cart_size; i++)
    {
        float total = qty[i] * cart[i].price; // Calculate total for this item
        grand_total += total;                 // Add to grand total
        fprintf(fp,"%s,%s,%d,%.2f,%.2f\n",
                cart[i].name,   // Item name
                cart[i].id,     // RFID
                qty[i],         // Quantity
                cart[i].price,  // Price per unit
                total);         // Total price for this item
    }

    // --- Write grand total ---
    fprintf(fp,"\n==================================================\n");
    fprintf(fp,"GRAND TOTAL : %.2f\n", grand_total);
    fprintf(fp,"====================================================\n");

    fclose(fp); // Close file after writing
}

