#define pin 24
#define RS 19
#define RW 17
#define EN 18

//LCD commands

#define CLEAR_LCD 0x01
#define RET_CUR_HOME 0x02

//Display Lcds

#define DISP_OFF 0x08
#define DISP_ON 0x0c
#define DISP_ON_CUR_ON 0x0E
#define DISP_ON_CUR_BLINK 0x0F

//Mode bits

#define MODE_8BIT_1LINE 0x30
#define MODE_8BIT_2LINE 0x38
#define MODE_4BIT_1LINE 0x20
#define MODE_4BIT_2LINE 0x28

//position bits

#define GOTO_LINE1_POS0 0x80
#define GOTO_LINE2_POS0 0xc0
#define GOTO_LINE3_POS0 0x94
#define GOTO_LINE4_POS0 0xD4

#define SHIFT_CUR_RIGHT 0x06
#define SHIFT_DISP_LEFT 0x10
#define SHIFT_DISP_RIGHT 0x14

//display symbols

#define GOTO_CGRAM 0x40

void InitLCD(void);
void strLCD(char *p);
void U32LCD(unsigned int num);
void charLCD(unsigned char ascii);
void cmdLCD(unsigned char cmd);
void writeLCD(unsigned char data);

