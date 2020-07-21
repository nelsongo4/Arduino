/********************************************
*
*  Name:Nelson Gonzalez 
*  Email:nelsongo@usc.edu
*  Section: WED 5:00-6:20 PM
*  Assignment: Lab 5 - Up/Down counter on LCD display
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lcd.h"

int main(void) 
{

        // Setup DDR and PORT bits for the 2 input buttons 
        // as necessary
        // Initialize the LCD
		lcd_init(); 
		DDRC &= ~(1<<2);
		DDRC &= ~(1<<4); 
		PORTC |= ((1<<2) | (1<<4)); 
        char curr_state = 0; //current state counter 

        //Here i created the splash screen using snprintf
        lcd_writecommand(1); 
		char buf[17];
        char name[16] = "Nelson Gonzalez";  /* Name */
        snprintf(buf, 17, "%s", name);
        lcd_stringout(buf);
		_delay_ms(1000);
		char buf2[17];
	    int m = 16;
        int d = 18;
	    snprintf(buf2, 17, "%02d/%02d", m, d);
		lcd_moveto(1,3);      //centers the bithdate on lcd
        lcd_stringout(buf2);
		_delay_ms(1000);
        // Use a state machine approach to organize your code
 
		lcd_writecommand(1); 
    while (1) 
  	{
		//If the current state is 0 then it checks for correct B button input to enter next state
		if(curr_state == 0)
		{
			if((PINC & (1<<4))==0)//Alows for transition to next state
			{
				curr_state = 1; 
				
             while((PINC & (1<<4)) == 0 )//Accounts for debouncing of button pressing
             {}
             _delay_ms(5);
			}
			else if((PINC & (1<<2)) == 0)//If wrong button sequence is pressed returns to state 0
			{
				curr_state = 0; 
				
                while((PINC & (1<<2)) == 0 )//Accounts for debouncing of button pressing
                 {}
                _delay_ms(5);
			}
		}
		else if(curr_state == 1)// if current state is 1 enter statement
		{
			if((PINC & (1<<2))==0)//if button is pressed, then transitions to state 2
			{
				curr_state = 2; 
				
                while((PINC & (1<<2)) == 0 )//Accounts for debouncing of button pressing
                 {}
                _delay_ms(5);
			}
			else if((PINC & (1<<4)) ==0)//if button is pressed indicates wrong sequence and goes back to S1
			{
				curr_state = 1; 
				
                 while((PINC & (1<<4)) == 0 )//Accounts for debouncing of button pressing
                {}
                 _delay_ms(5);
			}
		}
		
		else if(curr_state == 2)//if current state is 2
		{
			if((PINC & (1<<2))==0)
			{
				curr_state = 3; 
				
                while((PINC & (1<<2)) == 0 )//Accounts for debouncing of button pressing
                {}
                _delay_ms(5);
 			}
			else if((PINC & (1<<4)) ==0)//if button is pressed indicates wrong sequence and goes back to S1
			{
				curr_state = 1;
				
               while((PINC & (1<<4)) == 0)//Accounts for debouncing of button pressing
                {}
              _delay_ms(5);
			}
		}
		
		else if(curr_state == 3) //If current state is 3
		{
			if((PINC & (1<<4))==0)
			{
				curr_state = 4; 				
                while((PINC & (1<<4)) == 0 )//Accounts for debouncing of button pressing
                {}
                _delay_ms(5);
			}
			else if((PINC & (1<<2)) ==0)//if button is pressed indicates wrong sequence and goes back to S0
			{
		        curr_state = 0;
                while((PINC & (1<<2)) == 0 )//Accounts for debouncing of button pressing
                {}
               _delay_ms(5);					
			}
		}
		else if(curr_state == 4) //final state displays unlocked
		{
			if(((PINC & (1<<4))==0) || ((PINC & (1<<2)) ==0))//if button is pressed indicates wrong sequence and goes back to S0
			{
				curr_state = 0;
                while(((PINC & (1<<4)) == 0 ) || ((PINC & (1<<2)) == 0))//Accounts for debouncing of button pressing
                {}
                _delay_ms(5);			
			}
		}

         /*
		 In this section, I used snprintf to display the different states of the assigned state machine
         If it wase state 1, then the state 0 block would display and so on. 		 
		 */
		lcd_moveto(0,0); 
		char buf2[16]; 
		char buf[12]; 
		if(curr_state ==0)
		{
			snprintf(buf,12,"State = S0"); 
			lcd_stringout(buf); 
			snprintf(buf2, 16, "Status = Locked");	
		}
		/*
		 In this section, I used snprintf to display the different states of the assigned state machine
         If it wase state 1, then the state 1 block would display and so on. 		 
		 */
		if(curr_state ==1)
		{
			snprintf(buf,12,"State = S1"); 
			lcd_stringout(buf); 
			snprintf(buf2, 16, "Status = Locked"); 
		}
		/*
		 In this section, I used snprintf to display the different states of the assigned state machine
         If it wase state 1, then the state 2 block would display and so on. 		 
		 */
		if(curr_state == 2)
		{
			snprintf(buf,12,"State = S2"); 
			lcd_stringout(buf); 
			snprintf(buf2, 16, "Status = Locked"); 
		}
		/*
		 In this section, I used snprintf to display the different states of the assigned state machine
         If it wase state 1, then the state 3 block would display and so on. 		 
		 */
		if(curr_state ==3)
		{
			snprintf(buf,12,"State = S3"); 
			lcd_stringout(buf);
			snprintf(buf2, 16, "Status = Locked"); 
		}
		/*
		 In this section, I used snprintf to display the different states of the assigned state machine
         If it wase state 4, then the state 4 block would display and would be unlocked. 		 
		 */
		if(curr_state == 4)
		{
			snprintf(buf,12,"State = S4"); 
			lcd_stringout(buf); 
			snprintf(buf2, 16, "Status=Unlocked"); 
		}
		lcd_moveto(1,0);
		lcd_stringout(buf2); 
		_delay_ms(20);
			
       /* never reached */
	}
}

