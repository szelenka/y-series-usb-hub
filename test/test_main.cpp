#include <Arduino.h>
#include <unity.h>
#include "mutex.h"
#include "mock_helpers.h"

#include "Animation.h"
#include "AudioPlayer.h"
#include "Logger.h"
#include "WavData.h"

// Include all test files
#include "Animation/test_Animation.cpp"
#include "Animation/test_AnimationInputs.cpp"
#include "AudioPlayer/test_AudioPlayer.cpp"
#include "Logger/test_Logger.cpp"
#include "WavData/test_WavData.cpp"


int main(int argc, char **argv) {
    UNITY_BEGIN();
    // Run each test suite
    runAnimationTests();
    runAnimationInputsTests();
    runAudioPlayerTests();
    runLoggerTests();
    runWavDataTests();
    return UNITY_END();
}
