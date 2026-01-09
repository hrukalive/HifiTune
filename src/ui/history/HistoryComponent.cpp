#include "HistoryComponent.h"

HistoryComponent::HistoryComponent()
{
    historyList.setRowHeight(28);
    historyList.setClickingTogglesRowSelection(true);
    historyList.setOutlineThickness(1);
    addAndMakeVisible(historyList);

    clearButton.onClick = [this]
    {
        if (undoManager == nullptr)
            return;

        const auto confirmed = juce::AlertWindow::showOkCancelBox(
            juce::AlertWindow::WarningIcon,
            "Clear History",
            "This will remove all undo and redo steps. Do you want to continue?",
            "Clear",
            "Cancel",
            this);

        if (!confirmed)
            return;

        undoManager->clearUndoHistory();
        refreshHistory();
    };
    addAndMakeVisible(clearButton);
}

HistoryComponent::~HistoryComponent()
{
    setUndoManager(nullptr);
}

void HistoryComponent::setUndoManager(juce::UndoManager* manager)
{
    if (undoManager == manager)
        return;

    if (undoManager != nullptr)
        undoManager->removeChangeListener(this);

    undoManager = manager;

    if (undoManager != nullptr)
        undoManager->addChangeListener(this);

    refreshHistory();
}

void HistoryComponent::resized()
{
    auto bounds = getLocalBounds();
    auto buttonArea = bounds.removeFromBottom(32);
    clearButton.setBounds(buttonArea.reduced(4));
    historyList.setBounds(bounds);
}

int HistoryComponent::getNumRows()
{
    return static_cast<int>(entries.size());
}

void HistoryComponent::paintListBoxItem(int rowNumber,
                                        juce::Graphics& g,
                                        int width,
                                        int height,
                                        bool rowIsSelected)
{
    if (rowNumber < 0 || rowNumber >= static_cast<int>(entries.size()))
        return;

    const auto& entry = entries[static_cast<size_t>(rowNumber)];
    if (rowIsSelected)
        g.fillAll(findColour(juce::ListBox::backgroundColourId).brighter(0.1f));

    const auto isCurrent = rowNumber == currentHistoryIndex();
    g.setColour(findColour(juce::ListBox::textColourId));
    g.setFont(isCurrent ? 14.0f : 13.0f);

    auto textBounds = juce::Rectangle<int>(0, 0, width, height).reduced(8, 4);
    g.drawText(entry.description, textBounds.removeFromTop(textBounds.getHeight() / 2), juce::Justification::centredLeft);
    g.setFont(11.0f);
    g.setColour(findColour(juce::ListBox::textColourId).withAlpha(0.6f));
    g.drawText(entry.timeLabel, textBounds, juce::Justification::centredLeft);
}

void HistoryComponent::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    jumpToHistoryIndex(row);
}

void HistoryComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == undoManager)
        refreshHistory();
}

void HistoryComponent::refreshHistory()
{
    entries.clear();
    undoDescriptions.clear();
    redoDescriptions.clear();

    if (undoManager == nullptr)
    {
        historyList.updateContent();
        historyList.repaint();
        return;
    }

    undoDescriptions = undoManager->getUndoDescriptions();
    redoDescriptions = undoManager->getRedoDescriptions();

    const auto undoCount = undoDescriptions.size();
    const auto redoCount = redoDescriptions.size();

    entries.reserve(static_cast<size_t>(undoCount + redoCount));

    auto now = juce::Time::getCurrentTime();
    auto totalCount = undoCount + redoCount;

    for (int i = undoCount - 1; i >= 0; --i)
    {
        const auto indexFromEnd = undoCount - 1 - i;
        const auto timestamp = now - juce::RelativeTime(static_cast<double>(totalCount - 1 - indexFromEnd) * 60.0);
        entries.push_back({undoDescriptions[i], timestamp.toString(true, true)});
    }

    for (int i = 0; i < redoCount; ++i)
    {
        const auto indexFromEnd = undoCount + i;
        const auto timestamp = now - juce::RelativeTime(static_cast<double>(totalCount - 1 - indexFromEnd) * 60.0);
        entries.push_back({redoDescriptions[i], timestamp.toString(true, true)});
    }

    historyList.updateContent();
    const auto currentIndex = currentHistoryIndex();
    if (currentIndex >= 0 && currentIndex < historyList.getNumRows())
        historyList.selectRow(currentIndex, juce::NotificationType::dontSendNotification);
    historyList.repaint();
}

void HistoryComponent::jumpToHistoryIndex(int index)
{
    if (undoManager == nullptr)
        return;

    const auto totalCount = entries.size();
    if (index < 0 || static_cast<size_t>(index) >= totalCount)
        return;

    const auto currentIndex = currentHistoryIndex();
    if (index == currentIndex)
        return;

    if (index < currentIndex)
    {
        const auto steps = currentIndex - index;
        for (int i = 0; i < steps && undoManager->canUndo(); ++i)
            undoManager->undo();
    }
    else
    {
        const auto steps = index - currentIndex;
        for (int i = 0; i < steps && undoManager->canRedo(); ++i)
            undoManager->redo();
    }
}

int HistoryComponent::currentHistoryIndex() const
{
    if (undoManager == nullptr)
        return -1;

    return undoDescriptions.size() - 1;
}
