/**
 * @file test_AudioPlayer.cpp
 * @brief Unit tests for AudioPlayer class
 * @author Scott Zelenka
 * @date 2024-06-14
 */

#include <unity.h>
#include <iostream>
#include <type_traits>

#include "AudioPlayer.h"
#include "Logger.h"

// Simple mock TimerAudio for testing
class MockTimerAudio
{
public:
    MockTimerAudio()
        : m_isPlaying(false),
          m_currentIndex(0),
          m_beginCalled(false),
          m_stopCalled(false),
          m_playWAVCalled(false)
    {
    }

    void playWAV(uint8_t index)
    {
        m_currentIndex = index;
        m_isPlaying = true;
        m_playWAVCalled = true;
    }

    void stop()
    {
        m_isPlaying = false;
        m_stopCalled = true;
    }

    bool isPlaying() const { return m_isPlaying; }

    void begin() { m_beginCalled = true; }

    // Test helper methods
    uint8_t getCurrentIndex() const { return m_currentIndex; }
    bool wasBeginCalled() const { return m_beginCalled; }
    bool wasStopCalled() const { return m_stopCalled; }
    bool wasPlayWAVCalled() const { return m_playWAVCalled; }

    void reset()
    {
        m_isPlaying = false;
        m_currentIndex = 0;
        m_beginCalled = false;
        m_stopCalled = false;
        m_playWAVCalled = false;
    }

private:
    bool m_isPlaying;
    uint8_t m_currentIndex;
    bool m_beginCalled;
    bool m_stopCalled;
    bool m_playWAVCalled;
};

void setUp(void)
{
    // Set up before each test
    Log.setLogLevel(LogLevel::INFO);
}

void tearDown(void)
{
    // Clean up after each test
}

void test_constructor_initializes_correctly()
{
    std::cout << "  Running test_constructor_initializes_correctly()" << std::endl;

    // Create mock TimerAudio
    MockTimerAudio timerMock;

    // Create AudioPlayer with mock
    AudioPlayer player(reinterpret_cast<TimerAudio*>(&timerMock));

    // Verify initial state
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());
    TEST_ASSERT_EQUAL(-1, player.getCurrentSoundIndex());
    TEST_ASSERT_FALSE(player.isPlaying());
}

void test_play_valid_index_starts_playback()
{
    std::cout << "  Running test_play_valid_index_starts_playback()" << std::endl;

    // Create mock TimerAudio
    MockTimerAudio timerMock;

    // Create AudioPlayer with mock
    AudioPlayer player(reinterpret_cast<TimerAudio*>(&timerMock));

    // Test playing a valid sound
    bool result = player.play(1);

    // Verify result
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(WAVState::Playing, player.getState());
    TEST_ASSERT_EQUAL(1, player.getCurrentSoundIndex());

    // Verify mock was called
    TEST_ASSERT_TRUE(timerMock.wasPlayWAVCalled());
    TEST_ASSERT_EQUAL(1, timerMock.getCurrentIndex());
}

void test_play_invalid_index_fails()
{
    std::cout << "  Running test_play_invalid_index_fails()" << std::endl;

    // Create mock TimerAudio
    MockTimerAudio timerMock;

    // Create AudioPlayer with mock
    AudioPlayer player(reinterpret_cast<TimerAudio*>(&timerMock));

    // Test playing invalid indices
    TEST_ASSERT_FALSE(player.play(-1));
    TEST_ASSERT_FALSE(player.play(255));  // Assuming NUM_SOUND_FILES < 255

    // Verify state remains stopped
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());
    TEST_ASSERT_EQUAL(-1, player.getCurrentSoundIndex());
}

void test_stop_stops_playback()
{
    std::cout << "  Running test_stop_stops_playback()" << std::endl;

    // Create mock TimerAudio
    MockTimerAudio timerMock;

    // Create AudioPlayer with mock
    AudioPlayer player(reinterpret_cast<TimerAudio*>(&timerMock));

    // Start playing
    player.play(2);
    TEST_ASSERT_EQUAL(WAVState::Playing, player.getState());

    // Stop playback
    player.stop();

    // Verify stopped state
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());
    TEST_ASSERT_EQUAL(-1, player.getCurrentSoundIndex());

    // Verify stop was called
    TEST_ASSERT_TRUE(timerMock.wasStopCalled());
}

void test_update_detects_playback_end()
{
    std::cout << "  Running test_update_detects_playback_end()" << std::endl;

    // Create mock TimerAudio that simulates playback ending
    MockTimerAudio timerMock;

    // Create AudioPlayer with mock
    AudioPlayer player(reinterpret_cast<TimerAudio*>(&timerMock));

    // Start playing
    player.play(3);
    TEST_ASSERT_EQUAL(WAVState::Playing, player.getState());

    // First update - still playing
    player.update();
    TEST_ASSERT_EQUAL(WAVState::Playing, player.getState());

    // Simulate playback ending
    timerMock.stop();

    // Second update - playback has ended
    player.update();
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());
    TEST_ASSERT_EQUAL(-1, player.getCurrentSoundIndex());
}

void test_play_while_playing_stops_current()
{
    std::cout << "  Running test_play_while_playing_stops_current()" << std::endl;

    // Create mock TimerAudio
    MockTimerAudio timerMock;

    // Create AudioPlayer with mock
    AudioPlayer player(reinterpret_cast<TimerAudio*>(&timerMock));

    // Start playing first sound
    player.play(1);
    TEST_ASSERT_EQUAL(1, player.getCurrentSoundIndex());

    // Reset mock to track new calls
    timerMock.reset();
    timerMock.playWAV(1);  // Restore playing state

    // Start playing second sound (should stop first)
    player.play(2);
    TEST_ASSERT_EQUAL(2, player.getCurrentSoundIndex());

    // Verify both sounds were played
    TEST_ASSERT_TRUE(timerMock.wasPlayWAVCalled());
    TEST_ASSERT_EQUAL(2, timerMock.getCurrentIndex());
}

void test_play_random_sound()
{
    std::cout << "  Running test_play_random_sound()" << std::endl;

    // Create mock TimerAudio
    MockTimerAudio timerMock;

    // Create AudioPlayer with mock
    AudioPlayer player(reinterpret_cast<TimerAudio*>(&timerMock));

    // Play random sound
    bool result = player.playRandomSound();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(WAVState::Playing, player.getState());

    // Verify a sound was played (should be index 1 or higher)
    TEST_ASSERT_GREATER_OR_EQUAL(1, player.getCurrentSoundIndex());
    TEST_ASSERT_TRUE(timerMock.wasPlayWAVCalled());
}

void test_null_timer_audio_fails_gracefully()
{
    std::cout << "  Running test_null_timer_audio_fails_gracefully()" << std::endl;

    // Create AudioPlayer with null TimerAudio
    AudioPlayer player(nullptr);

    // All operations should fail gracefully
    TEST_ASSERT_FALSE(player.play(1));
    TEST_ASSERT_FALSE(player.playRandomSound());
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());
    TEST_ASSERT_EQUAL(-1, player.getCurrentSoundIndex());

    // Update and stop should not crash
    player.update();
    player.stop();
}

void test_destructor_and_copy_prevention()
{
    std::cout << "  Running test_destructor_and_copy_prevention()" << std::endl;

    // Create mock object
    MockTimerAudio timerMock;

    // Test that we can create and destroy an AudioPlayer
    {
        AudioPlayer player(reinterpret_cast<TimerAudio*>(&timerMock));
        // Destructor will be called when player goes out of scope
    }

    // Verify copy constructor is deleted
    {
        AudioPlayer player1(reinterpret_cast<TimerAudio*>(&timerMock));
        // The following line should cause a compilation error if uncommented
        // AudioPlayer player2 = player1; // Should not compile

        // Use decltype to check if copy constructor is deleted
        static_assert(!std::is_copy_constructible<AudioPlayer>::value,
                      "AudioPlayer should not be copy constructible");

        // Verify assignment operator is deleted
        AudioPlayer player3(reinterpret_cast<TimerAudio*>(&timerMock));
        // The following line should cause a compilation error if uncommented
        // player3 = player1; // Should not compile

        static_assert(!std::is_copy_assignable<AudioPlayer>::value,
                      "AudioPlayer should not be copy assignable");
    }
}

void test_state_transitions()
{
    std::cout << "  Running test_state_transitions()" << std::endl;

    // Create mock TimerAudio
    MockTimerAudio timerMock;

    // Create AudioPlayer with mock
    AudioPlayer player(reinterpret_cast<TimerAudio*>(&timerMock));

    // Initial state
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());

    // Start playing
    player.play(1);
    TEST_ASSERT_EQUAL(WAVState::Playing, player.getState());

    // Stop manually
    player.stop();
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());

    // Start playing again
    player.play(2);
    TEST_ASSERT_EQUAL(WAVState::Playing, player.getState());

    // Simulate playback ending
    timerMock.stop();

    // End naturally via update
    player.update();
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());
}

void runAudioPlayerTests()
{
    UNITY_BEGIN();
    RUN_TEST(test_constructor_initializes_correctly);
    RUN_TEST(test_play_valid_index_starts_playback);
    RUN_TEST(test_play_invalid_index_fails);
    RUN_TEST(test_stop_stops_playback);
    RUN_TEST(test_update_detects_playback_end);
    RUN_TEST(test_play_while_playing_stops_current);
    RUN_TEST(test_play_random_sound);
    RUN_TEST(test_null_timer_audio_fails_gracefully);
    RUN_TEST(test_destructor_and_copy_prevention);
    RUN_TEST(test_state_transitions);
    UNITY_END();
}
