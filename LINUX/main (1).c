#include "defines.h"   // Contains struct definitions and extern declarations
#include "cart.h"      // Prototypes for cart-related functions
#include "update.h"    // Prototypes for stock update functions
#include "payment.h"   // Prototypes for payment functions
#include "find.h"      // Prototypes for search functions
#include "uart.h"      // uart function header file

// Global variable definitions (actual memory allocation happens here)
// These match the extern declarations in defines.h

int bill_no = 1000;            // Starting bill number for transactions
struct product db[100];        // Product database (loaded from stock.csv)
struct product cart[100];      // Shopping cart for current customer
struct bank b[100];            // Bank database (loaded from bank.csv)
int qty[100];                  // Quantity array for items in cart
int db_size = 0;               // Current number of products in database
int cart_size = 0;             // Current number of items in cart
int bank_size = 0;             // Current number of bank records
int uart_fd;                   // File descriptor for UART device

int main()
{
    char rfid[20];       // Buffer to store complete RFID data received
    char clean_rfid[20]; // Buffer to store RFID without the command character
    char buf[10];        // Temporary buffer (unused here, but reserved)

    // --- Initialization phase ---
    load_stock();   // Load product database from stock.csv into db[]
    load_bank();    // Load bank database from bank.csv into b[]
    uart_init();    // Initialize UART communication (open device, configure settings)
    system("clear");// Clear terminal screen for a fresh display

    // Display current system information
    display_stock(); // Show product database
    display_bank();  // Show bank database
    display_cart();  // Show current cart contents

    // Welcome message
    printf("\n====================================\n");
    printf("      SMART BILLING SYSTEM\n");
    printf("====================================\n");
    printf("\nScan RFID Card...\n");

    // --- Main loop: continuously wait for RFID scans ---
    while(1)
    {
        memset(rfid,0,sizeof(rfid)); // Clear RFID buffer before reading new data
        read_rfid(rfid);             // Read RFID data from UART

        // Skip iteration if no RFID data received
        if(strlen(rfid)==0)
        {
            continue;
        }

        // Display received RFID string
        printf("Received RFID:%s\n",rfid);

        // First character of RFID determines operation type (command)
        char cmd = rfid[0];
        printf("%c\n",cmd);

        // Extract actual RFID by removing the command character
        strcpy(clean_rfid,rfid + 1);
        printf("Clean_RFID:%s\n",clean_rfid);

        // --- CUSTOMER MODE ---
        if(cmd == 'C')
        {
            add_to_cart(clean_rfid); // Add product to cart
            sleep(2);
            system("clear");
            display_stock();
            display_bank();
            display_cart();
        }

        // --- DELETE MODE ---
        else if(cmd == 'D')
        {
            delete_product(clean_rfid); // Remove product from cart
            sleep(2);
            system("clear");
            display_stock();
            display_bank();
            display_cart();
        }

        // --- BANK CARD PAYMENT MODE ---
        else if(cmd == 'B')
        {
            printf("Online payment mode\n");
            transaction_card(clean_rfid); // Process payment via bank card
            sleep(5);
            system("clear");
            display_stock();
            display_bank();
            display_cart();
            printf("waiting for another customer.....\n");
            printf("Welcome to LUCKY MART....\n");
        }

        // --- MANAGER MODE ---
        else if(cmd == 'M')
        {
            int choice;
            char item[20];
            char id[20];
            int quantity;
            float price;
            int index;

            while(1)
            {
                // Manager menu
                printf("\n====================================\n");
                printf("        MANAGER MODE\n");
                printf("=====================================\n");
                printf("1. UPDATE EXISTING STOCK\n");
                printf("2. ADD NEW PRODUCT\n");
                printf("3. EXIT\n");
                printf("\nEnter your Choice : ");
                scanf("%d",&choice);

                // Update existing product stock
                if(choice == 1)
                {
                    printf("\nEnter Product RFID : ");
                    scanf("%s",id);
                    index = find_product(id);
                    if(index == -1)
                    {
                        printf("\nPRODUCT NOT FOUND\n");
                        sleep(2);
                    }
                    else
                    {
                        printf("\nCurrent Stock : %d\n", db[index].stock);
                        printf("Enter Quantity To Add : ");
                        scanf("%d",&quantity);
                        db[index].stock += quantity;
                        update_stock(); // Save updated stock to file
                        printf("\nSTOCK UPDATED SUCCESSFULLY\n");
                        printf("NEW STOCK : %d\n", db[index].stock);
                        sleep(3);
                    }
                }

                // Add new product to database
                else if(choice == 2)
                {
                    printf("\nEnter Product Name : ");
                    scanf("%s",item);
                    printf("Enter RFID : ");
                    scanf("%s",id);
                    index = find_product(id);

                    if(index != -1)
                    {
                        printf("\nRFID ALREADY EXISTS\n");
                        sleep(2);
                    }
                    else
                    {
                        printf("Enter Quantity : ");
                        scanf("%d",&quantity);
                        printf("Enter Price : ");
                        scanf("%f",&price);
                        strcpy(db[db_size].name,item);
                        strcpy(db[db_size].id,id);
                        db[db_size].stock = quantity;
                        db[db_size].price = price;
                        db_size++;
                        update_stock(); // Save new product to file
                        printf("\nNEW PRODUCT ADDED\n");
                        sleep(3);
                    }
                    display_stock();
                    display_bank();
                    display_cart();
                }

                // Exit manager mode
                else
                {
                    uart_str_tx("0"); // Notify via UART
                    uart_tx('$');
                    printf("\nEXITING MANAGER MODE\n");
                    sleep(2);
                    printf("Scan RFID card...\n");
                    break;
                }

                system("clear");
                display_stock();
                display_bank();
                display_cart();
            }
        }

        // --- TRANSACTION MODE ---
        else if(cmd == 'T')
        {
            if(cart_size == 0)
            {
                uart_str_tx("EMPTY"); // Notify cart is empty
                uart_tx('$');
                printf("cart empty...");
                sleep(2);
                system("clear");
                display_stock();
                display_bank();
                display_cart();
                printf("Add items to the cart....\n");
                printf("Scan RFID card...\n");
            }
            else
            {
                uart_str_tx("NOTEMPTY"); // Notify cart has items
                uart_tx('$');
            }
        }

        // --- CASH PAYMENT MODE ---
        else if(cmd == 'H')
        {
            printf("payment in cash mode\n");
            transaction_cash(); // Process cash payment
            sleep(5);
            system("clear");
            display_stock();
            display_bank();
            display_cart();
            printf("waiting for another customer.....");
            printf("\nWelcome to LUCKY MART...\n");
        }

        // --- INVALID COMMAND ---
        else
        {
            printf("\nINVALID COMMAND\n");
            sleep(1);
            system("clear");
            display_stock();
            display_bank();
            display_cart();
            printf("\nScan RFID Card...\n");
        }
    }

    // Close UART before exiting program
    close(uart_fd);
    return 0;
}


