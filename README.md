# Y-Series USB Hub Controller

[![Code Coverage](coverage_badge.svg)](.coverage/index.html)

An interactive animatronic controller for the Y-Series USB Hub, designed to hold 4 [LEGO Lightsabers](https://github.com/szelenka/lightsaber). This project implements motorized movement, LED animations, and sound effects to create an engaging interactive experience.

## Features

- **Motorized Head Movement**: Smooth, animated rotation with configurable speed and direction
- **Interactive Sensors**:
  - Hall effect sensors for detecting head position
  - PIR motion sensor for detecting nearby movement
  - Tactile buttons for user input
- **Visual Feedback**:
  - NeoPixel LED animations
  - Rainbow color effects
  - Eye blink animations
- **Audio Playback**:
  - Plays WAV audio files
  - Multiple sound effects
  - Random sound selection
- **Configurable Behavior**:
  - Adjustable timing parameters

## Hardware Requirements

- Adafruit KB2040 (or compatible RP2040 board)
- NeoPixel 16-LED ring + 1-LED 5mm NeoPixel
- DC motor with H-bridge driver
- 2x Hall effect sensors
- PIR motion sensor
- 2x Push buttons
- Audio amplifier and speaker

## Pin Configuration

| Function             | Pin  | Notes                           |
|----------------------|------|---------------------------------|
| Neck Motor IN1       | A0   | H-bridge input 1                |
| Neck Motor IN2       | A1   | H-bridge input 2                |
| Audio Out            | A3   | PWM audio output                |
| Eye NeoPixel         | 5    | Data pin for LED ring/strip     |
| Dome LED (Green/Blue)| 2    | Status indicator               |
| Left Hall Sensor     | 3    | Detects left rotation limit    |
| Right Hall Sensor    | 4    | Detects right rotation limit   |
| PIR Sensor          | 6    | Motion detection               |
| Rectangle Button    | 7    | User input (with pull-up)      |
| Circle Button       | 8    | User input (with pull-up)      |


## Software Architecture

The code is organized into several modules:

1. **Animation** - Core animation and behavior logic
2. **AudioPlayer** - Handles WAV audio playback
3. **WavData** - Stores audio data in PROGMEM
4. **Logger** - Debug logging utilities

### Key Components

- **Animation Controller**: Manages motor movements, LED effects, and sensor inputs
- **Audio System**: Plays sound effects with support for multiple concurrent sounds
- **Input Handling**: Processes sensor and button inputs
- **State Management**: Tracks the current state of animations and interactions

## Building and Flashing

### Prerequisites

- PlatformIO Core
- Python 3.12+ (for development tools)
- clang-format (for code formatting)
- docker (for cross-platform code coverage)

### Build Commands

```bash
# Install dependencies
make setup

# Build the project
make build

# Upload to device
make upload

# Monitor serial output
make monitor

# Upload and monitor (combined)
make upload-and-monitor
```

### Development Tools

```bash
# Format code
make format

# Run tests
make test-local

# Generate code coverage report
make docker-coverage
```

## Customization

### Adding Sound Effects

1. Place your WAV files in any directory
2. Convert the WAV file to a C++ header:
   ```bash
   make wav-to-header WAV_FILE=path/to/your/sound.wav
   ```
3. The sound header will be generated in `lib/WavData/`
4. Include the header in `WavData.cpp` and add the sound to the appropriate arrays
5. Rebuild the project to include the new sound

### Modifying Animations

Edit the `Animation` class methods to change movement patterns, LED effects, and interactions. Key methods to modify:

- `update()` - Main update loop
- `performRotate()` - Controls motor movement
- `updateRainbow()` - LED color animations
- `eyeBlink()` - Eye animation effects

### Configuration

Adjust timing and behavior constants in `Animation.h`:

```cpp
namespace AnimationConstants {
    constexpr uint8_t kMaxMotorSpeed = 255;
    constexpr uint32_t kSpeedRampTime = 2000;
    constexpr uint32_t kMinRotateInterval = 500;
    constexpr uint32_t kMaxRotateInterval = 1000;
    // ...
}
```

## Testing

The project includes a comprehensive test suite:

```bash
# Run all tests
make test-local

# Run specific test file
pio test -e native --filter "test_Animation"

# Run with verbose output
pio test -e native -vvv
```

## License

This work is licensed under a [Creative Commons Attribution-NonCommercial 4.0 International License](http://creativecommons.org/licenses/by-nc/4.0/).

You are free to:
- **Share** — copy and redistribute the material in any medium or format
- **Adapt** — remix, transform, and build upon the material

Under the following terms:
- **Attribution** — You must give appropriate credit, provide a link to the license, and indicate if changes were made.
- **NonCommercial** — You may not use the material for commercial purposes.
- **No additional restrictions** — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.

For commercial use, please contact the author for licensing options.


## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a new Pull Request

## Acknowledgments

- [LEGO Lightsabers](https://github.com/szelenka/lightsaber)
- [PlatformIO](https://platformio.org/)
- [Adafruit KB2040](https://www.adafruit.com/product/5302)
