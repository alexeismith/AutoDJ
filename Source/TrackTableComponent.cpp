//
//  TrackTableComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/03/2021.
//

#include "TrackTableComponent.hpp"

TrackTableComponent::TrackTableComponent()
{
    table.getHeader().addColumn("Artist", 1, 100, 50, 200, juce::TableHeaderComponent::defaultFlags);
    table.getHeader().addColumn("Title", 2, 100, 50, 200, juce::TableHeaderComponent::defaultFlags);
    table.getHeader().addColumn("Length", 3, 100, 50, 200, juce::TableHeaderComponent::defaultFlags);
    table.getHeader().addColumn("BPM", 4, 100, 50, 200, juce::TableHeaderComponent::defaultFlags);
    table.getHeader().addColumn("Key", 5, 100, 50, 200, juce::TableHeaderComponent::defaultFlags);
    table.getHeader().addColumn("Energy", 6, 100, 50, 200, juce::TableHeaderComponent::defaultFlags);
    
    addAndMakeVisible(table);
    table.getHeader().setSortColumnId(1, true);
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

   juce::String text("Testing");

   g.drawText (text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);

   g.setColour (getLookAndFeel().findColour (juce::ListBox::backgroundColourId));
   g.fillRect (width - 1, 0, 1, height);
}


void TrackTableComponent::resized()
{
    table.setSize(getWidth(), getHeight());
}


void TrackTableComponent::sortOrderChanged (int newSortColumnId, bool isForwards)
{
    
}
