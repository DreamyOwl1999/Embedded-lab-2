/* ------------------------------------------
       ECS642/ECS714 Lab2

   Simulation of a button using random delays
   Toggle blue on every simulated press
  -------------------------------------------- */

#include <MKL25Z4.h>
#include <stdbool.h>
#include "SysTick.h"
#include "triColorLed.h"

/* --------------------------------------
     Documentation
     =============
     This is a cyclic system with a cycle time of 10ms

     The code has two tasks
     1. randomPress: this simulates presses at random intervals between 3 and 13 sec
     2. toggleBlue: this toggles the blue LED on every press

     In addition
     * External functions: 
       - configureLEDs  - sets up the peripherals so the LEDs can be used
       - Init_SysTick(), waitSysTickCounter() - time the cyclic system
       - setRedLED, setGreenLED, setBlueLED - control the LEDs
     * Function to generate random delays 

 -------------------------------------- */
 
 /*----------------------------------------------------------------------------
  * nextRand: get next random number 
  *   Based on https://en.wikipedia.org/wiki/Linear_congruential_generator
  * --------------------------------------------------------------------------- */
uint32_t seed = 0x12345678 ;

// Returns a 32 bit number which is too long for us
uint32_t nextRand(void) {
  seed = (1103515245 * seed + 12345) ; 
  return seed ; 
}

// Generate random count in range 3-13 sec 
//    - take top 10 bits - max is 1023
//    - add 277 so max is 1300 (13 sec), min is 277  (2.8 sec)
uint32_t randCount() {
  uint32_t r1023 = (nextRand() & 0xFFC00000) >> 22 ; // top 10 bits
  return r1023 + 277 ;
}

/*----------------------------------------------------------------------------
   Task: randomPressTask

   Generate a signal at random intervals between 5 and 15 sec
   DO NOT CHANGE
 *----------------------------------------------------------------------------*/
#define UP 0
#define DOWN 1
#define DOWNCOUNT 20

int stateRPress ;       // state of the task
uint32_t countRPress ;  // count for timing

// initialise state
void initRandomPressTask() {
  countRPress =randCount() ;
  stateRPress = UP ;
}

bool signalR = false ; // signal to other task

void randomPressTask() {
    if (countRPress > 0) countRPress-- ;
    switch (stateRPress) {
        case UP:
            if (countRPress == 0){
                signalR = true ;
                stateRPress = DOWN ;
                countRPress = DOWNCOUNT ;
            }
            break ;
        case DOWN:
            if (countRPress == 0){
                stateRPress = UP ;
                countRPress = randCount();
            }
            break ;
    }
}

/*----------------------------------------------------------------------------
   Task: toggleBlue

   Toggle blue LED on every signal

   REPLACE THIS TASK
*----------------------------------------------------------------------------*/

#define BLUEONREDOFF 0
#define BLUEOFFREDON 1
#define BOTHOFF_BLUEWASON 2	
#define BOTHOFF_REDWASON 3
#define PERIOD 300

int State ;  // this variable holds the current state

// initial state of task
void initToggleTask() {
  blueState = BLUEON ;
  setBlueLED(ON) ;
}

int counter = PERIOD

void toggleTask() {
	if (counter > 0) counter --;
	switch (State){
		//......................................
	
		case BLUEONREDOFF:
			if (counter == 0)
			{
				setBlueLED(OFF); setRedLED(ON);
				state = BLUEOFFREDON;
			}
      if (signalR) {           // signal received
          signalR = false ;    // acknowledge
          setBlueLED(OFF) ;     // turn off
					State = BOTHOFF_BLUEWASON  // ... the new state
      }
			break;
		//..........................................	
			
		case BLUEOFFREDON:
			if (counter == 0)
			{
				setRedLED(OFF); setBLUELED(ON);
				state = BLUEONREDOFF;
			}
      if (signalR) {           // signal received
          signalR = false ;    // acknowledge
          setBlueLED(OFF) ;     // turn off
					State = BOTHOFF_REDWASON  // ... the new state
      }
			break;
		//.............................................
			
		case BOTHOFF_BLUEWASON:
      if (signalR) {           // signal received
          signalR = false ;    // acknowledge
          setBlueLED(ON) ;     // turn ON
					State = BLUEONREDOFF // ... the new state
      }
			break;
		//...............................

		case BOTHOFF_REDWASON:
      if (signalR) {           // signal received
          signalR = false ;    // acknowledge
          setREDLED(ON) ;     // turn ON
					State = REDONBLUEOFF // ... the new state
      }
			break;
	}		
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
    configureLEDs() ;     // configure the GPIO outputs for the LED
    Init_SysTick(1000) ;  // initialse SysTick every 1 ms
    initRandomPressTask() ;  // initialise task state
    initToggleBlueTask() ;   // initialise task state
    waitSysTickCounter(10) ;  // initialise delay counter
    while (1) {      // this runs for ever
      randomPressTask() ;  // Generate signals for a simulated button
      toggleBlueTask();    // Toggle blue LED on every press signal
      // delay
      waitSysTickCounter(10) ;  // cycle every 10 ms 
    }
}
