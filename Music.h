#ifndef MUSIC_H
#define MUSIC_H
#include <stdint.h>

void Timer0A_Init(void);
void SysTickInit(void (*musicPlay)());
void Timer2A_Init(void (*harmonyPlay)(), void(*resetEnvelope)());
void Timer3A_Init(void(*envelope)(void));
void SongInit(void);
void setIndexZero(void);

void enableTimers(void);
void disableTimers(void);

#endif
