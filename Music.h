#ifndef MUSIC_H
#define MUSIC_H
#include <stdint.h>

#define G0 6378   // 195.998 Hz
#define C0 4778   // 261.626 Hz
#define DF 4510   // 277.183 Hz
#define D 4257   // 293.665 Hz
#define EF 4018   // 311.127 Hz
#define E 3792   // 329.628 Hz
#define F 3579   // 349.228 Hz
#define GF 3378   // 369.994 Hz
#define G 3189   // 391.995 Hz

#define half 80000000
#define quarter 40000000
#define eighth 20000000

const uint16_t SIZE = 62;

void Timer0A_Init(uint32_t duration);
void SysTickInit(void (*musicPlay)());
void SongInit();

void setIndexZero();
void enableTimers();
void disableTimers();

struct Note {
	uint32_t pitch;
	uint32_t duration;
};

struct Note Notes[SIZE];

uint32_t pitches[SIZE] = {
	E, E, F, G, G, F, E, D, C0, C0, D, E, E, D, D,
	E, E, F, G, G, F, E, D, C0, C0, D, E, D, C0, C0,
	D, D, E, C0, D, E, F, E, C0, D, E, F, E, D, C0, D, G0,
  E, E, F, G, G, F, E, D, C0, C0, D, E, D, C0, C0
};

uint32_t durations[SIZE] = {
	quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, half,
	quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, half,
	quarter, quarter, quarter, quarter, quarter, eighth, eighth, quarter, quarter, quarter, eighth, eighth, quarter, quarter, quarter, quarter, half,
	quarter, quarter, quarter, quarter, quarter, quarter, quarter,quarter, quarter, quarter, quarter, quarter, quarter, quarter, half
};

#endif
