//
//  DeckComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#include "DeckComponent.hpp"


DeckComponent::DeckComponent(int id) :
    deckId(id)
{
    waveform.reset(new WaveformComponent());
    addAndMakeVisible(waveform.get());
    
    startTimer(33); // 10ms interval = 100Hz, 15ms = 66.7Hz, 33ms = 30.3Hz
}


void DeckComponent::resized()
{
    waveform->setSize(getWidth(), WAVEFORM_HEIGHT);
    if (deckId == 0)
        waveform->setTopLeftPosition(0, getHeight()-WAVEFORM_HEIGHT);
    else
        waveform->setTopLeftPosition(0, 0);
}


void DeckComponent::hiResTimerCallback()
{
    if (newTrack.load())
        loadTrack();
        
    waveform->draw(playhead.load(), timeStretch.load(), gain.load());
}


void DeckComponent::buttonClicked(juce::Button* button)
{
    int id = button->getComponentID().getIntValue();
    
    switch (id)
    {
        default:
            jassert(false); // Unrecognised button ID
    }
}


void DeckComponent::loadTrack()
{
    // TODO: show title, artist, etc
    
    waveform->loadTrack(track);
    
    newTrack.store(false);
}


void DeckComponent::update(Track* trackPtr, int p, double t, double g)
{
    if (trackPtr)
    {
        track = trackPtr;
        newTrack.store(true);
    }
    
    playhead.store(p);
    timeStretch.store(t);
    gain.store(g);
}
