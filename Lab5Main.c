// Lab5Main.c
// Runs on LM4F120/TM4C123

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "../inc/PLL.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"
#include "Switch.h"
#include "DAC.h"
#include "Music.h"

uint32_t Index;
uint8_t Playing = 0;
uint8_t Inst = 0;

const unsigned short Wave[64] = {
	32,35,38,41,44,47,49,52,54,56,58,
  59,61,62,62,63,63,63,62,62,61,59,
  58,56,54,52,49,47,44,41,38,35,
  32,29,26,23,20,17,15,12,10,8,
  6,5,3,2,2,1,1,1,2,2,3,
  5,6,8,10,12,15,17,20,23,26,29
};

const unsigned short TrumpetWave[64] = {
	987, 1049, 1090, 1110, 1134, 1160, 1139, 1092, 1070, 
	1042, 1035, 1029, 1008, 1066, 1150, 1170, 1087, 915, 679,
	372, 151, 558, 1014, 1245, 1260, 1145, 1063, 984, 934, 960,
	1027, 1077, 1081, 1074, 1064, 1042, 1010, 974, 968, 974, 994,
	1039, 1094, 1129, 1125, 1092, 1056, 1056, 1082, 1059, 1046, 
	1058, 1061, 1045, 1034, 1050, 1094, 1112, 1092, 1063, 1053, 
	1065, 1052, 992
};

void playPause() {
	Playing ^= 1;
	switch(Playing) {
		case 0:
			disableTimers();
			break;
		case 1:
			enableTimers();
			break;
	}
}

void rewind() {
	setIndexZero();
}

void setInst() {
	Inst = (Inst + 1)%2;
}

void MusicPlay() {
	Index = (Index+1)%64; // 0 to 63
	switch(Inst) {
		case 0:
			DAC_Out(Wave[Index]);
			break;
		case 1:
			DAC_Out(TrumpetWave[Index]);
			break;
	}
}

//debug code
int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 80 MHz
  LaunchPad_Init();                // activate port F
	SwitchInit(&playPause, &rewind, &setInst);
	SongInit();
	DAC_Init(0);
	SysTickInit(&MusicPlay);
	Timer0A_Init(0);
  EnableInterrupts();
	
  while(1){
    WaitForInterrupt();
  }
}
