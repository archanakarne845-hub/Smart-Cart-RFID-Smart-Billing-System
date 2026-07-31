#include "cart.h"

// Function: update_cart
// Purpose: Writes the current cart contents into "cart.csv" for persistence.
// Each line contains item name, RFID, quantity, price, and total.
void update_cart()
{
    FILE *fp = fopen("cart.csv","w"); // Open file in write mode (overwrite)
    if(fp == NULL)
    {
         printf("Cart File Open Failed\n");
         return;
    }

    fprintf(fp,"item,rfid,quantity,price,total\n"); // CSV header

    // Write each cart item to file
    for(int i=0; i<cart_size; i++)
    {
        fprintf(fp,"%s,%s,%d,%.2f,%.2f\n",
                cart[i].name,          // Item name
                cart[i].id,            // RFID
                qty[i],                // Quantity
                cart[i].price,         // Price per unit
                qty[i] * cart[i].price // Total for this item
        );
    }

    fclose(fp); // Close file after writing
}

// Function: display_cart
// Purpose: Prints the current cart contents to the console in a formatted table.
void display_cart()
{
    float total = 0; // Variable to accumulate grand total

    printf("\n==================== CART =====================\n");
    printf("%-15s %-10s %-10s %-10s\n","ITEM","QTY","PRICE","TOTAL");
    printf("===================================================\n");

    // Print each cart item
    for(int i=0; i<cart_size; i++)
    {
        float item_total = qty[i] * cart[i].price; // Calculate total for this item
        printf("%-15s %-10d %-10.2f %-10.2f\n",
               cart[i].name,   // Item name
               qty[i],         // Quantity
               cart[i].price,  // Price per unit
               item_total);    // Total price for this item
        total += item_total;   // Add to grand total
    }

    // Print grand total
    printf("\n TOTAL : %.2f\n", total);
}


// Purpose: Adds a product to the cart using its RFID.
// If product already exists in cart, increments quantity.
// Otherwise, adds new product entry to cart.
// Updates stock and cart databases accordingly.
void add_to_cart(char *rfid)
{
    int cnt = 0;
    // Find product index in stock database
    int index = find_product(rfid);

    if(index == -1) // Product not found
    {
        printf("\nINVALID RFID\n");
        return;
    }
    int len = strlen(rfid);
    p[j] = calloc(1,len);
    strcpy(p[j],rfid);
    j++;
    for(int i = 0;i < j; i++)
    {
        if(strcmp(p[i],rfid)==0)
        {
            cnt++;
            if(!(cnt <= db[index].stock))
            {
                printf("\nOUT OF STOCK\n");
                j--;
                return;
            }
        }
    }
    for(int i=0; i<cart_size; i++)
    {
        if(db[index].stock <= 0) // Out of stock
        {
            printf("\nOUT OF STOCK\n");
            return;
        }
   
        if(strcmp(cart[i].id, rfid) == 0)
        {
            qty[i]++;              // Increment quantity in cart
            printf("\nPRODUCT UPDATED\n");
            return;
        }
    }

    // --- Add new product to cart ---
    cart[cart_size] = db[index];   // Copy product details into cart
    qty[cart_size] = 1;            // Set initial quantity
    cart_size++;                   // Increase cart size
    printf("\nPRODUCT ADDED\n");
}


// Function: delete_product
// Purpose: Removes a product from the cart using its RFID.
// If quantity > 1, it decrements the quantity. If quantity reaches 0, it removes the item entirely.
// Updates stock and cart databases accordingly.
void delete_product(char *rfid)
{
    // Find product index in stock database
    int index = find_product(rfid);
    if(index == -1) // Product not found in stock
    {
        printf("\nINVALID RFID\n");
        return;
    }

    for(int k = 0; k < j;k++)
    {
        if(strcmp(p[k],rfid)==0)
        {
            for(int i = k; i < j-1; i++)
            {
                p[i] = p[i+1];            
            }
            j--;
            break;
        }
    }        
    // Search for product in cart
    for(int i=0; i<cart_size; i++)
    {
        if(strcmp(cart[i].id, rfid) == 0) // Match found in cart
        {
            qty[i]--;              // Decrement quantity in cart
            // If quantity becomes zero or less, remove product from cart
            if(qty[i] <= 0)
            {
                // Shift remaining items left to fill the gap
                for(int j=i; j<cart_size-1; j++)
                {
                    cart[j] = cart[j+1]; // Move next product into current slot
                    qty[j] = qty[j+1];   // Move corresponding quantity
                }
                cart_size--; // Reduce cart size
            }

            // Update stock and cart files
           // update_stock();
            update_cart();

            printf("\nITEM WAS DELETED\n");
            return;
        }
    }

    // If product not found in cart
    printf("\nPRODUCT NOT AVALIABLE IN CART\n");
}

// Function: read_rfid
// Purpose: Reads RFID data from UART until '$' is encountered.
// Stores the received characters into the provided buffer and null-terminates it.
void read_rfid(char *rfid)
{
    int i = 0;    // Index for buffer
    char ch;      // Variable to hold each received character

    // Keep reading characters from UART
    while(read(uart_fd, &ch, 1) > 0)
    {
        // End of RFID data marker
        if(ch == '$')
        {
            break;
        }

        // Ignore newline and carriage return characters
        if(ch == '\n' || ch == '\r')
            continue;

        // Store character into buffer
        rfid[i++] = ch;

        // Prevent buffer overflow (max 19 chars + null terminator)
        if(i >= 19)
            break;
    }

    // Null-terminate the string
    rfid[i] = '\0';
}

