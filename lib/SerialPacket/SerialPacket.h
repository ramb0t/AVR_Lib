/*
 * SerialPacket.h
 *
 *  Created on: 08 Jul 2015
 *      Author: rambo
 */

// A simple protocol for serial comms!
/*
 * Host -->  [V02?]      // Request voltage #2
 * AVR  -->  [V02=2.34]  // Reply with voltage #2
 * Host -->  [V06=3.12]  // Set voltage #6
 * AVR  -->  [V06=3.15]  // Reply with voltage #6
 * From http://stackoverflow.com/questions/310826/protocols-used-to-talk-between-an-embedded-cpu-and-a-pc
 */


#ifndef SERIALPACKET_H_
#define SERIALPACKET_H_

// Standard AVR includes
/*********************************************/
#include <string.h>					// strcat etc
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>        			// itoa etc

#include "SerialPacketDefines.h"
#include "GFX_LCD.h"
#include "../lib/uart/uart.h"

// Constant Defines
#define		IDLE_STATE			0x01
#define 	READFIELD_STATE		0x02
#define		WAITCLOSE_STATE		0x04
#define 	ERROR_STATE			0x08
#define		READDATA_STATE		0x10
#define 	REPLY_STATE			0x20

#define		MAX_FIELD_LEN	3
#define 	MAX_DATA_LEN	16

void SerialPacket_init();
void SerialPacket_uart_ReadData();
void SerialPacket_spi_ReadData();
void SerialPacket_stateMachine(char input);
void SerialPacket_set_data();


#endif /* SERIALPACKET_H_ */
