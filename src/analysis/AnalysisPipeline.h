#pragma once

#include <vector>

#include <juce_data_structures/juce_data_structures.h>

#include "AnalyzerRegistry.h"
#include "SilenceSegmenter.h"
#include "../model/AnalysisConfigState.h"

class AnalysisPipeline
{
public:
    struct Result
    {
        juce::ValueTree analysisTree;
        std::vector<SegmentState> segments;
    };

    explicit AnalysisPipeline(AnalyzerRegistry& registryIn, SilenceSegmenter::Settings segmenterSettings = {});

    Result analyze(const AudioBufferSource& audioSource,
                   const AnalysisConfigState& config,
                   const juce::String& pitchAnalyzerId,
                   const juce::String& midiExtractorId,
                   const juce::String& separatorId,
                   juce::UndoManager* undoManager);

    static juce::ValueTree createAnalysisTree();
    static bool isAnalysisTree(const juce::ValueTree& tree);

    static const juce::Identifier analysisTypeId;
    static const juce::Identifier segmentsTypeId;

private:
    AnalyzerRegistry& registry;
    SilenceSegmenter segmenter;
};
