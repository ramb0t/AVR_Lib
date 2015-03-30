/*
 * CAN.c
 *
 *  Created on: 30 Mar 2015
 *      Author: RobThePyro
 *
 *      CAN application layer
 */

#include "CAN.h"
#include "mcp2515.h"

/*************************************************************************
Function: CAN_Init()
Purpose:  Init the CAN interface
Input:    Bit time speed setup
Returns:  Ok / Fail
**************************************************************************/
uint8_t CAN_Init(uint8_t speedset)
{
	uint8_t res;

	res = mcp2515_init(speedset);

	if (res == MCP2515_OK) return CAN_OK;
	else return CAN_FAILINIT;
}


/*************************************************************************
Function: CAN_sendMessage()
Purpose:  Takes a CAN message and sends it out :)
Input:    CANMessage Struct
Returns:  Ok / Fail
**************************************************************************/
uint8_t CAN_sendMessage(const CANMessage* message)
{
	uint8_t length = message->length;

	// ID set
	mcp2515_setRegister ( MCP_TXB0SIDH, ( uint8_t ) ( message->id>> 3 ) ) ;
	mcp2515_setRegister ( MCP_TXB0SIDL, ( uint8_t ) ( message->id<< 5 ) ) ;

	// If the message is a "Remote Transmit Request"
	if ( message->rtr )
	{
	/* Although A RTR message has a length, but no data */

	// Set message length + RTR
	mcp2515_setRegister( MCP_TXB0DLC, ( 1 <<RTR ) | length ) ;
	}
	else
	{
		// Set message length
	mcp2515_setRegister( MCP_TXB0DLC, length ) ;

	// Data
	for ( uint8_t i= 0 ;i<length;i++ ) {
	mcp2515_setRegister( MCP_TXB0D0 + i, message->data [ i ] ) ;
	}
	}

	// Send CAN message
	#define	SPI_RTS			0x80
	MCP2515_SELECT();
	SPI_ReadWrite( SPI_RTS | 0x01 ); // Sends the message!
	MCP2515_UNSELECT();

	return CAN_OK;

	//TODO: Implement this fancy stuff.. for now we use the germans!
//	uint8_t res, txbuf_n;
//	uint8_t timeout = 0;
//	uint16_t time;
//	time = timebase_actTime();
//
//	do {
//		res = mcp2515_getNextFreeTXBuf(&txbuf_n); // info = addr.
//		if (timebase_passedTimeMS(time) > CANSENDTIMEOUT ) timeout = 1;
//	} while (res == MCP_ALLTXBUSY && !timeout);
//
//	if (!timeout) {
//		mcp2515_write_canMsg( txbuf_n, msg);
//		mcp2515_start_transmit( txbuf_n );
//		return CAN_OK;
//	}
//	else {
//#if (CANDEBUG)
//		term_puts_P("Transmit timeout\n");
//#endif
//		return CAN_FAILTX;
//	}
}

/*************************************************************************
Function: CAN_readMessage()
Purpose:  Takes a CAN message pointer and stores the data into it
Input:    CANMessage Struct
Returns:  CAN_OK / CAN_NOMSG
**************************************************************************/
uint8_t CAN_readMessage(CANMessage *msg)
{
	uint8_t stat, res;

	stat = mcp2515_readStatus();

	if ( stat & MCP_STAT_RX0IF ) {
		// Msg in Buffer 0
		mcp2515_read_canMsg( MCP_RXBUF_0, msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
		res = CAN_OK;
	}
	else if ( stat & MCP_STAT_RX1IF ) {
		// Msg in Buffer 1
		mcp2515_read_canMsg( MCP_RXBUF_1, msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
		res = CAN_OK;
	}
	else {
		res = CAN_NOMSG;
	}

	return res;
}


/*************************************************************************
Function: CAN_checkReceiveAvaliable()
Purpose:  Checks if there is any CAN message sitting in a buffer?
Input:    None
Returns:  either
 #define CAN_MSGAVAIL   (3) - a message has been received
 #define CAN_NOMSG      (4) - no new message
**************************************************************************/
uint8_t CAN_checkReceiveAvaliable(void)
{
	uint8_t res;

	res = mcp2515_readStatus(); // RXnIF in Bit 1 and 0
	if ( res & MCP_STAT_RXIF_MASK ) {
		return CAN_MSGAVAIL;
	}
	else {
		return CAN_NOMSG;
	}
}

/*************************************************************************
Function: CAN_checkError
Purpose:  Checks Controller-Error-State (Only errors, not warnings.)
Input:    None
Returns:  CAN_OK or CAN_CTRLERROR
**************************************************************************/
uint8_t CAN_checkError(void)
{
	uint8_t eflg = mcp2515_readRegister(MCP_EFLG);

	if ( eflg & MCP_EFLG_ERRORMASK ) {
		return CAN_CTRLERROR;
	}
	else {
		return CAN_OK;
	}
}
