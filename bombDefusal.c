#include <util/delay.h>
#include "os.h"
#include <stdlib.h>



#define LED_INIT    DDRB  |=  _BV(PINB7)
#define LED_ON      PORTB |=  _BV(PINB7)
#define LED_OFF     PORTB &= ~_BV(PINB7)
#define LED_TOGGLE  PINB  |=  _BV(PINB7)

int detectWires(void);
void activateBomb(void);

void main(void) {
  init_lcd();

  // Toggles the led and turns it on
  LED_TOGGLE;
  LED_ON;
  LED_INIT;



  // Changes the direction of all the pins on port D to inputs
  DDRD = 0xFF;
  // Changes all the pins values on PORT D to 0
  PORTD = 0x00;
  // Changes all the pins values on PORT F to 1
  PORTF = 0xFF;

  
  activateBomb();

  // TODO add a reset button to restart the game at the end
  while(1) {
  }

}

void activateBomb(){
  int i;
  char snum[5];

  // Due to the clock speed of the atmel chip I had to adjust the speed of the time to give the illusion of a second
  int time2 = 80;
  int bombActive = 1;

  display_string("Bomb Activated!\n");
  
  // Loops to immitate 60 seconds
  for(i = 60; i >= 1; i--) {
	// The start of the loop contains a delay to immitate the timer.
	// TODO unfortunately the delay.h library works by freezing up all threads, instead try to adapt the code to make it work with schedulers
    _delay_ms(time2);

    // convert 123 to string [buf]
    itoa(i, snum, 10);

	// Displays the countr on screen
	// TODO make the countdown timer font size larger as opposed to one on top of another
    display_string(snum);
    display_string("\n");

	// detect wires is called before each next second to check if any wires have been cut or pulled
    bombActive = detectWires();
	
    if(bombActive == 0) {
      break;
    } else if(bombActive == 2) {
	// This speed up the timer countdown by 2 times the speed
      time2 = time2 / 2;
    }

    if(i == 30) {
	// At the halfway mark the countdown timer is sped up 2 times
      time2 = 40;
    } else if (i == 1) {

	// At the final second the screen is cleared and the user is display with a "You died." text
      clear_screen();
      display_string("You died.");
      LED_OFF;

    }
}



}

int detectWires() {
  //Displays the values in the PIND register
  //display_register(PIND);
  
  
  // The following if statements check the values of the connected pins in PORTF
  // The values if connected to any pins on PORTD should be zero due to the ground effect
  // When a wire is cut or pulled the value to switch to 1 which is what triggers the corresponding if statements
  if (PINF & _BV(PF0)) {
    clear_screen();
    display_string("You died.");
    return 0;
  } else if (PINF  & _BV(PF3)) {
    //display_string("Postive!\n");
    display_string("Bomb Defused\n");

    return 0;
  } else if (PINF & _BV(PF2)) {
	// PIN 2 in PORTF triggers the timer to speed up when cut or pulled  
    // flips the direction of the pin to output to prevent it getting checked twice
    DDRF  |=  _BV(PF2);
    PORTF &= ~_BV(PF2);
    display_string("WRONG!!!\n");
    return 2;
  } else if (PINF & _BV(PF1)) {
    return 1;
  } else {
    //display_string("Grounded!\n");
    return 1;

  }
}