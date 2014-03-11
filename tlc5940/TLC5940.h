/*
 * TLC5940_api.h
 *
 * Designed for TLC5940 - 16 CHANNEL LED DRIVER
 * WITH DOT CORRECTION (DC) AND GRAYSCALE (GS) PWM CONTROL
 * 12 bit (4096 Steps) GS,
 * 6 bit (64 Steps) DC (Storable in Integrated EEPROM)
 *
 *  Created on: Aug 5, 2013
 *      Author: Long Cao
 */

#ifndef TLC5940_API_H
#define TLC5940_API_H

#include <msp430.h>
#include <stdbool.h>		

#ifdef __cplusplus
extern "C" {
#endif

/********************** Pin Mapping **********************/
// ***** PORT 1 *****
// Reference clock for GS PWM control
//#define GSCLK_PIN       BIT0    // P1.0 - ACLK signal output
#define GSCLK_PIN       BIT4    // P1.4 - SMCLK signal output
// Serial data shift clock
#define SCLK_PIN        BIT5    // P1.5 - UCB0CLK - USCI_B0 clock input/output
// Blank all outputs. When BLANK = H, all OUTn outputs are forced OFF. GS counter is also
// reset. When BLANK = L, OUTn are controlled by GS PWM control.
#define BLANK_PIN       BIT3    // P1.3
// Serial data input pin on TLC5940
#define SIN_PIN         BIT7    // P1.7 - UCB0SIMO - USCI_B0 SPI mode: slave data in/master out
// Error output. XERR is an open-drain terminal. XERR goes L when LOD or TEF is detected.
//#define XERR_PIN        BIT4    // P1.4
//#define XERR_PIN        BIT0    // P1.0
#define XERR_PIN		0
// Status information/serial data output pin
//#define SOUT_PIN		BIT6
#define SOUT_PIN		0

// ***** PORT 2 *****
// Switch DC data input. When DCPRG = L, DC is connected to EEPROM. When DCPRG = H,
                                // DC is connected to the DC register.
                                // DCPRG also controls EEPROM writing, when VPRG = V(PRG). EEPROM data = 3Fh (default)
#define DCPRG_PIN       BIT0    // P2.0
// Multifunction input pin. When VPRG = GND, the device is in GS mode. When VPRG = VCC, the
// device is in DC mode. When VPRG = V(VPRG), DC register data can programmed into DC
// EEPROM with DCPRG=HIGH. EEPROM data = 3Fh (default)
#define VPRG_PIN        BIT1    // P2.1
// Level triggered latch signal. When XLAT = high, the TLC5940 writes data from the input shift
// register to either GS register (VPRG = low) or DC register (VPRG = high). When XLAT = low,
// the data in GS or DC register is held constant.
#define XLAT_PIN        BIT5    // P2.5
/*********************************************************/



/********************** Definitions **********************/

// Number of TLC
#define TLC5940_N       3

#define GSCLK_CYCLE		4096

#define TLC_DATA_READY	1
#define TLC_DATA_WAIT	0

// Latch signal state
#define TLC_LATCH_READY	1
#define	TLC_LATCH_WAIT	0
/*********************************************************/



/********************** Variable Declarations **********************/

// FirstCycle flag is set to high for first GS cycle after DC input cycle,
// add one additional SCLK pulse
extern volatile bool fFirstCycle;
// Latch signal flag - indicate a necessary signal when in state TLC_LATCH_READY
extern volatile bool fXLAT;

extern volatile bool fTLCdata;
// cntDataTLC = counter for DC and GS Data
extern unsigned char cntDataTLC;
// DC data
// Size of array = 96 bits x number of TLC = 12 bytes * number of TLC
extern unsigned char dataDC[12*TLC5940_N];
// GS data
// Size of array = 192 bits x number of TLC = 24 bytes * number of TLC
extern unsigned char dataGS[24*TLC5940_N];
// Status information array
// Including:	Led Open Detection (LOD)
// 				Thermal Error Flag (TEF)
//				DC value
// Size of array = 192 bits x number of TLC = 24 bytes * number of TLC
extern unsigned char status[24*TLC5940_N];

extern unsigned char bfrSer;
/*******************************************************************/



/********************** API Declarations **********************/

// Configure watchdog timer, system clocks, ports, timer and USCI modules
extern void SystemConfig_TLC(void);

extern void DCInputCycle(void);

extern void GSInputCycle(void);

extern void SerialTransmit(unsigned char *ptrData,unsigned char count);

//extern void ErrorCheck(void);

/************************************************************/

#ifdef __cplusplus
}
#endif

#endif
