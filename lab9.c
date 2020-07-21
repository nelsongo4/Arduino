/********************************************
 *
 *  Name:Nelson Gonzalez
 *  Email:nelsongo@usc.edu
 *  Section:5 pm wed
 *  Assignment: Lab 9 - Serial Communications
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "adc.h"
#include "check9.h"

// Serial communications functions and variables
void serial_init(unsigned short);
void serial_stringout(char *);
void serial_txchar(char);

#define FOSC 16000000           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR (FOSC/16/BAUD-1) // Value for UBRR0 register

// ADC functions and variables
void adc_init();

char buf[16];	//buffer for displaying things 

unsigned char downbuttons = 0;	//flags that check for button pressing

unsigned char msg = 0;	//Shows which message to diplay

unsigned char rec_data = 0;	//flags for when the message is received 

unsigned char index_ch = 0;	//for displaying lcd correctly 

#define ADC_CHAN 0              // Buttons use ADC channel 0


char *messages[] = {
    "Hello           ",
    "How are you?    ",
    // Add more messgages
	"How's the folks ",
	"Whats new?      ",
	"I'm great       ",
	"That's good     ",
	"Ha Ha!          ",
	"Knock wood      ",
	"Well well       ",
	"That's life     ",
	"What'd ya know? ",
	"How's the wife? ",
	"Gotta run!      ",
	"Oh MY           ",
	"Ta Ta!          ",
	"All aboard      ",
	"Goodbye!        ",
	"      ^m^       ",
};

int main(void) {

    // Initialize the LCD, ADC and serial modules
    lcd_init();
    adc_init();
    serial_init(MYUBRR);
    // Initialize the baud rate checker
    //check9_init();

    // Enable interrupts
    UCSR0B |= (1 << RXCIE0);    //receiver
	sei();	//global interrupts

    // Show the splash screen
	lcd_writecommand(1);	//clear whatever is on the screen
		
	/* lcd_stringout to print out name and splash*/
	lcd_moveto(0, 0);
	lcd_stringout("Nelson Gonzalez");	//print out name
	
	/* lcd moveto for moving to correct locations*/
	lcd_moveto(1,4);	//move to the second row close to middle 
	//Shows the lab number 
	
	lcd_stringout("Lab 9");
	/* Delay 1 second */
	_delay_ms(1000);

	lcd_writecommand(1); //clears screen of lcd


	//print out the first message 
	lcd_moveto(0,0);
	lcd_stringout(messages[msg]);


    while (1) {                 // Loop forever

	// Get an ADC sample to read buttons


        // Up button pressed?


        // Down button pressed?


        // Select button pressed?
        

        // Message received from remote device?
		// Get an ADC sample
		unsigned char adc_result = adc_sample(0);	//Holds value of adc read value of adc

        // Determines if up button is pressed
		if(adc_result >= 25 && adc_result < 75)
		{
			msg += 1; //Decides next message to be sent  
			if(msg == 18){
				//Goes back to 1 after reaching max 
				msg = 0;
			}
			downbuttons = 1;
			_delay_ms(100);
		}

        // If the lcd button is pressed
		
		else if(adc_result >= 75 && adc_result < 150)
		{
			if(msg == 0){
				
				//Goes back to 17 if less than 0 
				
				msg = 17;

			}
			else{
				msg--; //next message displayed 
			}
			downbuttons = 1;
			_delay_ms(100);
		}

        // Button pressing 
        if(adc_result >= 175 && adc_result < 275)
		{
			_delay_ms(100);
			
			//send messages using function 
			
			serial_stringout(messages[msg]);
		}
		
        // Messages recieved : 
		if(rec_data == 1){
			
			//here i use lcd moveto and string out to print messages 
			
			lcd_moveto(1,0);
			
			//prints out the messgae
			
			lcd_stringout(buf);
			
			//resets the flags again 
			
			rec_data = 0;
			
		}
		
		//change message when buttuns are pressed 
		
		if((downbuttons == 1)&&(adc_result >= 75 && adc_result < 150)){
			
			lcd_moveto(1,0);//move to beginning lines 
			
			lcd_stringout(messages[msg]);
			
			downbuttons = 0;
			
		}
		
		if((downbuttons == 1 )&& (adc_result >= 175 && adc_result < 275)){
			
			lcd_moveto(0,0);//move to beginning lines 
			
			lcd_stringout(messages[msg]);
			
			downbuttons = 0;
			
		}
		

    }
}

/* ----------------------------------------------------------------------- */

void serial_init(unsigned short ubrr_value)
{

    // Set up USART0 registers

    // Enable tri-state buffer
	DDRD |= (1<<3);
	
	PORTD &= ~(1<<3);
	
	//set BAUD rate
	
	UBRR0 = MYUBRR;
	
	//enables the transmitter and receiver
	
	UCSR0B |= (1 << TXEN0 | 1 << RXEN0);
	
	//desired communication settings
	
	UCSR0C = (3 << UCSZ00);

}

void serial_txchar(char ch)
{
    while ((UCSR0A & (1<<UDRE0)) == 0);
	
    UDR0 = ch;
}

void serial_stringout(char *s)
{

    // Call serial_txchar in loop to send a string
    // (Very similar to lcd_stringout in lcd.c)
	int i;
	for(i = 0; i < 16; i++){
		
		serial_txchar(s[i]);
		
	}

}

ISR(USART_RX_vect)
{

    // character receiver in this function 
	char ch;

	ch = UDR0;                  // retrieves charatcers 

	// Store in buffer
	buf[index_ch] = ch;
	index_ch++;
	
	
	// sets flag when they are finshed 
	if(index_ch == 16){
		index_ch = 0; 	//resets for the next msg
		
		rec_data = 1;	//sets the flag
	}

}
