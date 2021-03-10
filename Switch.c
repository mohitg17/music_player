// hardware connections
// SW1 connected to PB4, 3.3V, 10k resistor, ground
// SW2 connected to PB5, 3.3V, 10k resistor, ground
// SW3 connected to PB7, 3.3V, 10k resistor, ground

#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"

void (*PlayPause)();
void (*Rw)();
void (*SetInst)();

static void GPIOArm(){
  GPIO_PORTB_ICR_R = 0x38;      // (e) clear flag
  GPIO_PORTB_IM_R |= 0x38;      // (f) arm interrupts
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFF00FFF)|0x00002000; // (g) priority 1
  NVIC_EN0_R = 0x00000002;      // (h) enable interrupt 1 in NVIC	
}

void SwitchInit(void(*playPause)(), void(*rw)(), void(*setInst)()){
	SYSCTL_RCGCGPIO_R |= 0x00000002;
	while((SYSCTL_RCGCGPIO_R & 0x00000002) == 0) {};
		
	PlayPause = playPause;
	Rw = rw;
	SetInst = setInst;

	GPIO_PORTB_CR_R = 0x38;				// allow changes to PB5-3
	GPIO_PORTB_AMSEL_R &= ~0x38;	// disable analog functionality on PB5-3	
	GPIO_PORTB_DIR_R = 0xC7;			// make PB5-3 inputs
	GPIO_PORTB_DEN_R = 0x38;			// enable digital I/O on PB5-3
	GPIO_PORTB_PCTL_R &= ~0x00FFF000; // configure PB5-3 as GPIO
  GPIO_PORTB_IS_R &= ~0x38;     // (d) PB7-3 is edge-sensitive
  GPIO_PORTB_IBE_R &= ~0x38;    //     PB7-3 is not both edges
  GPIO_PORTB_IEV_R |= 0x38;     //     PB7-3 rising edge event
  GPIO_PORTB_ICR_R = 0x38;      // (e) clear flag5-3
	GPIO_PORTB_IM_R |= 0x38;      // (f) arm interrupt on PB5-3 *** No IME bit as mentioned in Book ***

	NVIC_PRI0_R = (NVIC_PRI0_R&0xFFF00FFF)|0x00002000; // (g) priority 1
  NVIC_EN0_R = 0x00000002;      // (h) enable interrupt 1 in NVIC
}

// ***************** TIMER1A_Init ****************
// Activate TIMER1A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
//          priority 0 (highest) to 7 (lowest)
// Outputs: none
void Timer1A_Init(){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x0000001;    // 3) 1-SHOT mode
  TIMER1_TAILR_R = 20000000;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|(4<<13); // priority 
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer1A_Handler(void){
  TIMER1_IMR_R = 0x00000000;    // disarm timeout interrupt
	GPIOArm();                    // re-arm switch interrupts
}

void GPIOPortB_Handler(void){
	if(GPIO_PORTB_RIS_R & 0x08) {    // if SW3 pressed, play/pause track
		GPIO_PORTB_IM_R &= ~0x08;
		(*PlayPause)();
		Timer1A_Init();                // one-shot timer initialization for debouncing
	}
	
	if(GPIO_PORTB_RIS_R & 0x20) {    // if SW2 pressed, rewind track
		GPIO_PORTB_IM_R &= ~0x20;
		(*Rw)();
		Timer1A_Init();                // one-shot timer initialization for debouncing
	}
	
	if(GPIO_PORTB_RIS_R & 0x10) {    // if SW1 pressed, change instrument
		GPIO_PORTB_IM_R &= ~0x10;
		(*SetInst)();
		Timer1A_Init();                // one-shot timer initialization for debouncing
	}
}
