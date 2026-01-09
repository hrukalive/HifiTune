#include "SomeMidiExtractor.h"

juce::Array<MidiExtractor::TrackInfo> SomeMidiExtractor::getTracks(const juce::File& midiFile)
{
    juce::Array<TrackInfo> tracks;
    juce::MidiFile file;
    if (!loadMidiFile(midiFile, file))
        return tracks;

    tracks.ensureStorageAllocated(file.getNumTracks());
    for (int i = 0; i < file.getNumTracks(); ++i)
    {
        auto* track = file.getTrack(i);
        if (track == nullptr)
            continue;

        TrackInfo info;
        info.index = i;
        info.name = findTrackName(*track);
        info.noteCount = 0;
        for (int eventIndex = 0; eventIndex < track->getNumEvents(); ++eventIndex)
        {
            auto* event = track->getEventPointer(eventIndex);
            if (event != nullptr && event->message.isNoteOn())
                ++info.noteCount;
        }
        if (info.name.isEmpty())
            info.name = "Track " + juce::String(i + 1);
        tracks.add(info);
    }

    return tracks;
}

std::vector<MidiNoteState> SomeMidiExtractor::extractNotes(const juce::File& midiFile, int trackIndex)
{
    std::vector<MidiNoteState> notes;
    juce::MidiFile file;
    if (!loadMidiFile(midiFile, file))
        return notes;

    auto* track = file.getTrack(trackIndex);
    if (track == nullptr)
        return notes;

    juce::MidiMessageSequence sequence(*track);
    sequence.updateMatchedPairs();

    const auto timeFormat = file.getTimeFormat();
    for (int eventIndex = 0; eventIndex < sequence.getNumEvents(); ++eventIndex)
    {
        auto* event = sequence.getEventPointer(eventIndex);
        if (event == nullptr || !event->message.isNoteOn())
            continue;

        const auto startTicks = event->message.getTimeStamp();
        const auto endTicks = event->noteOffObject != nullptr
                                  ? event->noteOffObject->message.getTimeStamp()
                                  : startTicks;

        const auto startBeat = ticksToBeats(startTicks, timeFormat);
        const auto lengthBeats = std::max(0.0, ticksToBeats(endTicks - startTicks, timeFormat));

        const auto midiNote = event->message.getNoteNumber();
        const auto velocity = static_cast<double>(event->message.getVelocity());
        notes.emplace_back(MidiNoteState::create(startBeat, lengthBeats, midiNote, velocity));
    }

    return notes;
}

bool SomeMidiExtractor::loadMidiFile(const juce::File& midiFile, juce::MidiFile& output) const
{
    if (!midiFile.existsAsFile())
        return false;

    juce::FileInputStream input(midiFile);
    if (!input.openedOk())
        return false;

    if (!output.readFrom(input))
        return false;

    return true;
}

juce::String SomeMidiExtractor::findTrackName(const juce::MidiMessageSequence& sequence)
{
    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const auto* event = sequence.getEventPointer(i);
        if (event != nullptr && event->message.isTrackNameEvent())
            return event->message.getTextFromTextMetaEvent();
    }
    return {};
}

double SomeMidiExtractor::ticksToBeats(double ticks, int timeFormat)
{
    if (timeFormat > 0)
        return ticks / static_cast<double>(timeFormat);

    const auto fallbackTicks = 960.0;
    return ticks / fallbackTicks;
}
