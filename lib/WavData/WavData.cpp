/**
 * @file WavData.cpp
 * @brief Implementation of WAV audio data storage
 *
 * This file contains the actual WAV file data stored in program memory.
 * The WAV files are 16-bit, 22.05kHz, mono for optimal memory usage.
 */

#include "WavData.h"

#include "wav_chat_01.cpp"
#include "wav_chat_02.cpp"
#include "wav_excited_03.cpp"
#include "wav_excited_04.cpp"
#include "wav_powerup.cpp"
#include "wav_question_02.cpp"
#include "wav_question_03.cpp"
#include "wav_question_04.cpp"
#include "wav_sad_01.cpp"
#include "wav_sad_02.cpp"
#include "wav_sad_03.cpp"

// Array of pointers to all WAV data files
const uint8_t* sound_data_pointers[NUM_SOUND_FILES] = {
    wav_chat_01_data, wav_chat_02_data,     wav_excited_03_data,  wav_excited_04_data,
    wav_powerup_data, wav_question_02_data, wav_question_03_data, wav_question_04_data,
    wav_sad_01_data,  wav_sad_02_data,      wav_sad_03_data};

// Array of sizes for all WAV data files
const size_t sound_data_sizes[NUM_SOUND_FILES] = {
    wav_chat_01_size, wav_chat_02_size,     wav_excited_03_size,  wav_excited_04_size,
    wav_powerup_size, wav_question_02_size, wav_question_03_size, wav_question_04_size,
    wav_sad_01_size,  wav_sad_02_size,      wav_sad_03_size};
