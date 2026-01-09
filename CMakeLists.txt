cmake_minimum_required(VERSION 3.22)

project(HiFiTune VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# If using vcpkg as submodule, hook its toolchain when configuring:
#   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=external/vcpkg/scripts/buildsystems/vcpkg.cmake

# --- JUCE setup -----------------------------------------------------------
add_subdirectory(externals/JUCE)

# Point JUCE at the ARA SDK checked out as a submodule
# (JUCE expects ARA_SDK 2.2.0) :contentReference[oaicite:4]{index=4}
juce_set_ara_sdk_path("${CMAKE_SOURCE_DIR}/externals/ARA_SDK")

# --- ONNX Runtime via vcpkg -----------------------------------------------
# After running `vcpkg install`, vcpkg's config files for onnxruntime
# will be available when this project is configured with the vcpkg toolchain.
# The exact target name may vary by vcpkg version; a common one is onnxruntime::onnxruntime.
# Adjust this if `cmake` complains.
find_package(onnxruntime CONFIG QUIET)

# ---------------------------
# Plugin Target
# ---------------------------
juce_add_plugin(HiFiTune
    # Company
    COMPANY_NAME "OpenVPI"
    COMPANY_WEBSITE "https://openvpi.ai"
    COMPANY_EMAIL "dev@openvpi.ai"

    # Bundle / IDs
    # Adjust these if you already have a reverse-DNS you use for Apple code signing.
    BUNDLE_ID "com.openvpi.hifitune"
    PLUGIN_MANUFACTURER_CODE OpVP
    PLUGIN_CODE HiFT

    # Product
    PRODUCT_NAME "HiFiTune"
    PLUGIN_NAME "HiFiTune"
    DESCRIPTION "HiFiTune by OpenVPI"

    # Plugin behavior
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

    # ARA
    IS_ARA_EFFECT TRUE

    # Formats (no CLAP)
    FORMATS VST3 AUv3 Standalone
)

juce_generate_juce_header(HiFiTune)

# ---------------------------
# Sources
# ---------------------------
target_sources(HiFiTune
    PRIVATE
        src/SharedResources.cpp
        src/analysis/AnalysisPipeline.cpp
        src/analysis/AnalyzerRegistry.cpp
        src/analysis/AudioBufferSource.cpp
        src/analysis/MelSpectrogram.cpp
        src/analysis/MidiNoteExtractor.cpp
        src/analysis/PitchAnalyzer.cpp
        src/analysis/SilenceSegmenter.cpp
        src/analysis/Stretching.cpp
        src/inference/ModelConfig.cpp
        src/inference/ModelLoader.cpp
        src/inference/ModelRegistry.cpp
        src/inference/OnnxRuntimeEngine.cpp
        src/midi/MidiReferenceManager.cpp
        src/midi/MidiReferenceSnapper.cpp
        src/midi/SomeMidiExtractor.cpp
        src/pitch/PitchDetector.cpp
        src/pitch/RmvpePitchDetector.cpp
        src/pitch/FcpePitchDetector.cpp
        src/ui/LookAndFeel.cpp
        src/ui/lookandfeel/EditorLookAndFeel.cpp
        src/ui/MidiReferenceOverlay.cpp
        src/ui/MainEditor.cpp
        src/ui/ToolbarComponent.cpp
        src/ui/SidebarComponent.cpp
        src/ui/NoteViewComponent.cpp
        src/ui/PitchViewComponent.cpp
        src/ui/history/HistoryComponent.cpp
        src/ui/QuickHelpOverlay.cpp
        src/ui/OverviewComponent.cpp
        src/ui/TimelineComponent.cpp
        src/ui/ViewportController.cpp
        src/vocoder/Vocoder.cpp
        src/vocoder/NsfHifiGanVocoder.cpp
        src/plugin/PluginProcessor.cpp
        src/plugin/PluginEditor.cpp
        src/ara/AraIntegration.cpp
        src/ara/AraDocumentController.cpp
        src/ara/AraRegionMapper.cpp
        src/model/AnalysisConfigState.cpp
        src/model/ProjectState.cpp
        src/model/TrackState.cpp
        src/model/ClipState.cpp
        src/model/SegmentState.cpp
        src/model/NoteState.cpp
        src/model/MidiNoteState.cpp
        src/model/MidiReferenceState.cpp
        src/model/PitchCurveState.cpp
        src/model/TensionState.cpp
        src/analysis/DiffSingerDecomposer.cpp
        src/analysis/HarmonicNoiseSeparator.cpp
        src/commands/CommandManager.cpp
)

juce_add_binary_data(HifiTuneResources
    SOURCES
        resources/icon.svg
        resources/lookandfeel.json
        resources/keyboard_shortcuts.txt
)

# ---------------------------
# JUCE Modules / Linkage
# ---------------------------
target_compile_definitions(HiFiTune
    PRIVATE
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(HiFiTune
    PRIVATE
        HifiTuneResources
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_plugin_client
        juce::juce_audio_utils
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
        # ARA support is hooked via IS_ARA_EFFECT + juce_set_ara_sdk_path
        # If you need headers explicitly:
        # juce::juce_ara_headers
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
        juce::juce_recommended_lto_flags
)

# ---------------------------
# ONNX Runtime (recommended: prebuilt)
# ---------------------------
if (onnxruntime_FOUND)
    target_link_libraries(HiFiTune PRIVATE onnxruntime::onnxruntime)
    target_compile_definitions(HiFiTune PRIVATE HIFITUNE_USE_ONNXRUNTIME=1)
endif()
