cmake_minimum_required(VERSION 3.22)
project(DJR2ROpticalTubeCompressor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
FetchContent_Declare(juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(DJR2ROpticalTubeCompressor
    COMPANY_NAME "DJ R2R Music"
    BUNDLE_ID "com.djr2rmusic.opticaltubecompressor"
    PLUGIN_MANUFACTURER_CODE DJR2
    PLUGIN_CODE R2OC
    FORMATS VST3
    PRODUCT_NAME "DJ R2R Optical Tube Compressor"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

juce_generate_juce_header(DJR2ROpticalTubeCompressor)

target_sources(DJR2ROpticalTubeCompressor PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)
target_compile_definitions(DJR2ROpticalTubeCompressor PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(DJR2ROpticalTubeCompressor PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags
)
