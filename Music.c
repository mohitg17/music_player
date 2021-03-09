#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"
#include "Music.h"
#include "DAC.h"

uint32_t SongIndex;
void (*MusicPlay)(void);

void setIndexZero() {
  SongIndex = 0;
}

void disableTimers() {
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; 			// disable timer0A
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
}

void enableTimers() {
  TIMER0_CTL_R |= TIMER_CTL_TAEN;    		// enable timer0A 32-b, periodic, interrupts
	NVIC_ST_CTRL_R = 0x07;								// enable SysTick with core clock
}

void SongInit() {
	for(int i = 0; i < SIZE; i++) {
		struct Note n = {pitches[i], durations[i]};
		Notes[i] = n;
	}
}

void SysTickInit(void (*musicPlay)()){
	MusicPlay = musicPlay;
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 1;  			      // maximum reload value
  NVIC_SYS_PRI3_R = ((NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000);
//	NVIC_ST_CTRL_R = 0x07;								// enable SysTick with core clock
}

void SysTick_Handler() {
	MusicPlay();
}

void Timer0A_Init(uint32_t duration){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      				// activate timer0
  delay = SYSCTL_RCGCTIMER_R;      				// allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; 				// disable timer0A during setup
  TIMER0_CFG_R = 0;                				// configure for 32-bit timer mode
  // **** timer0A initialization ****
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;	// configure for periodic mode
  TIMER0_TAILR_R = duration;         			// start value for 1 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;  			// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT; 			// clear timer0A timeout flag
//  TIMER0_CTL_R |= TIMER_CTL_TAEN;    			// enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
																					// Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              				// enable interrupt 19 in NVIC
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  TIMER0_TAILR_R = Notes[SongIndex].duration;
	NVIC_ST_RELOAD_R = Notes[SongIndex].pitch;
	SongIndex = (SongIndex+1)%SIZE;
}

