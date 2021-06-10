//
//  TrackTableComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/03/2021.
//

#include "TrackTableComponent.hpp"
#include "CommonDefs.hpp"
#include "CamelotKey.hpp"

#define INITIAL_SORT_COLUMN (2)

enum TrackTableColumns {
    artist = 1,
    title,
    length,
    bpm,
    key,
    groove
};


TrackTableComponent::TrackTableComponent(TrackEditor* editor) :
    trackEditor(editor)
{
    table.reset(new juce::TableListBox());
    table->setModel(this);
    addAndMakeVisible(table.get());
    
    sortOrderChanged(INITIAL_SORT_COLUMN, true);
}


void TrackTableComponent::resized()
{
    int availableWidth = getWidth() - fixedColumnWidth - table->getVerticalScrollBar().getWidth();

    table->getHeader().setColumnWidth(2, availableWidth);
    
    table->setSize(getWidth(), getHeight());
    
    table->getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::thumbColourId, juce::Colours::black.withAlpha(0.f));
}


void TrackTableComponent::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& e)
{
    TrackInfo* info = tracksSorted.getReference(rowNumber);
    Track track;
    track.info = info;
    trackEditor->load(track);
}


void TrackTableComponent::addColumns()
{
    uint8_t columnFlags = juce::TableHeaderComponent::visible | juce::TableHeaderComponent::sortable;
    
    table->getHeader().addColumn("", 1, 28, 0, 1000, juce::TableHeaderComponent::visible); // Dummy column to provide some visual padding at left
    table->getHeader().addColumn("Title", 2, 310, 0, 2000, columnFlags);
    table->getHeader().addColumn("Length", 3, 80, 0, 1000, columnFlags);
    table->getHeader().addColumn("BPM", 4, 60, 0, 1000, columnFlags);
    table->getHeader().addColumn("Key", 5, 60, 0, 1000, columnFlags);
    table->getHeader().addColumn("Groove", 6, 100, 0, 1000, columnFlags);
    
    fixedColumnWidth = table->getHeader().getColumnWidth(1) + table->getHeader().getColumnWidth(3) + table->getHeader().getColumnWidth(4) + table->getHeader().getColumnWidth(5) + table->getHeader().getColumnWidth(6);
}


void TrackTableComponent::populate(TrackInfo* tracks, int numTracks)
{
    for (int i = 0; i < numTracks; i++)
        tracksSorted.add(&tracks[i]);
    
    table->getHeader().setSortColumnId(INITIAL_SORT_COLUMN, true);
}


void TrackTableComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
    // Generate a colour for rows when unselected
    juce::Colour alternateColour = getLookAndFeel().findColour(juce::ListBox::backgroundColourId)
                                           .interpolatedWith(getLookAndFeel().findColour (juce::ListBox::textColourId), 0.03f);
    
    // Paint row colour based on selection state
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll(alternateColour);
}


void TrackTableComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    TrackInfo* track = tracksSorted.getReference(rowNumber);
    
    // Text colour depends on the state of the track, as well as the column being painted
    if (rowIsSelected) // Dark blue colour for the selected row
        g.setColour(juce::Colours::darkblue);
    else if (columnId == TrackTableColumns::key && track->analysed) // Colour-coded with key signature for the 'Key' column
        g.setColour(CamelotKey(track->key).getColour());
    else if (!track->analysed) // Grey text for un-analysed tracks
        g.setColour(juce::Colours::lightslategrey.brighter());
    else if (track->playing) // Cyan for tracks currently playing
        g.setColour(juce::Colours::cyan);
    else if (track->played) // Green for tracks already played
        g.setColour(juce::Colours::lightgreen.darker());
    else // Normal text colour for analysed tracks, that are not playing or played
        g.setColour(getLookAndFeel().findColour(juce::ListBox::textColourId));
        
    g.setFont(font);

    juce::String text = getValueForColumn(track, columnId);

    g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
}


void TrackTableComponent::sortOrderChanged(int columnId, bool ascending)
{
    sorter.reset(new TrackTableSorter(columnId, ascending));
    sort();
}


void TrackTableComponent::sort()
{
    tracksSorted.sort(*sorter.get());
    repaint();
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
            result = CamelotKey::sort(CamelotKey(first->key), CamelotKey(second->key));
            break;
        case TrackTableColumns::groove:
            if (first->groove == second->groove) result = 0;
            else result = first->groove > second->groove ? 1 : -1;
            break;
        default:
            jassert(false); // Unrecognised column ID
            return 0;
    }
    
    if (result == 0)
        result = first->getArtist().compareIgnoreCase(second->getArtist());
    
    if (result == 0)
        result = first->getTitle().compareIgnoreCase(second->getTitle());
    
    if (!first->analysed && second->analysed)
        result = 1;
    else if (first->analysed && !second->analysed)
        result = -1;

    return direction * result;
}


juce::String TrackTableComponent::getValueForColumn(TrackInfo* track, int columnId)
{
    switch(columnId)
    {
        case TrackTableColumns::artist:
            return juce::String();
//            return track->getArtist(); // TODO: Artist metadata extraction not working, so only showing title for now
            
        case TrackTableColumns::title:
            return track->getTitle();
            
        case TrackTableColumns::length:
            return AutoDJ::getLengthString(track->length);
            
        case TrackTableColumns::bpm:
            if (track->bpm == -1) return juce::String("-");
            return juce::String(track->bpm);
            
        case TrackTableColumns::key:
            return CamelotKey(track->key).getName();
            
        case TrackTableColumns::groove:
            if (track->groove < 0.f) return juce::String("-");
            return AutoDJ::getGrooveString(track->groove);
            
        default:
            jassert(false); // Unrecognised column ID
            return juce::String();
    }
}
