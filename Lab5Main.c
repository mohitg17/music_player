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

uint32_t Index = 0;
uint8_t Playing = 0;
uint8_t Inst = 0;

const uint16_t Wave[64] = {  
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
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
	Timer0A_Init(1);
  EnableInterrupts();
	
  while(1){
    WaitForInterrupt();
  }
}
