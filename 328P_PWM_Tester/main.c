/*
 * main.c
 *
 *  Created on: 11 Sep 2015
 *      Author: RobThePyro
 */


// Includes
/*********************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// ADC - Module for reading from the ADC, used to get throttle pedal
#include "ADC.h"

// Timer1 - Interface for setting the PWM output for the pedal
#include "Timer1.h"

// Pin Defines - define what and how things are wired up the the mcu
#include "pinDefines.h"


// Static Functions
/*********************************************/

// Sets up the IO...
static void	IOInit(void){

	/* Speed control setup */
	// PWM as output
	PWM1_DDR |= (1<<PWM1_A);
	// PWM on
	PWM1_PORT |= (1<<PWM1_A);

	// ADC as inputs
	ALOG_DDR &= ~(1<<ALOG_0);  //input
	ALOG_PORT &= ~(1<<ALOG_0); // pullup off since adc!

	Timer1_init();
	PWM1A_REG = 128;

}

// Main Function
/*********************************************/
int main(void)
{
	IOInit();
	// Enable Interrupts
	sei();

	uint8_t A0Val;

	while(1){
		A0Val = ADC_start(ALOG_0MUX)>>2; // get the value
		PWM1A_REG = A0Val;
	}

}
