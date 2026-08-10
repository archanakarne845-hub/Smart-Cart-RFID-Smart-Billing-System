#include <stdio.h> // Standard input/output functions
#include <stdlib.h> // General utility functions
#include <string.h>// String handling functions
#include <unistd.h>// POSIX API (read, write, close, sleep)
#include <fcntl.h>// File control options (open)
#include <termios.h>// UART/Serial communication configuration
#include <time.h>// Date and time functions(for sales database)


                /* Product database structure */
struct product
{
    char name[20]; // Product name
    char id[20];// RFID tag of Product
    int stock;// Available quantity of Products
    float price;// Price of one Unit of Product
};


            /* Bank account/card information structure */
struct bank
{
    char card_id[20]; // RFID card ID
    char place[20]; // Branch location
    char ifsc_code[20];// IFSC code of bank
    char bank_name[20]; // Bank name
    char acc_no[20];// Account number
    long int balance; // Available account balance
                     //long int bcz balance can become large value
    char atm_pin[10];// ATM PIN of that account
};

                /* Global Variables */

int bill_no = 1000; //Starting bill number
struct product db[100];//Product database (loaded from stock.csv)
struct product cart[100];//Shopping cart containing selected products
struct bank b[100];//Bank database (loaded from bank.csv)
int qty[100];//Quantity of each product in the cart
int db_size = 0;//Current number of products in stock database
int cart_size = 0;//Current number of items in cart
int bank_size = 0;//Current number of bank records
char *p[100];
int j = 0;
int uart_fd;//UART file descriptor used for serial communication

void save_sales();// Saves completed sales transaction to sales.csv
void transaction_cash(void);//Handles cash payment transaction

// UART INIT
void uart_init()
{
    uart_fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);//Open UART device
    //Check whether UART opened successfully
    if(uart_fd < 0)
    {
        printf("UART OPEN FAILED\n");
        exit(1);
    }
    struct termios options;

    tcgetattr(uart_fd,&options);//Get current UART settings
    cfsetispeed(&options,B9600);//Set baud rate to 9600
    cfsetospeed(&options,B9600);//Set baud rate to 9600
    options.c_cflag |= (CLOCAL | CREAD);//Enable receiver and ignore modem control lines
    options.c_cflag &= ~PARENB;//Disable parity bit
    options.c_cflag &= ~CSTOPB;//Use 1 stop bit
    options.c_cflag &= ~CSIZE;//Clear current data size settings
    options.c_cflag |= CS8;//Set 8-bit data frame
    tcsetattr(uart_fd,TCSANOW,&options);//Apply new UART settings immediately
}

//UART RECEIVE CHARACTER
char uart_rx(void)
{
    char ch;
    while(read(uart_fd, &ch, 1) <= 0);
    return ch;
}

//UART TRANSMIT CHARACTER
void uart_tx(char ch){
        write(uart_fd,&ch,1);
}

//UART RECEIVE STRING
void uart_str_rx(char *buf,int size)
{
        char ch;
        int i=0;
        while(1)
        {
             ch=uart_rx();
             if(ch=='$')
             {
                 break;
	     }
	     if(ch=='\n'||ch=='\r')
	     {
		 continue;
             }
             if(i<=size-1)
	     {
                 buf[i++]=ch;
             }
        }
        buf[i]='\0';
}
//UART TRANSMIT STRING
void uart_str_tx(char *str)
{
    write(uart_fd,str,strlen(str));
}


// READ RFID
void read_rfid(char *rfid)
{
    int i = 0;
    char ch;
    while(read(uart_fd,&ch,1) > 0)
    {
        if(ch == '$')
        {
            break;
        }
        if(ch == '\n' || ch == '\r')
        {
            continue;
        }
        rfid[i++] = ch;
        if(i >= 19)
        {
            break;
        }
    }
    rfid[i] = '\0';
}

// LOAD STOCK
void load_stock()
{
    FILE *fp = fopen("stock.csv","r");
    if(fp == NULL)
    {
        printf("STOCK FILE MISSING\n");
        return;
    }
    char line[200];
    fgets(line,sizeof(line),fp);
    while(fscanf(fp,"%[^,],%[^,],%d,%f\n",
			    db[db_size].name,
			    db[db_size].id,
			    &db[db_size].stock,
			    &db[db_size].price) == 4)
    {
        db_size++;
    }
    fclose(fp);
}

// DISPLAY STOCK
void display_stock()
{
    printf("\n=========== STOCK DATABASE ===========\n");
    printf("%-15s %-15s %-10s %-10s\n","ITEM","RFID","STOCK","PRICE");
    printf("-----------------------------------------------------\n");
    for(int i=0;i<db_size;i++)
    {
        printf("%-15s %-15s %-10d %-10.2f\n",db[i].name,db[i].id,db[i].stock,db[i].price);
    }
}

// UPDATE STOCK
void update_stock()
{
    FILE *fp = fopen("stock.csv","w");
    fprintf(fp,"item,Rfid,quantity,price\n");
    for(int i=0;i<db_size;i++)
    {
        fprintf(fp,"%s,%s,%d,%.2f\n",db[i].name,db[i].id,db[i].stock,db[i].price);
    }
    fclose(fp);
}

// LOAD BANK
void load_bank()
{
    FILE *fp = fopen("bank.csv","r");
    if(fp == NULL)
    {
        printf("BANK FILE MISSING\n");
        return;
    }
    char line[200];
    fgets(line,sizeof(line),fp);
    while(fscanf(fp,"%[^,],%[^,],%[^,],%[^,],%[^,],%ld,%s\n",
			    b[bank_size].card_id,
			    b[bank_size].place,
			    b[bank_size].ifsc_code,
			    b[bank_size].bank_name,
			    b[bank_size].acc_no,
			    &b[bank_size].balance,
			    b[bank_size].atm_pin) == 7)
    {
        bank_size++;
    }
    fclose(fp);
}

// DISPLAY BANK
void display_bank()
{
    printf("\n=========== BANK DATABASE ===========\n");
    printf("%-15s %-15s %-10s %-10s %-10s %-8s %-8s\n",
		    "RFID","PLACE","IFSC_CODE","BANK_NAME","ACC_NO","BALANCE","PIN");
    printf("-----------------------------------------------------\n");
    for(int i=0;i<bank_size;i++)
    {
        printf("%-15s %-15s %-10s %-10s %-10s %-8ld %-8s\n",
			b[i].card_id,
			b[i].place,
			b[i].ifsc_code,
			b[i].bank_name,
			b[i].acc_no,
			b[i].balance,
			b[i].atm_pin);
    }
}
// UPDATE BANK
void update_bank()
{
	// Open bank.csv in write mode.
    FILE *fp = fopen("bank.csv","w");
	// Write the CSV column headings.
    fprintf(fp,"card_id,place,ifsc_code,bank_name,acc_no,balance,pin\n");
    for(int i=0;i<bank_size;i++)
    {
		// Write bank details into the CSV file.
        fprintf(fp,"%s,%s,%s,%s,%s,%ld,%s\n",
			b[i].card_id,
			b[i].place,
			b[i].ifsc_code,
			b[i].bank_name,
			b[i].acc_no,
			b[i].balance,
			b[i].atm_pin);
    }
    fclose(fp);
}

// FIND PRODUCT
int find_product(char *rfid)
{
    for(int i=0;i<db_size;i++)
    {
		// Compare the received RFID/card ID with
        // the card ID stored in the stock database.
        if(strcmp(db[i].id,rfid)==0)
        {
			// Matching card found.
            // Return its index in the stock array.
            return i;
        }
    }
	// No matching card was found.
    return -1;
}

// FIND BANK
int find_bank(char *rfid)
{
    for(int i=0;i<bank_size;i++)
    {
		// Compare the received RFID/card ID with
        // the card ID stored in the bank database.
        if(strcmp(b[i].card_id,rfid)==0)
        {
			// Matching card found.
            // Return its index in the bank array.
            return i;
        }
    }
	// No matching card was found.
    return -1;
}
// UPDATE CART FILE
void update_cart()
{
	// Open cart.csv in write mode.
    FILE *fp = fopen("cart.csv","w");
	 // Write the CSV column headings.
    fprintf(fp,"item,rfid,quantity,price,total\n");
    for(int i=0;i<cart_size;i++)
    {
		 // Write product details into the CSV file.
        fprintf(fp,"%s,%s,%d,%.2f,%.2f\n",
			cart[i].name,
			cart[i].id,
			qty[i],
			cart[i].price,
			qty[i] * cart[i].price);
    }
	// Close the file after updating the cart.
    fclose(fp);
}

// DISPLAY CART
void display_cart()
{
    float total = 0;
    printf("\n=========== CART ===========\n");
    printf("%-15s %-10s %-10s %-10s\n","ITEM","QTY","PRICE","TOTAL");
    printf("-------------------------------------------------\n");
    for(int i=0;i<cart_size;i++)
    {
        float item_total =qty[i] * cart[i].price;
        printf("%-15s %-10d %-10.2f %-10.2f\n",cart[i].name,qty[i],cart[i].price,item_total);
        total += item_total;
    }
    printf("\nGRAND TOTAL : %.2f\n",total);
}

// ADD TO CART
void add_to_cart(char *rfid)
{
    int cnt = 0;
    int index = find_product(rfid);

    if(index == -1)
    {
		// RFID is not present in the product database.
        printf("\nINVALID RFID\n");
        return;
    }
    // Find the length of the received RFID string.
    int len = strlen(rfid);
	// Allocate memory for storing the RFID.
    p[j] = calloc(1,len);
	// Copy the received RFID into the allocated memory.
    strcpy(p[j],rfid);
	// Move to the next RFID storage position.
    j++;
    
    for(int i = 0; i < j; i++)
    {
        if(strcmp(p[i],rfid)==0)
        {
            cnt++;
			// Check whether the number of scans exceeds
            // the available stock.
            if(!(cnt <= db[index].stock))
            {
				// Product is not available in sufficient quantity.
                printf("\nOUT OF STOCK\n");
                j--;
                return;
            }
        }
    }

    for(int i = 0; i < cart_size; i++)
    {
		 // Check whether the product has no stock remaining.
         if(db[index].stock <= 0)
         {
			
              printf("\nOUT OF STOCK\n");
              return;
         }
         if(strcmp(cart[i].id,rfid)==0)
         {
            qty[i]++;
            //db[index].stock--;
            //update_stock();
            //update_cart();
            printf("\nPRODUCT UPDATED\n");
            return;
         }
    }
    cart[cart_size] = db[index];
    qty[cart_size] = 1;
    cart_size++;
    //db[index].stock--;
    //update_stock();
    //update_cart();
    printf("\nPRODUCT ADDED\n");
}

// DELETE PRODUCT
void delete_product(char *rfid)
{
    int index = find_product(rfid);
    if(index == -1)
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

    for(int i=0;i<cart_size;i++)
    {
        if(strcmp(cart[i].id,rfid)==0)
        {
            qty[i]--;
            //db[index].stock++;
            if(qty[i] <= 0)
            {
                for(int j=i;j<cart_size-1;j++)
                {
                    cart[j] = cart[j+1];
                    qty[j] = qty[j+1];
                }
                cart_size--;
            }
           // update_stock();
            update_cart();
            printf("\nITEM DELETED\n");
            return;
        }
    }
    printf("\nPRODUCT NOT IN CART\n");
}
// DISPLAY BILL
void display_bill()
{
    float total = 0;
    printf("\n=========== FINAL BILL ===========\n");
    for(int i=0;i<cart_size;i++)
    {
        float item_total =qty[i] * cart[i].price;
        printf("%s  Qty=%d  Total=%.2f\n",cart[i].name,qty[i],item_total);
        total += item_total;
    }
    printf("\nTOTAL BILL = %.2f\n",total);
}

// SEND TOTAL BILL
void send_total_bill()
{
    char buf[20];
    float total = 0;
    for(int i=0;i<cart_size;i++)
    {
        total += qty[i] * cart[i].price;
    }
    sprintf(buf,"%d$",(int)total);
    uart_str_tx(buf);
}
// CARD PAYMENT
void transaction_card(char *card_no)
{
    printf("Entered into bank mode..\n");
    char buf[50];
    int chances = 3;
    int found = 0;
    int total = 0;
    // CALCULATE TOTAL
    for(int j=0;j<cart_size;j++)
    {
        total += qty[j] * cart[j].price;
    }
    for(int i=0;i<bank_size;i++)
    {
        if(strcmp(card_no,b[i].card_id)==0)
        {
            found = 1;
            printf("Card Found\n");
            printf("Bank=%s\n",b[i].bank_name);
            printf("Total Bill=%d\n",total);
            uart_str_tx("CARD_OK");
            uart_tx('$');
            sprintf(buf,"%s",b[i].atm_pin);//copies the stored pin in buffer
            uart_str_tx(buf);
            uart_tx('$');
            while(chances > 0)
            {
	chance:memset(buf,0,sizeof(buf));
                uart_str_rx(buf,sizeof(buf));
                buf[strcspn(buf,"$\r\n")] = '\0';//replace $ as '\0'
                printf("Received Status:%s",buf);
                // CORRECT PIN
                if(strcmp(buf,"1")==0)
                {
                    if(b[i].balance >= total)
                    {
                        b[i].balance -= total;
			update_stock();
                        update_bank();
                        save_sales(card_no,"ONLINE");

                        char *rfid;
                        for(int k = 0; k < j; k++)
                        {
                            rfid = p[k];
                            int index = find_product(rfid);
                            int num = 0;
                            if(num == 0)
                            {
                                for(int i = 0;i <db_size; i++)
                                {
                                    if(strcmp(db[i].id,rfid)==0)
                                    {
                                        db[i].stock--;
                                        update_stock();
                                        update_cart();
                                        break;
                                    }
                                }
                                num++;
                            }
                        }
                        j = 0;
                        cart_size = 0;
                        uart_str_tx("SUCCESS");
                        uart_tx('$');
                        printf("PAYMENT SUCCESS\n");
                        return;
                    }
                    else
                    {
                        printf("Payment failed...due to low balance\n");
                        uart_str_tx("LOW_BAL");
                        uart_tx('$');
                        j = 0;
                        cart_size = 0;
                        return;
                    }
                }
                // WRONG PIN
                else
                {
                    chances--;
                    if(chances == 0)
                    {
                        uart_str_tx("CASH");
                        uart_tx('$');
                        printf("Card Blocked\n");
                        printf("Switch to Cash Payment\n");
                        sleep(1);
                       // transaction_cash();
                        return;
                    }
                    printf("payment failed..due to wrong pin\n");
                    uart_str_tx("RETRY");
                    uart_tx('$');
                    goto chance;
                }
            }
    	}
    }
    // INVALID CARD
    if(found == 0)
    {
        uart_str_tx("INVALID");
        uart_tx('$');
    }
}
//transaction cash
void transaction_cash(void)
{
    int cnt = 0;
    char buf[50];
    int total=0;
    for(int i=0;i<cart_size;i++)
    {
        total += qty[i] * cart[i].price;
    }
    // SEND TOTAL
    sprintf(buf,"%d",total);
    uart_str_tx(buf);
    uart_tx('$');
    printf("Sending Money:%s\n",buf);
    // RECEIVE result
l1:memset(buf,0,sizeof(buf));
    cnt++;
    uart_str_rx(buf,sizeof(buf));
    if(cnt != 1)
        printf("Received Result:%s\n",buf);
    int result=atoi(buf);
    // SUCCESS
    if(result == 1)
    {
        save_sales("CASH","CASH");
        char *rfid;
        for(int k = 0;k < j; k++)
        {
            rfid = p[k];
            int index = find_product(rfid);
            for(int i = 0; i < db_size; i++)
            {
                if(strcmp(db[i].id,rfid)==0)
                {
                     db[i].stock--;
                     update_stock();
                     update_cart();
                }
            }
        }
        j = 0;
        cart_size = 0;
        printf("PAID SUCCESSFULLY\n");
        cnt = 0;
        return;
    }
     //payment fail
    else
    {
         if(cnt != 1)
               printf("Payment Failed due to Less Money\n");
         goto l1;
    }
}

//saves sales data
void save_sales(char *card_id,char *payment_mode)
{
    FILE *fp;
    time_t t;
    struct tm *tm_info;
    char date[20];
    char day[20];
    char time_str[20];
    float grand_total = 0;
    fp = fopen("sales.csv","a");//append mode
    //Check whether file opened successfully
    if(fp == NULL)
    {
        printf("Sales File Open Failed\n");
        return;
    }
    //Get current system date and time
    time(&t);
    tm_info = localtime(&t);
    strftime(date,sizeof(date),"%d-%m-%Y",tm_info);
    strftime(day,sizeof(day),"%A",tm_info);
    strftime(time_str,sizeof(time_str),"%I:%M %p",tm_info);
    //Generate next bill number
    bill_no++;
    //Write bill header
    fprintf(fp,"\n====================================================\n");
    fprintf(fp,"BILL NO : %d\n",bill_no);
    fprintf(fp,"DATE    : %s\n",date);
    fprintf(fp,"DAY     : %s\n",day);
    fprintf(fp,"TIME    : %s\n",time_str);
    fprintf(fp,"CARD ID : %s\n",card_id);
    fprintf(fp,"PAYMENT : %s\n",payment_mode);
    fprintf(fp,"--------------------------------------------------------\n");
    fprintf(fp,"\nITEM,RFID,QTY,PRICE,TOTAL\n\n");
    for(int i=0;i<cart_size;i++)
    {
        float total;
        total = qty[i] * cart[i].price;
        grand_total += total;
        fprintf(fp,"%s,%s,%d,%.2f,%.2f\n",
			cart[i].name,
			cart[i].id,
			qty[i],cart[i].price,total);
    }
    fprintf(fp,"\n----------------------------------------------------\n");
    fprintf(fp,"GRAND TOTAL : %.2f\n",grand_total);
    fprintf(fp,"====================================================\n");
    fclose(fp);
}
// MAIN
int main()
{
    char rfid[20]; // Stores complete RFID data received
    char clean_rfid[20]; //Stores RFID without command character
    char buf[10];
    load_stock(); //Load product database from stock.csv
    load_bank();  //Load bank database from bank.csv
    uart_init();  //Initialize UART communication
    system("clear"); //Clear terminal screen
    //Display current system information
    display_stock();
    display_bank();
    display_cart();
    //Display welcome message
    printf("\n====================================\n");
    printf("      SMART BILLING SYSTEM\n");
    printf("====================================\n");
    printf("\nScan RFID Card...\n");
    //Infinite loop waiting for RFID scans
    while(1)
    {
         memset(rfid,0,sizeof(rfid));//Clear RFID buffer before reading new data
         read_rfid(rfid);//Read RFID data from UART
         //Ignore empty RFID data
        if(strlen(rfid)==0)
        {
            continue;
        }
        //Display received RFID
        printf("Received RFID:%s\n",rfid);
        //First character determines operation type
        char cmd = rfid[0];
        printf("%c\n",cmd);
        //Extract actual RFID by removing command character
        strcpy(clean_rfid,rfid + 1);
        printf("Clean_RFID:%s\n",clean_rfid);
        //CUSTOMER mode
        if(cmd == 'C')
        {
            add_to_cart(clean_rfid);
            sleep(2);
            system("clear");//removes previous display and prepares a clean screen
            display_stock();
            display_bank();
            display_cart();
        }
	 // DELETE mode
        else if(cmd == 'D')
        {
            delete_product(clean_rfid);
            sleep(2);
            system("clear");//linux cmd
            display_stock();
            display_bank();
            display_cart();

        }

        // BANK CARD
        else if(cmd == 'B')
        {
            printf("payment in online mode\n");
            transaction_card(clean_rfid);
            sleep(5);
            system("clear");
            display_stock();
            display_bank();
            display_cart();
            printf("waiting for the another customer.....\n");
            printf("Scan RFID Card...\n");
        }

        // MANAGER mode
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
              printf("\n====================================\n");
              printf("        MANAGER MODE\n");
              printf("=====================================\n");
              printf("1. UPDATE EXISTING STOCK\n");
              printf("2. ADD NEW PRODUCT\n");
              printf("3. EXIT\n");
              printf("\nEnter ur Choice : ");
              scanf("%d",&choice);
	     // UPDATE EXISTING PRODUCT
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
             	     printf("\nCurrent Stock : %d\n",
              	     db[index].stock);
              	     printf("Enter Quantity To Add : ");
              	     scanf("%d",&quantity);
              	     db[index].stock += quantity;
               	     update_stock();
              	     printf("\nSTOCK UPDATED SUCCESSFULLY\n");
              	     printf("NEW STOCK : %d\n",
              	     db[index].stock);
              	    sleep(3);
                }
              }
 	     // ADD NEW PRODUCT
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
                    update_stock();
                    printf("\nNEW PRODUCT ADDED\n");
                    sleep(3);
                }
       	        display_stock();
        	display_bank();
     	   	display_cart();
    	     }
 	   // EXIT
    	   else
    	   {
               uart_str_tx("0");
               uart_tx('$');
               printf("\nEXITING MANAGER MODE\n");
               sleep(2);
               printf("scan rfid card...\n");
               break;
           }
           system("clear");
           display_stock();
           display_bank();
           display_cart();
        }
     } 
    //transaction mode
    else if(cmd == 'T')
    {
    if(cart_size == 0)
    {
        uart_str_tx("EMPTY");
        uart_tx('$');
        printf("cart empty...");
        sleep(2);
        system("clear");
        display_stock();
        display_bank();
        display_cart();
        printf("Add items to the cart....\n");
        printf("Scan Rfid card...\n");
    }
    else
    {
        uart_str_tx("NOTEMPTY");
        uart_tx('$');
    }
}
//cash payment
    else if(cmd=='H')
    {
        printf("payment in cash mode\n");
        transaction_cash();
        sleep(5);
        system("clear");
        display_stock();
        display_bank();
        display_cart();
        printf("waiting for the another customer.....");
        printf("\nScan RFID Card...\n");
    }

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
    close(uart_fd);
    return 0;
}

