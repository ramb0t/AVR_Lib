/*
 * AVR_SPI.h
 *
 *  Created on: 21 Feb 2015
 *      Author: RobThePyro
 */

#ifndef AVR_SPI_H_
#define AVR_SPI_H_

// Includes:
/*********************************************/
#include <inttypes.h>
#include <avr/io.h>

// Defines:
/*********************************************/
#define SPIPORT PORTB
#define SPIPIN  PINB
#define SPIDDR  DDRB

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
#define SPISCK   PB5
#define SPIMISO  PB4
#define SPIMOSI  PB3
#define SPISS    PB2
#elif defined(__AVR_ATmega32__) || defined(__AVR_ATmega16__)
#define SPISCK   PB7
#define SPIMISO  PB6
#define SPIMOSI  PB5
#define SPISS    PB4
#else
//#error "SPI pins undefined for this target"
#endif

#define SPI_SS_HIGH() (SPIPORT |= (1<<SPISS))
#define SPI_SS_LOW() (SPIPORT &= ~(1<<SPISS))

#define SPIDONTCARE (0x00)

// Functions:
/*********************************************/

void SPI_Init(void);
uint8_t spi_putc(uint8_t);

uint8_t SPI_ReadWrite(uint8_t);
uint8_t SPI_Read();
void SPI_Write(uint8_t);


#endif /* AVR_SPI_H_ */
