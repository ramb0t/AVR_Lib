#ifndef MCP2515_H_
#define MCP2515_H_

// Includes:
/*********************************************/
#include "AVR_SPI.h"
#include "mcp2515_defs.h"
#include "mcp2515_bittime.h"
#include <util/delay.h>
#include "CAN.h"
#include <inttypes.h>


// Defines:
/*********************************************/

#define MCPDEBUG (0)
#define MCPDEBUG_TXBUF (0)

// Hardware:
#define MCP_CS_PORT PORTB
#define MCP_CS_DDR  DDRB
#define MCP_CS_BIT  PB2

// Buffers:
#define MCP_N_TXBUFFERS (3)

#define MCP_RXBUF_0 (MCP_RXB0SIDH)
#define MCP_RXBUF_1 (MCP_RXB1SIDH)

#define MCP_TXBUF_0 (MCP_TXB0SIDH)
#define MCP_TXBUF_1 (MCP_TXB1SIDH)
#define MCP_TXBUF_2 (MCP_TXB2SIDH)

// #define MCP2515_SELECT() (SPI_SS_LOW())
// #define MCP2515_UNSELECT() (SPI_SS_HIGH())

#define MCP2515_SELECT()   ( MCP_CS_PORT &= ~(1<<MCP_CS_BIT) )
#define MCP2515_UNSELECT() ( MCP_CS_PORT |=  (1<<MCP_CS_BIT) )

#define MCP2515_OK         (0)
#define MCP2515_FAIL       (1)
#define MCP_ALLTXBUSY      (2)

// Speed:
#define CAN_20KBPS   (1)
#define CAN_125KBPS  (CAN_20KBPS+1)
#define CAN_125KBPS_16MHZ  (CAN_20KBPS+2)


// Functions:
/*********************************************/

// Low Level Functions
void mcp2515_reset(void);
uint8_t mcp2515_readRegister(const uint8_t address);
void mcp2515_readRegisterS(const uint8_t address, uint8_t values[], const uint8_t n);
void mcp2515_setRegister(const uint8_t address, const uint8_t value);
void mcp2515_setRegisterS(const uint8_t address, const uint8_t values[], const uint8_t n);
void mcp2515_modifyRegister(const uint8_t address, const uint8_t mask, const uint8_t data);

// Control Functions
uint8_t mcp2515_init(const uint8_t canSpeed);
uint8_t mcp2515_setCANCTRL_Mode(const uint8_t newmode);
uint8_t mcp2515_configRate(const uint8_t canSpeed);
void mcp2515_initCANBuffers(void);



uint8_t mcp2515_readStatus(void);
uint8_t mcp2515_RXStatus(void);







// Data Functions

void mcp2515_write_can_id( const uint8_t mcp_addr, 
	const uint8_t ext, const uint32_t can_id );
void mcp2515_read_can_id( const uint8_t mcp_addr, 
	uint8_t* ext, uint32_t* can_id );

void mcp2515_write_canMsg( const uint8_t buffer_sidh_addr, const CANMessage *);
void mcp2515_read_canMsg( const uint8_t buffer_sidh_addr, CANMessage *);

void mcp2515_start_transmit(const uint8_t mcp_addr);

uint8_t mcp2515_getNextFreeTXBuf(uint8_t *txbuf_n);

#ifdef MCPDEBUG
void mcp2515_dumpExtendedStatus(void);
#endif

#endif
