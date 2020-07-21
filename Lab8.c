/*
Name : Nelson Gonzalez 
Section: Wed 5-6:20 pm
USC ID: nelsongo@usc.edu 

*/
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include<avr/interrupt.h>

#include "lcd.h"
#include "adc.h"

//holds seconds
unsigned char tenths = 0;
unsigned char ones = 0;
unsigned char tens = 0;
//state (initially 0)
unsigned char state = 0;

int main(void)
{
    unsigned char adc_result;


    // Initialize the LCD
	lcd_init();

    // Initialize the ADC
	adc_init();

    // Write splash screen
	lcd_writecommand(1);	//clears screen
		
	/* Call lcd_stringout to print out your name */
	lcd_moveto(0, 1);	//center text
	lcd_stringout("Nelson Gonzalez");	//prints out my name
	lcd_moveto(1,5);	//move to second row somwher near middle 
	
	lcd_stringout("Lab 8");//prints out lab 8
	_delay_ms(1000);//delay one sec

	lcd_writecommand(1); //clears  screen
	
	// WGM13 = 0, WGM12 = 1
	
	TCCR1B |= (1<<WGM12);
	
	//interrupt – OCIE1A = 1
	
	TIMSK1 |= (1 << OCIE1A);
	
	//loads counter modulus
	
	OCR1A = 6250;
	
	//set prescalar as asked 
	
	TCCR1B |=(1<<CS12);
	
	//enable global like in lab 7
	
	sei();
	
	// time=0
	
	char time[6];
	
	snprintf(time, 5, "%d%d.%d",tens,ones,tenths);
	
	lcd_stringout(time);
	
    while (1) {  // Loops forever
	
        unsigned char adc_result = adc_sample(0);	//stores the adc sample of zer0 like asked
		
		if(state == 0){	//if button is pressed and stopped 
		
			if(adc_result >= 25 && adc_result < 75 ){	//if start and or stop button has been  pressed
			
				state = 1;	//change state to 1
				
				_delay_ms(100);//delay
				
			}
			else if(adc_result >= 76 && adc_result < 150 ){	//if lap and or reset button has been pressed
				//reset to 0 
				
				ones = 0;
				
				tens = 0;
				
				tenths = 0;
				
				//display the 0 using lcd_stringout
				
				lcd_writecommand(1);
				
				snprintf(time, 5, "%d%d.%d",tens,ones,tenths);//formatting
				
				lcd_stringout(time);//print 
				
				_delay_ms(100);
				
			}
			
		}
		else if(state == 1){// state 1
		
			if(adc_result >= 25 && adc_result < 75 ){	//if start and or stop button has been pressed
			
				state = 0;	//changes to st 0 
				
				_delay_ms(100);//delay
				
			}
			else if(adc_result >= 76 && adc_result < 150 ){	//if start and or stop button has been pressed
			
				state = 2;	//lap
				
				_delay_ms(100);
				
			}
			
		}
		else if(state == 2){
			
			if(adc_result >= 25 && adc_result < 150 ){	//if start and or stop button has been pressed
			
				state = 1;	//st 1 starts 
				
				_delay_ms(100);//delay 
				
			}
		}

    }
	

    return 0;   /* not reached */
}

ISR(TIMER1_COMPA_vect){
	if(state != 0){
		
		//incremements every tenths of a second
		
		if(tenths == 9){	//if tenths is 9 and needs to carry
		
			tenths = 0;	//reset to zero
			
			ones++;	//carry the change of the tenths part 
			
			if(ones == 10){	//if ones is over 9
			
				ones = 0;	//reset to zero
				
				tens++;	//carry the change of the ones
				
				if(tens == 6){	//Once 1 minute is reached 
				
					tens = 0;	//rests the tens to 0 
				}
			}
		}
		else{
			tenths++;	//adds the tenths 
		}
	}
	
	if(state == 1){	//displays the value onto lcd 
	
		lcd_writecommand(1);
		
		char time[6];
		
		snprintf(time, 5, "%d%d.%d",tens,ones,tenths);
		
		lcd_stringout(time);
		
	}
}