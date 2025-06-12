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

// WAV file format reference:
// Offset  Size  Description
// 0       4     "RIFF" marker
// 4       4     File size - 8
// 8       4     "WAVE"
// 12      4     "fmt " chunk
// 16      4     16 (size of fmt chunk)
// 20      2     Audio format (1 = PCM)
// 22      2     Number of channels (1 = mono)
// 24      4     Sample rate (e.g., 22050)
// 28      4     Byte rate (sample rate * channels * bits per sample / 8)
// 32      2     Block align (channels * bits per sample / 8)
// 34      2     Bits per sample (16)
// 36      4     "data" chunk
// 40      4     Data size
// 44      *     Raw PCM data

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
