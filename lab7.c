/********************************************
 *
 *  Name:Nelson Gonzalez
 *  Email:nelsongo@usc.edu
 *  Section:Wed 5pm
 *  Assignment: Lab 7 - Rotary Encoder
 *
 ********************************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lcd.h"

void variable_delay_us(int);
void play_note(unsigned short);

volatile unsigned char newst, oldst;// the different state machine 

volatile unsigned char changed = 0;  // Flags as learned in class

volatile int count = 0;		// Count to know frequency tone to play at certain time

volatile unsigned char a, b;

volatile unsigned char pinc;

// Frequencies for natural notes 
// up one octave to C5.

unsigned short frequency[8] =
    { 262, 294, 330, 349, 392, 440, 494, 523 };

int main(void) {

    // Initialize DDR and PORT registers and LCD
    DDRB |= (1 << PB4);
    PORTC |= (1 << PC1) | (1 << PC5);

    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT9) | (1 << PCINT13);
    sei();

    lcd_init();
    lcd_writecommand(1);

    // Write a spash screen
    char nel[7];
	
    snprintf(nel, 7, "Nelson");
	
    lcd_moveto(0, 4);
	
    lcd_stringout(nel);
	
    char buf[7];
	
    snprintf(buf, 7, "LAB 7");// dsiplay labs 
	
    lcd_moveto(1, 4);
	
    lcd_stringout(buf);
	
    // Delay 1 second as before in labs 
    _delay_ms(1000);
    lcd_writecommand(1);

    // Read the A and B inputs to determine the intial state
    // Warning: Do NOT read A and B separately.  You should read BOTH inputs
    // at the same time, then determine the A and B values from that value.

    pinc = PINC;
    a = pinc & (1 << PC1);// current value 
    b = pinc & (1 << PC5);// current value 
    // state machine 
    if (!b && !a)
	oldst = 0;
    else if (!b && a)
	oldst = 1;
    else if (b && !a)
	oldst = 2;
    else
	oldst = 3;

    newst = oldst;



    while (1) {                 // Loop forever
        if (changed) { // Did state change?
      	    changed = 0;        // Reset changed flag
      	    // Output count to LCD
            char val[6];
            lcd_writecommand(1);
            lcd_moveto(0, 0);
            snprintf(val, 6, "%d", count);
            lcd_stringout(val);
			
      	    // Do we play a note?
      	    if ((count % 8) == 0) {
				
        		  // Determine what to play 
              int note = (abs(count) % 64) / 8;
        		  
				  
              unsigned short freq = frequency[note];// Determine the hz 
        		  // Plays the note
              play_note(freq);
			  
      	    }
        }
    }
}

/*
  Play a tone at the frequency specified for one second
*/
void play_note(unsigned short freq)
{
    unsigned long period;

    period = 1000000 / freq;      // Period of note in microseconds

    while (freq--) {
    	// Make PB4 high
      PORTB |= (1 << PB4);
    	// Use variable_delay_us to delay for half the period
      variable_delay_us(period / 2);
    	// Make PB4 low
      PORTB &= ~(1 << PB4);
    	// Delay for half the period again
      variable_delay_us(period / 2);
    }
}

/*
    variable_delay_us - Delay a variable number of microseconds
*/
// This was given to us 
void variable_delay_us(int delay)
{
    int i = (delay + 5) / 10;

    while (i--)
        _delay_us(10);
	
}

ISR(PCINT1_vect) {
  // Read the input bits and determine A and B
  pinc = PINC;
  a = pinc & (1 << PC1);// setting current value 
  
  b = pinc & (1 << PC5);// setting current value 

	
	//See the two input bits to determine state and then adjust the counter.
	if (oldst == 0) {
	    // Handle A and B inputs for state 0
      if (a) {
		  
        newst = 1;
		
        count++;// keep count 
		
      } else if (b) {
        newst = 2;
		
        count--;// kee p count 
		
      }

	}
	else if (oldst == 1) {
	    // Handle A and B inputs for state 1
      if (b) {
		  
        newst = 3;
		
        count++;
      } else if (!a) {
        newst = 0;
		
        count--;// keep count 
      }

	}
	else if (oldst == 2) {
	    // Handle A and B inputs for state 2
      if (!b) {
		  
        newst = 0;
		
        count++;// keep count 
		
      } else if (a) {
		  
        newst = 3;
		
        count--;// keep count 
		
      }

	}
	else {   // oldst = 3
	    // Handle A and B inputs for state 3
      if (!a) {
		  
        newst = 2;// keep count 
		
        count++;
      } else if (!b) {
        newst = 1;
		
        count--;// keep count 
		
      }
	}

	// If state changed change to new state 
	// S et a flag to indicate a change 
	if (newst != oldst) {
	    changed = 1;
	    oldst = newst;
	}
}