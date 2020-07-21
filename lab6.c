/********************************************
 *
 *  Name:
 *  Email:
 *  Section:
 *  Assignment: Lab 6 - Analog-to-digital conversion
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#include "lcd.h"
#include "adc.h"

int main(void)
{
    //unsigned char adc_result;

    // Initialize the LCD
	lcd_init();

    // Initialize the ADC
	adc_init();
	//initialize rand_init function
	rand_init(); 

    // Write splash screen
	lcd_writecommand(1);	//clear whatever is on the screen
		
	/* Call lcd_stringout to print out your name */
	lcd_moveto(0, 1);	//center text
	lcd_stringout("Nelson Gonzalez");	//print out name
	
	/* Use lcd_moveto to start at an appropriate column  */
	lcd_moveto(1,5);	//move to second row
	
	lcd_stringout("Lab 6");		//print lab 6
	/* Delay 1 second */
	_delay_ms(1000);

	lcd_writecommand(1); //clear screen	
    // Find a random initial position of the 'X'
	unsigned char posx = rand() % 16;	//pos holds the initial position of x
	
	lcd_moveto(0, posx);		//move to the place in position
	lcd_stringout("X");		//print out an X

	unsigned char oldcar = (unsigned char)adc_sample(3)/16;	//for the new position
	unsigned char count = 0;
	lcd_moveto(1,oldcar);	//starting point for carrot
	lcd_stringout("^");		//print out the starting carrot
	
	
    while (1) 
	{                 // Loop forever
			// Do a conversion
		unsigned char samx = adc_sample(0); //sample for x movement 
		if(samx >=0 && samx <=5)
		{
			lcd_moveto(0,posx);//move a space to curr position
			lcd_stringout(" ");//Output space  
			posx++; //increase position
			if(posx>15)//Keeps within walls of lcd
			{
				posx = 15; 
			}
			lcd_moveto(0,posx);//move x to a position in front of old
			lcd_stringout("X");//Output x to lcd 
			_delay_ms(100); 
		}
		if(samx>=140 && samx <=160)
		{
			lcd_moveto(0,posx);//Moves space to current position 
			lcd_stringout(" "); // ouputs space
			posx--;//decrements for movement 
			if(posx<0)//Keeps withing left lcd wall
			{
				posx = 0; 
			}
			lcd_moveto(0,posx);//moves x to position ahead
			lcd_stringout("X"); //Outputs x to lcd 
			_delay_ms(100); 
		}
	
		unsigned char car = adc_sample(3)/16; 
		 //convert to position #
		// Move '^' to new position
		if(car != oldcar)
		{	//if the position has changed
			lcd_moveto(1,oldcar);	//where x is currently
			lcd_stringout(" ");		//clear it
			lcd_moveto(1,car);	//where x is now
			lcd_stringout("^");		//print a ^ in the new place
			oldcar = car;		//equalize
		}
		
		// Delay
		_delay_ms(10);
	
		if(car == posx)//Position of x must equal carrot
		{
			count++;//adding counter for display 
			if(count == 200)//once 200 cnt have passed you win will be displayed
			{
				lcd_writecommand(1); 
				lcd_moveto(0,4); 
				lcd_stringout("You Win!"); 
			}
		}
		if(car != posx)//Reassigns back to zero
		{
			count = 0; 
		}
    }
    return 0;   /* never reached */
}


void rand_init()
{
    int seed = 0;
    unsigned char i, j, x;

    // Build a 15-bit number from the LSBs of an ADC
    // conversion of the channels 1-5, 3 times each
    for (i = 0; i < 3; i++) {
	for (j = 1; j < 6; j++) {
	    x = adc_sample(j);
	    x &= 1;	// Get the LSB of the result
	    seed = (seed << 1) + x; // Build up the 15-bit result
	}
    }
    srand(seed);	// Seed the rand function
}
