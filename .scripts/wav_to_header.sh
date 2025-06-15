#!/bin/bash

# Check if input file is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input_wav_file>"
    exit 1
fi

INPUT_FILE="$1"
# Set output file to lib/WavData/<base_name>.h
OUTPUT_DIR="$(dirname "$0")/../lib/WavData"
mkdir -p "$OUTPUT_DIR"
BASE_FILENAME=$(basename "${INPUT_FILE%.*}" | tr '[:upper:] ' '[:lower:]_' | tr -cd '[:alnum:]_')
OUTPUT_FILE="$OUTPUT_DIR/wav_${BASE_FILENAME}.h"

# Get the base name for the variable (e.g., "wav_chat_01" from "path/to/wav_chat_01.wav")
# Already processed in the OUTPUT_FILE section above
BASE_NAME="wav_${BASE_FILENAME}"

# Generate the header file with proper variable names
{
    # Convert to uppercase for header guard
    HEADER_GUARD=$(echo "${BASE_NAME}_H" | tr '[:lower:]' '[:upper:]')
    echo "// Auto-generated from $(basename "$INPUT_FILE")"
    echo "#ifndef $HEADER_GUARD"
    echo "#define $HEADER_GUARD"
    echo ""
    echo "#include <Arduino.h>"
    echo "#include <stddef.h>"
    echo "#include <cstdint>"
    echo ""
    echo "// WAV file data"
    echo "const uint8_t ${BASE_NAME}_data[] PROGMEM = {"
    echo -n ""
    
    # Use xxd to generate C-style array with 12 bytes per line
    xxd -i < "$INPUT_FILE" | \
        sed '1d' | \
        sed 's/unsigned char/    /' | \
        sed 's/ = /_data[] = /' | \
        sed 's/;//' | \
        sed 's/0x/0x/g' | \
        sed 's/, 0x/, 0x/g' | \
        sed 's/  0x/    0x/'
    
    echo "};";
    echo ""
    echo "// Size of the WAV data in bytes"
    echo "const size_t ${BASE_NAME}_size = sizeof(${BASE_NAME}_data);"
    echo ""
    echo "#endif // $HEADER_GUARD"
} > "$OUTPUT_FILE"

echo "Generated $OUTPUT_FILE"
