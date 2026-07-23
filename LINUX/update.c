#include "update.h"

// Function: load_stock
// Purpose: Reads product information from "stock.csv" and loads it into the db[] array.
// Each line in stock.csv is expected to have: name, RFID, stock, price
void load_stock()
{
    FILE *fp = fopen("stock.csv","r");   // Open stock.csv in read mode
    if(fp == NULL)                       // Check if file exists
    {
        printf("STOCK FILE MISSING\n");  // Error message if file not found
        return;                          // Exit function
    }

    char line[200];                      // Buffer to hold one line of CSV
    fgets(line,sizeof(line),fp);         // Read header line (skip column names)

    // Read product records line by line until EOF
    while(fscanf(fp,"%[^,],%[^,],%d,%f\n",
                 db[db_size].name,       // Product name
                 db[db_size].id,         // RFID code
                 &db[db_size].stock,     // Stock quantity
                 &db[db_size].price)     // Price
          == 4)                          // Ensure all 4 fields are read
    {
        db_size++;                       // Increment database size counter
    }

    fclose(fp);                          // Close file after reading
}

// Function: display_stock
// Purpose: Prints the current product database (db[]) in a formatted table
void display_stock()
{
    printf("\n================= STOCK DATABASE ==================\n");
    printf("%-15s %-15s %-10s %-10s\n","ITEM","RFID","STOCK","PRICE");
    printf("=======================================================\n");

    // Loop through all products in db[] and print details
    for(int i=0;i<db_size;i++)
    {
        printf("%-15s %-15s %-10d %-10.2f\n",
               db[i].name,   // Product name
               db[i].id,     // RFID code
               db[i].stock,  // Stock quantity
               db[i].price); // Price
    }
}

// Function: update_stock
// Purpose: Writes the current product database (db[]) back into "stock.csv"
// This ensures that any changes made in memory (adding/updating products) are saved permanently.
void update_stock()
{
    FILE *fp = fopen("stock.csv","w");   // Open stock.csv in write mode (overwrite existing file)
    fprintf(fp,"item,Rfid,quantity,price\n"); // Write header line for CSV

    // Loop through all products in db[] and write their details to the file
    for(int i=0;i<db_size;i++)
    {
        fprintf(fp,"%s,%s,%d,%.2f\n",
                db[i].name,   // Product name
                db[i].id,     // RFID code
                db[i].stock,  // Stock quantity
                db[i].price); // Price
    }

    fclose(fp); // Close file after writing
}

// Function: load_bank
// Purpose: Reads bank account information from "bank.csv" and loads it into the b[] array.
// Each line in bank.csv is expected to have: card_id, place, ifsc_code, bank_name, acc_no, balance, atm_pin
void load_bank()
{
    FILE *fp = fopen("bank.csv","r");   // Open bank.csv in read mode
    if(fp == NULL)                      // Check if file exists
    {
        printf("BANK FILE MISSING\n");  // Error message if file not found
        return;                         // Exit function
    }

    char line[200];                     // Buffer to hold one line of CSV
    fgets(line,sizeof(line),fp);        // Read header line (skip column names)

    // Read bank records line by line until EOF
    while(fscanf(fp,"%[^,],%[^,],%[^,],%[^,],%[^,],%ld,%s\n",
                 b[bank_size].card_id,   // Card ID
                 b[bank_size].place,     // Place/branch
                 b[bank_size].ifsc_code, // IFSC code
                 b[bank_size].bank_name, // Bank name
                 b[bank_size].acc_no,    // Account number
                 &b[bank_size].balance,  // Account balance
                 b[bank_size].atm_pin)   // ATM PIN
          == 7)                          // Ensure all 7 fields are read
    {
        bank_size++;                     // Increment bank database size counter
    }

    fclose(fp); // Close file after reading
}

// Function: display_bank
// Purpose: Prints the current bank database (b[]) in a formatted table
void display_bank()
{
    printf("\n==================== BANK DATABASE =====================\n");
    printf("%-15s %-15s %-10s %-10s %-10s %-8s %-8s\n",
           "RFID","PLACE","IFSC_CODE","BANK_NAME","ACC_NO","BALANCE","PIN");
    printf("===========================================================\n");

    // Loop through all bank records and print details
    for(int i=0;i<bank_size;i++)
    {
        printf("%-15s %-15s %-10s %-10s %-10s %-8ld %-8s\n",
               b[i].card_id,   // Card ID
               b[i].place,     // Place/branch
               b[i].ifsc_code, // IFSC code
               b[i].bank_name, // Bank name
               b[i].acc_no,    // Account number
               b[i].balance,   // Balance
               b[i].atm_pin);  // ATM PIN
    }
}

// Function: update_bank
// Purpose: Writes the current bank database (b[]) back into "bank.csv"
// This ensures that any changes made in memory (transactions, updates) are saved permanently.
void update_bank()
{
    FILE *fp = fopen("bank.csv","w");   // Open bank.csv in write mode (overwrite existing file)
    fprintf(fp,"card_id,place,ifsc_code,bank_name,acc_no,balance,pin\n"); // Write header line

    // Loop through all bank records and write their details to the file
    for(int i=0;i<bank_size;i++)
    {
        fprintf(fp,"%s,%s,%s,%s,%s,%ld,%s\n",
                b[i].card_id,   // Card ID
                b[i].place,     // Place/branch
                b[i].ifsc_code, // IFSC code
                b[i].bank_name, // Bank name
                b[i].acc_no,    // Account number
                b[i].balance,   // Balance
                b[i].atm_pin);  // ATM PIN
    }

    fclose(fp); // Close file after writing
}

