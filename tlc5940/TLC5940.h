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
#include "TLC5940_config.h"
#include "TLC5940_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************** Variable Declarations **********************/

// FirstCycle flag is set to high for first GS cycle after DC input cycle,
// add one additional SCLK pulse
extern volatile bool fFirstCycle;
// Latch signal flag - indicate a necessary signal when in state TLC_LATCH_READY
extern volatile bool fXLAT;

extern volatile bool fDataGSUpd;
// cntDataTLC=counter for serial (both DC & GS) data
extern unsigned char cntDataTLC;
// DC data
// Size of array=96 bits x number of TLC=12 bytes * number of TLC
extern unsigned char dataDC[12*TLC5940_N];
// GS data
// Size of array=192 bits x number of TLC=24 bytes * number of TLC
extern unsigned char dataGS[DATA_GS_LENGTH*TLC5940_N];

extern unsigned char bfrSer;
/*******************************************************************/



/********************** API Declarations **********************/

// Configure watchdog timer, system clocks, ports, timer and USCI modules
extern void SystemConfig_TLC(void);

extern void DCInputCycle(void);

extern void GSInputCycle(void);

extern void SerialTransmit(unsigned char *ptrData,unsigned char count);


/************************************************************/
#if (ERROR_DETECTION==1)
// Status information array
// Including:	Led Open Detection (LOD)
// 				Thermal Error Flag (TEF)
//				DC value
// Size of array=192 bits x number of TLC=24 bytes *number of TLC
extern unsigned char status[24*TLC5940_N];
extern void ErrorCheck(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
