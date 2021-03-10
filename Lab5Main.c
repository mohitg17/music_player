// Lab5Main.c
// Runs on LM4F120/TM4C123

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "../inc/PLL.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"
#include "../inc/ST7735.h"
#include "../inc/Texas.h"
#include "Switch.h"
#include "DAC.h"
#include "Music.h"

uint32_t Index = 0;
//uint32_t harmonyIndex = 0;
uint8_t Playing = 0;
uint8_t Inst = 0;
uint8_t Counter = 0;
uint8_t scale[16] = {8, 4, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 6, 7, 8}; 

	
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

const unsigned short Bassoon64[64]={
1068, 1169, 1175, 1161, 1130, 1113, 1102, 1076, 1032, 985, 963, 987, 1082, 1343, 1737, 1863, 
1575, 1031, 538, 309, 330, 472, 626, 807, 1038, 1270, 1420, 1461, 1375, 1201, 1005, 819, 658, 
532, 496, 594, 804, 1055, 1248, 1323, 1233, 1049, 895, 826, 826, 850, 862, 861, 899, 961, 1006, 
1023, 1046, 1092, 1177, 1224, 1186, 1133, 1098, 1102, 1109, 1076, 1027, 1003};

const unsigned short Oboe64[64]={
1024, 1024, 1014, 1008, 1022, 1065, 1093, 1006, 858, 711, 612, 596, 672, 806, 952, 1074, 1154, 1191, 
1202, 1216, 1236, 1255, 1272, 1302, 1318, 1299, 1238, 1140, 1022, 910, 827, 779, 758, 757, 782, 856, 
972, 1088, 1177, 1226, 1232, 1203, 1157, 1110, 1067, 1028, 993, 958, 929, 905, 892, 900, 940, 1022, 
1125, 1157, 1087, 965, 836, 783, 816, 895, 971, 1017};

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
	Inst = (Inst + 1)%4;
}

void musicPlay() {
	Index = (Index+1)%64; // 0 to 63
	switch(Inst) {
		case 0:
			DAC_Out((Wave[Index]-1250)/scale[Counter] + 1250);
			break;
		case 1:
			DAC_Out((TrumpetWave[Index]-650)/scale[Counter] + 650);
			break;
		case 2:
			DAC_Out((Bassoon64[Index]-1000)/scale[Counter] + 1000);
			break;
		case 3:
			DAC_Out((Oboe64[Index]-1000)/scale[Counter] + 1000);
			break;
	}
}

uint16_t envelope(uint32_t duration, uint32_t pitch, uint32_t songIndex, uint16_t size) {
	Counter += 1;
	if(Counter == duration/5000000) {
		Counter = 0;
		NVIC_ST_RELOAD_R = pitch;
		songIndex = (songIndex+1)%size;
	}
	return songIndex;
}

//void musicPlay() {
//	Index = (Index+1)%64; // 0 to 63
//	switch(Inst) {
//		case 0:
//			DAC_Out(Wave[Index]+Wave[harmonyIndex]);
//			break;
//		case 1:
//			DAC_Out(TrumpetWave[Index]+TrumpetWave[harmonyIndex]);
//			break;
//		case 2:
//			DAC_Out(Bassoon64[Index]+Bassoon64[harmonyIndex]);
//			break;
//		case 3:
//			DAC_Out(Oboe64[Index]+Oboe64[harmonyIndex]);
//	}
//}

//void harmonyPlay() {
//	harmonyIndex = (harmonyIndex+1)%64; // 0 to 63
//	switch(Inst) {
//		case 0:
//			DAC_Out(Wave[Index]+Wave[harmonyIndex]);
//			break;
//		case 1:
//			DAC_Out(TrumpetWave[Index]+TrumpetWave[harmonyIndex]);
//			break;
//		case 2:
//			DAC_Out(Bassoon64[Index]+Bassoon64[harmonyIndex]);
//			break;
//		case 3:
//			DAC_Out(Oboe64[Index]+Oboe64[harmonyIndex]);
//	}
//}

void LED_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x00000020;
	while((SYSCTL_RCGCGPIO_R & 0x00000020) == 0) {};
		
	GPIO_PORTF_DIR_R = 0x04;			// make PF2 output
	GPIO_PORTF_DEN_R = 0x04;			// enable digital I/O on PF2
	GPIO_PORTF_PCTL_R &= ~0x00000F00; // configure PF2 as GPIO
}


//debug code
int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 80 MHz
  LaunchPad_Init();                // activate port F
	SwitchInit(&playPause, &rewind, &setInst);
	SongInit();
	DAC_Init(0);
	SysTickInit(&musicPlay);
	Timer0A_Init(&envelope);
	// Timer2A_Init(&harmonyPlay);
	TExaS_Init(SCOPE_PD2,80000000);
  EnableInterrupts();
	LED_Init();
	GPIO_PORTF_DATA_R = 0x04;
	

  while(1){
		WaitForInterrupt();
	}
}
