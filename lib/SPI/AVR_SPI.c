/*
 * AVR_SPI.c
 *
 *  Created on: 21 Feb 2015
 *      Author: RobThePyro
 */

#include "AVR_SPI.h"

/*************************************************************************
Function: SPI_Init()
Purpose:  Sets up the SPI Interface on AVR
Input:    None
Returns:  None
**************************************************************************/
void SPI_Init(void){

	// SCK, SS!!, MOSI as outputs
	SPIDDR |= (1<<SPISCK)|(1<<SPISS)|(1<<SPIMOSI);
	// MISO as input
	SPIDDR &= ~(1<<SPIMISO);	// cbi(SPIDDR,SPIMISO);

	SPCR |= (1 << MSTR); /* clockmaster */
	SPCR |= (1 << SPE); /* enable */
	SPSR = ( 1 <<SPI2X ) ; /* fosc = fclk / 2 */

	// INIT interface, Master, set clock rate fck/128 TODO: check prescaler
	// SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);

}

// Deprecated
uint8_t spi_putc ( uint8_t data )
{
	// The byte to send
	SPDR = data;

	// Wait until the byte is sent!
	while ( ! ( SPSR & ( 1 <<SPIF ) ) );

	// Return the register with rx loop data back
	return SPDR;
}

/*************************************************************************
Function: SPI_WriteRead()
Purpose:  Write a byte to the SPI bus and return Read byte simultaneously
Input:    Byte to Write
Returns:  Read Byte
**************************************************************************/
uint8_t SPI_ReadWrite(uint8_t WriteByte){
	// Put the Write byte in the SPI SPDR
	SPDR = WriteByte;

	// Wait until the byte is sent!
	while ( ! ( SPSR & ( 1 <<SPIF ) ) );

	// Return SPI read byte
	return SPDR;
}

/*************************************************************************
Function: SPI_Read()
Purpose:  Write a null byte to the SPI bus and return Read byte simultaneously
Input:    none
Returns:  Read Byte
**************************************************************************/
uint8_t SPI_Read(){
	// write SPIDONTCARE and return result
	return SPI_ReadWrite(SPIDONTCARE);
}

/*************************************************************************
Function: SPI_Write()
Purpose:  Write a byte to the SPI bus and discard Read byte
Input:    Byte to Write
Returns:  node
**************************************************************************/
void SPI_Write(uint8_t WriteByte){
	// Write the byte and discard the result
	SPI_ReadWrite(WriteByte);
}

