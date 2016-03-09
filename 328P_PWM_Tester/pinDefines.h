/*
 * pinDefines.h
 *
 *  Created on: 04 Apr 2015
 *      Author: RobThePyro
 */

#ifndef PINDEFINES_H_
#define PINDEFINES_H_



#define LED_DDR			DDRD
#define LED_PORT		PORTD
#define LED_RF			PD4
#define LED_B			PD5
#define LED_GPS			PD6
#define LED_OFF(led)	LED_PORT |= (1<<led)
#define LED_ON(led)		LED_PORT &= ~(1<<led)
#define LED_FLIP(led)	LED_PORT ^= (1<<led)

#define BTN_DDR			DDRC
#define BTN_PORT		PORTC
#define BTN_PIN			PINC
#define BTN_1			PC5
#define BTN_1_INT		PCINT13
#define BTN_2			PC4
#define BTN_2_INT		PCINT12
#define BTN_3			PC3
#define BTN_3_INT		PCINT11
#define BTN_4			PC2
#define BTN_4_INT		PCINT10
#define BTN_5			PC1
#define BTN_5_INT		PCINT9

#define BUZZ_DDR		DDRD
#define BUZZ_PORT		PORTD
#define	BUZZ0			PD7
#define BUZZ_ON			BUZZ_PORT |= (1<<BUZZ0)
#define BUZZ_OFF		BUZZ_PORT &= ~(1<<BUZZ0)

#define ALOG_DDR	DDRC
#define ALOG_PORT	PORTC
#define ALOG_PIN	PINC
#define ALOG_0		PC0
#define ALOG_0MUX	0x00

#define PWM1_DDR	DDRB
#define PWM1_PORT	PORTB
#define PWM1_A		PB1
#define PWM1A_REG	OCR1A






#endif /* PINDEFINES_H_ */
