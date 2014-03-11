/*
 * TLC5940_config.h
 *
 *  Created on: Mar 12, 2014
 *      Author: longcao
 */

#ifndef TLC5940_CONFIG_H_
#define TLC5940_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

//==================================================================================================
// Clock Routing
//==================================================================================================
#define GSCLK_SRC	1
//        	0 = SMCLK
//        	1 = ACLK
#define ACLK_SRC    0
//        	0 = LFXT1:	32.768 	kHz
//        	1 = VLO:	12		kHz

#define ERROR_DETECTION	1
//			0: no error detection feature
//			1: enable led open detection and thermal error flag

// Number of TLC
#define TLC5940_N       3
// GS data bit-depth
#define GS_BIT_DEPTH	20
// GS cycle depending on bit-depth
#if GS_BIT_DEPTH<=12
#define GSCLK_CYCLE		2^GS_BIT_DEPTH
#else
#error	'1'
#endif

/********************** Pin Mapping **********************/
// ***** PORT 1 *****
// Reference clock for GS PWM control
#if (GSCLK_SRC==0)
#define GSCLK_PIN       BIT4    // P1.4 - SMCLK signal output
#elif (GSCLK_SRC==1)
#define GSCLK_PIN       BIT0    // P1.0 - ACLK signal output
#endif
// Blank all outputs. When BLANK = H, all OUTn outputs are forced OFF. GS counter is also
// reset. When BLANK = L, OUTn are controlled by GS PWM control.
#if (GSCLK_SRC==0)
#define BLANK_PIN       BIT0    // P1.0
#elif (GSCLK_SRC==1)
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
// Switch DC data input. 	When DCPRG = L, DC is connected to EEPROM.
// 							When DCPRG = H, DC is connected to the DC register.
// DCPRG also controls EEPROM writing, when VPRG = V(PRG). EEPROM data = 3Fh (default)
#define DCPRG_PIN       BIT0    // P2.0
// Multifunction input pin. When VPRG = GND, the device is in GS mode. When VPRG = VCC, the
// device is in DC mode. When VPRG = V(VPRG), DC register data can programmed into DC
// EEPROM with DCPRG=HIGH. EEPROM data = 3Fh (default)
#define VPRG_PIN        BIT1    // P2.1
// Level triggered latch signal. When XLAT = high, the TLC5940 writes data from the input shift
// register to either GS register (VPRG = low) or DC register (VPRG = high). When XLAT = low,
// the data in GS or DC register is held constant.
#define XLAT_PIN        BIT2    // P2.2
/*********************************************************/

/********************** Definitions **********************/
// GS data update state
#define TLC_DATA_READY	1
#define TLC_DATA_WAIT	0

// Latch signal state
#define TLC_LATCH_READY	1
#define	TLC_LATCH_WAIT	0
/*********************************************************/

#endif /* TLC5940_CONFIG_H_ */
