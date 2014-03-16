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

// Number of TLC
#define TLC5940_N       3
// GS data bit-depth
#define GS_BIT_DEPTH	8
// TLC5940 clock setting
#define CLOCK_SETTING	DEFAULT_LOW_POWER
#if (CLOCK_SETTING==CUSTOM_CLOCK)
	#define GSCLK_SRC	TASSEL_1
//        	TASSEL_1:	ACLK
//        	TASSEL_2:	SMCLK
	#if (GSCLK_SRC==TASSEL_1)
		#define ACLK_SRC    LFXT1S_0
//        	LFXT1S_0:	32768-Hz crystal on LFXT1
//        	LFXT1S_2:	12000-Hz VLOCLK. This frequency is not preferable unless
//						running with low GS bit-depth.
	#endif
#endif
//	Error detection
#define ERROR_DETECTION	0
//			0: no error detection feature
//			1: enable led open detection and thermal error flag
#define PIN_MAPPING	0
//			0: use default pin configuration for MSP430G2553
//					see TLC5940_internal.h
//			1: use custom configuration
//			please uncomment the following part
#if (PIN_MAPPING==1)
/********************** Pin Mapping **********************/
// ***** PORT 1 *****
// Reference clock for GS PWM control
#if (GSCLK_SRC==TASSEL_2)
	#define GSCLK_PIN       BIT
#elif (GSCLK_SRC==TASSEL_1)
	#define GSCLK_PIN       BIT
#endif
// Blank all outputs. When BLANK=H, all OUTn outputs are forced OFF. GS counter is also
// reset. When BLANK=L, OUTn are controlled by GS PWM control.
#if (GSCLK_SRC==TASSEL_2)
	#define BLANK_PIN       BIT
#elif (GSCLK_SRC==TASSEL_1)
	#define BLANK_PIN       BIT
#endif
// Serial data shift clock
#define SCLK_PIN        BIT
// MSP430 master output pin / TLC5940 serial data input pin
#define SOUT_PIN		BIT
// Error output. XERR is an open-drain terminal. XERR goes L when LOD or TEF is detected.
#if (ERROR_DETECTION==1)
	#define XERR_PIN        BIT
#elif (ERROR_DETECTION==0)
	#define XERR_PIN		0
#endif
// Status information/serial data output pin
#if (ERROR_DETECTION==1)
	#define SIN_PIN		BIT
#elif (ERROR_DETECTION==0)
	#define SIN_PIN		0
#endif
// ***** PORT 2 *****
// Switch DC data input. 	When DCPRG=L, DC is connected to EEPROM.
// 							When DCPRG=H, DC is connected to the DC register.
// DCPRG also controls EEPROM writing, when VPRG=V(PRG). EEPROM data=3Fh (default)
#define DCPRG_PIN       BIT
// Multifunction input pin. When VPRG=GND, the device is in GS mode. When VPRG=VCC, the
// device is in DC mode. When VPRG=V(VPRG), DC register data can programmed into DC
// EEPROM with DCPRG=HIGH. EEPROM data=3Fh (default)
#define VPRG_PIN        BIT
// Level triggered latch signal. When XLAT=high, the TLC5940 writes data from the input shift
// register to either GS register (VPRG=low) or DC register (VPRG=high). When XLAT=low,
// the data in GS or DC register is held constant.
#define XLAT_PIN        BIT
/*********************************************************/
#endif

#ifdef __cplusplus
}
#endif

#endif /* TLC5940_CONFIG_H_ */
