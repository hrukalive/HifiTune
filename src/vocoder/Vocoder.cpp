#include "Vocoder.h"

#include <algorithm>
#include <cmath>

namespace
{
int clampToRange(int value, int minValue, int maxValue)
{
    return juce::jlimit(minValue, maxValue, value);
}

juce::AudioBuffer<float> ensureChannelCount(const juce::AudioBuffer<float>& source, int channels)
{
    if (source.getNumChannels() == channels)
        return source;

    juce::AudioBuffer<float> adjusted(channels, source.getNumSamples());
    adjusted.clear();

    for (int channel = 0; channel < channels; ++channel)
    {
        const auto sourceChannel = std::min(channel, source.getNumChannels() - 1);
        adjusted.copyFrom(channel, 0, source, sourceChannel, 0, source.getNumSamples());
    }

    return adjusted;
}
}

Vocoder::RenderOutput Vocoder::render(const RenderInput& input, const RenderSettings& settings)
{
    RenderOutput output;
    if (! isValid())
        return output;

    if (input.sampleRate <= 0.0 || input.rawAudio.getNumSamples() == 0)
        return output;

    const int hopSize = resolveHopSize(input, settings);
    if (hopSize <= 0)
        return output;

    auto synthesized = synthesize(input, settings, hopSize);
    if (synthesized.getNumSamples() == 0)
        return output;

    if (settings.replaceWholeSegment)
    {
        output.audio = ensureChannelCount(synthesized, input.rawAudio.getNumChannels());
    }
    else
    {
        output.audio = input.rawAudio;
        auto synthChannels = ensureChannelCount(synthesized, output.audio.getNumChannels());
        const int extendSamples = static_cast<int>(std::round(settings.extendSeconds * input.sampleRate));
        const int crossfadeSamples = static_cast<int>(std::round(settings.crossfadeSeconds * input.sampleRate));
        auto regions = normalizeEditedRegions(input.editedRegions, output.audio.getNumSamples(), extendSamples);
        applyBlend(output.audio, input.rawAudio, synthChannels, regions, crossfadeSamples);
    }

    if (! input.cacheKey.isEmpty())
    {
        const juce::ScopedLock lock(cacheMutex);
        renderCache[input.cacheKey] = output.audio;
    }

    return output;
}

const juce::AudioBuffer<float>* Vocoder::getCachedRender(const juce::String& cacheKey) const
{
    const juce::ScopedLock lock(cacheMutex);
    if (auto it = renderCache.find(cacheKey); it != renderCache.end())
        return &it->second;

    return nullptr;
}

int Vocoder::resolveHopSize(const RenderInput& input, const RenderSettings& settings)
{
    if (settings.hopSizeOverride > 0)
        return settings.hopSizeOverride;

    if (settings.hopSecondsOverride > 0.0 && input.sampleRate > 0.0)
        return static_cast<int>(std::round(settings.hopSecondsOverride * input.sampleRate));

    if (input.hopSeconds > 0.0 && input.sampleRate > 0.0)
        return static_cast<int>(std::round(input.hopSeconds * input.sampleRate));

    const int melFrames = input.melSpectrogram.getNumSamples();
    if (melFrames > 0 && input.rawAudio.getNumSamples() > 0)
        return std::max(1, input.rawAudio.getNumSamples() / melFrames);

    return 0;
}

juce::Array<juce::Range<int>> Vocoder::normalizeEditedRegions(const juce::Array<juce::Range<int>>& regions,
                                                              int totalSamples,
                                                              int extendSamples)
{
    if (totalSamples <= 0)
        return {};

    juce::Array<juce::Range<int>> normalized;
    normalized.ensureStorageAllocated(regions.size());

    for (const auto& region : regions)
    {
        const int start = clampToRange(region.getStart() - extendSamples, 0, totalSamples);
        const int end = clampToRange(region.getEnd() + extendSamples, 0, totalSamples);
        if (end > start)
            normalized.add({ start, end });
    }

    if (normalized.isEmpty())
        return normalized;

    std::sort(normalized.begin(), normalized.end(),
              [](const juce::Range<int>& a, const juce::Range<int>& b)
              {
                  return a.getStart() < b.getStart();
              });

    juce::Array<juce::Range<int>> merged;
    merged.add(normalized.getFirst());

    for (int i = 1; i < normalized.size(); ++i)
    {
        auto& last = merged.getReference(merged.size() - 1);
        const auto& current = normalized.getReference(i);
        if (current.getStart() <= last.getEnd())
            last = last.getUnionWith(current);
        else
            merged.add(current);
    }

    return merged;
}

void Vocoder::applyBlend(juce::AudioBuffer<float>& output,
                         const juce::AudioBuffer<float>& raw,
                         const juce::AudioBuffer<float>& synth,
                         const juce::Array<juce::Range<int>>& regions,
                         int crossfadeSamples)
{
    if (regions.isEmpty())
        return;

    const int numSamples = output.getNumSamples();
    const int numChannels = output.getNumChannels();
    const int safeCrossfade = std::max(0, crossfadeSamples);

    for (const auto& region : regions)
    {
        const int start = clampToRange(region.getStart(), 0, numSamples);
        const int end = clampToRange(region.getEnd(), 0, numSamples);
        if (end <= start)
            continue;

        for (int sample = start; sample < end; ++sample)
        {
            float weight = 1.0f;
            if (safeCrossfade > 0)
            {
                if (sample < start + safeCrossfade)
                {
                    weight = static_cast<float>(sample - start) / static_cast<float>(safeCrossfade);
                }
                else if (sample > end - safeCrossfade)
                {
                    weight = static_cast<float>(end - sample) / static_cast<float>(safeCrossfade);
                }
            }

            weight = juce::jlimit(0.0f, 1.0f, weight);

            for (int channel = 0; channel < numChannels; ++channel)
            {
                const float rawSample = raw.getSample(channel, sample);
                const float synthSample = synth.getSample(channel, sample);
                output.setSample(channel, sample, rawSample + (synthSample - rawSample) * weight);
            }
        }
    }
}
