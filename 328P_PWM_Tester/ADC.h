/*
 * ADC.h
 *
 *  Created on: 25 Apr 2015
 *      Author: rambo
 */

#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include "../lib/macros.h"

// ADC input defines
#define	ADC_0		0x00					  // ADC0 - PC0
//#define ADC_VBAT	0x07					  // ADC7
//#define ADC_SRVOFF	0		   			  // ADC0
#define ADC_TEMP	0x08					  // ADC8 - Temp Sens


uint16_t ADC_start(uint8_t pin);
int16_t ADC_getTemp(void); // signed because maybe -ve temp?


#endif /* ADC_H_ */
