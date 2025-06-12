#ifndef WAVDATA_H
#define WAVDATA_H

#include <Arduino.h>

// WAV data stored in PROGMEM
extern const uint8_t sound1_data[] PROGMEM;
extern const size_t sound1_size;

extern const uint8_t sound2_data[] PROGMEM;
extern const size_t sound2_size;

extern const uint8_t sound3_data[] PROGMEM;
extern const size_t sound3_size;

// WAV data pointers (referenced from WavData.h)
extern const uint8_t* sound_data_pointers[];
extern const size_t sound_data_sizes[];

// Number of sound files
static constexpr int NUM_SOUND_FILES = 3;

#endif // WAVDATA_H
