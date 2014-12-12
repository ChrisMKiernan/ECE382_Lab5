## Lab 5 - IR Remote Sensing

Objective - to detect an incoming signal from the IR remote, and to decode the value to interpret which button was pressed.

### Testing

The first step in the lab was to test the IR remote in order to obtain several relevant values. The values we needed were lengths of logic high and low pulses as averages, standard deviations of those values, and the 32-bit strings of a few different buttons. In order to retrieve these values, the IR sensor was connected to the logic analyzer and a button was pressed. In the "test5.c" file, pressing a button would result in the timer storing values of how long the signal went logic high or logic low. These values were then compared to lengths of time the same signal went logic high or logic low in order to verify. By using these techniques, we could find how long a high or low pulse corresponding to a 1 or 0 was. Putting several of these values together resulted in 32-bit strings of 1s and 0s, wherein there is a unique string to each button on the remote.

### Design

In this lab, it was important to use a .h file to store values. This, more than any other lab, was full of constants that were relevant to the code. Rather than "dump" all of those constants into the .c file, they were implemented into the .h file to compartmentalize and to be more organized. A shell, "start5.c" was provided for the lab, which already had several initializations and some timer setup code. All that was left to do was to collect the values and create an interrupt service routine.

To collect the values from the timer (telling us whether a 1 or 0 was received), the timer value had to be collected for each individual high pulse (the low pulses were all ignored - they were determined by what edge was present). The value of the timer was checked against a known value (and error margin) of a logic 1. If the values matched, then a 1 was pushed into the array and the whole array was shifted one bit to move on to the next value. Once the index of the array reached its maximum value, the interrupt flag was raised, and the program moved to the ISR.

ISR---
