#include <ArduinoFake.h>
#include <unity.h>
#include "BackgroundAudioWAV.h"
#include "AudioPlayer.h"
#include "mock_helpers.h"

// Mock random() - use a simple deterministic approach
int random(int min, int max) {
    // Use a simple counter that wraps around
    static int counter = 0;
    counter = (counter + 1) % max;
    return counter;
}

using namespace fakeit;

void test_audio_player_play()
{
    std::cout << "  Running test_audio_player_play()" << std::endl;
    
    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    
    // Mock player methods
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t)))
        .Do([](const uint8_t* data, size_t size) {
            return 1;
        });
    When(OverloadedMethod(playerMock, write, size_t(uint8_t)))
        .Do([](uint8_t c) {
            return 1;
        });
    When(Method(playerMock, playing))
        .Do([]() {
            return true;  // Mock as playing
        });
    When(Method(playerMock, flush))
        .Do([]() {
            return;  // Mock flush
        });
    When(Method(playerMock, begin))
        .Do([]() {
            return true;  // Mock begin
        });

    // Create audio player
    AudioPlayer* player = new AudioPlayer(&playerMock.get());
    
    // Test playing a sound
    player->play(0);
    TEST_ASSERT_EQUAL(WAVState::Playing, player->getState());
    TEST_ASSERT_EQUAL(0, player->getCurrentSoundIndex());
    TEST_ASSERT_TRUE(player->isPlaying());
    
    // Test invalid sound index
    player->play(-1);
    TEST_ASSERT_EQUAL(WAVState::Playing, player->getState());
    TEST_ASSERT_EQUAL(0, player->getCurrentSoundIndex());
    
    player->play(NUM_SOUND_FILES);
    TEST_ASSERT_EQUAL(WAVState::Playing, player->getState());
    TEST_ASSERT_EQUAL(0, player->getCurrentSoundIndex());
}

void test_audio_player_stop()
{
    std::cout << "  Running test_audio_player_stop()" << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    
    // Mock player methods
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t)))
        .Do([](const uint8_t* data, size_t size) {
            return 1;
        });
    When(OverloadedMethod(playerMock, write, size_t(uint8_t)))
        .Do([](uint8_t c) {
            return 1;
        });
    When(Method(playerMock, playing))
        .Do([]() {
            return true;  // Mock as playing
        });
    When(Method(playerMock, flush))
        .Do([]() {
            return;  // Mock flush
        });
    When(Method(playerMock, begin))
        .Do([]() {
            return true;  // Mock begin
        });
    
    // Create audio player
    AudioPlayer* player = new AudioPlayer(&playerMock.get());
    
    // Start playing
    player->play(0);
    TEST_ASSERT_EQUAL(WAVState::Playing, player->getState());
    
    // Stop
    player->stop();
    TEST_ASSERT_EQUAL(WAVState::Stopped, player->getState());
    TEST_ASSERT_EQUAL(-1, player->getCurrentSoundIndex());
    TEST_ASSERT_FALSE(player->isPlaying());
}

void test_audio_player_update()
{
    std::cout << "  Running test_audio_player_update()" << std::endl;
    
    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    
    // Mock player methods
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t)))
        .Do([](const uint8_t* data, size_t size) {
            return 1;
        });
    When(OverloadedMethod(playerMock, write, size_t(uint8_t)))
        .Do([](uint8_t c) {
            return 1;
        });
    When(Method(playerMock, playing))
        .Do([]() {
            return false;  // Mock as not playing
        });
    When(Method(playerMock, flush))
        .Do([]() {
            return;  // Mock flush
        });
    When(Method(playerMock, begin))
        .Do([]() {
            return true;  // Mock begin
        });
    
    // Create audio player
    AudioPlayer* player = new AudioPlayer(&playerMock.get());
    
    // Start playing
    player->play(0);
    TEST_ASSERT_EQUAL(WAVState::Playing, player->getState());
    
    // Update should stop when not playing
    player->update();
    TEST_ASSERT_EQUAL(WAVState::Stopped, player->getState());
    TEST_ASSERT_EQUAL(-1, player->getCurrentSoundIndex());
    TEST_ASSERT_FALSE(player->isPlaying());
}

void test_audio_player_random_sound()
{
    std::cout << "  Running test_audio_player_random_sound()" << std::endl;
    
    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    
    // Mock player methods
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t)))
        .Do([](const uint8_t* data, size_t size) {
            return 1;
        });
    When(OverloadedMethod(playerMock, write, size_t(uint8_t)))
        .Do([](uint8_t c) {
            return 1;
        });
    When(Method(playerMock, playing))
        .Do([]() {
            return true;  // Mock as playing
        });
    When(Method(playerMock, flush))
        .Do([]() {
            return;  // Mock flush
        });
    When(Method(playerMock, begin))
        .Do([]() {
            return true;  // Mock begin
        });
    
    // Create audio player
    AudioPlayer* player = new AudioPlayer(&playerMock.get());
    
    // Play random sound
    std::cout << "  Playing random sound" << std::endl;
    player->playRandomSound();
    std::cout << "  Random sound index: " << player->getCurrentSoundIndex() << std::endl;
    TEST_ASSERT_EQUAL(WAVState::Playing, player->getState());
    TEST_ASSERT_TRUE(player->getCurrentSoundIndex() >= 0);
    TEST_ASSERT_TRUE(player->getCurrentSoundIndex() < NUM_SOUND_FILES);
    TEST_ASSERT_TRUE(player->isPlaying());
}

void runAudioPlayerTests() {
    UNITY_BEGIN();
    RUN_TEST(test_audio_player_play);
    RUN_TEST(test_audio_player_stop);
    RUN_TEST(test_audio_player_update);
    RUN_TEST(test_audio_player_random_sound);
    UNITY_END();
}
