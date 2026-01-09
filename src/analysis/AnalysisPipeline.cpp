#include "AnalysisPipeline.h"

const juce::Identifier AnalysisPipeline::analysisTypeId{"AnalysisResults"};
const juce::Identifier AnalysisPipeline::segmentsTypeId{"AnalysisSegments"};

AnalysisPipeline::AnalysisPipeline(AnalyzerRegistry& registryIn, SilenceSegmenter::Settings segmenterSettings)
    : registry(registryIn), segmenter(segmenterSettings)
{
}

AnalysisPipeline::Result AnalysisPipeline::analyze(const AudioBufferSource& audioSource,
                                                   const AnalysisConfigState& config,
                                                   const juce::String& pitchAnalyzerId,
                                                   const juce::String& midiExtractorId,
                                                   const juce::String& separatorId,
                                                   const TensionState& tension,
                                                   juce::UndoManager* undoManager)
{
    Result result;
    result.analysisTree = createAnalysisTree();

    juce::ValueTree segmentsTree(segmentsTypeId);
    result.segments = segmenter.detectSegments(audioSource.getClip(0, audioSource.getNumSamples()),
                                               audioSource.getSampleRate());
    for (const auto& segment : result.segments)
        segmentsTree.addChild(segment.getValueTree(), -1, undoManager);

    result.analysisTree.addChild(segmentsTree, -1, undoManager);

    if (auto separator = registry.createHarmonicNoiseSeparator(separatorId))
    {
        auto separatorTree = separator->analyze(audioSource,
                                                config,
                                                result.segments,
                                                segmenter.getSettings().beatsPerSecond,
                                                tension,
                                                undoManager);
        if (separatorTree.isValid())
            result.analysisTree.addChild(separatorTree, -1, undoManager);
    }

    if (auto pitchAnalyzer = registry.createPitchAnalyzer(pitchAnalyzerId))
    {
        auto pitchTree = pitchAnalyzer->analyze(audioSource, config, undoManager);
        if (pitchTree.isValid())
            result.analysisTree.addChild(pitchTree, -1, undoManager);
    }

    if (auto noteExtractor = registry.createMidiNoteExtractor(midiExtractorId))
    {
        auto notesTree = noteExtractor->analyze(audioSource, config, undoManager);
        if (notesTree.isValid())
            result.analysisTree.addChild(notesTree, -1, undoManager);
    }

    return result;
}

juce::ValueTree AnalysisPipeline::createAnalysisTree()
{
    return juce::ValueTree(analysisTypeId);
}

bool AnalysisPipeline::isAnalysisTree(const juce::ValueTree& tree)
{
    return tree.hasType(analysisTypeId);
}
