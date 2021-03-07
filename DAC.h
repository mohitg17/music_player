// DAC.h
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Hardware connections

#ifndef DAC_H
#define DAC_H
#include <stdint.h>

// **************DAC_Init*********************
// Initialize 12-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void);


// **************DAC_Out*********************
// output to DAC
// Input: 12-bit data
// Output: none
void DAC_Out(uint8_t data);

#endif
