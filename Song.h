#ifndef SONG_H
#define SONG_H
#include <stdint.h>

const uint32_t size = 1000; 

struct Note {
	uint32_t pitch;
	uint32_t duration;
};

struct Note Notes[size];

#endif
