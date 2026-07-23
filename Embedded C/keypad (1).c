#include<lpc24xx.h>

//#include "types.h"

//#include "kpm_defines.h"

#include "lcd.h"
#include "keypad.h"

//#include "lcd_commands.h"


unsigned char KPMLUT[4][4]={{'1','2','3','A'},

                 {'4','5','6','B'},

                 {'7','8','9','C'},

                 {'*','0','#','D'}};


void InitKPM(void)

{

        IODIR1|=(15<<ROW0);

}


unsigned int colscan(void)

{

        return((((IOPIN1>>COL0)&15)<15)?0:1);

}


unsigned int rowcheck(void)

{

        unsigned char rno;

        for(rno=0;rno<4;rno++)

        {

                IOPIN1=~(1<<rno)<<ROW0;

                if(colscan()==0)

                        break;

        }

        IOPIN1=0x0<<ROW0;

        return rno;

}


unsigned int colcheck(void)

{

        unsigned char cno;

        for(cno=0;cno<4;cno++)

        {

                if(((IOPIN1>>(COL0+cno))&1)==0)

                {

                        break;

                }

        }

                return cno;

}


unsigned int keyscan(void)

{

        unsigned char rno,cno,keyv;

        while(colscan());

        rno=rowcheck();

        cno=colcheck();

        keyv=KPMLUT[rno][cno];

        while(!(colscan()));

        return keyv;

}


unsigned int ReadNum(void)

{

        unsigned char key;

        int sum=0;

        while(1)

        {

                key=keyscan();

                if((key>='0')&&(key<='9'))

                {

                        charLCD(key);

                        sum=(sum*10)+(key-'0');

                }

                //Backspace operation

        else if(key=='D')

                {

                if(sum>0)

                {

                        sum=sum/10;// Remove last digit

                        cmdLCD(0X10);

                        charLCD(' '); //Erase digit

                        cmdLCD(0X10);//Move cursor back

                }

                }

                else if(key=='#')

                {

                        break;

                }

        }


        return sum;

}
