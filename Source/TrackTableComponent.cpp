//
//  TrackTableComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/03/2021.
//

#include "TrackTableComponent.hpp"
#include "CommonDefs.hpp"

TrackTableComponent::TrackTableComponent()
{
    table.reset(new juce::TableListBox());
    table->setModel(this);
    addAndMakeVisible(table.get());
}


void TrackTableComponent::addColumns()
{
    uint8_t columnFlags = juce::TableHeaderComponent::visible | juce::TableHeaderComponent::sortable;
    
    table->getHeader().addColumn("Artist", 1, 150, 0, 0, columnFlags);
    table->getHeader().addColumn("Title", 2, 200, 0, 0, columnFlags);
    table->getHeader().addColumn("Length", 3, 100, 0, 0, columnFlags);
    table->getHeader().addColumn("BPM", 4, 100, 0, 0, columnFlags);
    table->getHeader().addColumn("Key", 5, 100, 0, 0, columnFlags);
    table->getHeader().addColumn("Energy", 6, 100, 0, 0, columnFlags);
}


void TrackTableComponent::populate(juce::Array<TrackData>* trackList)
{
    tracks = trackList;
    numRows = tracks->size();
    table->getHeader().setSortColumnId(1, true);
}


void TrackTableComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
    auto alternateColour = getLookAndFeel().findColour (juce::ListBox::backgroundColourId)
                                           .interpolatedWith (getLookAndFeel().findColour (juce::ListBox::textColourId), 0.03f);
    if (rowIsSelected)
        g.fillAll (juce::Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll (alternateColour);
}


void TrackTableComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour (rowIsSelected ? juce::Colours::darkblue : getLookAndFeel().findColour (juce::ListBox::textColourId));
    g.setFont (font);

    juce::String text = getValueForColumn(tracks->getReference(rowNumber), columnId);

    g.drawText (text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);

    g.setColour (getLookAndFeel().findColour (juce::ListBox::backgroundColourId));
    g.fillRect (width - 1, 0, 1, height);
}


void TrackTableComponent::resized()
{
    table->setSize(getWidth(), getHeight());
}


void TrackTableComponent::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    TrackTableSorter sorter(newSortColumnId, isForwards);
    tracks->sort(sorter);
}


juce::String TrackTableComponent::getValueForColumn(TrackData& track, int columnId)
{
    switch(columnId)
    {
        case 1:
            return track.artist;
        case 2:
            return track.title;
        case 3:
            return AutoDJ::getLengthString(track.length);
        case 4:
            return juce::String(track.bpm);
        case 5:
            return juce::String(track.key);
        case 6:
            return juce::String(track.energy);
        default:
            jassert(false); // Unrecognised column ID
            return juce::String();
    }
}


int TrackTableSorter::compareElements(TrackData first, TrackData second)
{
    int result;
    
    switch(columnId)
    {
        case 1:
            result = first.artist.compareIgnoreCase(second.artist);
            break;
        case 2:
            result = first.title.compareIgnoreCase(second.title);
            break;
        case 3:
            result = 2 * (second.length < first.length) - 1;
            break;
        case 4:
            result = 2 * (second.bpm < first.bpm) - 1;
            break;
        case 5:
            result = 2 * (second.key < first.key) - 1;
            break;
        // TODO: Check this energy comparison is in line with scale
        case 6:
            result = 2 * (second.energy < first.energy) - 1;
            break;
        default:
            jassert(false); // Unrecognised column ID
            return 0;
    }
    
    // TODO: Check this extra sorting works
    if (result == 0)
        result = first.artist.compareIgnoreCase(second.artist);
    
    if (result == 0)
        result = first.title.compareIgnoreCase(second.title);

    return direction * result;
}
