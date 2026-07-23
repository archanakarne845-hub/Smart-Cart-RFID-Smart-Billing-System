#include "defines.h"   // Brings in global structs and extern variables
#include "find.h"      // Provides search functions for products/bank
#include "update.h"    // Provides stock/bank update functions

#ifndef CART_H         // Start include guard
#define CART_H         // Define guard macro

// --- Cart management functions ---
void update_cart();                 // Updates cart database (saves changes)
void display_cart();                // Displays current cart contents
void add_to_cart(char *rfid);       // Adds product to cart using RFID
void delete_product(char *rfid);    // Removes product from cart using RFID
void read_rfid(char *rfid);         // Reads RFID data into buffer

#endif // CART_H

