#include "defines.h"   // Includes global structs and extern variables (db[], b[], etc.)

#ifndef UPDATE_H       // Prevents multiple inclusion of this header file
#define UPDATE_H

// --- Product (Stock) database functions ---
void load_stock();     // Reads product data from stock.csv into db[]
void display_stock();  // Prints product database in a formatted table
void update_stock();   // Writes current db[] contents back to stock.csv

// --- Bank database functions ---
void load_bank();      // Reads bank account data from bank.csv into b[]
void display_bank();   // Prints bank database in a formatted table
void update_bank();    // Writes current b[] contents back to bank.csv

#endif                 // End of include guard

