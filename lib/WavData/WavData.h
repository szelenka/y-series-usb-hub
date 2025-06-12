/**
 * @file WavData.h
 * @brief WAV audio data storage and management for embedded systems
 * 
 * This module provides storage for WAV audio files in program memory (PROGMEM)
 * and utilities to access them. It's designed for use with audio playback
 * systems on memory-constrained embedded devices.
 */

#ifndef WAVDATA_H
#define WAVDATA_H

#include <Arduino.h>

/// @brief Number of available sound files
static constexpr int NUM_SOUND_FILES = 11;

// WAV data stored in PROGMEM
extern const uint8_t wav_chat_01_data[] PROGMEM;
extern const size_t wav_chat_01_size;

extern const uint8_t wav_chat_02_data[] PROGMEM;
extern const size_t wav_chat_02_size;

extern const uint8_t wav_excited_03_data[] PROGMEM;
extern const size_t wav_excited_03_size;

extern const uint8_t wav_excited_04_data[] PROGMEM;
extern const size_t wav_excited_04_size;

extern const uint8_t wav_sad_01_data[] PROGMEM;
extern const size_t wav_sad_01_size;

extern const uint8_t wav_sad_02_data[] PROGMEM;
extern const size_t wav_sad_02_size;

extern const uint8_t wav_sad_03_data[] PROGMEM;
extern const size_t wav_sad_03_size;

extern const uint8_t wav_powerup_data[] PROGMEM;
extern const size_t wav_powerup_size;

extern const uint8_t wav_question_02_data[] PROGMEM;
extern const size_t wav_question_02_size;

extern const uint8_t wav_question_03_data[] PROGMEM;
extern const size_t wav_question_03_size;

extern const uint8_t wav_question_04_data[] PROGMEM;
extern const size_t wav_question_04_size;

/// @brief Array of pointers to all WAV data files
extern const uint8_t* sound_data_pointers[NUM_SOUND_FILES];

/// @brief Array of sizes for all WAV data files
extern const size_t sound_data_sizes[NUM_SOUND_FILES];

#endif // WAVDATA_H
