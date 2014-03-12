/*
 * TLC5940_api.c
 *
 *  Created on: Sep 7, 2013
 *      Author: Long Cao
 *
 *      TLC5940 APIs, based on slvc106 - TLC5940 Programming Flowchart v0.1 (29/9/2005)
 */

#include "TLC5940.h"

/********************** Variable Definitions **********************/
volatile bool fFirstCycle=false, fXLAT=TLC_LATCH_WAIT, fDataGSUpd=GS_DATA_UPD_READY;
unsigned char cntDataTLC=0, bfrSPIRx=0;
unsigned char \
dataDC[12*TLC5940_N]={
		//		Initial dot correction data = 50% default max output current.
		//		The maximum output current per channel is programmed by a single resistor, R(IREF), which is placed between
		//		IREF pin and GND pin. The voltage on IREF is set by an internal band gap V(IREF) with a typical value of
		//		1.24V. The maximum channel current is equivalent to the current flowing through R(IREF) multiplied by a factor of
		//		31.5. The maximum output current per channel can be calculated by Equation 6:
		//		                V(IREF)
		//		      I max = ----------- x 31.5
		//		                R(IREF)                                                                                      (6)
		//		where:
		//		     V(IREF) = 1.24 V
		//		     R(IREF) = User-selected external resistor.
		//		Imax must be set between 5 mA and 120 mA. The output current may be unstable if Imax is set lower than 5 mA.
		//		Output currents lower than 5 mA can be achieved by setting Imax to 5 mA or higher and then using dot
		//		correction.
		0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,
		0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,
		0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,
		0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,	0x55,
},
dataGSraw[24*TLC5940_N]={
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
};
DATA_GS_TYPE dataGS[16*TLC5940_N]={
		0,		0,		0,		0,		0,		0,		0,		0,
		0,		0,		0,		0,		0,		0,		0,		0,
};
/******************************************************************/

/************************** APIs *******************************/
void ConfigSystemTLC5940(void)
{
	/*
	 *	Edit ACLK as configed in TLC5940_config.h
	 */
#if (GSCLK_SRC==TASSEL_1)
	BCSCTL1 &=~(XTS+DIVA1+DIVA0);
	BCSCTL3 |= ACLK_SRC;
#endif

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
	// Update Interval=(GSCLK_CYCLE)/(TA0CLK)
	TA0CCR0=GSCLK_CYCLE;
	TA0CTL=GSCLK_SRC + MC_1 + ID_0; // TA0CLK=SMCLK
	TA0CCTL0 |= CCIE; // CCR0 interrupt enabled

	// Config USCI B module - 3-pin, 8-bit SPI master
	UCB0CTL1 |= UCSWRST;
	UCB0CTL0=UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
	UCB0CTL1 |= UCSSEL_2; // SMCLK
	UCB0BR0=1;	// 16MHz SCLK
	UCB0BR1=0;
	UCB0CTL1 &= ~UCSWRST; // clear SW, start fisrt serial transmission
}
void DCInputCycle(void)
{
	/*
	 * Write DC data to DC register
	 * Comment out the following part if use default DC data
	 */
	P2OUT |= DCPRG_PIN + VPRG_PIN;
	TransmitSerialData(dataDC,12*TLC5940_N);
	P2OUT |= XLAT_PIN;
	P2OUT &= ~XLAT_PIN;
	// Switch from DC to GS mode
	P2OUT &= ~VPRG_PIN;
	fFirstCycle=true;
}
void GSInputCycle(void)
{
	P1OUT &= ~BLANK_PIN;
	GenGSDataRaw(dataGS,dataGSraw);
	if (fDataGSUpd==GS_DATA_UPD_READY)
		TransmitSerialData(dataGSraw,24*TLC5940_N);
}
void TransmitSerialData(unsigned char *ptrData,unsigned char count)
{
	cntDataTLC=0;
	while (cntDataTLC < count)
	{
		UCB0TXBUF=ptrData[cntDataTLC];
		while (!(IFG2 & UCB0TXIFG)); // TX buffer ready?
		/*if ((P1IN & XERR_PIN) == 0)
    		error[DataCnt]=UCB0RXBUF;
    	else*/
		bfrSPIRx=UCB0RXBUF;
		cntDataTLC++;
	}
	fXLAT=TLC_LATCH_READY;
	fDataGSUpd=GS_DATA_UPD_WAIT;
}

void GenGSDataRaw(DATA_GS_TYPE *ptrData,unsigned char* ptrDataRaw)
{
	unsigned char cntData=0,cntDataRaw=0;
	for (cntDataRaw=0;cntDataRaw<24*TLC5940_N;cntDataRaw++)
	{
		switch (cntDataRaw%3)
		{
		case 0:
			ptrDataRaw[cntDataRaw]=ptrData[cntData]&0xff;
			break;
		case 1:
			ptrDataRaw[cntDataRaw]=(ptrData[cntData]>>8)|(ptrData[cntData+1]<<4);
			break;
		case 2:
			ptrDataRaw[cntDataRaw]=ptrData[cntData+1]>>4;
			cntData+=2;
			break;
		}
	}
}

/*void ErrorCheck(void)
{
	UCB0CTL1 &= ~UCSWRST; // clear SW, start fisrt serial transmission
	if ((P1IN & XERR_PIN) == 0)
	{
		cntDataTLC=0;
		P1OUT &= ~BLANK_PIN; // blank all output
		TransmitSerialData(dataGS,TLC5940_N*24);
		P2OUT |= XLAT_PIN;
		P2OUT &= ~XLAT_PIN;
		P1OUT |= BLANK_PIN; // blank all output
		IE2 |= UCA0TXIE;
		__bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/ interrupts enabled
		IE1=0;
		IE2=0;
		__bis_SR_register(GIE + LPM4_bits);
	}
}*/
/***************************************************************/

#pragma vector=TIMER0_A0_VECTOR
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
	UCA0TXBUF=status[cntDataTLC++];
	if (cntDataTLC==TLC5940_N*24)
		__bic_SR_register(LPM4_bits);
}*/
