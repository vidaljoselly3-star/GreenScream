cmake_minimum_required(VERSION 3.22)

# Runtime de MSVC enlazado estáticamente: el .vst3 NO necesita instalar "Visual C++ Redistributable".
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)

project(GreenScreamOD VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.6
    GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(GreenScreamOD
    COMPANY_NAME               "Vidal"
    PLUGIN_MANUFACTURER_CODE   Vdal
    PLUGIN_CODE                Gsod
    FORMATS                    VST3
    PRODUCT_NAME               "GreenScream by Vidal"
    VST3_CATEGORIES            Fx Distortion
    IS_SYNTH                   FALSE
    NEEDS_MIDI_INPUT           FALSE
    NEEDS_MIDI_OUTPUT          FALSE
    IS_MIDI_EFFECT             FALSE
    COPY_PLUGIN_AFTER_BUILD    FALSE
)

target_sources(GreenScreamOD PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/TS808Engine.h
    Source/PedalLookAndFeel.h
    Source/Presets.h
)

target_compile_definitions(GreenScreamOD PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_MODAL_LOOPS_PERMITTED=0
)

target_link_libraries(GreenScreamOD
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
)
