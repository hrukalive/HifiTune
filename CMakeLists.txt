cmake_minimum_required(VERSION 3.22)
project(HifiTune VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(HIFITUNE_ENABLE_ARA "Enable ARA integration" ON)
if(HIFITUNE_ENABLE_ARA)
    add_compile_definitions(HIFITUNE_ENABLE_ARA=1)
else()
    add_compile_definitions(HIFITUNE_ENABLE_ARA=0)
endif()

set(JUCE_ARA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE_ARA")

add_subdirectory(external/juce)

juce_add_binary_data(HifiTuneResources
    SOURCES
        resources/icon.svg
        resources/lookandfeel.json
        resources/keyboard_shortcuts.txt
)

set(HIFITUNE_SHARED_SOURCES
    src/ui/LookAndFeel.cpp
    src/ui/LookAndFeel.h
    src/SharedResources.cpp
    src/SharedResources.h
)

set(HIFITUNE_PLUGIN_SOURCES
    src/plugin/PluginProcessor.cpp
    src/plugin/PluginProcessor.h
    src/plugin/PluginEditor.cpp
    src/plugin/PluginEditor.h
)

set(HIFITUNE_STANDALONE_SOURCES
    src/app/Main.cpp
)

if(HIFITUNE_ENABLE_ARA)
    list(APPEND HIFITUNE_PLUGIN_SOURCES
        src/ara/AraIntegration.cpp
        src/ara/AraIntegration.h
    )
endif()

juce_add_gui_app(HifiTune
    PRODUCT_NAME "HifiTune"
    COMPANY_NAME "HifiTune"
)

juce_add_plugin(HifiTunePlugin
    PRODUCT_NAME "HifiTune"
    COMPANY_NAME "HifiTune"
    FORMATS VST3 AU
    ARA TRUE
    PLUGIN_MANUFACTURER_CODE Hftn
    PLUGIN_CODE Ht01
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
)

foreach(target HifiTune HifiTunePlugin)
    target_sources(${target} PRIVATE
        ${HIFITUNE_SHARED_SOURCES}
    )
    target_link_libraries(${target} PRIVATE
        HifiTuneResources
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_audio_basics
        juce::juce_dsp
    )
    target_compile_definitions(${target} PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
endforeach()

target_sources(HifiTune PRIVATE ${HIFITUNE_STANDALONE_SOURCES})

target_sources(HifiTunePlugin PRIVATE ${HIFITUNE_PLUGIN_SOURCES})

if(HIFITUNE_ENABLE_ARA)
    target_compile_definitions(HifiTunePlugin PRIVATE JUCE_PLUGIN_ENABLE_ARA=1)
endif()
