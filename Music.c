#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"

#define G0 6378   // 195.998 Hz
#define B0 3164   // 246.942 Hz
#define C0 4778   // 261.626 Hz
#define DF 4510   // 277.183 Hz
#define D 4257   // 293.665 Hz
#define EF 4018   // 311.127 Hz
#define E 3792   // 329.628 Hz
#define F 3579   // 349.228 Hz
#define GF 3378   // 369.994 Hz
#define G 3189   // 391.995 Hz

#define half 80000000
#define dottedquarter 60000000
#define quarter 40000000
#define eighth 20000000

#define A0 3551

void (*MusicPlay)(void);
void (*HarmonyPlay)(void);

const uint16_t SIZE = 62;
uint32_t SongIndex = 0;


struct Note {
	uint32_t pitch;
	uint32_t duration;
	uint32_t harmony;
};

struct Note Notes[SIZE];

uint32_t pitches[SIZE] = {
	E, E, F, G, G, F, E, D, C0, C0, D, E, E, D, D,
	E, E, F, G, G, F, E, D, C0, C0, D, E, D, C0, C0,
	D, D, E, C0, D, E, F, E, C0, D, E, F, E, D, C0, D, G0,
  E, E, F, G, G, F, E, D, C0, C0, D, E, D, C0, C0
};

uint32_t harmony[SIZE] = {
	C0, C0, D, E, E, D, C0, B0, A0, A0, B0, C0, C0, B0, B0,
	C0, C0, D, E, E, D, C0, B0, A0, A0, B0, C0, B0, A0, A0,
	B0, B0, C0, A0, B0, C0, D, C0, A0, B0, C0, D, C0, A0, B0, C0,
	C0, C0, D, E, E, D, C0, B0, A0, A0, B0, C0, B0, A0, A0
};

uint32_t durations[SIZE] = {
	quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, dottedquarter, eighth, half,
	quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, dottedquarter, eighth, half,
	quarter, quarter, quarter, quarter, quarter, eighth, eighth, quarter, quarter, quarter, eighth, eighth, quarter, quarter, quarter, quarter, half,
	quarter, quarter, quarter, quarter, quarter, quarter, quarter,quarter, quarter, quarter, quarter, quarter, dottedquarter, eighth, half
};

void SongInit() {
	for(int i = 0; i < SIZE; i++) {
		struct Note n = {pitches[i], durations[i], harmony[i]};
		Notes[i] = n;
	}
}

void setIndexZero() {
  SongIndex = 0;
}

void disableTimers() {
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; 			// disable timer0A
	TIMER2_CTL_R = 0x00000000;            // disable timer2A
	NVIC_ST_CTRL_R = 0;                   // disable SysTick
}

void enableTimers() {
  TIMER0_CTL_R |= TIMER_CTL_TAEN;    		// enable timer0A 32-b, periodic, interrupts
	TIMER2_CTL_R = 0x00000001;            // enable timer2A
	NVIC_ST_CTRL_R = 0x07;								// enable SysTick with core clock
}

void SysTickInit(void (*musicPlay)()){
	MusicPlay = musicPlay;
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 3792;  			      // maximum reload value
  NVIC_SYS_PRI3_R = ((NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000);
	NVIC_ST_CTRL_R = 0x07;								// enable SysTick with core clock
}

void SysTick_Handler() {
	MusicPlay();
}

void Timer0A_Init(){
  SYSCTL_RCGCTIMER_R |= 0x01;      // 0) activate timer0
  TIMER0_CTL_R &= ~0x00000001;     // 1) disable timer0A during setup
  TIMER0_CFG_R = 0x00000000;       // 2) configure for 32-bit timer mode
  TIMER0_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = 80000-1;       // 4) reload value
  TIMER0_TAPR_R = 0;               // 5) 12.5ns timer0A
  TIMER0_ICR_R = 0x00000001;       // 6) clear timer0A timeout flag
  TIMER0_IMR_R |= 0x00000001;      // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|(2<<29); 
  NVIC_EN0_R = 1<<19;     // 9) enable interrupt 19 in NVIC
  TIMER0_CTL_R |= 0x00000001;      // 10) enable timer0A
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;           // acknowledge timer0A timeout
	TIMER0_TAILR_R = Notes[SongIndex].duration;
	NVIC_ST_RELOAD_R = Notes[SongIndex].pitch;
	TIMER2_TAILR_R = Notes[SongIndex].harmony;
	
	SongIndex = (SongIndex+1)%SIZE;
}

// ***************** Timer2A_Init ****************
// Activate Timer2 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
//          priority 0 (highest) to 7 (lowest)
// Outputs: none
void Timer2A_Init(void (*harmonyPlay)()){
	HarmonyPlay = harmonyPlay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = 7999;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|(2<<29); // priority  
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

void Timer2A_Handler(){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER2A timeout
	HarmonyPlay();
}

