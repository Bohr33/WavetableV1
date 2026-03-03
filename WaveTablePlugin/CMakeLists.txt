cmake_minimum_required(VERSION 3.22)

project(WaveTablePlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(JUCE)

juce_add_plugin(WaveTablePlugin
    VERSION "1.0.0"
    COMPANY_NAME "Ben Ward"

    BUNDLE_ID "com.benward.wavetableplugin"

    PLUGIN_MANUFACTURER_CODE Bdub
    PLUGIN_CODE Prjt

    FORMATS AU VST3 Standalone

    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE

    PRODUCT_NAME "WaveTablePlugin"
    AU_MAIN_TYPE "kAudioUnitType_MusicDevice"

    COPY_PLUGIN_AFTER_BUILD TRUE
)

juce_generate_juce_header(WaveTablePlugin)

# ============================================================
# Source Files
# ============================================================

target_sources(WaveTablePlugin PRIVATE

    # Plugin Core
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h

    # Synth Engine
    Source/Synthesizer.cpp
    Source/Synthesizer.h
    Source/BWADSR.cpp
    Source/BWADSR.h

    # DSP / Wavetable
    Source/WaveTable.cpp
    Source/WaveTable.h
    Source/WavetableGenerator.cpp
    Source/WavetableGenerator.h
    Source/MipMapGenerator.cpp
    Source/MipMapGenerator.h
    Source/ImportWavetable.cpp
    Source/ImportWavetable.h

    # UI
    Source/Graphics.cpp
    Source/Graphics.h
)

target_include_directories(WaveTablePlugin PRIVATE
    Source
)

# ============================================================
# Compile Definitions
# ============================================================

target_compile_definitions(WaveTablePlugin PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# ============================================================
# Module Linking (Minimal + Correct)
# ============================================================

target_link_libraries(WaveTablePlugin PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

# ============================================================
# IDE Organization (Optional but Clean)
# ============================================================

source_group("Plugin" FILES
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

source_group("Synth" FILES
    Source/Synthesizer.cpp
    Source/BWADSR.cpp
)

source_group("DSP" FILES
    Source/WaveTable.cpp
    Source/WavetableGenerator.cpp
    Source/MipMapGenerator.cpp
    Source/ImportWavetable.cpp
)

source_group("UI" FILES
    Source/Graphics.cpp
)
