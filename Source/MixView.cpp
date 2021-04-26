//
//  MixView.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#include "MixView.hpp"

#include "CommonDefs.hpp"


MixView::MixView(TrackProcessor** trackProcessors)
{
    decks.add(new DeckComponent(trackProcessors[0], true));
    decks.add(new DeckComponent(trackProcessors[1], false));
    addAndMakeVisible(decks.getUnchecked(0));
    addAndMakeVisible(decks.getUnchecked(1));
    
    startTimerHz(60);
}


void MixView::resized()
{
    decks.getUnchecked(0)->setSize(getWidth(), getHeight()/2);
    decks.getUnchecked(1)->setSize(getWidth(), getHeight()/2);
    
    decks.getUnchecked(0)->setTopLeftPosition(0, 0);
    decks.getUnchecked(1)->setTopLeftPosition(0, getHeight()/2);
}


void MixView::timerCallback()
{
    decks.getUnchecked(0)->update();
    decks.getUnchecked(1)->update();
}


void MixView::buttonClicked(juce::Button* button)
{
    int id = button->getComponentID().getIntValue();
    
    switch (id)
    {
        default:
            jassert(false); // Unrecognised button ID
    }
}


void MixView::paintOverChildren(juce::Graphics &g)
{
    g.setColour(juce::Colours::lightblue);
    g.drawVerticalLine(getWidth()/2, getHeight()/2 - WAVEFORM_HEIGHT - 10, getHeight()/2 + WAVEFORM_HEIGHT + 10);
}
