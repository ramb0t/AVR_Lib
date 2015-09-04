/*
 * SerialPacket.c
 *
 *  Created on: 08 Jul 2015
 *      Author: rambo
 */

#include "SerialPacket.h"


//Variable Definitions
//*****************************
uint8_t		state;
// input char array
char field_string[MAX_FIELD_LEN+1];  // defines the maximum string length that we are willing at accept
char data_string[MAX_DATA_LEN+1];  // defines the maximum string length that we are willing at accept
// index pointer
uint8_t field_idx, data_idx;

void SerialPacket_init(){
	// reset state
	state = IDLE_STATE;

	// clear the field and data strings
	memset(field_string, 0, sizeof field_string);
	memset(data_string, 0, sizeof data_string);
	// reset the index pointers of the strings.
	field_idx = 0;
	data_idx = 0;
}

void SerialPacket_uart_ReadData(){
	SerialPacket_stateMachine(uart_getc());
}

void SerialPacket_spi_ReadData(){

}

void SerialPacket_stateMachine(char input){
	switch(state)
	{
		case IDLE_STATE:
			//if(strcasecmp(input, START_CHAR) == 0){
			if(input == START_CHAR){// Change state to ReadField
				state = READFIELD_STATE;
			}
			break;

		case READFIELD_STATE:
			// Make sure not control char
			if(input != GET_CHAR && input != SET_CHAR && input != START_CHAR && input != END_CHAR){
				// Make sure not too long
				if(field_idx < MAX_FIELD_LEN){
					field_string[field_idx] = input;
					field_idx++;
				}else{ //problem!!
					state = ERROR_STATE;
					// reset all!
					SerialPacket_init();
				}
			}else if(field_idx > 0){ // Make sure something exists!
				if(input == SET_CHAR){ // Set mode, record data
					state = READDATA_STATE;
				}else if (input == GET_CHAR){ // Get mode, send data from field
					state = REPLY_STATE;
				}else{ // unexpected control char!!
					state = ERROR_STATE;
					// reset
					SerialPacket_init();
				}
			}else { // string is zero length!!
				state = ERROR_STATE;
				// reset
				SerialPacket_init();
			}
			break;

		case READDATA_STATE:
			// Make sure not control char
			if(input != GET_CHAR && input != SET_CHAR && input != START_CHAR && input != END_CHAR){
				// Make sure not too long
				if(data_idx < MAX_DATA_LEN){
					data_string[data_idx] = input;
					data_idx++;
				}else{ //problem length too long!!
					state = ERROR_STATE;
					// reset all!
					SerialPacket_init();
				}
			}else if(data_idx > 0){ // Make sure something exists!
				if(input == END_CHAR){ // End of line!
					SerialPacket_set_data(); // prase the data to set..
					//reset machine
					SerialPacket_init();
				}else{ // unexpected control char!!
					state = ERROR_STATE;
					// reset
					SerialPacket_init();
				}
			}else { // string is zero length!!
				state = ERROR_STATE;
				// reset
				SerialPacket_init();
			}
			break;

		case REPLY_STATE:
			SerialPacket_init();
			break;

		case WAITCLOSE_STATE:
			SerialPacket_init();
			break;
		case ERROR_STATE:
			SerialPacket_init();
			break;

		default:
			SerialPacket_init();
			break;
	} // Switch

}// state machine

void SerialPacket_set_data(){

	if(strcasecmp(field_string, VAR1_STR) == 0){ // Var1
		var1 = atoi(data_string);
	}else if (strcasecmp(field_string, VAR2_STR) == 0){ // Var2
		var2 = atoi(data_string);
	}

}
