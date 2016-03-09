/*
 * Timer1.h
 *
 *  Created on: 04 Apr 2015
 *      Author: RobThePyro
 */

// For Atmega328p !!
#ifndef TIMER1_H_
#define TIMER1_H_


// Includes:
/*****************************************************************************/
#include <avr/io.h> /* Defines pins, ports, etc */
#include <util/delay.h> /* Functions to waste time */
#include "pinDefines.h"

// Defines:
/*****************************************************************************/

// Variables:
/*****************************************************************************/


// Function Prototypes:
/*****************************************************************************/
void Timer1_init(void);
void Timer1_PWM_Off(void);
void Timer1_PWM_On(void);


#endif /* TIMER1_H_ */
