#include <lpc21xx.h>
#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include "interrupt.h"
 #include "rf_reader.h"
int entry_mode = 0;
int delete_mode = 0;
int exit_mode = 0;
int value = 0;
extern int switch_to_cash;
int actual_pin = 0;
char buf[50];
 int entered_pin;
void cash_payment_frontend(void);
void card_payment_frontend(void);

void int_to_str(char *buf, int size, int num)
{
	memset(buf, 0, size);
	sprintf(buf, "%d", num);
}

void RFID(void)
{
	if((entry_mode || delete_mode || exit_mode) == 0)
	{
		cmdLCD(0x01);
		cmdLCD(0x80);
		strLCD("   Welcome To   ");
		cmdLCD(0xC0);
		strLCD("   LUCKY MART   ");
		delay_s(10);
	}
	if(entry_mode == 1)
	{
		int status = -1;
		cmdLCD(0x01);
		cmdLCD(0x80);
		strLCD("Entry mode");
		cmdLCD(0xc0);
		strLCD("Scan RFID Card");
		while(entry_mode == 1)
		{
			if(uart1_ready)
			{
				uart1_ready = 0;
				uart1_buf[strcspn(uart1_buf,"\r\n")]=0;
				//Manager card
				if(strcmp(uart1_buf, "12603355")==0)
				{
					cmdLCD(0x01);
					cmdLCD(0x80);
					strLCD("Manager card");
					cmdLCD(0xC0);
					strLCD("Update Stock...");
					tx_uart0('M');
					 uart0_txstring(uart1_buf);
					 tx_uart0('$');
					 delay_s(2);
					 memset(buf, 0, sizeof(buf));
					 uart0_rxstring(buf);
					 buf[strcspn(buf,"$\r\n")] = '\0';
					 status = atoi(buf);
					 if(status == 0)
					 {
					 	entry_mode = 0;
					 }
					 else
					 {
					 	entry_mode = 1;
					 }
				}
				//customer card
				else
				{
					tx_uart0('C');
					uart0_txstring(uart1_buf);
					tx_uart0('$');
					cmdLCD(0x01);
					cmdLCD(0x80);
					strLCD("customer card");
					cmdLCD(0xC0);
					strLCD("Adding Items..");
					delay_s(2);
				}
			}
			if(delete_mode || exit_mode)
			{
				break;
			}
		}
		entry_mode = 0;
		cmdLCD(0x01);
	}
	//delete_mode
	if(delete_mode == 1)
	{
		cmdLCD(0x01);
		cmdLCD(0x80);
		strLCD("Delete_mode");
		cmdLCD(0xC0);
		strLCD("Scan Card..");
		while(delete_mode == 1)
		{
			if(uart1_ready)
			{
				uart1_ready = 0;
				tx_uart0('D');
				uart0_txstring(uart1_buf);
				tx_uart0('$');
				cmdLCD(0x01);
				cmdLCD(0x80);
				strLCD("Item deleted");
				delay_s(2);
			}
			if(entry_mode || exit_mode)
			{
				break;
			}
		}
		delete_mode = 0;
		cmdLCD(0x01);
	}
	//exit mode
	if(exit_mode == 1)
	{
		char choice;
		char buf[20];
		tx_uart0('T');
		tx_uart0('$');
		memset(buf, 0, sizeof(buf));
		uart0_rxstring(buf);
		buf[strcspn(buf, "\r'n")] = '\0';

		if(strcmp(buf, "EMPTY")==0)
		{
			cmdLCD(0x01);
			cmdLCD(0x80);
			strLCD("cart was empty");
			delay_s(5);
			exit_mode = 0;
			return ;
		}
		else
		{
			cmdLCD(0x01);
			cmdLCD(0x80);
			strLCD("1.cash 2.online");
			while(exit_mode == 1)
			{
				if(entry_mode == 1 || delete_mode == 1)
				{
					exit_mode = 0;
					break;
				}
				choice = keyscan();
				if(choice == '0')
				{
					delay_ms(10);
					continue;
				}
				if(choice =='1')
				{
					cash_payment_frontend();
					exit_mode = 0;
					break;
				}
				else if(choice == '2')
				{
					card_payment_frontend();
					exit_mode = 0;
					break;
				}
			}
			cmdLCD(0x01);
		}
	}
}


void cash_payment_frontend(void)
{
	char buf[50];
	int total = 0;
	int entered_money = 0;
	tx_uart0('H');
	tx_uart0('$');
	memset(buf, 0, sizeof(buf));
	uart0_rxstring(buf);
	buf[strcspn(buf,"$\r\n")]=='\0';
	if(strlen(buf)==0)
		return ;
	total = atoi(buf);

	cmdLCD(0x01);
	cmdLCD(0x80);
	strLCD("total bill");
	cmdLCD(0xC0);
	U32LCD(total);
	delay_s(5);
	label1:cmdLCD(0x01);
	cmdLCD(0x80);
	strLCD("enter money");
	cmdLCD(0xC0);
	entered_money=ReadNum();
	if(entered_money == total)
	{
		tx_uart0('1');
		tx_uart0('$');
		cmdLCD(0x01);
		cmdLCD(0x80);
		strLCD("paid successfully");
		delay_s(5);
		cmdLCD(0x01);
		cmdLCD(0x80);
		strLCD("Thank you ");
		cmdLCD(0xC0);
		strLCD("    VISIT AGAIN!!   ");
		delay_s(5);
		return ;
	}

	else if(entered_money > total)
	{
		int return_money = entered_money - total;
		tx_uart0('1');
		tx_uart0('$');
		cmdLCD(0x01);
		cmdLCD(0x80);
		strLCD("payment success");
		cmdLCD(0xc0);
		strLCD("returns:");
		cmdLCD(0xC0+8);
		U32LCD(return_money);
		delay_s(5);
		cmdLCD(0x01);
		cmdLCD(0x80);
		return ;
	}
	else
	{
		tx_uart0('0');
		tx_uart0('$');
		cmdLCD(0x01);
		cmdLCD(0x80);
		strLCD("payment failed");
		cmdLCD(0xC0);
		strLCD("less money");
		delay_s(5);
		goto label1;
	}

}

void card_payment_frontend(void)
{
	char buf[50];
	//int pin;
	cmdLCD(0x01);
	cmdLCD(0x80);
	strLCD("Scan ATM card");
	while(uart1_ready == 0);
	uart1_ready = 0;
	tx_uart0('B');
	uart0_txstring(uart1_buf);
	tx_uart0('$');
	memset(buf, 0, sizeof(buf));
	uart0_rxstring(buf);
	buf[strcspn(buf,"$\r\n")] = '\0';

	if(strcmp(buf,"INVALID") == 0)
	{
		cmdLCD(0x01);
		cmdLCD(0x80);
		strLCD("invalid card");
		delay_s(2);
		return ;
	}
	else if(strcmp(buf, "CARD_OK") == 0)
	{
		memset(buf, 0, sizeof(buf));
		uart0_rxstring(buf);
		buf[strcspn(buf,"$\r|n")]='\0';
		actual_pin = atoi(buf);
		cmdLCD(0x01);
		cmdLCD(0x80);
		strLCD("received PIN:");
		cmdLCD(0xC0);
		U32LCD(actual_pin);
		delay_s(2);
		label2:cmdLCD(0x01);
		cmdLCD(0x80);
		strLCD("Enter pin:");
		cmdLCD(0xC0);
		entered_pin = ReadNum();
		delay_s(2);
		if(entered_pin == actual_pin)
		{
			tx_uart0('1');
			tx_uart0('$');
			memset(buf, 0, sizeof(buf));
			uart0_rxstring(buf);
			buf[strcspn(buf,"$\r\n")] = '\0';
			if(strcmp(buf,"SUCCESS")==0)
			{
				cmdLCD(0x01);
				cmdLCD(0x80);
				strLCD("payment OK");
				delay_s(2);
				cmdLCD(0x01);
				cmdLCD(0x80);
				strLCD("Thank you");
				cmdLCD(0xC0);
				strLCD("    VISIT AGAIN!!   ");
	     		delay_s(5);
				return ;
			}
			else if(strcmp(buf,"LOW_BAL")==0)
			{
				cmdLCD(0x01);
				cmdLCD(0x80);
				strLCD("Less Bal");
				delay_s(2);
				return ;
			}
		}
		else if(entered_pin != actual_pin)
		{
			tx_uart0('0');
			tx_uart0('$');
			memset(buf, 0, sizeof(buf));
			uart0_rxstring(buf);
			buf[strcspn(buf,"$\r\n")]='\0';
			cmdLCD(0x01);
			cmdLCD(0x80);
			strLCD("wrong Pin");
			delay_s(2);
			if(strcmp(buf,"RETRY")==0)
			{
				goto label2;
			}
			else if(strncmp(buf,"CASH",4)==0)
			{
			cmdLCD(0x01);
			cmdLCD(0x80);
			strLCD("Card Blocked");
			cmdLCD(0xc0);
			strLCD("use CASH");
			delay_s(2);
			cmdLCD(0x01);
			cmdLCD(0x80);
			strLCD("switch to");
			cmdLCD(0xC0);
			strLCD("CASH MODE");
			delay_s(2);
			memset(buf,0,sizeof(buf));
			switch_to_cash=1;
			uart1_ready = 0;
			//cash_payment_frontend();
			return;
			}
		}
	}
}
