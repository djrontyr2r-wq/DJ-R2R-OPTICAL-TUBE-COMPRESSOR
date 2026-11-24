# DJ R2R Optical Tube Compressor

An original optical-style tube compressor VST3 built with C++ and JUCE. Inspired by the broad behavior of classic optical compressors; this project does not use Waves/CLA-2A code, artwork, trademarks, or copied interface assets.

## Features
- Native 64-bit Windows VST3 target
- Stereo-linked optical-style gain reduction envelope
- Peak Reduction, Gain, Compress/Limit mode
- Smooth program-dependent attack/release behavior
- Subtle tube-style saturation
- Custom dark DJ R2R interface with animated VU meter

## Build (Windows 10/11, Visual Studio 2022)
1. Install Visual Studio 2022 with **Desktop development with C++** and a recent CMake.
2. Clone this repository.
3. Configure: `cmake -S . -B build -A x64`
4. Build: `cmake --build build --config Release`
5. The VST3 bundle is produced under the build tree (JUCE VST3 target output).

CMake downloads JUCE 8 from its official Git repository at configure time, so an internet connection is required on the first configure.

## Important
This is source code, not a prebuilt installer. Test in a DAW before production use. No third-party commercial plugin code is included.
