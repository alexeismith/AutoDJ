//
//  TrackTableComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/03/2021.
//

#include "TrackTableComponent.hpp"
#include "CommonDefs.hpp"

enum TrackTableColumns {
    artist = 1,
    title,
    length,
    bpm,
    key,
    energy
};


TrackTableComponent::TrackTableComponent()
{
    table.reset(new juce::TableListBox());
    table->setModel(this);
    addAndMakeVisible(table.get());
}


void TrackTableComponent::addColumns()
{
    uint8_t columnFlags = juce::TableHeaderComponent::visible | juce::TableHeaderComponent::sortable;
    
    table->getHeader().addColumn("Artist", 1, 210, 0, 0, columnFlags);
    table->getHeader().addColumn("Title", 2, 310, 0, 0, columnFlags);
    table->getHeader().addColumn("Length", 3, 80, 0, 0, columnFlags);
    table->getHeader().addColumn("BPM", 4, 60, 0, 0, columnFlags);
    table->getHeader().addColumn("Key", 5, 60, 0, 0, columnFlags);
    table->getHeader().addColumn("Energy", 6, 80, 0, 0, columnFlags);
}


void TrackTableComponent::populate(TrackInfo* tracks, int numTracks)
{
    for (int i = 0; i < numTracks; i++)
        tracksSorted.add(&tracks[i]);
    
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
    if (rowIsSelected)
        g.setColour(juce::Colours::darkblue);
    else if(!tracksSorted.getReference(rowNumber)->analysed)
        g.setColour(juce::Colours::lightslategrey.brighter());
    else
        g.setColour(getLookAndFeel().findColour (juce::ListBox::textColourId));
        
    g.setFont (font);

    juce::String text = getValueForColumn(tracksSorted.getReference(rowNumber), columnId);

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
    sorter.reset(new TrackTableSorter(newSortColumnId, isForwards));
    sort();
}


void TrackTableComponent::sort()
{
    tracksSorted.sort(*sorter.get());
    repaint();
}


juce::String TrackTableComponent::getValueForColumn(TrackInfo* track, int columnId)
{
    switch(columnId)
    {
        case TrackTableColumns::artist:
            return track->getArtist();
        case TrackTableColumns::title:
            if (track->getTitle().isEmpty()) return track->getFilename();
            else return track->getTitle();
        case TrackTableColumns::length:
            return AutoDJ::getLengthString(track->length);
        case TrackTableColumns::bpm:
            if (track->bpm == -1) return juce::String("-");
            return juce::String(track->bpm);
        case TrackTableColumns::key:
            return AutoDJ::getKeyName(track->key);
        case TrackTableColumns::energy:
            if (track->energy == -1) return juce::String("-");
            return juce::String(track->energy);
        default:
            jassert(false); // Unrecognised column ID
            return juce::String();
    }
}


int TrackTableSorter::compareElements(TrackInfo* first, TrackInfo* second)
{
    int result;
    
    switch(columnId)
    {
        case TrackTableColumns::artist:
            result = first->getArtist().compareIgnoreCase(second->getArtist());
            break;
        case TrackTableColumns::title:
            result = first->getTitle().compareIgnoreCase(second->getTitle());
            break;
        case TrackTableColumns::length:
            if (first->length == second->length) result = 0;
            else result = first->length > second->length ? 1 : -1;
            break;
        case TrackTableColumns::bpm:
            if (first->bpm == second->bpm) result = 0;
            else result = first->bpm > second->bpm ? 1 : -1;
            break;
        case TrackTableColumns::key:
            if (first->key == second->key) result = 0;
            else result = first->key > second->key ? 1 : -1;
            break;
        // TODO: Check this energy comparison is in line with scale
        case TrackTableColumns::energy:
            if (first->energy == second->energy) result = 0;
            else result = first->energy > second->energy ? 1 : -1;
            break;
        default:
            jassert(false); // Unrecognised column ID
            return 0;
    }
    
    if (result == 0)
        result = first->getArtist().compareIgnoreCase(second->getArtist());
    
    if (result == 0)
        result = first->getTitle().compareIgnoreCase(second->getTitle());
    
    if (result == 0)
        result = first->getFilename().compareIgnoreCase(second->getFilename());

    return direction * result;
}
