#include <unity.h>

#include "WavData.h"
#include "mock_helpers.h"

void test_wav_data_pointers()
{
    // Test that we have the correct number of sound files
    TEST_ASSERT_GREATER_THAN(0, NUM_SOUND_FILES);

    // Test that all pointers are not null
    for (int i = 0; i < NUM_SOUND_FILES; i++)
    {
        TEST_ASSERT_NOT_NULL(sound_data_pointers[i]);
    }

    // Test that all sizes are non-zero
    for (int i = 0; i < NUM_SOUND_FILES; i++)
    {
        TEST_ASSERT_GREATER_THAN(0, sound_data_sizes[i]);
    }
}

void test_wav_data_sizes()
{
    // Test that each WAV file has a valid size
    TEST_ASSERT_GREATER_THAN(0, wav_chat_01_size);
    TEST_ASSERT_GREATER_THAN(0, wav_chat_02_size);
    TEST_ASSERT_GREATER_THAN(0, wav_excited_03_size);
}

void runWavDataTests()
{
    UNITY_BEGIN();
    RUN_TEST(test_wav_data_pointers);
    RUN_TEST(test_wav_data_sizes);
    UNITY_END();
}
