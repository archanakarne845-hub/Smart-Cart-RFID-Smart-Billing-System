#include "defines.h"   // Brings in global structs and extern variables 

#ifndef FIND_H         // Start include guard
#define FIND_H         // Define guard macro

// --- Search functions ---
// Searches product database (db[]) for a product by RFID.
// Returns index if found, otherwise -1.
int find_product(char *rfid);

// Searches bank database (b[]) for a bank record by card ID.
// Returns index if found, otherwise -1.
int find_bank(char *rfid);

#endif                 // End include guard

