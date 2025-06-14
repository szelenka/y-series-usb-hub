#include <ArduinoFake.h>
#include <unity.h>

#include "AudioPlayer.h"
#include "BackgroundAudioWAV.h"
#include "mock_helpers.h"

using namespace fakeit;

void test_audio_player_play()
{
    std::cout << "  Running test_audio_player_play()" << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    Log.setLogLevel(LogLevel::NONE);
    // Mock player methods
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t)))
        .Do([](const uint8_t* data, size_t size) { return 1; });
    When(OverloadedMethod(playerMock, write, size_t(uint8_t))).Do([](uint8_t c) { return 1; });
    When(Method(playerMock, playing))
        .Do(
            []()
            {
                return true;  // Mock as playing
            });
    When(Method(playerMock, flush))
        .Do(
            []()
            {
                return;  // Mock flush
            });
    When(Method(playerMock, begin))
        .Do(
            []()
            {
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
    Log.setLogLevel(LogLevel::NONE);
    // Mock player methods
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t)))
        .Do([](const uint8_t* data, size_t size) { return 1; });
    When(OverloadedMethod(playerMock, write, size_t(uint8_t))).Do([](uint8_t c) { return 1; });
    When(Method(playerMock, playing))
        .Do(
            []()
            {
                return true;  // Mock as playing
            });
    When(Method(playerMock, flush))
        .Do(
            []()
            {
                return;  // Mock flush
            });
    When(Method(playerMock, begin))
        .Do(
            []()
            {
                return true;  // Mock begin
            });
    When(OverloadedMethod(ArduinoFake(), random, long(long, long)))
        .AlwaysDo([](long min, long max) { return 1; });

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
    Log.setLogLevel(LogLevel::NONE);
    // Mock player methods
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t)))
        .Do([](const uint8_t* data, size_t size) { return 1; });
    When(OverloadedMethod(playerMock, write, size_t(uint8_t))).Do([](uint8_t c) { return 1; });
    When(Method(playerMock, playing))
        .Do(
            []()
            {
                return false;  // Mock as not playing
            });
    When(Method(playerMock, flush))
        .Do(
            []()
            {
                return;  // Mock flush
            });
    When(Method(playerMock, begin))
        .Do(
            []()
            {
                return true;  // Mock begin
            });
    When(OverloadedMethod(ArduinoFake(), random, long(long, long)))
        .AlwaysDo([](long min, long max) { return 1; });

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
    Log.setLogLevel(LogLevel::NONE);
    // Mock player methods
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t)))
        .Do([](const uint8_t* data, size_t size) { return 1; });
    When(OverloadedMethod(playerMock, write, size_t(uint8_t))).Do([](uint8_t c) { return 1; });
    When(Method(playerMock, playing))
        .Do(
            []()
            {
                return true;  // Mock as playing
            });
    When(Method(playerMock, flush))
        .Do(
            []()
            {
                return;  // Mock flush
            });
    When(Method(playerMock, begin))
        .Do(
            []()
            {
                return true;  // Mock begin
            });
    When(OverloadedMethod(ArduinoFake(), random, long(long, long)))
        .AlwaysDo([](long min, long max) { return 1; });

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

void test_constructor_initialization()
{
    std::cout << "  Running test_constructor_initialization()" << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    Log.setLogLevel(LogLevel::NONE);
    // Set up expectations
    When(Method(playerMock, begin)).Return(true);

    // Create audio player - this should call begin() on the player
    AudioPlayer player(&playerMock.get());

    // Verify begin() was called
    Verify(Method(playerMock, begin)).Once();

    // Verify initial state
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());
    TEST_ASSERT_EQUAL(-1, player.getCurrentSoundIndex());
}

void test_play_stops_current_playback()
{
    std::cout << "  Running test_play_stops_current_playback()" << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    Log.setLogLevel(LogLevel::NONE);
    // Set up mock behavior
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t))).Return(1);
    When(Method(playerMock, playing)).Return(true);
    When(Method(playerMock, flush)).Do([]() {});
    When(Method(playerMock, begin)).Return(true);

    // Create audio player
    AudioPlayer player(&playerMock.get());

    // First play - should work
    TEST_ASSERT_TRUE(player.play(0));
    TEST_ASSERT_EQUAL(WAVState::Playing, player.getState());

    // Reset mock to track calls
    playerMock.ClearInvocationHistory();

    // Play again - should stop current playback first
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t))).Return(1);

    // Second play should trigger stop() which calls flush()
    TEST_ASSERT_TRUE(player.play(1));

    // Verify flush was called (from stop())
    Verify(Method(playerMock, flush)).Once();
}

void test_stop_behavior()
{
    std::cout << "  Running test_stop_behavior()" << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    Log.setLogLevel(LogLevel::NONE);
    // Set up mock behavior
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t))).Return(1);
    When(Method(playerMock, playing)).Return(true);
    When(Method(playerMock, flush)).Do([]() {});
    When(Method(playerMock, begin)).Return(true);

    // Create audio player
    AudioPlayer player(&playerMock.get());

    // Play first to set state
    player.play(0);

    // Reset mock to track calls
    playerMock.ClearInvocationHistory();

    // Call stop
    player.stop();

    // Verify flush was called
    Verify(Method(playerMock, flush)).Once();

    // Verify state was updated
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());
    TEST_ASSERT_EQUAL(-1, player.getCurrentSoundIndex());
}

void test_update_transitions_to_stopped_when_playback_finishes()
{
    std::cout << "  Running test_update_transitions_to_stopped_when_playback_finishes()"
              << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    Log.setLogLevel(LogLevel::NONE);
    // Set up mock behavior
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t))).Return(1);
    When(Method(playerMock, playing)).Return(false);  // Simulate playback finished
    When(Method(playerMock, flush)).Do([]() {});
    When(Method(playerMock, begin)).Return(true);

    // Create audio player
    AudioPlayer player(&playerMock.get());

    // Play first to set state
    player.play(0);

    // Reset mock to track calls
    playerMock.ClearInvocationHistory();

    // Call update - should detect playback finished and stop
    player.update();

    // Verify state was updated to Stopped
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());
    TEST_ASSERT_EQUAL(-1, player.getCurrentSoundIndex());
}

void test_play_random_sound_generates_valid_index()
{
    std::cout << "  Running test_play_random_sound_generates_valid_index()" << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    Log.setLogLevel(LogLevel::NONE);
    // Set up mock behavior
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t))).AlwaysReturn(1);
    When(Method(playerMock, playing)).AlwaysReturn(true);
    When(Method(playerMock, flush)).AlwaysDo([]() {});
    When(Method(playerMock, begin)).AlwaysReturn(true);

    // Mock random to return specific values for testing
    int testIndices[] = {0, 1, NUM_SOUND_FILES - 1};
    int callCount = 0;

    When(OverloadedMethod(ArduinoFake(), random, long(long, long)))
        .AlwaysDo(
            [&testIndices, &callCount](long min, long max)
            {
                int result = testIndices[callCount % 3];
                callCount++;
                return result;
            });

    // Create audio player
    AudioPlayer player(&playerMock.get());

    // Test multiple calls to ensure random index is within bounds
    for (int i = 0; i < 3; i++)
    {
        TEST_ASSERT_TRUE(player.playRandomSound());
        int index = player.getCurrentSoundIndex();
        TEST_ASSERT_TRUE(index >= 0);
        TEST_ASSERT_TRUE(index < NUM_SOUND_FILES);
    }
}

void test_constructor_with_null()
{
    std::cout << "  Running test_constructor_with_null()" << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    Log.setLogLevel(LogLevel::NONE);
    // Set up expectation that begin() will be called
    When(Method(playerMock, begin)).AlwaysReturn(true);

    // Create AudioPlayer - should not call begin() on the player
    AudioPlayer player(nullptr);

    player.stop();
    player.update();

    // Verify begin() was not called
    Verify(Method(playerMock, begin)).Never();
}

void test_play_invalid_write()
{
    std::cout << "  Running test_play_invalid_write()" << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    Log.setLogLevel(LogLevel::NONE);
    // Set up mock behavior
    When(Method(playerMock, flush)).AlwaysReturn();
    When(Method(playerMock, begin)).AlwaysReturn(true);
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t))).AlwaysReturn(0);

    // Create AudioPlayer and set initial state
    AudioPlayer player(&playerMock.get());
    bool result = player.play(1);  // Set state to Playing
    TEST_ASSERT_FALSE(result);

    // Call stop()
    player.stop();

    // Verify flush() was called
    Verify(Method(playerMock, flush)).Once();

    // Verify state was updated to Stopped
    TEST_ASSERT_EQUAL(WAVState::Stopped, player.getState());
    TEST_ASSERT_EQUAL(-1, player.getCurrentSoundIndex());
}

void test_play_random_sound_with_sounds()
{
    std::cout << "  Running test_play_random_sound_with_sounds()" << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    Log.setLogLevel(LogLevel::NONE);
    // Set up mock behavior
    When(OverloadedMethod(playerMock, write, size_t(const uint8_t*, size_t))).Return(1);
    When(Method(playerMock, begin)).AlwaysReturn(true);

    // Mock random to return specific value
    When(OverloadedMethod(ArduinoFake(), random, long(long, long)))
        .AlwaysReturn(2);  // Return index 2

    // Create AudioPlayer
    AudioPlayer player(&playerMock.get());

    // Play random sound - should succeed
    TEST_ASSERT_TRUE(player.playRandomSound());

    // Verify play was called with index 2
    TEST_ASSERT_EQUAL(2, player.getCurrentSoundIndex());
}

void test_destructor_and_copy_prevention()
{
    std::cout << "  Running test_destructor_and_copy_prevention()" << std::endl;

    // Create mock object
    Mock<ROMBackgroundAudioWAV> playerMock;
    Log.setLogLevel(LogLevel::NONE);
    When(Method(playerMock, begin)).AlwaysReturn(true);

    // Test that we can create and destroy an AudioPlayer
    {
        AudioPlayer player(&playerMock.get());
        // Destructor will be called when player goes out of scope
    }

    // Verify copy constructor is deleted
    {
        AudioPlayer player1(&playerMock.get());
        // The following line should cause a compilation error if uncommented
        // AudioPlayer player2 = player1; // Should not compile

        // Use decltype to check if copy constructor is deleted
        static_assert(!std::is_copy_constructible<AudioPlayer>::value,
                      "AudioPlayer should not be copy constructible");

        // Verify assignment operator is deleted
        AudioPlayer player3(&playerMock.get());
        // The following line should cause a compilation error if uncommented
        // player3 = player1; // Should not compile

        static_assert(!std::is_copy_assignable<AudioPlayer>::value,
                      "AudioPlayer should not be copy assignable");
    }

    // Verify move constructor is deleted (since we have user-declared destructor)
    static_assert(!std::is_move_constructible<AudioPlayer>::value ||
                      !std::is_move_assignable<AudioPlayer>::value,
                  "AudioPlayer should not be move constructible or assignable");
}

void runAudioPlayerTests()
{
    UNITY_BEGIN();
    // Original tests
    RUN_TEST(test_audio_player_play);
    RUN_TEST(test_audio_player_stop);
    RUN_TEST(test_audio_player_update);
    RUN_TEST(test_audio_player_random_sound);

    // Constructor and initialization tests
    RUN_TEST(test_constructor_initialization);
    RUN_TEST(test_constructor_with_null);

    // Playback control tests
    RUN_TEST(test_play_stops_current_playback);
    RUN_TEST(test_stop_behavior);

    // State transition tests
    RUN_TEST(test_update_transitions_to_stopped_when_playback_finishes);
    RUN_TEST(test_play_invalid_write);

    // Random sound tests
    RUN_TEST(test_play_random_sound_generates_valid_index);
    RUN_TEST(test_play_random_sound_with_sounds);

    // Special cases
    RUN_TEST(test_destructor_and_copy_prevention);

    UNITY_END();
}
