#include "VocalRemoverSeparator.h"

#include <algorithm>
#include <cmath>

VocalRemoverSeparator::VocalRemoverSeparator() = default;

bool VocalRemoverSeparator::SegmentKey::operator<(const SegmentKey& other) const
{
    if (startBeat != other.startBeat)
        return startBeat < other.startBeat;
    return lengthBeats < other.lengthBeats;
}

juce::ValueTree VocalRemoverSeparator::analyze(const AudioBufferSource& audioSource,
                                               const AnalysisConfigState& config,
                                               const std::vector<SegmentState>& segments,
                                               double beatsPerSecond,
                                               const TensionState& tension,
                                               juce::UndoManager* undoManager)
{
    const double sampleRate = audioSource.getSampleRate();
    const juce::String nextSignature = makeSettingsSignature(config, sampleRate);
    if (nextSignature != settingsSignature)
    {
        settingsSignature = nextSignature;
        cache.clear();
    }

    auto resultTree = createResultTree();
    auto segmentsTree = createSegmentsTree();

    const auto gains = getOutputGains();
    const int hopSize = config.getHopSize();

    for (size_t index = 0; index < segments.size(); ++index)
    {
        const auto& segment = segments[index];
        SegmentKey key{ segment.getStartBeat(), segment.getLengthBeats() };
        auto blend = tension.getBlendForIndex(static_cast<int>(index));

        auto cacheIt = cache.find(key);
        if (cacheIt == cache.end())
        {
            double startSeconds = 0.0;
            double lengthSeconds = 0.0;
            if (beatsPerSecond > 0.0)
            {
                startSeconds = segment.getStartBeat() / beatsPerSecond;
                lengthSeconds = segment.getLengthBeats() / beatsPerSecond;
            }

            auto segmentAudio = audioSource.getSegment(startSeconds, lengthSeconds);
            CachedSegment cached;
            cached.analysisBlend = blend;
            cached.outputGains = gains;
            cached.decomposition = decomposer.decompose(segmentAudio, sampleRate, config, blend, gains);
            computeMasks(cached.decomposition.harmonic, cached.decomposition.noise, hopSize,
                         cached.harmonicMask, cached.noiseMask);
            cacheIt = cache.emplace(key, std::move(cached)).first;
        }
        else
        {
            if (cacheIt->second.analysisBlend != blend)
            {
                cacheIt->second.analysisBlend = blend;
                decomposer.refreshAnalysis(cacheIt->second.decomposition, sampleRate, config, blend);
            }

            if (cacheIt->second.outputGains.harmonic != gains.harmonic
                || cacheIt->second.outputGains.noise != gains.noise)
            {
                cacheIt->second.outputGains = gains;
                decomposer.refreshOutput(cacheIt->second.decomposition, gains);
            }
        }

        auto segmentTree = createSegmentTree(segment.getStartBeat(), segment.getLengthBeats());
        setHarmonicMask(segmentTree, cacheIt->second.harmonicMask, undoManager);
        setNoiseMask(segmentTree, cacheIt->second.noiseMask, undoManager);
        setSegmentGains(segmentTree, gains, undoManager);
        segmentsTree.addChild(segmentTree, -1, undoManager);
    }

    resultTree.addChild(segmentsTree, -1, undoManager);
    return resultTree;
}

void VocalRemoverSeparator::setDecomposerSettings(const DiffSingerDecomposer::Settings& settings)
{
    decomposer.setSettings(settings);
    settingsSignature.clear();
    cache.clear();
}

juce::String VocalRemoverSeparator::makeSettingsSignature(const AnalysisConfigState& config, double sampleRate) const
{
    const auto& settings = decomposer.getSettings();
    return juce::String(sampleRate)
        + ":" + juce::String(config.getSamplingRate())
        + ":" + juce::String(config.getHopSize())
        + ":" + juce::String(settings.splitFrequencyHz)
        + ":" + juce::String(settings.melBins)
        + ":" + juce::String(settings.fftSize);
}

void VocalRemoverSeparator::computeMasks(const juce::AudioBuffer<float>& harmonic,
                                         const juce::AudioBuffer<float>& noise,
                                         int hopSize,
                                         juce::Array<double>& harmonicMask,
                                         juce::Array<double>& noiseMask) const
{
    harmonicMask.clear();
    noiseMask.clear();

    if (harmonic.getNumSamples() == 0 || hopSize <= 0)
        return;

    const int frames = 1 + (harmonic.getNumSamples() - 1) / hopSize;
    harmonicMask.ensureStorageAllocated(frames);
    noiseMask.ensureStorageAllocated(frames);

    for (int frame = 0; frame < frames; ++frame)
    {
        const int startSample = frame * hopSize;
        const int endSample = std::min(harmonic.getNumSamples(), startSample + hopSize);

        double harmonicEnergy = 0.0;
        double noiseEnergy = 0.0;

        for (int sample = startSample; sample < endSample; ++sample)
        {
            float harmonicSample = 0.0f;
            float noiseSample = 0.0f;

            for (int channel = 0; channel < harmonic.getNumChannels(); ++channel)
                harmonicSample += harmonic.getSample(channel, sample);
            for (int channel = 0; channel < noise.getNumChannels(); ++channel)
                noiseSample += noise.getSample(channel, sample);

            harmonicSample /= static_cast<float>(juce::jmax(1, harmonic.getNumChannels()));
            noiseSample /= static_cast<float>(juce::jmax(1, noise.getNumChannels()));

            harmonicEnergy += static_cast<double>(harmonicSample * harmonicSample);
            noiseEnergy += static_cast<double>(noiseSample * noiseSample);
        }

        const double totalEnergy = harmonicEnergy + noiseEnergy;
        const double harmonicRatio = totalEnergy > 0.0 ? (harmonicEnergy / totalEnergy) : 0.5;
        harmonicMask.add(harmonicRatio);
        noiseMask.add(1.0 - harmonicRatio);
    }
}
