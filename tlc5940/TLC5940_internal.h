/*
 * TLC5940_internal.h
 *
 *  Created on: Mar 12, 2014
 *      Author: longcao
 *
 */
//===============================================
//	ATTENTION!!!
//	DO NOT MAKE ANY CHANGES TO THIS FILE
//	FOR CONFIGURATION SEE TLC5940_config.h
//===============================================

#ifndef TLC5940_INTERNAL_H_
#define TLC5940_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define GSCLK_CYCLE		2^GS_BIT_DEPTH

#if GS_BIT_DEPTH>=12
	#error Invalid GS_BIT_DEPTH
#endif
#if GS_BIT_DEPTH<=8
	#define SIMPLE_LIB
#else
	#define EXTENDED_LIB
#endif
#ifdef SIMPLE_LIB
	#define DATA_GS_LENGTH	16
#endif
#ifdef EXTENDED_LIB
	#define DATA_GS_LENGTH	16+2*(GS_BIT_DEPTH-8)
#endif

// GS data update state
#define GS_DATA_UPD_READY	1
#define GS_DATA_UPD_WAIT	0
// Latch signal state
#define TLC_LATCH_READY	1
#define	TLC_LATCH_WAIT	0

#if (PIN_MAPPING==0)
/********************** Pin Mapping **********************/
// ***** PORT 1 *****
// Reference clock for GS PWM control
#if (GSCLK_SRC==TASSEL_2)
	#define GSCLK_PIN       BIT4    // P1.4 - SMCLK signal output
#elif (GSCLK_SRC==TASSEL_1)
	#define GSCLK_PIN       BIT0    // P1.0 - ACLK signal output
#endif
// Blank all outputs. When BLANK=H, all OUTn outputs are forced OFF. GS counter is also
// reset. When BLANK=L, OUTn are controlled by GS PWM control.
#if (GSCLK_SRC==TASSEL_2)
	#define BLANK_PIN       BIT0    // P1.0
#elif (GSCLK_SRC==TASSEL_1)
	#define BLANK_PIN       BIT4    // P1.4
#endif
// Serial data shift clock
#define SCLK_PIN        BIT5    // P1.5 - UCB0CLK - USCI_B0 clock input/output
// MSP430 master output pin / TLC5940 serial data input pin
#define SOUT_PIN		BIT7    // P1.7 - UCB0SIMO - USCI_B0 SPI mode: slave data in/master out
// Error output. XERR is an open-drain terminal. XERR goes L when LOD or TEF is detected.
#if (ERROR_DETECTION==1)
	#define XERR_PIN        BIT3    // P1.3
#elif (ERROR_DETECTION==0)
	#define XERR_PIN		0
#endif
// Status information/serial data output pin
#if (ERROR_DETECTION==1)
	#define SIN_PIN		BIT6	// P1.6 - UCB0SOMI - USCI_B0 SPI mode: slave data out/master in
#elif (ERROR_DETECTION==0)
	#define SIN_PIN		0
#endif
// ***** PORT 2 *****
// Switch DC data input. 	When DCPRG=L, DC is connected to EEPROM.
// 							When DCPRG=H, DC is connected to the DC register.
// DCPRG also controls EEPROM writing, when VPRG=V(PRG). EEPROM data=3Fh (default)
#define DCPRG_PIN       BIT0    // P2.0
// Multifunction input pin. When VPRG=GND, the device is in GS mode. When VPRG=VCC, the
// device is in DC mode. When VPRG=V(VPRG), DC register data can programmed into DC
// EEPROM with DCPRG=HIGH. EEPROM data=3Fh (default)
#define VPRG_PIN        BIT1    // P2.1
// Level triggered latch signal. When XLAT=high, the TLC5940 writes data from the input shift
// register to either GS register (VPRG=low) or DC register (VPRG=high). When XLAT=low,
// the data in GS or DC register is held constant.
#define XLAT_PIN        BIT2    // P2.2
/*********************************************************/
#endif /* DEFAULT PIN MAP */

#ifdef __cplusplus
}
#endif

#endif /* TLC5940_INTERNAL_H_ */
