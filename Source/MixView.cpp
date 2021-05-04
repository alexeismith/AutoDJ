//
//  MixView.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#include "MixView.hpp"

#include "CommonDefs.hpp"


MixView::MixView(TrackProcessor** processors)
{
    decks.add(new DeckComponent(0, processors[0]));
    decks.add(new DeckComponent(1, processors[1]));
    addAndMakeVisible(decks.getUnchecked(0));
    addAndMakeVisible(decks.getUnchecked(1));
    
    startTimer(33); // 10ms interval = 100Hz, 15ms = 66.7Hz, 33ms = 30.3Hz
}


void MixView::hiResTimerCallback()
{
    // Tell each deck to fetch the playhead position before updating
    // This is so that the decks are as synchronised as possible
    // (If this was done in the update() function, the audio thread playhead would
    // have moved by the time the second deck updates)
    decks.getUnchecked(0)->logPlayheadPosition();
    decks.getUnchecked(1)->logPlayheadPosition();
    
    decks.getUnchecked(0)->update();
    decks.getUnchecked(1)->update();
    
    juce::MessageManager::callAsync(std::function<void()>([this]() {
        this->repaint();
    }));
}


void MixView::resized()
{
    decks.getUnchecked(0)->setSize(getWidth(), getHeight()/2);
    decks.getUnchecked(1)->setSize(getWidth(), getHeight()/2);
    
    decks.getUnchecked(0)->setTopLeftPosition(0, 0);
    decks.getUnchecked(1)->setTopLeftPosition(0, getHeight()/2);
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
    g.drawVerticalLine(getWidth()/2, getHeight()/2 - WAVEFORM_HEIGHT, getHeight()/2 + WAVEFORM_HEIGHT);
}
