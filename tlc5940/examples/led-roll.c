/*
 *  	Created on: Aug 5, 2013
 *      Author: Long Cao
 *
 */

#include "TLC5940.h"
#include "main.h"

volatile unsigned char cnt=0;

int main()
{
	SysCfg();
	ConfigSystemTLC5940();
	DCInputCycle();
	while(1)
	{
		GSInputCycle();
		__bis_SR_register(GIE + LPM0_bits);
		if (dataGS[cnt]==0x00) dataGS[cnt++]=0xff;
		else dataGS[cnt++]=0x00;
		if (cnt==16*TLC5940_N) cnt=0;
		fDataGSUpd=GS_DATA_UPD_READY;
	}
	return 0;
}

void SysCfg(void)
{
	WDTCTL = WDTPW + WDTHOLD;

	// Set MCLK clock
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P1SEL = TXD + RXD ;                     // P1.1 = RXD, P1.2=TXD
	P1SEL2 = TXD + RXD;

	TA1CCR0=62500;
	TA1CTL=TASSEL_2 + MC_1 + ID_0; // TA0CLK=SMCLK
	TA1CCTL0 |= CCIE; // CCR0 interrupt enabled
}
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Refresh(void)
{
	if ((fXLAT==TLC_LATCH_WAIT)&&(fDataGSUpd==GS_DATA_UPD_WAIT))
			LPM0_EXIT;
}
