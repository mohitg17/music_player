#ifndef MUSIC_H
#define MUSIC_H
#include <stdint.h>

void Timer0A_Init(uint16_t (*envelope)(uint32_t, uint32_t, uint32_t, uint16_t));
void SysTickInit(void (*musicPlay)());
void Timer2A_Init(void (*harmonyPlay)());
void SongInit(void);
void setIndexZero(void);

void enableTimers(void);
void disableTimers(void);

#endif
