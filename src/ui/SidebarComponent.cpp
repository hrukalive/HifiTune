#include "SidebarComponent.h"

SidebarComponent::SidebarComponent()
{
    setLookAndFeel(&lookAndFeel);

    settingsBox.addItem("Default", 1);
    settingsBox.addItem("Vocal", 2);
    settingsBox.addItem("Instrument", 3);
    settingsBox.setSelectedId(1);

    scaleBox.addItem("C Major", 1);
    scaleBox.addItem("G Major", 2);
    scaleBox.addItem("A Minor", 3);
    scaleBox.setSelectedId(1);

    midiRefBox.addItem("Guide Track", 1);
    midiRefBox.addItem("Imported", 2);
    midiRefBox.setSelectedId(1);

    timeBox.addItem("Bars/Beats", 1);
    timeBox.addItem("Seconds", 2);
    timeBox.setSelectedId(1);

    addAndMakeVisible(settingsGroup);
    addAndMakeVisible(scaleGroup);
    addAndMakeVisible(midiRefGroup);
    addAndMakeVisible(timeGroup);

    settingsGroup.addAndMakeVisible(settingsBox);
    scaleGroup.addAndMakeVisible(scaleBox);
    midiRefGroup.addAndMakeVisible(midiRefBox);
    timeGroup.addAndMakeVisible(timeBox);
}

SidebarComponent::~SidebarComponent()
{
    setLookAndFeel(nullptr);
}

void SidebarComponent::resized()
{
    auto bounds = getLocalBounds().reduced(8);
    const int groupHeight = (bounds.getHeight() - 24) / 4;

    auto settingsBounds = bounds.removeFromTop(groupHeight);
    settingsGroup.setBounds(settingsBounds);
    settingsBox.setBounds(settingsBounds.reduced(12, 24));

    bounds.removeFromTop(8);
    auto scaleBounds = bounds.removeFromTop(groupHeight);
    scaleGroup.setBounds(scaleBounds);
    scaleBox.setBounds(scaleBounds.reduced(12, 24));

    bounds.removeFromTop(8);
    auto midiBounds = bounds.removeFromTop(groupHeight);
    midiRefGroup.setBounds(midiBounds);
    midiRefBox.setBounds(midiBounds.reduced(12, 24));

    bounds.removeFromTop(8);
    auto timeBounds = bounds.removeFromTop(groupHeight);
    timeGroup.setBounds(timeBounds);
    timeBox.setBounds(timeBounds.reduced(12, 24));
}
