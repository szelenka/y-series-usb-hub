#include "WavData.h"

// WAV data stored in PROGMEM
const uint8_t sound1_data[] PROGMEM = {
    // WAV data here
    0x10
};
const size_t sound1_size = sizeof(sound1_data);

const uint8_t sound2_data[] PROGMEM = {
    // WAV data here
    0x20
};
const size_t sound2_size = sizeof(sound2_data);

const uint8_t sound3_data[] PROGMEM = {
    // WAV data here
    0x30
};
const size_t sound3_size = sizeof(sound3_data);

// WAV data pointers
const uint8_t* sound_data_pointers[NUM_SOUND_FILES] = {
    sound1_data,
    sound2_data,
    sound3_data
};

const size_t sound_data_sizes[NUM_SOUND_FILES] = {
    sound1_size,
    sound2_size,
    sound3_size
};
