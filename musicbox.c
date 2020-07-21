/********************************************
 *
 *  Name:Nelson Gonzalez
 *  Email:nelsongo@usc.edu
 *  Section:WED 5pm
 *  Assignment: Project - Music Box
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include "lcd.h"
#include "adc.h"

#define NUM_NOTES 21
#define NUM_TONES 26

/*
  The note_freq array contains the frequencies of the notes from C3 to C5 in
  array locations 1 to 25.  Location 0 is a zero for rest(?) note.  Used
  to calculate the timer delays.
*/

volatile unsigned char newst, oldst;// state machine variables 
volatile unsigned char changed = 0;  // Flag for state change
volatile unsigned char a, b;//Holds values pinc 
volatile unsigned char cvalue;//
volatile unsigned char pinc;
volatile unsigned char x;
volatile int count = 0;
volatile unsigned int couttt;
signed char posx = 0; 
signed char second_posx = 0; 
unsigned char page_num = 1; 
volatile int when_should_we_play ;

void timer1_initializations();
void timer2_initializations();
char *get_letter(char);
unsigned char* wheres_the_note_number(char);
void does_it_have_sharp_part(char,signed char);
void what_page_number(unsigned char);
void times_the_output_note(int);
void turn_on_LED();
void turn_off_the_LED();
void take_care_of_the_brightness(int,int);
void play_note(unsigned int);
void output_music_to_people();

unsigned int note_freq[NUM_TONES] =
{ 0,   131, 139, 147, 156, 165, 176, 185, 196, 208, 220, 233, 247,
  262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523};


/* Some sample tunes for testing */
/*
  The "notes" array is used to store the music that will be played. Each value
  is an index into the tone value in the note_freq array.
*/

// G, A, F, F(octive lower), C     Close Encounters
//unsigned char notes[NUM_NOTES] = {20, 22, 18, 6, 13};

// D D B C A B G F# E D A C A B    Fight On
unsigned char notes[NUM_NOTES] = {15, 15, 12, 13, 10, 12, 8, 7, 5, 3, 10, 13, 10, 12};

// E E F G G F E D C C D E E D D   Ode to Joy
//unsigned char notes[NUM_NOTES] = {17, 17, 18, 20, 20, 18, 17, 15, 13, 13, 15, 17, 17, 15, 15};


int main(void)
{
    // Initialize various modules and functions
	//eeprom_read_block(notes, (void *) 0, 25);
	DDRB |= (1<<PB4) | (1<<PB3); 
	//DDRC &= ~((1<<PC1) | (1<<PC5));
	PORTB |= (1<<PB3) | (1<<PB4); // Set bits 3 and 4 
	PORTC |= (1 << PC1) | (1<< PC5);//Set bits 1 and 5 
	
	PCICR |= (1 << PCIE1); 
    PCMSK1 |= (1 << PCINT9) | (1 << PCINT13);// For interrupts 1 and 5
    sei();//enale global interrupts 
	lcd_init(); 
	adc_init(); 
	timer1_initializations();
	timer2_initializations();
    // Show splash screen
	lcd_writecommand(1); //clear whatever is on the screen	
	/* lcd_stringout to print out name and splash*/
	lcd_moveto(0, 0);
	
	lcd_stringout("Nelson Gonzalez");	//print out name
	/* lcd moveto for moving to correct locations*/
	
	lcd_moveto(1,3);	//move to the second row close to middle 
	//Shows the lab musicbox 
	lcd_stringout("Music box");
	/* Delay 1 second */
	_delay_ms(1000);
	lcd_writecommand(1);
	unsigned int adc_result = adc_sample(0);
	if(adc_result>=0 && adc_result<160){
		//		eeprom_update_block(notes, (void *) 270, 50);
    }else{
		eeprom_read_block(notes,(void *) 10, 25);
	}
	cvalue = PINC;
	a = cvalue&(1<<PC1); //a = pinc bit 1
	b = cvalue&(1<<PC5); //b = pinc bit 5
    if (!b && !a)
	oldst = 0;
    else if (!b && a)
	oldst = 1;
    else if (b && !a)
	oldst = 2;
    else
	oldst = 3;
    newst = oldst;
	
	//Prints out first page for initial values
	if(page_num == 1){
		lcd_moveto(1,0);
		
		lcd_stringout("1");
		
		lcd_moveto(0,15);
		
		lcd_stringout(">");
		
	}
	
    unsigned char *notenumber1;// Used to get the value of the octave
	char *gettingletter;// gets the letter
    int j;
    char i=0;
	/*
	Here i use a for loop to get all the values of the array and put them into 
	seperate variables to display onto the lcd
	*/
	for(j = 0; j<15; j = j + 1){
		if(j%2 !=0)//To move to only odd positions 1,3,5...
		{
		    lcd_moveto(1,j);//moves to second row and on odd posistions
			notenumber1 = wheres_the_note_number(notes[i]);// gets the value of the octave by index 
			lcd_stringout(notenumber1);//display on lcd 
			lcd_moveto(0,j);// move to the first row in odd posistions
			gettingletter = get_letter(notes[i]);// gets the letter that comes with it 
			lcd_stringout(gettingletter);//display onto lcd
			does_it_have_sharp_part(notes[i],j);// determines if the letter has a sharp note part
			i++;//incremeent
	    }
	 }
	//Prints out first page for initial values
	while(1){
	/* Check if a button on the LCD was pressed */
    unsigned char adc_result = adc_sample(0); // holds value for buttons
	int loc = (page_num-1)*7 +(posx)/2;// Logic to get values within the array
    /*
	     For loop to siplay all the next notes onto the lcd panel
    */	
	for(i = 0; i<=15-1;i= i + 1){ 
		if(i%2!=0){
			loc = (page_num-1)*7 + i/2;// gets value of the note based on the page number and position
			lcd_moveto(1,i);//moveto second row 
			lcd_stringout(wheres_the_note_number(notes[loc]));//display on screen
			lcd_moveto(0,i);//move to first row
			lcd_stringout(get_letter(notes[loc]));//display letter on screen
			does_it_have_sharp_part(notes[loc],i);// determines if letter has a sharp part 
		}
	}
	if((adc_result >= 25 && adc_result < 75)||(adc_result >=0 && adc_result <5))
		{//up button/ right button  ------- moves right 
			//lcd_moveto(0,posx);//move a space to curr position  
			posx++; //increase position
			//second_posx++;
			if(posx>15)//Keeps within walls of lcd
			{
				posx = 0; //goes to beginning of lcd cursor 
				page_num++; //add pagenum
				what_page_number(page_num);//displays page numbers and arrows when moved
				
			}
			lcd_moveto(0,posx);//move x to a position in front of old
			_delay_ms(100); 
		} else if((adc_result >= 76 && adc_result < 150 )||(adc_result >=140 && adc_result<160))
		{//down button, left button -------- moves right 
			//lcd_moveto(0,posx);//Moves space to current position 
			posx--;//decrements for movement 
			//second_posx--;
			if(posx<0)//Keeps withing left lcd wall
			{
				posx = 15; // goes to the end of the lcd panel 
				page_num--;//decrement page number 
				what_page_number(page_num);// determines what pagenumber to display what number
			} 
			
			lcd_moveto(0,posx);//moves x to position ahead
			_delay_ms(100); 
		}
		
		
	/* If rotary encoder was rotated */
     //if the rotary encoder has moved
		if(changed)
		{
			//reset flag
			changed = 0;
			count = count%26;// gets an option for each of the avaible notes... correctly 
			if(posx%2 != 0){
				    loc = (page_num-1)*7 +(posx)/2;// determines what position the note is in the array
					notes[loc] = count; // changes the value held in array with new value 
				    lcd_moveto(1,posx);// go to current position 
			        lcd_stringout(wheres_the_note_number(count));// display new note ocatve onto the screen 
			        lcd_moveto(0,posx);//move to first row 
			        lcd_stringout(get_letter(count));// displays letternote  on lcd 
					does_it_have_sharp_part(count,posx);//determines if it has a sharp part and displays it 
			}
		} 
		if(adc_result >= 175 && adc_result < 220)
		{//if seect button is pressed 
	       
		    output_music_to_people(); 
			eeprom_update_block(notes,(void *) 10,25);// updates new notes 
			TCCR1B &= ~(1<<CS10);
	    }
   }
   
    while (1){                 // Loop forever
        
	}
}

/* ------------------------------------------------------------------ */

/*
  Code for showing notes on the screen and playing the notes.
*/





/* ------------------------------------------------------------------ */

/*
  Code for initializing TIMER1 and its ISR
*/

ISR(PCINT1_vect){
  // Read the input bits and determine A and B
  pinc = PINC;
  a = pinc & (1 << PC1);// setting current value 
  
  b = pinc & (1 << PC5);// setting current value 

	//See the two input bits to determine state and then adjust the counter.
	if (oldst == 0) 
	{
	    // Handle A and B inputs for state 0
      if (a) {
        newst = 1;
        count++;// keep count 
		
      } else if (b) {
        newst = 2;
        count--;// kee p count 
      }

	} else if (oldst == 1) {
	    // Handle A and B inputs for state 1
      if (b) {
		  
        newst = 3;
		
        count++;
      } else if (!a) {
          
        newst = 0;
		
        count--;// keep count 
      }

	} else if (oldst == 2) {
	    // Handle A and B inputs for state 2
      if (!b) 
	  {
        newst = 0;
        count++;// keep count 
      } else if (a) {
        newst = 3;
        count--;// keep count 
      }

	} else {   // oldst = 3
	    // Handle A and B inputs for state 3
      if (!a) {  
        newst = 2;// keep count 
        count++;
      }  else if (!b) {
        newst = 1;
        count--;// keep count 
      }
	}
    if(count<0){
		count=0;// min value for encoder
	}
	// If state changed change to new state 
	
	if (newst != oldst) 
	{
	    changed = 1;
	    oldst = newst;
	}
	// Set a flag to indicate a change 
}
ISR(TIMER1_COMPA_vect)
{
	couttt = couttt-1;
	//Main function of this function is to flip bits
	
	if(PORTB != (PORTB |= (1<<PB4))){
		
		PORTB |= (1<<PB4);
		
	}
	else{
		
		 PORTB &= ~(1<<PB4);	// turn off portb
		 
	} 
}

/* ------------------------------------------------------------------ */



ISR(TIMER2_COMPA_vect){}

void timer1_initializations()
{
	TIMSK1 |= (1 << OCIE1A);//interupt for 
	TCCR1B |= (1 << WGM12); //take off compare mode 
}
/*
  Code for initializing TIMER2
*/
void timer2_initializations(){
	DDRB |= (1<<3); //flip bits
	
	OCR2A = 0; // sets initial value to 0 
	
	TCCR2B &= ~((1<<CS22) | (1<<CS21) | (1<<CS20));//waits for timer 
	
	TCCR2A |= ((1 << WGM20) | (1 << WGM21));//begins the pulse width modulation mode
	
	TCCR2A |= (1<<COM2A1);//clear 
	
	TCCR2A &= ~(1<<COM2A0); 
}

/*
Create function to determine the letter that should be displayed onto the lcd 
by the index within the array.

*/
char *get_letter(char num){
	//I KNOW THAT I COULDVE USED OR STATEMENTS TO SHORTEN THIS DOWN
	//BUT IT HELPS ME VISUALIZE THE TASKS BETTER
	if(num == 0){
		return " ";
	}
	if(num == 1){
		return "C ";
	}
	if(num == 2){
		return "C";
	}
	if(num == 3){
		return "D ";
	}
	if(num == 4){
		return "D";
	}
	if(num == 5){
		return "E ";
	}
	if(num == 6){
		return "F ";
	}
	if(num == 7){
		return "F";
	}
	if(num == 8){
		return "G ";
	}
	if(num == 9){
		return "G";
	}
	if(num == 10){
		return "A ";
	}
	if(num == 11){
		return "A";
	}		
	if(num == 12){
		return "B ";
	}
	if(num == 13){
		return "C ";
	}
	if(num == 14){
		return "C";
	}		
	if(num == 15){
		return "D ";
	}
	if(num == 16){
		return "D";
	}
	if(num == 17){
		return "E ";
	}
	if(num == 18){
		return "F ";
	}
	if(num ==19){
		return "F";
	}	
	if(num == 20){
		return "G ";
	}
	if(num == 21){
		return "G";
	}
	if(num == 22){
		return "A ";
	}
	if(num == 23){
		return "A";
	}
	if(num == 24){
		return "B ";
	}
	if(num == 25){
		return "C ";
	}
}
/*
create a function to determine the octave of the note based on the index of the note in the array 
which is later used to ouput on lcd panel.
*/

unsigned char* wheres_the_note_number(char noote){
	//I KNOW THAT I COULDVE USED OR STATEMENTS TO SHORTEN THIS DOWN
	//BUT IT HELPS ME VISUALIZE THE TASKS BETTER
	if(noote == 0){
		return " ";
	}
	if(noote == 1){
		return "3";
	}
	if(noote == 2){
		return "3";
	}
	if(noote == 3){
		return "3";
	}
	if(noote == 4){
		return "3";
	}
	if(noote == 5){
		return "3";
	}
	if(noote == 6){
		return "3";
	}
	if(noote == 7){
		return "3";
	}
	if(noote == 8){
		return "3";
	}
	if(noote == 9){
		return "3";
	}
	if(noote == 10){
		return "3";
	}
	if(noote == 11){
		return "3";
	}
	if(noote == 12){
		return "3";
	}
	if(noote == 13){
		return "4";
	}
	if(noote == 14){
		return "4";
	}
	if(noote == 15){
		return "4";
	}
	if(noote == 16){
		return "4";
	}
	if(noote == 17){
		return "4";
	}
	if(noote == 18){
		return "4";
	}
	if(noote == 19){
		return "4";
	}
	if(noote == 20){
		return "4";
	}
	if(noote == 21){
		return "4";
	}
	if(noote == 22){
		return "4";
	}
	if(noote == 23){
		return "4";
	}
	if(noote == 24){
		return "4";
	}
	if(noote == 25){
		return "5";
	}
}
/*
Created a function to determine whether the letter had a sharp part to its musical note.
If so , then, I use the position to display the hashtag in front of it. I change values of 
array if needed in main().
*/
void does_it_have_sharp_part(char num,signed char posx){
	//AGAIN, I KNOW THAT THIS COULD BE SHORTENED WITH ORS BUT IT HELPS ME VISUALIZE
    //MY TASKS A LOT BETTER 	
	
	if(num == 2){
		lcd_moveto(0,posx+1);
		lcd_stringout("#");
	}
	
	else if(num == 4){
		lcd_moveto(0,posx+1);
		lcd_stringout("#");
	}
	
	else if(num == 7){
		lcd_moveto(0,posx+1);
		lcd_stringout("#");
	}
	else if(num == 9){
		lcd_moveto(0,posx+1);
		lcd_stringout("#");
	}
	else if(num == 11){
		lcd_moveto(0,posx+1);
		lcd_stringout("#");
	}		
	else if(num == 14){
		lcd_moveto(0,posx+1);
		lcd_stringout("#");
	}		
	else if(num == 16){
		lcd_moveto(0,posx+1);
		lcd_stringout("#");
	}
	else if(num ==19){
		lcd_moveto(0,posx+1);
		lcd_stringout("#");
	}	
	else if(num == 21){
		lcd_moveto(0,posx+1);
		lcd_stringout("#");
	}
	else if(num == 23){
		lcd_moveto(0,posx+1);
		lcd_stringout("#");
	}
	else{
		lcd_moveto(0,posx+1);
		lcd_stringout(" ");
	}
	return; 
}
/*
	This function determines the page number of the lcd by the position of cursor and then 
	translates to the page number 
*/
void what_page_number(unsigned char pagenumber){
	/*
	     Based on the page number ther is a 1 2 3 with arrows or without, 
		 Lcd string out to get them 
	*/
	
	if(pagenumber == 1){
		lcd_moveto(1,0);//move to 
		lcd_stringout("1");//ouput
		lcd_moveto(0,15);
		lcd_stringout(">");//output
		lcd_moveto(0,0);
		lcd_stringout(" ");//ouput
       }
	if(pagenumber == 2){
	    lcd_moveto(1,0);
		lcd_stringout("2");
		lcd_moveto(0,15);
		lcd_stringout(">");
		lcd_moveto(0,0);
		lcd_stringout("<");
	}
	if(pagenumber == 3){
		lcd_moveto(1,0);
		lcd_stringout("3");
		lcd_moveto(0,0);
		lcd_stringout("<");
		lcd_moveto(0,3);
		lcd_stringout("               ");
	}
	return; 
}
void times_the_output_note(int note_freqq)
{
	OCR1A = 1000000/note_freqq; //16 bit modulus 
}
void turn_on_LED(){
	TCCR2B |= 1;// starts timer
}
void turn_off_the_LED(){
	PORTB &= ~(1<<3);//turns off 
	OCR2A = 0;//GOES TO ZERO
}
void take_care_of_the_brightness(int num_notes,int start_increment){//start_increment goesfrom 0,1,2...to noteend
	OCR2A = (start_increment * 255)/(num_notes);//max value is 255 so with each start0,1,2.. it should get brighter and brighter
}
void play_note(unsigned int note_freq)
{
	unsigned long note_period = 1000000/ note_freq;// frquency determines the period 
	unsigned int period_cycle_for_note =16*(note_period/2);// 16k divided by half period
	unsigned int interger_value= period_cycle_for_note; 
	OCR1A = interger_value;//
}

void output_music_to_people()
{
	turn_on_LED();// turn on intitial led
	int i;//initiialization
	for(i = 0; i <=NUM_NOTES-2; i++){
		take_care_of_the_brightness(NUM_NOTES,i);// deermines what needs to be lit up
		if(notes[i] ==0){//rest frequency 
			_delay_ms(500); //delays full note
		}
		else{
			couttt = 50000/(note_freq[notes[i]]/2);// volatile value foor note duration
			TCCR1B |= (1<<CS10);
			OCR2A = OCR2A-255;
			while(couttt>0){//timing 
			    
				play_note(note_freq[notes[i]]);//
			}
			TCCR1B &= ~(1<<CS10);
            			
		}
		TCCR1B &= ~(1<<CS10);
		turn_off_the_LED();//turn off led
		_delay_ms(50);
	}
	turn_off_the_LED();//turn off led
}
/*
void output_notes(){
	turn_on_LED();
	int w = 0; 
	w = sizeof(notes);
	//while(notes[w]!='\0'){// determines the size of array
	//	w++;
	//}
	int q; 
	for(q = 0; q < w;q++){
		int frequency_of_note = note_freq[notes[q]];// determines frequency of note in array
		take_care_of_the_brightness(w,q);// decides brightness of the led
		 //controls select button
		if(notes[q] == 0){//if its a rest then delay for that time
			_delay_ms(500);// delay time for rest
		}
		else{
			TCNT1 = 0;
			times_the_output_note(frequency_of_note);// timer 
			TCCR1B |=(1 << CS11);
			when_should_we_play = 1;//select button part
		}
		while(when_should_we_play){
		}
		PORTB &= ~(1<<4);// zeros portb bit 
	}
	turn_off_the_LED();	// turns off led for snog finishing 
}
*/









