#include "PluginProcessor.h"
#include "PluginEditor.h"

#if HIFITUNE_ENABLE_ARA
#include "../ara/AraIntegration.h"
#endif

HifiTuneAudioProcessor::HifiTuneAudioProcessor()
    : juce::AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                              .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
#if HIFITUNE_ENABLE_ARA
    HifiTuneAraIntegration::configureProcessor(*this);
#endif
}

HifiTuneAudioProcessor::~HifiTuneAudioProcessor() = default;

const juce::String HifiTuneAudioProcessor::getName() const
{
    return "HifiTune";
}

bool HifiTuneAudioProcessor::acceptsMidi() const
{
    return false;
}

bool HifiTuneAudioProcessor::producesMidi() const
{
    return false;
}

bool HifiTuneAudioProcessor::isMidiEffect() const
{
    return false;
}

double HifiTuneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HifiTuneAudioProcessor::getNumPrograms()
{
    return 1;
}

int HifiTuneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HifiTuneAudioProcessor::setCurrentProgram(int)
{
}

const juce::String HifiTuneAudioProcessor::getProgramName(int)
{
    return {};
}

void HifiTuneAudioProcessor::changeProgramName(int, const juce::String&)
{
}

void HifiTuneAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    localRecorder.prepare(sampleRate, getTotalNumInputChannels(), samplesPerBlock);
}

void HifiTuneAudioProcessor::releaseResources()
{
    localRecorder.reset();
}

bool HifiTuneAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto stereo = juce::AudioChannelSet::stereo();
    return layouts.getMainInputChannelSet() == stereo && layouts.getMainOutputChannelSet() == stereo;
}

void HifiTuneAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
#if HIFITUNE_ENABLE_ARA
    const auto realtime = isNonRealtime() ? juce::AudioProcessor::Realtime::no : juce::AudioProcessor::Realtime::yes;
    if (processBlockForARA(buffer, realtime, getPlayHead()))
        return;
#endif
    localRecorder.pushBlock(buffer);
    buffer.clear();
}

bool HifiTuneAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* HifiTuneAudioProcessor::createEditor()
{
    return new HifiTuneAudioProcessorEditor(*this);
}

void HifiTuneAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
}

void HifiTuneAudioProcessor::setStateInformation(const void*, int)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HifiTuneAudioProcessor();
}
