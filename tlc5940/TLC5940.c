/*
 * TLC5940_api.c
 *
 *  Created on: Sep 7, 2013
 *      Author: Long Cao
 *
 *      TLC5940 APIs, based on slvc106 - TLC5940 Programming Flowchart v0.1 (29/9/2005)
 */

#include "TLC5940_api.h"


/********************** Variable Definitions **********************/

volatile bool fFirstCycle=false, fXLAT=TLC_LATCH_WAIT, fTLCdata=TLC_DATA_READY;
unsigned char cntDataTLC=0, bfrSer=0;
unsigned char dataDC[12*TLC5940_N] = {
		//		1		2		3		4		5		6		7		8		9
		0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	//->	RGBled 1 	2	3	4
		0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	//->	RGBled 5	6	7	8
		0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	//->	RGBled 9	10	11	12
		0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	//->	RGBled 13	14	15	16
},
dataGS[24*TLC5940_N] = {
		//		1		2		3		4		5		6		7		8		9
		// Initial GS data
		/*0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//->	RGBled 1 	2
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//->	RGBled 3	4
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//->	RGBled 5	6
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//->	RGBled 7 	8
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//->	RGBled 9	10
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//->	RGBled 11	12
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//->	RGBled 13 	14
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//->	RGBled 15	16*/

		0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	//->	RGBled 1 	2
		0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	//->	RGBled 3	4
		0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	//->	RGBled 5	6
		0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	//->	RGBled 7 	8
		0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	//->	RGBled 9	10
		0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	//->	RGBled 11	12
		0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	//->	RGBled 13 	14
		0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	//->	RGBled 15	16
};

/******************************************************************/


/************************** APIs *******************************/

void SystemConfig_TLC(void)
{
	/*
	 *	ACLK for GSCLK
	 *	if commented out, use SMCLK for GSCLK instead
	 */
	// ACLK = VLOCLK
	//BCSCTL1 &=~(XTS+DIVA1+DIVA0);
	//BCSCTL3 |= LFXT1S_2;                      // ACLK = VLO

	// Set GPIO direction, function and value
	P1OUT &= ~(GSCLK_PIN + SCLK_PIN);
	P1OUT |= 	BLANK_PIN + // blank all output at first
			XERR_PIN;	// pull-up resistor for error pin
	P1DIR &= ~XERR_PIN;
	P1REN |= XERR_PIN;		// pull-up resistor enable
	P1DIR |= BLANK_PIN + SCLK_PIN + GSCLK_PIN;
	P1SEL |= SIN_PIN + SOUT_PIN + SCLK_PIN + GSCLK_PIN;
	P1SEL2 |= SIN_PIN + SOUT_PIN + SCLK_PIN;

	P2OUT &= ~(DCPRG_PIN + XLAT_PIN);
	P2OUT |= VPRG_PIN;
	P2DIR |= VPRG_PIN + DCPRG_PIN + XLAT_PIN;

	// Timer A0 CCR0 to send LATCH signal
	// Update Interval = (GSCLK_CYCLE)/(TA0CLK)
	TA0CCR0 = GSCLK_CYCLE;
	//TA0CTL = TASSEL_1 + MC_1 + ID_0; // TA0CLK = ACLK
	TA0CTL = TASSEL_2 + MC_1 + ID_0; // TA0CLK = SMCLK
	TA0CCTL0 |= CCIE; // CCR0 interrupt enabled

	// Config USCI B module - 3-pin, 8-bit SPI master
	UCB0CTL1 |= UCSWRST;
	UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
	UCB0CTL1 |= UCSSEL_2; // SMCLK
	UCB0BR0 = 1;	// 16MHz SCLK
	UCB0BR1 = 0;
	UCB0CTL1 &= ~UCSWRST; // clear SW, start fisrt serial transmission
}

void DCInputCycle(void)
{
	/*
	 * Write DC data to DC register
	 * Comment out the following part if use default DC data
	 */
	P2OUT |= DCPRG_PIN + VPRG_PIN;
	SerialTransmit(dataDC,TLC5940_N*12);
	P2OUT |= XLAT_PIN;
	P2OUT &= ~XLAT_PIN;
	// Switch from DC to GS mode
	P2OUT &= ~VPRG_PIN;
	fFirstCycle = true;
}

void GSInputCycle(void)
{
	P1OUT &= ~BLANK_PIN;
	if (fTLCdata==TLC_DATA_READY)
		SerialTransmit(dataGS,TLC5940_N*24);
}

void SerialTransmit(unsigned char *ptrData,unsigned char count)
{
	cntDataTLC = 0;
	while (cntDataTLC < count)
	{
		UCB0TXBUF = ptrData[cntDataTLC];
		while (!(IFG2 & UCB0TXIFG)); // TX buffer ready?
		/*if ((P1IN & XERR_PIN) == 0)
    		error[DataCnt] = UCB0RXBUF;
    	else*/
		bfrSer = UCB0RXBUF;
		cntDataTLC++;
	}
	fXLAT = TLC_LATCH_READY;
	fTLCdata = TLC_DATA_WAIT;
}

/*void ErrorCheck(void)
{
	UCB0CTL1 &= ~UCSWRST; // clear SW, start fisrt serial transmission
	if ((P1IN & XERR_PIN) == 0)
	{
		cntDataTLC=0;
		P1OUT &= ~BLANK_PIN; // blank all output
		SerialTransmit(dataGS,TLC5940_N*24);
		P2OUT |= XLAT_PIN;
		P2OUT &= ~XLAT_PIN;
		P1OUT |= BLANK_PIN; // blank all output
		IE2 |= UCA0TXIE;
		__bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/ interrupts enabled
		IE1 = 0;
		IE2 = 0;
		__bis_SR_register(GIE + LPM4_bits);
	}
}*/

/***************************************************************/


#pragma vector = TIMER0_A0_VECTOR
__interrupt void GSCLK_Control(void)
{
	P1OUT |= BLANK_PIN;
	if (fXLAT==TLC_LATCH_READY)		// New data already transmitted, send XLAT pulse to latch
	{
		P2OUT |= XLAT_PIN;
		P2OUT &= ~XLAT_PIN;
		fXLAT=TLC_LATCH_WAIT;
	}
	/*if (fFirstCycle==true)
	{
		P1SEL &= ~SCLK_PIN;
		P1SEL2 &= ~SCLK_PIN;
		P1OUT |= SCLK_PIN;
		P1OUT &= ~SCLK_PIN;
		P1SEL |= SCLK_PIN;
		P1SEL2 |= SCLK_PIN;
		fFirstCycle=false;
	}*/
	P1OUT &= ~BLANK_PIN;
}

/*#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
	UCA0TXBUF = status[cntDataTLC++];
	if (cntDataTLC==TLC5940_N*24)
		__bic_SR_register(LPM4_bits);
}*/
