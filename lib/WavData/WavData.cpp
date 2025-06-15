/**
 * @file WavData.cpp
 * @brief Implementation of WAV audio data storage for Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This file contains the actual WAV file data stored in program memory (PROGMEM).
 * The WAV files are optimized for embedded playback with the following specs:
 * - Format: 16-bit PCM
 * - Sample Rate: 22.05kHz
 * - Channels: Mono
 * - Bitrate: 352.8 kbps
 *
 * The audio files are stored in separate .cpp files and included here to keep
 * the main codebase clean and maintainable.
 */

#include "WavData.h"

// Include the generated WAV data files
// These are generated from actual WAV files using a conversion tool
#include "wav_chat_01.h"
#include "wav_chat_02.h"
#include "wav_excited_03.h"
#include "wav_excited_04.h"
#include "wav_sad_01.h"
#include "wav_sad_02.h"
#include "wav_sad_03.h"
#include "wav_powerup.h"
#include "wav_question_02.h"
#include "wav_question_03.h"
#include "wav_question_04.h"

// Array of pointers to all WAV data files in PROGMEM
// The order of files in this array must match the order in sound_data_sizes
const uint8_t* const sound_data_pointers[NUM_SOUND_FILES] = {
    // Chat sounds
    wav_chat_01_data, wav_chat_02_data,

    // Excited sounds
    wav_excited_03_data, wav_excited_04_data,

    // System sounds
    wav_powerup_data, wav_question_02_data, wav_question_03_data, wav_question_04_data,

    // Sad sounds
    wav_sad_01_data, wav_sad_02_data, wav_sad_03_data};

// Array of sizes (in bytes) for all WAV data files
// The order of sizes in this array must match the order in sound_data_pointers
const size_t sound_data_sizes[NUM_SOUND_FILES] = {
    // Chat sounds
    wav_chat_01_size, wav_chat_02_size,

    // Excited sounds
    wav_excited_03_size, wav_excited_04_size,

    // System sounds
    wav_powerup_size, wav_question_02_size, wav_question_03_size, wav_question_04_size,

    // Sad sounds
    wav_sad_01_size, wav_sad_02_size, wav_sad_03_size};

// Static assertions to ensure data consistency
static_assert(sizeof(sound_data_pointers) / sizeof(sound_data_pointers[0]) == NUM_SOUND_FILES,
              "Mismatch between NUM_SOUND_FILES and sound_data_pointers array size");

static_assert(sizeof(sound_data_sizes) / sizeof(sound_data_sizes[0]) == NUM_SOUND_FILES,
              "Mismatch between NUM_SOUND_FILES and sound_data_sizes array size");
