#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_data_structures/juce_data_structures.h>

#include <vector>

class HistoryComponent final : public juce::Component,
                               private juce::ListBoxModel,
                               private juce::ChangeListener
{
public:
    HistoryComponent();
    ~HistoryComponent() override;

    void setUndoManager(juce::UndoManager* manager);

    void resized() override;

private:
    struct HistoryEntry
    {
        juce::String description;
        juce::String timeLabel;
    };

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void refreshHistory();
    void jumpToHistoryIndex(int index);
    int currentHistoryIndex() const;

    juce::UndoManager* undoManager = nullptr;
    juce::StringArray undoDescriptions;
    juce::StringArray redoDescriptions;
    std::vector<HistoryEntry> entries;

    juce::ListBox historyList { "historyList", this };
    juce::TextButton clearButton { "Clear History" };
};
