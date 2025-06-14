/**
 * @file WavData.h
 * @brief WAV audio data storage and management for the Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This module provides storage for WAV audio files in program memory (PROGMEM)
 * and utilities to access them. It's designed for use with audio playback
 * systems on memory-constrained embedded devices.
 *
 * Features:
 * - Stores WAV audio data in program memory (PROGMEM) to conserve RAM
 * - Provides easy access to audio data and metadata
 * - Supports multiple audio files with efficient storage
 * - Designed for use with the Y-Series USB Hub audio system
 */

#ifndef Y_SERIES_USB_HUB_WAV_DATA_H
#define Y_SERIES_USB_HUB_WAV_DATA_H

// System includes
#include <Arduino.h>

/**
 * @brief Number of available sound files in the system
 */
static constexpr uint8_t NUM_SOUND_FILES = 11;

// Forward declarations for WAV data stored in PROGMEM
// These are defined in their respective .cpp files

/// @name Chat Sounds
/// @{
extern const uint8_t wav_chat_01_data[] PROGMEM;
extern const size_t wav_chat_01_size;

extern const uint8_t wav_chat_02_data[] PROGMEM;
extern const size_t wav_chat_02_size;
/// @}

/// @name Excited Sounds
/// @{
extern const uint8_t wav_excited_03_data[] PROGMEM;
extern const size_t wav_excited_03_size;

extern const uint8_t wav_excited_04_data[] PROGMEM;
extern const size_t wav_excited_04_size;
/// @}

/// @name Sad Sounds
/// @{
extern const uint8_t wav_sad_01_data[] PROGMEM;
extern const size_t wav_sad_01_size;

extern const uint8_t wav_sad_02_data[] PROGMEM;
extern const size_t wav_sad_02_size;

extern const uint8_t wav_sad_03_data[] PROGMEM;
extern const size_t wav_sad_03_size;
/// @}

/// @name System Sounds
/// @{
extern const uint8_t wav_powerup_data[] PROGMEM;
extern const size_t wav_powerup_size;

extern const uint8_t wav_question_02_data[] PROGMEM;
extern const size_t wav_question_02_size;

extern const uint8_t wav_question_03_data[] PROGMEM;
extern const size_t wav_question_03_size;

extern const uint8_t wav_question_04_data[] PROGMEM;
extern const size_t wav_question_04_size;
/// @}

/**
 * @brief Array of pointers to all WAV data files in PROGMEM
 *
 * @note The order of files in this array must match the order in sound_data_sizes
 */
extern const uint8_t* const sound_data_pointers[NUM_SOUND_FILES];

/**
 * @brief Array of sizes (in bytes) for all WAV data files
 *
 * @note The order of sizes in this array must match the order in sound_data_pointers
 */
extern const size_t sound_data_sizes[NUM_SOUND_FILES];

/**
 * @brief Get a pointer to the WAV data for a specific sound
 *
 * @param[in] index Index of the sound (0 to NUM_SOUND_FILES-1)
 * @return const uint8_t* Pointer to the WAV data in PROGMEM, or nullptr if index is invalid
 */
inline const uint8_t* getWavData(uint8_t index)
{
    return (index < NUM_SOUND_FILES) ? sound_data_pointers[index] : nullptr;
}

/**
 * @brief Get the size of a specific WAV file
 *
 * @param[in] index Index of the sound (0 to NUM_SOUND_FILES-1)
 * @return size_t Size of the WAV data in bytes, or 0 if index is invalid
 */
inline size_t getWavSize(uint8_t index)
{
    return (index < NUM_SOUND_FILES) ? sound_data_sizes[index] : 0;
}

/**
 * @brief Get the total number of available sound files
 *
 * @return uint8_t Number of sound files available
 */
constexpr inline uint8_t getNumSoundFiles()
{
    return NUM_SOUND_FILES;
}

#endif  // Y_SERIES_USB_HUB_WAV_DATA_H
