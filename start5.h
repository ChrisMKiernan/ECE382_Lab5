//-----------------------------------------------------------------
// Name:	Chris Kiernan
// File:	lab5.h
// Date:	Fall 2014
// Purp:	Include file for the MSP430 for IR remote sensors
//			modified from Dr Coulston's code
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Page 76 : MSP430 Optimizing C/C++ Compiler v 4.3 User's Guide
//-----------------------------------------------------------------
typedef		unsigned char		int8;
typedef		unsigned short		int16;
typedef		unsigned long		int32;
typedef		unsigned long long	int64;

#define		TRUE				1
#define		FALSE				0

//-----------------------------------------------------------------
// Function prototypes found in lab5.c
//-----------------------------------------------------------------
void initMSP430();
__interrupt void pinChange (void);
__interrupt void timerOverflow (void);


//-----------------------------------------------------------------
// Each PxIES bit selects the interrupt edge for the corresponding I/O pin.
//	Bit = 0: The PxIFGx flag is set with a low-to-high transition
//	Bit = 1: The PxIFGx flag is set with a high-to-low transition
//-----------------------------------------------------------------

#define		IR_PIN			(P2IN & BIT6)
#define		HIGH_2_LOW		P2IES |= BIT6
#define		LOW_2_HIGH		P2IES &= ~BIT6


#define		averageLogic0Pulse	0500
#define		averageLogic1Pulse	1600
#define		averageStartPulse	4500
#define		minLogic0Pulse		averageLogic0Pulse - 160
#define		maxLogic0Pulse		averageLogic0Pulse + 160
#define		minLogic1Pulse		averageLogic1Pulse - 160
#define		maxLogic1Pulse		averageLogic1Pulse + 160
#define		minStartPulse		averageStartPulse - 160
#define		maxStartPulse		averageStartPulse + 160

//#define		PWR		0x02FD48B7
#define		ZER		0xE144728D
#define		ONE		0xE144A25D
/*#define		TWO		0x02FD40BF
#define		THR		0x02FDC03F

#define		VOL_UP	0x02FD58A7
#define		VOL_DW	0x02FD7887
#define		CH_UP	0x02FDD827
#define		CH_DW	0x02FDF807
*/
