#include "find.h"

// Function: find_product
// Purpose: Searches the product database (db[]) for a product with a matching RFID.
// Returns: index of the product if found, otherwise -1.
int find_product(char *rfid)
{
    for(int i=0; i<db_size; i++) // Loop through all products in db[]
    {
        if(strcmp(db[i].id, rfid) == 0) // Compare RFID with product ID
        {
            return i; // Return index if match found
        }
    }
    return -1; // Return -1 if product not found
}

// Function: find_bank
// Purpose: Searches the bank database (b[]) for a bank record with a matching card ID.
// Returns: index of the bank record if found, otherwise -1.
int find_bank(char *rfid)
{
    for(int i=0; i<bank_size; i++) // Loop through all bank records
    {
        if(strcmp(b[i].card_id, rfid) == 0) // Compare RFID with card ID
        {
            return i; // Return index if match found
        }
    }
    return -1; // Return -1 if bank record not found
}

