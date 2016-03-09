/*
 * ADC.c
 *
 *  Created on: 25 Apr 2015
 *      Author: rambo
 */

#include "ADC.h"

uint16_t ADC_start(uint8_t pin){
	ADMUX = 0x40; // clear ADMUX first!!, also set the refrence to AVCC with 100nF cap @ AREF, best?
	ADMUX |= pin; // connect the mux to the required pin.
	// Turn ADC on, Start Conversion,  16mhz clk/128 scale = 125khz ADC clock, what datasheet suggests
	SETBITMASK(ADCSRA, (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));

	// ok ADC started, now wait for result
	while(!CHECKBIT(ADCSRA, ADIF)){
		  // wait for bit to go 1
	}
	SETBIT(ADCSRA, ADIF); // clear flag by writing 1
	// turn ADC off
	//ADCSRA = 0;
	CLEARBIT(ADCSRA,ADEN);

	return ADC; // send back the adc result!
}

int16_t ADC_getTemp(void){
	ADMUX = 0; // clear ADMUX first!!
	// Set ADC to use 1.1v reference
	ADMUX |= (1<<REFS1)|(1<<REFS0);
	// Set ADC mux to read the temp sensor
	ADMUX |= ADC_TEMP;
	// Turn ADC on, Start Conversion,  clk/128 scale = 125khz ADC clock
	SETBITMASK(ADCSRA, (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));

	// ok ADC started, now wait for result
	while(!CHECKBIT(ADCSRA, ADIF)){
		  // wait for bit to go 1
		}
		SETBIT(ADCSRA, ADIF); // clear flag by writing 1
	// turn ADC off?
	//ADCSRA = 0;
	CLEARBIT(ADCSRA,ADEN);

	int16_t tempTemp; //xD Best name!

	// now do some maths
	// NB: ~ 1mV / 'C and accuracy = +-10'C
	// 25'C ~= 314mV
	// Thus 0'C ~= 289mV
	// 1.1V ref and 10bit adc therefore
	// 1100mV -> 1024 bits -> can approx 1mV / bit?
	tempTemp = ADC ; // Read ADC and convert to 0'C ref


	return tempTemp; // send back the Temp result!

}


