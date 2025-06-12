#!/usr/bin/env python3
"""
Convert WAV files to C++ arrays for WavData.cpp

Usage:
    python3 wav_to_cpp.py input.wav [-o output.cpp] [-n array_name]
"""
import wave
import struct
import os
import re
import sys
import argparse

def wav_to_c_array(wav_path, array_name="sound_data"):
    """Convert a WAV file to a C++ byte array."""
    try:
        with wave.open(wav_path, 'rb') as wav_file:
            # Get WAV parameters
            n_channels = wav_file.getnchannels()
            sample_width = wav_file.getsampwidth()
            frame_rate = wav_file.getframerate()
            n_frames = wav_file.getnframes()
            data = wav_file.readframes(n_frames)
            
            # Verify format (16-bit PCM)
            if sample_width != 2:
                print(f"Error: Only 16-bit PCM WAV files are supported (got {sample_width*8}-bit)", file=sys.stderr)
                sys.exit(1)
            
            # Convert to array of 16-bit integers
            fmt = f"<{n_frames * n_channels}h"  # Little-endian 16-bit
            samples = list(struct.unpack(fmt, data))
            
            # If stereo, convert to mono by averaging channels
            if n_channels == 2:
                samples = [(samples[i] + samples[i+1]) // 2 for i in range(0, len(samples), 2)]
            
            # Generate C array
            hex_values = []
            for i, sample in enumerate(samples):
                # Convert to little-endian bytes
                b1 = sample & 0xFF
                b2 = (sample >> 8) & 0xFF
                hex_values.append(f"0x{b1:02X}, 0x{b2:02X}, ")
                
                # Add newline every 8 samples (16 bytes)
                if (i + 1) % 8 == 0:
                    hex_values.append("\n    ")
            
            # Format as C array
            array_str = "    " + "".join(hex_values).strip(",\n ")
                    
            # Generate the complete C++ code
            cpp_code = f"""// Auto-generated from {os.path.basename(wav_path)}
#include <stddef.h>
#include <cstdint>
#include <avr/pgmspace.h>

const uint8_t {array_name}_data[] PROGMEM = {{
{array_str}
}};
const size_t {array_name}_size = sizeof({array_name}_data);

// WAV file parameters:
// - Sample rate: {frame_rate} Hz
// - Channels: {n_channels} ({'mono' if n_channels == 1 else 'stereo'})
// - Duration: {n_frames/frame_rate:.2f} seconds
// - Total samples: {len(samples)}"""
            
            return cpp_code
            
    except Exception as e:
        print(f"Error processing WAV file: {str(e)}", file=sys.stderr)
        sys.exit(1)

def sanitize_var_name(filename):
    """Convert a filename to a valid C++ variable name."""
    # Remove directory path and extension
    base = os.path.splitext(os.path.basename(filename))[0]
    # Replace invalid characters with underscores
    var_name = re.sub(r'[^a-zA-Z0-9_]', '_', base)
    return 'wav_' + var_name

def main():
    parser = argparse.ArgumentParser(description='Convert WAV file to C++ array for WavData.cpp')
    parser.add_argument('input_file', help='Input WAV file path')
    parser.add_argument('-o', '--output', help='Output C++ directory', default="./lib/WavData/")
    
    args = parser.parse_args()
    
    try:
        filename = os.path.expanduser(os.path.expandvars(args.input_file))
        if not os.path.exists(filename):
            print(f"Error: Input file '{filename}' not found", file=sys.stderr)
            sys.exit(1)
        
        # Use provided name or generate from filename
        var_name = sanitize_var_name(args.input_file)
        cpp_code = wav_to_c_array(filename, var_name)
        output_filename = f"{var_name}.cpp"
        
        if args.output:
            dir_name = os.path.abspath(args.output)
            os.makedirs(dir_name, exist_ok=True)
            with open(os.path.join(dir_name, output_filename), 'w') as f:
                f.write(cpp_code)
            print(f"Successfully converted {filename} to {dir_name}/{output_filename}")
        else:
            print(cpp_code)
            
    except Exception as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
