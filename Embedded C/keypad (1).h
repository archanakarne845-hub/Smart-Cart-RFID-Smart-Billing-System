//---- Keypad Row Pin Definitions ----//

#define ROW0 16        // P1.16 connected to Row 0
#define ROW1 17        // P1.17 connected to Row 1
#define ROW2 18        // P1.18 connected to Row 2
#define ROW3 19        // P1.19 connected to Row 3

//---- Keypad Column Pin Definitions -----//

#define COL0 20        // P1.20 connected to Column 0
#define COL1 21        // P1.21 connected to Column 1
#define COL2 22        // P1.22 connected to Column 2
#define COL3 23        // P1.23 connected to Column 3

//---- Function Prototypes -----//

// Initializes the 4×4 matrix keypad
void InitKPM(void);

// Checks whether any key is pressed
// Returns: 0 = Key pressed, 1 = No key pressed
unsigned int colscan(void);

// Identifies the row number of the pressed key
unsigned int rowcheck(void);


// Identifies the column number of the pressed key
unsigned int colcheck(void);


// Scans the keypad and returns the pressed key value
unsigned int keyscan(void);


unsigned int ReadNum(void);
