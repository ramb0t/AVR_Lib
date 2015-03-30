/******************************************************************************
 * 
 * Controller Area Network (CAN) Demo-Application
 * Atmel AVR with Microchip MCP2515 
 * 
 * Copyright (C) 2005 Martin THOMAS, Kaiserslautern, Germany
 * <eversmith@heizung-thomas.de>
 * http://www.siwawi.arubi.uni-kl.de/avr_projects
 *
 *****************************************************************************
 *
 * File    : mcp2515.c
 * Version : 0.9
 * 
 * Summary : MCP2515 "low-level" driver
 *
 * Parts of this code are adapted from a MCP2510 sample-application 
 * by KVASER AB, http://www.kvaser.com (KVASER-code is marked as free)
 *
 * This code-module is free to use but you have to keep the copyright
 * notice.
 *
 *****************************************************************************/



//#include "delay.h"
#include "mcp2515.h"


#if (MCPDEBUG)
#include "termio.h"
#include "debughelper.h"
#endif

/*****************************************************************************
 * Low Level Functions
 *****************************************************************************/


/*************************************************************************
Function: mcp2515_reset()
Purpose:  Reset the MCP2515 IC
Input:    None
Returns:  None
**************************************************************************/
void mcp2515_reset(void)
{
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_RESET); // Resets the IC
	_delay_ms(1);
	MCP2515_UNSELECT();
	_delay_ms(10); // rough - but > 128 MCP clock-cycles
}

/*************************************************************************
Function: mcp2515_readRegister()
Purpose:  Reads the specified register address
Input:    Register address byte
Returns:  Register contents Byte
**************************************************************************/
uint8_t mcp2515_readRegister(const uint8_t address)
{
	// stores the result
	uint8_t ret;

	MCP2515_SELECT();			// Activate the MCP
	SPI_ReadWrite(MCP_READ);	// Tell the MCP we want to read a register by passing the READ instruction
	SPI_ReadWrite(address);		// Next send the address we want to read
	ret = SPI_Read();			// Finally get the result of the read action
	MCP2515_UNSELECT();			// Deactivate the MCP

	// return the result
	return ret;
}

/*************************************************************************
Function: mcp2515_setRegister()
Purpose:  Sets a register address in MCP2515 to a specified value
Input:    Register address byte, Data Byte
Returns:  None
**************************************************************************/
void mcp2515_setRegister(const uint8_t address, const uint8_t value)
{
	MCP2515_SELECT();			// Activate the IC
	SPI_ReadWrite(MCP_WRITE);	// Write Register Mode
	SPI_ReadWrite(address);		// Register address to write
	SPI_ReadWrite(value);		// Value to write to register
	MCP2515_UNSELECT();			// Deactivate the IC
}

/*************************************************************************
Function: mcp2515_modifyRegister()
Purpose:  Modifies bits using a mask and data byte. BEWARE, only works on certain registers
Input:    Register address byte, Bit Mask byte, Data Byte
Returns:  None
**************************************************************************/
void mcp2515_modifyRegister(const uint8_t address,
	const uint8_t mask, const uint8_t data)
{
	MCP2515_SELECT();			// Activate the IC
	SPI_ReadWrite(MCP_BITMOD);	// Send the Bit Modify Instruction
	SPI_ReadWrite(address);		// Send the address we wish to modify
	SPI_ReadWrite(mask);		// Send the mask of the bits to be modified
	SPI_ReadWrite(data);		// Send the modified bits
	MCP2515_UNSELECT();			// Deactivate the IC
}


/*****************************************************************************
 * Control Functions
 *****************************************************************************/

/*************************************************************************
Function: mcp2515_init()
Purpose:  Setup the MCP2515 IC
Input:    Can Speed Required
Returns:  Init Status OK / Fail
**************************************************************************/
uint8_t mcp2515_init(const uint8_t canSpeed)
{
	/*Procedure:
	 * Put MCP2515 into Reset / Config mode
	 * Setup Bit Timings
	 *
	 */


	// To store the result
	uint8_t res;

	// Chip Select High
	MCP2515_UNSELECT();  // Can take this out?
	//MCP_CS_DDR |= ( 1 << MCP_CS_BIT );

	/* Reset the IC first to put it in config mode
	 * The following registers can only be changed in config mode
	 * CNF1, CNF2, CNF3
	 * TXRTSCTRL
	 * Filter registers
	 * Mask registers
	 */
	mcp2515_reset();

	// Set the CAN Control register to Config mode. Should be the same thing as a reset?
	//TODO: Should maybe depricate this function? and use a more general one?
	res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);

	if ( res == MCP2515_FAIL ) return res;  /* function exit on error */



	// Set the MCP2515 to operate at the correct speed.
	res = mcp2515_configRate(canSpeed);

	if ( res == MCP2515_OK ) { // Only continue if no fail.
		//TODO: fix this hack to get things working at 4AM..
		//mcp2515_initCANBuffers();

		/*
		* Set the filter
		*/

		// Buffer 0: Receive all news
		mcp2515_setRegister (MCP_RXB0CTRL, (1 << RXM1) | (1 << RXM0));

		// Buffer 1: Receive all news
		mcp2515_setRegister (MCP_RXB1CTRL, (1 << RXM1) | (1 << RXM0));

		// All bits of the receiving mask delete,
		// So that all messages are received
		mcp2515_setRegister (MCP_RXM0SIDH, 0);
		mcp2515_setRegister (MCP_RXM0SIDL, 0);
		mcp2515_setRegister (MCP_RXM0EID8, 0);
		mcp2515_setRegister (MCP_RXM0EID0, 0);

		mcp2515_setRegister(MCP_RXM1SIDH, 0);
		mcp2515_setRegister (MCP_RXM1SIDL, 0);
		mcp2515_setRegister (MCP_RXM1EID8, 0);
		mcp2515_setRegister (MCP_RXM1EID0, 0);

		/*
		* Set the pin functions
		*/

		// Disable the pins RXnBF Pins (High Impedance State)
		mcp2515_setRegister (MCP_BFPCTRL, 0);

		// Turn TXnRTS bits as inputs
		mcp2515_setRegister (MCP_TXRTSCTRL, 0);

		// Device back to normal mode offset
		mcp2515_modifyRegister (MCP_CANCTRL, 0xE0, 0);

#if (DEBUG_RXANY==1)
		// enable both receive-buffers to receive any message
		// and enable rollover
		mcp2515_modifyRegister(MCP_RXB0CTRL,
			MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
			MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK);
		mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
			MCP_RXB_RX_ANY);
#else
		// enable both receive-buffers to receive messages
		// with std. and ext. identifiers
		// and enable rollover
		//mcp2515_modifyRegister(MCP_RXB0CTRL,
		//	MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
		//	MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK );
		//mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
		//	MCP_RXB_RX_STDEXT);
#endif
	}

	return res;
}

/*************************************************************************
Function: mcp2515_setCANCTRL_Mode()
Purpose:  Modifies the mode bits of the CANCTRL Register
Input:    newmode Byte
Returns:  MCP2515_OK / MCP2515_FAIL result
**************************************************************************/
uint8_t mcp2515_setCANCTRL_Mode(const uint8_t newmode)
{
	// Used for error Checking
	uint8_t i;

	// Modify the CANCTRL Register using the MODE mask and newmode Byte
	mcp2515_modifyRegister(MCP_CANCTRL, MODE_MASK, newmode);

	// verify as advised in datasheet
	i = mcp2515_readRegister(MCP_CANCTRL);		// Read the register into i
	i &= MODE_MASK;								// bitmask it against the MODE mask

	// check if the register matches the value written
	if ( i == newmode ) {
		return MCP2515_OK;
	}
	else {
		return MCP2515_FAIL;
	}
}

/*************************************************************************
Function: mcp2515_configRate()
Purpose:  Sets up the bit timings to match those needed.
Input:    canSpeed Byte
Returns:  MCP2515_OK / MCP2515_FAIL result
**************************************************************************/
uint8_t mcp2515_configRate(const uint8_t canSpeed)
{
	// Temp variables for the register data
	uint8_t set, cfg1, cfg2, cfg3;

	// Just flag..
	set = 0;

	// Bit Timings defined in mcp2515_bittime.h
	switch (canSpeed) {
		case (CAN_125KBPS_16MHZ) :
			cfg1 = MCP_16MHz_125kBPS_CFG1 ;
			cfg2 = MCP_16MHz_125kBPS_CFG2 ;
			cfg3 = MCP_16MHz_125kBPS_CFG3 ;
			set = 1;
			break;
		case (CAN_125KBPS) :
			cfg1 = MCP_4MHz_125kBPS_CFG1 ;
			cfg2 = MCP_4MHz_125kBPS_CFG2 ;
			cfg3 = MCP_4MHz_125kBPS_CFG3 ;
			set = 1;
			break;
		case (CAN_20KBPS) :
			cfg1 = MCP_4MHz_20kBPS_CFG1 ;
			cfg2 = MCP_4MHz_20kBPS_CFG2 ;
			cfg3 = MCP_4MHz_20kBPS_CFG3 ;
			set = 1;
			break;
		default:
			set = 0;
			break;
	}

	// if we have a setting available, write it
	if (set) {
		mcp2515_setRegister(MCP_CNF1, cfg1);
		mcp2515_setRegister(MCP_CNF2, cfg2);
		mcp2515_setRegister(MCP_CNF3, cfg3);
		return MCP2515_OK;
	}
	else {
		return MCP2515_FAIL;
	}
}

/*************************************************************************
Function: mcp2515_initCANBuffers()
Purpose:  Sets up Can buffer to initial state
Input:    None
Returns:  None
**************************************************************************/
void mcp2515_initCANBuffers(void)
{
	uint8_t i, a1, a2, a3;

	// TODO: check why this is needed to receive extended
	//   and standard frames
	// Mark all filter bits as don't care:
    mcp2515_write_can_id(MCP_RXM0SIDH, 0, 0);
    mcp2515_write_can_id(MCP_RXM1SIDH, 0, 0);
    // Anyway, set all filters to 0:
    mcp2515_write_can_id(MCP_RXF0SIDH, 1, 0); // RXB0: extended
    mcp2515_write_can_id(MCP_RXF1SIDH, 0, 0); //       AND standard
    mcp2515_write_can_id(MCP_RXF2SIDH, 1, 0); // RXB1: extended
    mcp2515_write_can_id(MCP_RXF3SIDH, 0, 0); //       AND standard
    mcp2515_write_can_id(MCP_RXF4SIDH, 0, 0);
    mcp2515_write_can_id(MCP_RXF5SIDH, 0, 0);

	// Clear, deactivate the three transmit buffers
	// TXBnCTRL -> TXBnD7
    a1 = MCP_TXB0CTRL;
	a2 = MCP_TXB1CTRL;
	a3 = MCP_TXB2CTRL;
    for (i = 0; i < 14; i++) { // in-buffer loop
		mcp2515_setRegister(a1, 0);
		mcp2515_setRegister(a2, 0);
		mcp2515_setRegister(a3, 0);
        a1++;
		a2++;
		a3++;
    }

    // and clear, deactivate the two receive buffers.
    mcp2515_setRegister(MCP_RXB0CTRL, 0);
    mcp2515_setRegister(MCP_RXB1CTRL, 0);
}

void mcp2515_readRegisterS(const uint8_t address, 
	uint8_t values[], const uint8_t n)
{
	uint8_t i;
	
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_READ);
	SPI_ReadWrite(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++) {
		values[i] = SPI_Read();
	}
	MCP2515_UNSELECT();
}

void mcp2515_setRegisterS(const uint8_t address, 
	const uint8_t values[], const uint8_t n)
{
	uint8_t i;
	
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_WRITE);
	SPI_ReadWrite(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++) {
		SPI_ReadWrite(values[i]);
	}
	MCP2515_UNSELECT();
}


static uint8_t mcp2515_readXXStatus_helper(const uint8_t cmd)
{
	uint8_t i;
	
	MCP2515_SELECT();
	SPI_ReadWrite(cmd);
	i = SPI_Read();
	MCP2515_UNSELECT();
	
	return i;
}
	
uint8_t mcp2515_readStatus(void)
{
	return mcp2515_readXXStatus_helper(MCP_READ_STATUS);
}

uint8_t mcp2515_RXStatus(void)
{
	return mcp2515_readXXStatus_helper(MCP_RX_STATUS);
}






// ---

void mcp2515_read_can_id( const uint8_t mcp_addr, 
	uint8_t* ext, uint32_t* can_id )
{
    uint8_t tbufdata[4];
	
    *ext = 0;
    *can_id = 0;
    
	mcp2515_readRegisterS( mcp_addr, tbufdata, 4 );
    
	*can_id = (tbufdata[MCP_SIDH]<<3) + (tbufdata[MCP_SIDL]>>5);
	
    if ( (tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M ) {
		// extended id
        *can_id = (*can_id<<2) + (tbufdata[MCP_SIDL] & 0x03);
        *can_id <<= 16;
        *can_id = *can_id +(tbufdata[MCP_EID8]<<8) + tbufdata[MCP_EID0];
        *ext = 1;
    }
}

// Buffer can be MCP_RXBUF_0 or MCP_RXBUF_1
void mcp2515_read_canMsg( const uint8_t buffer_sidh_addr,
	CANMessage* msg)
{

    uint8_t mcp_addr, ctrl;

	mcp_addr = buffer_sidh_addr;
	
    mcp2515_read_can_id( mcp_addr, &(msg->extended_id),
		&(msg->id) );
    
	ctrl = mcp2515_readRegister( mcp_addr-1 );
    msg->length = mcp2515_readRegister( mcp_addr+4 );
    
	if (/*(*dlc & RTR_MASK) || */(ctrl & 0x08)) {
        msg->rtr = 1;
    } else {
        msg->rtr = 0;
    }
    
	msg->length &= MCP_DLC_MASK;
    mcp2515_readRegisterS( mcp_addr+5, &(msg->data[0]), msg->length );
}


void mcp2515_write_can_id( const uint8_t mcp_addr, 
	const uint8_t ext, const uint32_t can_id )
{
    uint16_t canid;
    uint8_t tbufdata[4];
	
    canid = (uint16_t)(can_id & 0x0FFFF);
    
	if ( ext == 1) {
        tbufdata[MCP_EID0] = (uint8_t) (canid & 0xFF);
        tbufdata[MCP_EID8] = (uint8_t) (canid / 256);
        canid = (uint16_t)( can_id / 0x10000L );
        tbufdata[MCP_SIDL] = (uint8_t) (canid & 0x03);
        tbufdata[MCP_SIDL] += (uint8_t) ((canid & 0x1C )*8);
        tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
        tbufdata[MCP_SIDH] = (uint8_t) (canid / 32 );
    }
    else {
        tbufdata[MCP_SIDH] = (uint8_t) (canid / 8 );
        tbufdata[MCP_SIDL] = (uint8_t) ((canid & 0x07 )*32);
        tbufdata[MCP_EID0] = 0;
        tbufdata[MCP_EID8] = 0;
    }
	mcp2515_setRegisterS( mcp_addr, tbufdata, 4 );
}

// Buffer can be MCP_TXBUF_0 MCP_TXBUF_1 or MCP_TXBUF_2
void mcp2515_write_canMsg( const uint8_t buffer_sidh_addr, const CANMessage* msg)
{
    uint8_t mcp_addr, length;

	mcp_addr = buffer_sidh_addr;
	length = msg->length;
	
    mcp2515_setRegisterS(mcp_addr+5, &(msg->data[0]), length );  // write data bytes
    mcp2515_write_can_id( mcp_addr, msg->extended_id,
		msg->id );  // write CAN id
    if ( msg->rtr == 1)  length |= MCP_RTR_MASK;  // if RTR set bit in byte
    mcp2515_setRegister( (mcp_addr+4), length );  // write the RTR and DLC
}

/*
 ** Start the transmission from one of the tx buffers.
 */
// Buffer can be MCP_TXBUF_0 MCP_TXBUF_1 or MCP_TXBUF_2
void mcp2515_start_transmit(const uint8_t buffer_sidh_addr)
{
	// TXBnCTRL_addr = TXBnSIDH_addr - 1
    mcp2515_modifyRegister( buffer_sidh_addr-1 , MCP_TXB_TXREQ_M, 
		MCP_TXB_TXREQ_M );
}

uint8_t mcp2515_getNextFreeTXBuf(uint8_t *txbuf_n)
{
	uint8_t res, i, ctrlval;
	uint8_t ctrlregs[MCP_N_TXBUFFERS] = { MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL };
	
	res = MCP_ALLTXBUSY;
	*txbuf_n = 0x00;
	
	// check all 3 TX-Buffers
	for (i=0; i<MCP_N_TXBUFFERS; i++) {
		ctrlval = mcp2515_readRegister( ctrlregs[i] );
		if ( (ctrlval & MCP_TXB_TXREQ_M) == 0 ) {
#if (MCPDEBUG_TXBUF)
			Debug_ByteToUart_P("Selected TX-Buffer", i+1);
#endif
			*txbuf_n = ctrlregs[i]+1; // return SIDH-address of Buffer
			res = MCP2515_OK;
			return res; /* ! function exit */
		}
	}
	
	return res;
}




// ---



#ifdef MCPDEBUG
void mcp2515_dumpExtendedStatus(void)
{
	uint8_t tec, rec, eflg;
	
	tec  = mcp2515_readRegister(MCP_TEC);
	rec  = mcp2515_readRegister(MCP_REC);
	eflg = mcp2515_readRegister(MCP_EFLG);
	
	//term_puts_P("MCP2515 Extended Status:\n");
	//Debug_ByteToUart_p(PSTR("MCP Transmit Error Count"), tec);
	////term_puts_P(PSTR("MCP Receiver Error Count"), rec);
	//Debug_ByteToUart_p(PSTR("MCP Error Flag"), eflg);
	
//	if ( (rec>127) || (tec>127) ) {
//		//term_puts_P("Error-Passive or Bus-Off\n");
//	}
//
//	if (eflg & MCP_EFLG_RX1OVR)
//		//term_puts_P("Receive Buffer 1 Overflow\n");
//	if (eflg & MCP_EFLG_RX0OVR)
//		//term_puts_P("Receive Buffer 0 Overflow\n");
//	if (eflg & MCP_EFLG_TXBO)
//		//term_puts_P("Bus-Off\n");
//	if (eflg & MCP_EFLG_TXEP)
//		//term_puts_P("Receive Error Passive\n");
//	if (eflg & MCP_EFLG_TXWAR)
//		//term_puts_P("Transmit Error Warning\n");
//	if (eflg & MCP_EFLG_RXWAR)
//		//term_puts_P("Receive Error Warning\n");
//	if (eflg & MCP_EFLG_EWARN )
//		//term_puts_P("Receive Error Warning\n");
}
#endif

#if 0 
// read-modify-write - better: Bit Modify Instruction
uint8_t mcp2515_setCANCTRL_Mode(uint8_t newmode)
{
	uint8_t i;
	
	i = mcp2515_readRegister(MCP_CANCTRL);
	i &= ~(MODE_MASK);
	i |= newmode;
	mcp2515_setRegister(MCP_CANCTRL, i);
	
	// verify as advised in datasheet
	i = mcp2515_readRegister(MCP_CANCTRL);
	i &= MODE_MASK;
	if ( i == newmode ) {
		return MCP2515_OK; 
	}
	else {
		return MCP2515_FAIL;
	}
}
#endif
