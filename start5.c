//-----------------------------------------------------------------
// Name:	Chris Kiernan
// File:	lab5.c
// Date:	Fall 2014
// Purp:	Decode the IR packet and output a signal for button presses
//-----------------------------------------------------------------
#include <msp430g2553.h>
#include "start5.h"

int8	newIrPacket = FALSE;
int16	packetData[34];
int8	packetIndex = 0;
int32	irPacket = 0;

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void main(void) {

	initMSP430();				// Setup MSP to process IR and buttons

	while(1)  {
		if (packetIndex > 33) {
			packetIndex = 0;
		} // end if new IR packet arrived
		if(newIrPacket){
			if(irPacket==ZER){
				P1OUT ^= BIT0;
			}else if(irPacket==ONE){
				P1OUT ^= BIT6;
			}
			newIrPacket = FALSE;
			irPacket = 0;
		}
	} // end infinite loop
} // end main

// -----------------------------------------------------------------------
// In order to decode IR packets, the MSP430 needs to be configured to
// tell time and generate interrupts on positive going edges.  The
// edge sensitivity is used to detect the first incoming IR packet.
// The P2.6 pin change ISR will then toggle the edge sensitivity of
// the interrupt in order to measure the times of the high and low
// pulses arriving from the IR decoder.
//
// The timer must be enabled so that we can tell how long the pulses
// last.  In some degenerate cases, we will need to generate a interrupt
// when the timer rolls over.  This will indicate the end of a packet
// and will be used to alert main that we have a new packet.
// -----------------------------------------------------------------------
void initMSP430() {

	IFG1=0; 					// clear interrupt flag1
	WDTCTL=WDTPW+WDTHOLD; 		// stop WD

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P2SEL  &= ~BIT6;						// Setup P2.6 as GPIO not XIN
	P2SEL2 &= ~BIT6;
	P2DIR &= ~BIT6;
	P2IFG &= ~BIT6;						// Clear any interrupt flag
	P2IE  |= BIT6;						// Enable PORT 2 interrupt on pin change

	HIGH_2_LOW;
	P1DIR |= BIT0 | BIT6;				// Enable updates to the LED
	P1OUT &= ~(BIT0 | BIT6);			// An turn the LED off

	TA0CCR0 = 0xFFFF;					// create a 16mS roll-over period
	TACTL &= ~TAIFG;					// clear flag before enabling interrupts = good practice
	TACTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:1 presclar off MCLK and enable interrupts

	_enable_interrupt();
}

// -----------------------------------------------------------------------
// Since the IR decoder is connected to P2.6, we want an interrupt
// to occur every time that the pin changes - this will occur on
// a positive edge and a negative edge.
//
// Negative Edge:
// The negative edge is associated with end of the logic 1 half-bit and
// the start of the logic 0 half of the bit.  The timer contains the
// duration of the logic 1 pulse, so we'll pull that out, process it
// and store the bit in the global irPacket variable. Going forward there
// is really nothing interesting that happens in this period, because all
// the logic 0 half-bits have the same period.  So we will turn off
// the timer interrupts and wait for the next (positive) edge on P2.6
//
// Positive Edge:
// The positive edge is associated with the end of the logic 0 half-bit
// and the start of the logic 1 half-bit.  There is nothing to do in
// terms of the logic 0 half bit because it does not encode any useful
// information.  On the other hand, we going into the logic 1 half of the bit
// and the portion which determines the bit value, the start of the
// packet, or if the timer rolls over, the end of the ir packet.
// Since the duration of this half-bit determines the outcome
// we will turn on the timer and its associated interrupt.
// -----------------------------------------------------------------------
#pragma vector = PORT2_VECTOR			// This is from the MSP430G2553.h file

__interrupt void pinChange (void) {

	int8	pin;
	int16	pulseDuration;			// The timer is 16-bits

	if (IR_PIN)		pin=1;	else pin=0;

	switch (pin) {					// read the current pin level
		case 0:						// !!!!!!!!!NEGATIVE EDGE!!!!!!!!!!
			pulseDuration = TAR;

			irPacket<<=1;

			if((pulseDuration > minLogic1Pulse) && (pulseDuration < maxLogic1Pulse)){	// Length of TAR is at least as long as our logic 1 threshold
				//irPacket = (irPacket<<=1);
				irPacket += 1;	// set the next bit of the packet to a 1
			/*}else if(pulseDuration < maxLogic0Pulse){	// Length of TAR is shorter than the logic 0 threshold
				irPacket = (irPacket<<=1);
				irPacket += 0;	// set the next bit of the packet to 0
				*/
			}

			packetData[packetIndex++] = pulseDuration;
			//TACTL = MC_0;	// Turn the timer off to make changes
			TAR = 0x0000;	// redundant, reset the TAR so that I can count up from 0 later
			LOW_2_HIGH; 				// Setup pin interrupr on positive edge
			break;

		case 1:							// !!!!!!!!POSITIVE EDGE!!!!!!!!!!!
			TAR = 0x0000;						// time measurements are based at time 0
			TACTL |= MC_0;
			TACCR0 = 0xFFFF;
			TACTL &= ~TAIFG;
			//TACTL = ID_3 | TASSEL_2 | MC_1 | TAIE;
			TACTL |= TAIE;
			HIGH_2_LOW; 						// Setup pin interrupr on positive edge
			break;
	} // end switch

	P2IFG &= ~BIT6;			// Clear the interrupt flag to prevent immediate ISR re-entry

} // end pinChange ISR



// -----------------------------------------------------------------------
//			0 half-bit	1 half-bit		TIMER A COUNTS		TIMER A COUNTS
//	Logic 0	xxx
//	Logic 1
//	Start
//	End
//
// -----------------------------------------------------------------------
#pragma vector = TIMER0_A1_VECTOR			// This is from the MSP430G2553.h file
__interrupt void timerOverflow (void) {

	//TACTL = MC_0;	// Turn off the timer to make changes
	TAR = 0x0000;	// Reset my timer variable
	TACTL &= ~TAIE;
	newIrPacket = TRUE;	// Set the flag so that the packet can be read
	packetIndex = 0;	// reset packet index
	TACTL &= ~TAIFG;	// Clear interrupt flag
}
